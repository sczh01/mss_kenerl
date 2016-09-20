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
#define FRAME_WIDTH  (1080)
#define FRAME_HEIGHT (1920)

//#define FRAME_WIDTH  (720)
//#define FRAME_HEIGHT (1280)

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define REGFLAG_DELAY                                       0XFFE
#define REGFLAG_END_OF_TABLE                                0xFFF   // END OF REGISTERS MARKER


#define LCM_ID_NT35590 (0x90)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
//==========fenggy add=========================
//#undef SET_RESET_PIN

//#define SET_RESET_PIN(v) \
//do{ \
//	mt_set_gpio_mode(GPIO_LCM_RST_PIN, GPIO_LCM_RST_PIN_M_GPIO); \
//	mt_set_gpio_dir(GPIO_LCM_RST_PIN, GPIO_DIR_OUT); \
//	if((v) == 0) mt_set_gpio_out(GPIO_LCM_RST_PIN, GPIO_OUT_ZERO); \
//	else mt_set_gpio_out(GPIO_LCM_RST_PIN, GPIO_OUT_ONE); \
//}while(0)

//==========fenggy add end=====================



#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   


#define   LCM_DSI_CMD_MODE							1

static struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] =
{
	
//R69006_MIPI_4Lane_2016.03.04
#if 0
{0xB0,   1,  {0X00}},
{0xB1,   1,  {0X00}},
{0xd6,   1,  {0X01}},
{0xB3,   5,  {0x04,0x10,0x00,0x00,0x00}},
{0xb4,   2,  {0X0c,0x00}},
{0xB6,   3,  {0x3b,0xd3,0x00}},
{0xc0,   1,  {0X00}},
{0x36,   1,  {0X98}},
{0xcc,   1,  {0X04}},


{0xC1,  31,  {0x84,0x00,0x10,0xEF,0x8B,0xF1,0xFF,0xFF,0xDF,0x9C,
              0xC5,0x9A,0x73,0x8D,0xAD,0x63,0xFE,0xFF,0xFF,0xCB,
              0xF8,0x01,0x00,0xAA,0x40,0x00,0xC2,0x01,0x08,0x00,
              0x01}},
{0xCB,  9,  {0x0D,0xFE,0x1F,0x2C,0x00,0x00,0x00,0x00,0x00}},
{0xC2,   10,  {0x01,0xF7,0x80,0x04,0x63,0x00,0x60,0x00,0x01,0x30}},
{0xC3,   6,  {0x55,0x01,0x00,0x01,0x00,0x00}},
{0xC4,  17,  {0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
	      0x01,0x00,0x05,0x01,0x00,0x00,0x00}},

{0xC6,  14,  {0x59,0x07,0x4a,0x07,0x4a,0x01,0x0E,0x01,0x02,0x01,
              0x02,0x09,0x15,0x07}},
              
{0xC7,  30,  {0x00,0x06,0x0C,0x16,0x27,0x35,0x3F,0x4D,0x33,0x3C,
              0x49,0x5B,0x64,0x66,0x67,0x00,0x06,0x0C,0x16,0x27,
              0x35,0x3F,0x4D,0x33,0x3C,0x49,0x5B,0x64,0x67,0x7a}},

{0xC8,  19,  {0x00,0x00,0xFE,0x01,0x08,0xE7,0x00,0x00,0xFD,0x02,
             0x03,0xA8,0x00,0x00,0xFC,0xE7,0xE9,0xC9,0x00}},

{0xC9,  8,  {0x1F,0x68,0x1F,0x68,0x4C,0x4C,0xC4,0x11}},


{0xd3,  28,  {0x1B,0x3B,0xBB,0xAD,0xA5,0x33,0x33,0x33,0x00,0x80,
              0xAD,0xA8,0x37,0x33,0x33,0x33,0x33,0xF7,0xF2,0x1F,
              0x7D,0x7C,0xFF,0x0F,0x99,0x00,0xFF,0xFF}},
{0xd4,   3,  {0x57,0x33,0x03}},
{0xd5,  11,  {0x66,0x00,0x00,0x01,0x32,0x01,0x32,0x00,0x0B,0x00,0x0B}},

{0xd6,   1,  {0X01}},
{0xbe,   1,  {0X04}},

{0xcf,  16,  {0x40,0x10,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,
              0x00,0x00,0x00,0x00,0x00,0x0}},

{0xDe,  5,  {0x00,0x00,0x3F,0xFF,0x10}},

{0xe9,   1,  {0x00}},
{0xf2,   1,  {0x00}},

{0x29,   1,  {0X00}}, 
{REGFLAG_DELAY, 100, {}},
	
{0x11,   1,  {0X00}},
{REGFLAG_DELAY, 200, {}},

{REGFLAG_END_OF_TABLE, 0x00, {}},
#else
	

{0x29,   0,  {0X00}}, 
{REGFLAG_DELAY, 200, {}},

{0x11,   0,  {0X00}},
{REGFLAG_DELAY, 200, {}},

{REGFLAG_END_OF_TABLE, 0x00, {}},


#endif


};


static struct LCM_setting_table lcm_sleep_in_setting[] =
{
    // Display off sequence
    {0x28, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},
    // Sleep Mode On
    {0x10, 0, {0x00}},
     {REGFLAG_DELAY, 120, {}},

    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] =
{
    // Sleep Out
    {0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 0, {0x00}},
    {REGFLAG_DELAY, 60, {}},

    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd)
        {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
				
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
//                dsi_set_cmdq_dcs(cmd, table[i].count, table[i].para_list, force_update);
        }
    }

}

static void init_lcm_registers(void)
{
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

char LCDTiming[256]={0x04,0x38,0x07,0x80,0x01,0x04,4,12,15,20,70,70,1,1,25};   //FHD 1080*1920
static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		//params->dsi.mode   = SYNC_PULSE_VDO_MODE;
		params->dsi.mode   = BURST_VDO_MODE;
		//params->dsi.mode   = SYNC_EVENT_VDO_MODE; 
		
        #endif
	
		// DSI
		/* Command mode setting */
		//1 Three lane or Four lane
//		params->dsi.LANE_NUM				= LCM_THREE_LANE;
    		params->dsi.LANE_NUM     = LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=FRAME_WIDTH*3;

		
		params->dsi.vertical_sync_active				= 0x2;//
		params->dsi.vertical_backporch					= 8;//=>{0XD3,1,{0X05}} VBP(lcm) = vertical_sync_active + vertical_backporch
		params->dsi.vertical_frontporch					= 8; //=>{0XD4,1,{0X04}} VFP
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.horizontal_sync_active				= 20;// 50  2
		params->dsi.horizontal_backporch				= 100;
		params->dsi.horizontal_frontporch				= 100;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		params->dsi.pll_select=0;	//0: MIPI_PLL; 1: LVDS_PLL

		params->dsi.compatibility_for_nvk=0;
        	params->dsi.HS_PRPR=6;
	    params->dsi.LPX=8; 
		params->dsi.HS_PRPR=5;
		params->dsi.HS_TRAIL=13;
		params->dsi.CLK_TRAIL = 10;
		// Bit rate calculation
		//1 Every lane speed
		params->dsi.pll_div1=0;//0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;//1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =25;// 72; // 58; // 48;//34; // 19;0x12    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

}

unsigned int LCM_ID_SHOW=0;
unsigned int lcm_read_id(void)
{
		unsigned int id = 0;
		unsigned char buffer[5];
		unsigned int array[16];
		SET_RESET_PIN(1);	 
		MDELAY(10); 
		SET_RESET_PIN(0);
		MDELAY(10); 
		SET_RESET_PIN(1);
		MDELAY(120); 
		
	
		array[0] = 0x00053700;// read id return 5 byte
		dsi_set_cmdq(array, 1, 1);
	
		array[0] = 0x04B02300;// unlock for reading ID
		dsi_set_cmdq(array, 1, 1);
		MDELAY(50);
	
		read_reg_v2(0xBF, buffer, 5);
		LCM_ID_SHOW= (buffer[2] << 8) | buffer[3]; //we only need ID
#if defined(BUILD_LK)
		printf("r63417a %s, id1 = 0x%x\n", __func__, id);
#else
		printk("r63417a %s, id1 = 0x%x\n", __func__, id);
#endif
		//return (LCM_ID_R63417 == id)?1:0;
return 1;
}
//********************************************************
void lcm_init(void)
{
mt_set_gpio_mode(131,0);   //lcm reset control
mt_set_gpio_dir(131,1);
mt_set_gpio_out(131,0);   //
MDELAY(10);
mt_set_gpio_mode(140,0);   //ctp reset control
mt_set_gpio_dir(140,1);
mt_set_gpio_out(140,1);   
MDELAY(10);

 mt_set_gpio_mode(126,0);   //vci +iovcc on
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,1);   //
MDELAY(10);
 mt_set_gpio_mode(125,0);   //VSP  on
mt_set_gpio_dir(125,1);
mt_set_gpio_out(125,1); 
 MDELAY(10);
 mt_set_gpio_mode(117,0);   //VSN  on
mt_set_gpio_dir(117,1);
mt_set_gpio_out(117,1);    
 MDELAY(10);


mt_set_gpio_mode(131,0);   //lcm reset control
mt_set_gpio_dir(131,1);
mt_set_gpio_out(131,1);   //
MDELAY(10);


push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);


	//init_lcm_registers();
mt_set_gpio_mode(174,0);   //BL power
mt_set_gpio_dir(174,1);
mt_set_gpio_out(174,1);   

 mt_set_gpio_mode(129,0);   //BL enable
mt_set_gpio_dir(129,1);
mt_set_gpio_out(129,1);   
}


static void lcm_suspend(void)
{
 mt_set_gpio_mode(140,0);   //ctp reset control
mt_set_gpio_dir(140,1);
mt_set_gpio_out(140,1);   
MDELAY(10);
	
push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
/*
mt_set_gpio_mode(131,0);   //lcm reset control
mt_set_gpio_dir(131,1);
mt_set_gpio_out(131,0);   //
MDELAY(10);
mt_set_gpio_mode(117,0);   //VSN  on
mt_set_gpio_dir(117,1);
mt_set_gpio_out(117,0);    
 MDELAY(10);
 mt_set_gpio_mode(125,0);   //VSP  on
mt_set_gpio_dir(125,1);
mt_set_gpio_out(125,0); 
 MDELAY(10);
mt_set_gpio_mode(126,0);   //vci +iovcc on
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,0);   //
MDELAY(10);
*/
}

static unsigned int lcm_compare_id(void);
static void lcm_resume(void)
{	
//lcm_compare_id() ;
lcm_init();
//lcm_compare_id() ;
	//init_lcm_registers();
}
         
#if (LCM_DSI_CMD_MODE)
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

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);

	data_array[0]= 0x00290508; //HW bug, so need send one HS packet
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(&data_array, 1, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{
		unsigned int id = 0;
		unsigned char buffer[5];
		unsigned int array[16];
		SET_RESET_PIN(1);	 
		MDELAY(10); 
		SET_RESET_PIN(0);
		MDELAY(10); 
		SET_RESET_PIN(1);
		MDELAY(120); 
		
	
		array[0] = 0x00053700;// read id return 5 byte
		dsi_set_cmdq(array, 1, 1);
	
		array[0] = 0x04B02300;// unlock for reading ID
		dsi_set_cmdq(array, 1, 1);
		MDELAY(50);
	
		read_reg_v2(0xBF, buffer, 5);
		id = (buffer[2] << 8) | buffer[3]; //we only need ID
#if defined(BUILD_LK)
		printf("r63417a %s, id1 = 0x%x\n", __func__, id);
#else
		printk("r63417a %s, id1 = 0x%x\n", __func__, id);
#endif
		//return (LCM_ID_R63417 == id)?1:0;
return 1;
}

static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
//#if 1
	char  buffer[3];
	int   array[4];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x0a, buffer, 1);
	printk("kevin: lcm_esd_check buffer[0]=0x%x\n",buffer[0]);
	if((buffer[0]==0x9c)||(buffer[0]==0x1c)||(buffer[0]==0x0c))
	{
		return FALSE;
	}
	else
	{	
		return TRUE;
	}
#else
	return FALSE;
#endif
}
static unsigned int lcm_esd_recover(void)
{
#ifndef BUILD_LK	
	printk("kevin: lcm_esd_recover");
#endif
	return TRUE;
}

LCM_DRIVER r69006_fhd_dsi_cmd_lcm_drv = 
{
    .name			= "r69006_fhd_dsi_cmd",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id     = lcm_compare_id,
	//.esd_check = lcm_esd_check,
	//.esd_recover = lcm_esd_recover,		
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
