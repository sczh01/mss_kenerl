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


#ifndef BUILD_LK

#include <mach/mt_typedefs.h>
#include <mach/mt_pm_ldo.h>

#include <mach/mt_boot.h>
#include <cust_eint.h>
#include <cust_gpio_usage.h>
#include <linux/vmalloc.h>  

#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/system.h>  // for SMP

#include <linux/ioctl.h>
#endif



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

//#define FRAME_WIDTH  (1080)
//#define FRAME_HEIGHT (1920)

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)
#define   normal_test      0
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
#define dsi_set_cmdq_V3(ppara, size, force_update)	        	lcm_util.dsi_set_cmdq_V3(ppara, size, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   


#define   LCM_DSI_CMD_MODE							0

static struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] =
{

{0x00, 1 , {0x00}},
{0xFF,  3 ,{0x12,0x83,0x01}},

{0x00, 1 , {0x80}},
{0xFF,  2 ,{0x12,0x83}},

{0x00, 1 , {0x80}},
{0xC0,  9 ,{0x00,0x64,0x00,0x0F,0x11,0x00,0x64,0x0F,0x11}},

{0x00, 1 , {0x90}},
{0xC0,  6 ,{0x00,0x55,0x00,0x01,0x00,0x04}},

{0x00, 1 , {0xA4}},
{0xC0,  1 ,{0x00}},

{0x00, 1 , {0xB3}},
{0xC0,  2 ,{0x00,0x50}},

{0x00, 1 , {0x81}},
{0xC1,  1 ,{0x55}},

{0x00, 1 , {0x81}},
{0xC4,  1 ,{0x82}},

{0x00, 1 , {0x82}},
{0xC4,  1 ,{0x02}},

{0x00, 1 , {0x90}},
{0xC4,  1 ,{0x49}},

{0x00, 1 , {0xC6}},
{0xB0,  1 ,{0x03}},

{0x00, 1 , {0x8B}},
{0xC4,  1 ,{0x40}},

{0x00, 1 , {0x90}},
{0xF5,  4 ,{0x02,0x11,0x02,0x11}},

{0x00, 1 , {0x90}},
{0xC5,  1 ,{0x50}},

{0x00, 1 , {0x94}},
{0xC5,  1 ,{0x66}},

{0x00, 1 , {0xB2}},
{0xF5,  2 ,{0x00,0x00}},

{0x00, 1 , {0xB4}},
{0xF5,  2 ,{0x00,0x00}},

{0x00, 1 , {0xB6}},
{0xF5,  2 ,{0x00,0x00}},

{0x00, 1 , {0xB8}},
{0xF5,  2 ,{0x00,0x00}},

{0x00, 1 , {0x94}},
{0xF5,  1 ,{0x02}},

{0x00, 1 , {0xBA}},
{0xF5,  1 ,{0x03}},

{0x00, 1 , {0xB2}},
{0xC5,  1 ,{0x40}},

{0x00, 1 , {0xB4}},
{0xC5,  1 ,{0xC0}},

{0x00, 1 , {0xA0}},
{0xC4, 14 ,{0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10}},

{0x00, 1 , {0xB0}},
{0xC4,  2 ,{0x00,0x00}},

{0x00, 1 , {0x91}},
{0xC5,  2 ,{0x19,0x50}},

{0x00, 1 , {0x00}},
{0xD8,  2 ,{0xBC,0xBC}},

{0x00, 1 , {0xB0}},
{0xC5,  2 ,{0x04,0xB8}},

{0x00, 1 , {0xBB}},
{0xC5,  1 ,{0x80}},

{0x00, 1 , {0x00}},
{0xD0,  1 ,{0x40}},

{0x00, 1 , {0x00}},
{0xD1,  2 ,{0x00,0x00}},

{0x00, 1 , {0x00}},

{0x00, 1 , {0x00}},

{0x00, 1 , {0x00}},
{0xD9,  1 ,{0x47}},

{0x00, 1 , {0x80}},
{0xCB, 11 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0x90}},
{0xCB, 15 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xA0}},
{0xCB, 15 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xB0}},
{0xCB, 15 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xC0}},
{0xCB, 15 ,{0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xD0}},
{0xCB, 15 ,{0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00}},

{0x00, 1 , {0xE0}},
{0xCB, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05}},

{0x00, 1 , {0xF0}},
{0xCB, 11 ,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},

{0x00, 1 , {0x80}},
{0xCC, 15 ,{0x0A,0x0C,0x0E,0x10,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0x90}},
{0xCC, 15 ,{0x00,0x00,0x00,0x00,0x00,0x2E,0x2D,0x09,0x0B,0x0D,0x0F,0x01,0x03,0x00,0x00}},

{0x00, 1 , {0xA0}},
{0xCC, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2E,0x2D}},

{0x00, 1 , {0xB0}},
{0xCC, 15 ,{0x0F,0x0D,0x0B,0x09,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xC0}},
{0xCC, 15 ,{0x00,0x00,0x00,0x00,0x00,0x2D,0x2E,0x10,0x0E,0x0C,0x0A,0x04,0x02,0x00,0x00}},

{0x00, 1 , {0xD0}},
{0xCC, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2D,0x2E}},

{0x00, 1 , {0x80}},
{0xCE, 12 ,{0x8D,0x03,0x00,0x8C,0x03,0x00,0x8B,0x03,0x00,0x8A,0x03,0x00}},

{0x00, 1 , {0xA0}},
{0xCE, 14 ,{0x38,0x0B,0x04,0xFC,0x00,0x10,0x10,0x38,0x0A,0x04,0xFD,0x00,0x10,0x10}},

{0x00, 1 , {0xB0}},
{0xCE, 14 ,{0x38,0x09,0x04,0xFE,0x00,0x10,0x10,0x38,0x08,0x04,0xFF,0x00,0x10,0x10}},

{0x00, 1 , {0xC0}},
{0xCE, 14 ,{0x38,0x07,0x05,0x00,0x00,0x10,0x10,0x38,0x06,0x05,0x01,0x00,0x10,0x10}},

{0x00, 1 , {0xD0}},
{0xCE, 14 ,{0x38,0x05,0x05,0x02,0x00,0x10,0x10,0x38,0x04,0x05,0x03,0x00,0x10,0x10}},

{0x00, 1 , {0x80}},
{0xCF, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0x90}},
{0xCF, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xA0}},
{0xCF, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xB0}},
{0xCF, 14 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00, 1 , {0xC0}},
{0xCF, 11 ,{0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0x08}},

{0x00, 1 , {0xB5}},
{0xC5,  6 ,{0x33,0xF1,0xFF,0x33,0xF1,0xFF}},


{0x00, 1 , {0x00}},
{0x35,  1 ,{0x00}},

{0x00, 1 , {0xa0}},
{0xc1,  1 ,{0x02}},

//{0x00, 1 , {0x00}},
//{0xFF,  3 ,{0xFF,0xFF,0xFF}},	   
	{0x11,  1 , {0x00}},//Sleep out
	{REGFLAG_DELAY, 200, {}},                                    

	   
	{0x29,  1 , {0x00}},//Display on
	{REGFLAG_DELAY, 60, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
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


static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
//		params->dsi.mode   = BURST_VDO_MODE;
//		params->dsi.mode   = SYNC_EVENT_VDO_MODE; 
		
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
		//params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		//params->dsi.word_count=720*3;	

		
		params->dsi.vertical_sync_active				= 4;    
		params->dsi.vertical_backporch					= 16;// 
		params->dsi.vertical_frontporch					= 16; // 4//20
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				=10;
		params->dsi.horizontal_backporch				=50;
		params->dsi.horizontal_frontporch				=50;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.pll_select=0;	//0: MIPI_PLL; 1: LVDS_PLL

		//params->dsi.compatibility_for_nvk=1;
        //	params->dsi.HS_PRPR=6;
//	    params->dsi.LPX=8; 
		//params->dsi.HS_PRPR=5;
		//params->dsi.HS_TRAIL=13;
//		params->dsi.CLK_TRAIL = 10;
		// Bit rate calculation
		//1 Every lane speed
		params->dsi.pll_div1=1;//0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;//1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =31;// 72; // 58; // 48;//34; // 19;0x12    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

	params->dsi.noncont_clock = TRUE;
		params->dsi.noncont_clock_period = 2;	// Unit : frames

}
static unsigned int lcm_compare_id(void);
static void lcm_init(void)
{
mt_set_gpio_mode(126,0);
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,1);  //iovcc  vcc  power on
MDELAY(10);
#ifndef BUILD_LK	
	printk("kevin test lcm_init\n");
#endif
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(5);
	
	SET_RESET_PIN(1);
	MDELAY(120);    
push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	//init_lcm_registers();
mt_set_gpio_mode(129,0);   //BL on
mt_set_gpio_dir(129,1);
mt_set_gpio_out(129,1);
}


static void lcm_suspend(void)
{
mt_set_gpio_mode(129,0);   //BL on
mt_set_gpio_dir(129,1);
mt_set_gpio_out(129,0);	

	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(5);
	
	SET_RESET_PIN(1);
	MDELAY(120); 
    push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);

mt_set_gpio_mode(126,0);
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,0);  //iovcc  vcc  power on
MDELAY(50);

}


static void lcm_resume(void)
{
 #ifndef BUILD_LK	
	printk("kevin test lcm_resume\n");
#endif


lcm_init();
//lcm_compare_id();  
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
	unsigned char lcd_id = 0;
char  buffer[5];
char  buffer1[2];
	int   array[4];

	array[0] = 0x00053700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xa1, buffer, 5);

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x0a, buffer1, 1);

#ifdef BUILD_LK
	printf("thl_ykl:%s, LK nt35590 debug: nt35590 id = 0x%08x\n", __func__, lcd_id);
#else
	printk("kevin:%s, kernel otm1283_hsd50_dsi_vdo debug:  0xa1_2 = 0x%08x\n", __func__, buffer[2]);
printk("kevin:%s, kernel otm1283_hsd50_dsi_vdo: 0xa1_3 = 0x%08x\n", __func__, buffer[3]);
printk("kevin:%s, kernel otm1283_hsd50_dsi_vdo: 0x0a = 0x%08x\n", __func__, buffer1[0]);
#endif

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
	if((buffer[0]==0x1c)||(buffer[0]==0x9c)||(buffer[0]==0x0c))
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
extern int lcm_debug_online(void);

static unsigned int lcm_esd_recover(void)
{
#ifndef BUILD_LK	
	printk("kevin test lcm_esd_recover\n");

	lcm_debug_online();


#endif
	return TRUE;
}
//********************************************

LCM_DRIVER otm1283_boe50_dsi_vdo_lcm_drv = 
{
    .name			= "otm1283_boe50_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.esd_check = lcm_esd_check,
	.esd_recover = lcm_esd_recover,		
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
