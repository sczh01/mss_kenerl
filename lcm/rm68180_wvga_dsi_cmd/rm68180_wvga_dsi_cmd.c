/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)

#define REGFLAG_DELAY             							0xEF
#define REGFLAG_END_OF_TABLE      							0xE1  // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE								0
#define LCM_ID       (0x1868)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

static unsigned int lcm_compare_id(void);
    

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


// LCD  RUIXING  RX-43RM-416A  driver IC  RM68180
static const struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/
	{0xF0,	5,	{0x55,0xAA,0x52,0x08,0x01}},
		
	{0xB0,	3,	{0x00,0x00,0x00}},
	
	{0xB1,	3,	{0x00,0x00,0x00}},
	
	//{0xB2,	3,	{0x00,0x00,0x00}},
	
	{0xB3,	3,	{0x0F,0x0F,0x0F}},
	
	{0xB5,	3,	{0x08,0x08,0x08}},
	
	{0xB6,	3,	{0x34,0x34,0x34}},
	
	{0xB7,	3,	{0x34,0x34,0x34}},
	
	{0xB8,	3,	{0x24,0x24,0x24}},
	
	{0xB9,	3,	{0x14,0x14,0x14}},
	
	{0xBA,	3,	{0x14,0x14,0x14}},
	
	{0xBC,	3,	{0x00,0x45,0x05}},
		
	{0xBD,	3,	{0x00,0x45,0x05}},
	
	{0xBE,	2,	{0x00,0x2D}},
	
	{0xBF,	1,	{0x01}},
	
	//{0xC2,	1,	{0x03}},

	/*{0xD1,	52, {0x00,0x03,0x00,0x0C,0x00,0x1B,0x00,0x3D,0x00,0x4F,0x00,0x6C,0x00,0x76,0x00,0x7A,
               0x00,0x9B,0x00,0xE1,0x01,0x1D,0x01,0x82,0x01,0xD5,0x01,0xD7,0x02,0x13,0x02,0x7D,
               0x02,0xC2,0x03,0x36,0x03,0x4F,0x03,0x68,0x03,0x81,0x03,0x9A,0x03,0xB3,0x03,0xCC,
               0x03,0xE5,0x03,0xFF}},
	

	{0xD2,	52, {0x00,0x03,0x00,0x0C,0x00,0x1B,0x00,0x3D,0x00,0x4F,0x00,0x6C,0x00,0x76,0x00,0x7A,
               0x00,0x9B,0x00,0xE1,0x01,0x1D,0x01,0x82,0x01,0xD5,0x01,0xD7,0x02,0x13,0x02,0x7D,
               0x02,0xC2,0x03,0x36,0x03,0x4F,0x03,0x68,0x03,0x81,0x03,0x9A,0x03,0xB3,0x03,0xCC,
               0x03,0xE5,0x03,0xFF}},
	
	
	{0xD3,	52, {0x00,0x03,0x00,0x0C,0x00,0x1B,0x00,0x3D,0x00,0x4F,0x00,0x6C,0x00,0x76,0x00,0x7A,
               0x00,0x9B,0x00,0xE1,0x01,0x1D,0x01,0x82,0x01,0xD5,0x01,0xD7,0x02,0x13,0x02,0x7D,
               0x02,0xC2,0x03,0x36,0x03,0x4F,0x03,0x68,0x03,0x81,0x03,0x9A,0x03,0xB3,0x03,0xCC,
               0x03,0xE5,0x03,0xFF}},

	
	{0xD4,	52, {0x00,0x03,0x00,0x0C,0x00,0x1B,0x00,0x3D,0x00,0x4F,0x00,0x6C,0x00,0x76,0x00,0x7A,
               0x00,0x9B,0x00,0xE1,0x01,0x1D,0x01,0x82,0x01,0xD5,0x01,0xD7,0x02,0x13,0x02,0x7D,
               0x02,0xC2,0x03,0x36,0x03,0x4F,0x03,0x68,0x03,0x81,0x03,0x9A,0x03,0xB3,0x03,0xCC,
               0x03,0xE5,0x03,0xFF}},
	
	
	{0xD5,	52, {0x00,0x03,0x00,0x0C,0x00,0x1B,0x00,0x3D,0x00,0x4F,0x00,0x6C,0x00,0x76,0x00,0x7A,
               0x00,0x9B,0x00,0xE1,0x01,0x1D,0x01,0x82,0x01,0xD5,0x01,0xD7,0x02,0x13,0x02,0x7D,
               0x02,0xC2,0x03,0x36,0x03,0x4F,0x03,0x68,0x03,0x81,0x03,0x9A,0x03,0xB3,0x03,0xCC,
               0x03,0xE5,0x03,0xFF}},
		
	{0xD6,	52, {0x00,0x03,0x00,0x0C,0x00,0x1B,0x00,0x3D,0x00,0x4F,0x00,0x6C,0x00,0x76,0x00,0x7A,
               0x00,0x9B,0x00,0xE1,0x01,0x1D,0x01,0x82,0x01,0xD5,0x01,0xD7,0x02,0x13,0x02,0x7D,
               0x02,0xC2,0x03,0x36,0x03,0x4F,0x03,0x68,0x03,0x81,0x03,0x9A,0x03,0xB3,0x03,0xCC,
               0x03,0xE5,0x03,0xFF}},*/

	{0xD1,	52, {0x00,0x00,0x00,0x07,0x00,0x15,0x00,0x23,0x00,0x30,0x00,0x49,0x00,0x5F,0x00,0x87,
               0x00,0xAA,0x00,0xE7,0x01,0x1B,0x01,0x72,0x01,0xBB,0x01,0xBD,0x02,0x04,0x02,0x52,
               0x02,0x84,0x02,0xC6,0x02,0xF3,0x03,0x2D,0x03,0x51,0x03,0x7B,0x03,0x92,0x03,0xA5,
               0x03,0xB2,0x03,0xFF}},

	{0xD2,	52, {0x00,0x00,0x00,0x07,0x00,0x15,0x00,0x23,0x00,0x30,0x00,0x49,0x00,0x5F,0x00,0x87,
               0x00,0xAA,0x00,0xE7,0x01,0x1B,0x01,0x72,0x01,0xBB,0x01,0xBD,0x02,0x04,0x02,0x52,
               0x02,0x84,0x02,0xC6,0x02,0xF3,0x03,0x2D,0x03,0x51,0x03,0x7B,0x03,0x92,0x03,0xA5,
               0x03,0xB2,0x03,0xFF}},

	{0xD3,	52, {0x00,0x00,0x00,0x07,0x00,0x15,0x00,0x23,0x00,0x30,0x00,0x49,0x00,0x5F,0x00,0x87,
               0x00,0xAA,0x00,0xE7,0x01,0x1B,0x01,0x72,0x01,0xBB,0x01,0xBD,0x02,0x04,0x02,0x52,
               0x02,0x84,0x02,0xC6,0x02,0xF3,0x03,0x2D,0x03,0x51,0x03,0x7B,0x03,0x92,0x03,0xA5,
               0x03,0xB2,0x03,0xFF}},

	{0xD4,	52, {0x00,0x00,0x00,0x07,0x00,0x15,0x00,0x23,0x00,0x30,0x00,0x49,0x00,0x5F,0x00,0x87,
               0x00,0xAA,0x00,0xE7,0x01,0x1B,0x01,0x72,0x01,0xBB,0x01,0xBD,0x02,0x04,0x02,0x52,
               0x02,0x84,0x02,0xC6,0x02,0xF3,0x03,0x2D,0x03,0x51,0x03,0x7B,0x03,0x92,0x03,0xA5,
               0x03,0xB2,0x03,0xFF}},

	{0xD5,	52, {0x00,0x00,0x00,0x07,0x00,0x15,0x00,0x23,0x00,0x30,0x00,0x49,0x00,0x5F,0x00,0x87,
               0x00,0xAA,0x00,0xE7,0x01,0x1B,0x01,0x72,0x01,0xBB,0x01,0xBD,0x02,0x04,0x02,0x52,
               0x02,0x84,0x02,0xC6,0x02,0xF3,0x03,0x2D,0x03,0x51,0x03,0x7B,0x03,0x92,0x03,0xA5,
               0x03,0xB2,0x03,0xFF}},

	{0xD6,	52, {0x00,0x00,0x00,0x07,0x00,0x15,0x00,0x23,0x00,0x30,0x00,0x49,0x00,0x5F,0x00,0x87,
               0x00,0xAA,0x00,0xE7,0x01,0x1B,0x01,0x72,0x01,0xBB,0x01,0xBD,0x02,0x04,0x02,0x52,
               0x02,0x84,0x02,0xC6,0x02,0xF3,0x03,0x2D,0x03,0x51,0x03,0x7B,0x03,0x92,0x03,0xA5,
               0x03,0xB2,0x03,0xFF}},
	                 
	{0xF0,	5,	{0x55,0xAA,0x52,0x08,0x00}},
	
	//{0xB5,	1,	{0x6B}}, //480x854

	{0xB1,	1,	{0xFC}},  //fc video mode cc cmd mode
	
	{0xB4,	1,	{0x10}},
	
	{0xB6,	1,	{0x05}},
		
	{0xB7,	2,	{0x70,0x70}},
	
	{0xB8,	4,	{0x00,0x02,0x02,0x02}},  //01 05 05 05
	
	{0xBC,	3,	{0x00,0x00,0x00}},// Inversion Driver Control

	{0x44,	2,	{0x01,0x50}},
	
	//{0x2B,	4,	{0x00,0x00,0x03,0x55}},// PASSET
	
	//{0xBD,	5,	{0x01,0x84,0x1C,0x1C,0x00}},{REGFLAG_DELAY, 10, {}},//TIMING
			
	{0xCC,	1,	{0x03}},
	
	{0xF0,	5,	{0x55,0xAA,0x52,0x08,0x02}},

	{0xF6,	1,	{0x60}},
	
	
    {0x36, 1, {0x00}},
     //{0xF6, 1, {0x60}},   // 
    
    {0x35, 1, {0x00}},
    //{0x44, 2, {0x02,0x8A}},
    
    {0x3A, 1, {0x77}},
//{REGFLAG_DELAY, 5, {}},
{0x11, 0,{0x00}},
{REGFLAG_DELAY, 120, {}},
{0x29, 0,{0x00}},
{REGFLAG_DELAY, 100, {}},
	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.

	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};




static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
		dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
				MDELAY(3);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 4;
		params->dsi.vertical_backporch					= 16;//14;
		params->dsi.vertical_frontporch					= 16;//16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;//8;
		params->dsi.horizontal_backporch				= 32;//104;
		params->dsi.horizontal_frontporch				= 32;//24;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =13;//34; // 19;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
}


static void lcm_init(void)
{
#ifdef BUILD_LK
		printf("RM68180 lk %s \n", __func__);
	#else
		printk("RM68180 kernel %s \n", __func__);
	#endif
//lcm_compare_id();
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(100);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);



}


static void lcm_suspend(void)
{
	unsigned int data_array[16];

	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(10); 
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(100);
}


static void lcm_resume(void)
{
        unsigned int data_array[16];

	data_array[0] = 0x00110500; // Sleep Out
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(100);
	data_array[0] = 0x00290500; // Display On
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(120);
//	lcm_init();
	
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

		//data_array[0]= 0x00290500;
	      //// dsi_set_cmdq(&data_array, 1, 1);
	data_array[0]= 0x00052902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00052902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c2909;

	dsi_set_cmdq(&data_array, 7, 0);

}


static const struct LCM_setting_table lcm_compare_id_setting[] = {
  {0xf0,  5,      {0x55,0xaa,0x52,0x08,0x02}},
    {REGFLAG_DELAY, 10, {}},
    {0xEA,    1,{0x02}},
    {REGFLAG_DELAY, 10, {}},

 {0xf0,  5,      {0x55,0xaa,0x52,0x08,0x01}},
    {REGFLAG_DELAY, 10, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static unsigned int lcm_compare_id(void)
{
    unsigned int panel_id = 0;
    unsigned char buffer[3];
    unsigned int array[16];
    
    SET_RESET_PIN(1);  //TE:should reset LCM firstly
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(100);

    push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);

    read_reg_v2(0xC5, buffer, 2);
 panel_id = ((buffer[0] << 8) | buffer[1]); //we only need ID
 
#if defined(BUILD_LK)
 printf("lk rm68180_dsi_cmd_fwvga panel_id:0x%x\n", panel_id);
#else
 printk("kernel rm68180_dsi_cmd_fwvga panel_id:0x%x\n", panel_id);
#endif    

    return (LCM_ID == panel_id)?1:0;
}



LCM_DRIVER rm68180_dsi_cmd_lcm_drv = 
{
    .name			= "rm68180_dsi_cmd_lcm_drv",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.compare_id    = lcm_compare_id,
	//.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id    = lcm_compare_id,
	.init           = lcm_init,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };

