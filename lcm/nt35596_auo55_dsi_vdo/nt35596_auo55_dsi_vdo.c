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

//#define FRAME_WIDTH  (1080)
//#define FRAME_HEIGHT (1920)

#define FRAME_WIDTH  (1080)
#define FRAME_HEIGHT (1920)

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
static struct LCM_setting_table lcm_initialization_setting[] = {

{0xff,1,{0xee}},
{0xfb,1,{0x01}},
{0x18,1,{0x40}},

    {REGFLAG_DELAY, 20, {0}}, 

{0x18,1,{0x00}},

    {REGFLAG_DELAY, 20, {0}}, 

{0xFF,1,{0x05}},	
 {0xFB,1,{0x01}},	
 {0xC5,1,{0x31}},	//Turn On NT50198

    {REGFLAG_DELAY, 20, {0}}, 

 {0xFF,1,{0xEE}},	
 {0xFB,1,{0x01}},	
 {0x24,1,{0x4F}},	
 {0x38,1,{0xC8}},	
 {0x39,1,{0x27}},	//Modified VP_HSSI Voltage
 {0x1E,1,{0x77}},	//Modified VDD Voltage
 {0x1D,1,{0x0F}},	
 {0x7E,1,{0x71}},	//Modified VDD Voltage
 {0x7C,1,{0x03}},	
 {0xFF,1,{0x01}},	
 {0xFB,1,{0x01}},	
 {0x00,1,{0x01}},	
 {0x01,1,{0x55}},	
 {0x02,1,{0x40}},	
 {0x05,1,{0x40}},	
 {0x06,1,{0x4A}},	//Modified VGH Level
 {0x07,1,{0x24}},	
 {0x08,1,{0x0C}},	
 {0x0B,1,{0x7D}},	
 {0x0C,1,{0x7D}},	
 {0x0E,1,{0xB0}},	
 {0x0F,1,{0xAE}},	//Set VGLO = -8.0 V
 {0x11,1,{0x10}},	//VCOM DC,no need to issue for OTP LCM
 {0x12,1,{0x10}},	
 {0x13,1,{0x03}},	//VCOM for  Forward and Backward Scan
 {0x14,1,{0x4A}},	
 {0x15,1,{0x12}},	//Set AVDDR = 4.7 V
 {0x16,1,{0x12}},	//Set AVEER = - 4.7 V
 {0x18,1,{0x00}},	
 {0x19,1,{0x77}},	
 {0x1A,1,{0x55}},	
 {0x1B,1,{0x13}},	
 {0x1C,1,{0x00}},	
 {0x1D,1,{0x00}},	
 {0x1E,1,{0x13}},	
 {0x1F,1,{0x00}},	
 {0x23,1,{0x00}},	
 {0x24,1,{0x00}},	
 {0x25,1,{0x00}},	
 {0x26,1,{0x00}},	
 {0x27,1,{0x00}},	
 {0x28,1,{0x00}},	
 {0x35,1,{0x00}},	
 {0x66,1,{0x00}},	
 {0x58,1,{0x82}},	
 {0x59,1,{0x02}},	
 {0x5A,1,{0x02}},	
 {0x5B,1,{0x02}},	
 {0x5C,1,{0x82}},	
 {0x5D,1,{0x82}},	
 {0x5E,1,{0x02}},	
 {0x5F,1,{0x02}},	
 {0x72,1,{0x31}},	
 {0xFF,1,{0x05}},	
 {0xFB,1,{0x01}},	
 {0x00,1,{0x01}},	
 {0x01,1,{0x0B}},	
 {0x02,1,{0x0C}},	
 {0x03,1,{0x09}},	
 {0x04,1,{0x0A}},	
 {0x05,1,{0x00}},	
 {0x06,1,{0x0F}},	
 {0x07,1,{0x10}},	
 {0x08,1,{0x00}},	
 {0x09,1,{0x00}},	
 {0x0A,1,{0x00}},	
 {0x0B,1,{0x00}},	
 {0x0C,1,{0x00}},	
 {0x0D,1,{0x13}},	
 {0x0E,1,{0x15}},	
 {0x0F,1,{0x17}},	
 {0x10,1,{0x01}},	
 {0x11,1,{0x0B}},	
 {0x12,1,{0x0C}},	
 {0x13,1,{0x09}},	
 {0x14,1,{0x0A}},	
 {0x15,1,{0x00}},	
 {0x16,1,{0x0F}},	
 {0x17,1,{0x10}},	
 {0x18,1,{0x00}},	
 {0x19,1,{0x00}},	
 {0x1A,1,{0x00}},	
 {0x1B,1,{0x00}},	
 {0x1C,1,{0x00}},	
 {0x1D,1,{0x13}},	
 {0x1E,1,{0x15}},	
 {0x1F,1,{0x17}},	
 {0x20,1,{0x00}},	
 {0x21,1,{0x03}},	
 {0x22,1,{0x01}},	
 {0x23,1,{0x40}},	
 {0x24,1,{0x40}},	
 {0x25,1,{0xED}},	
 {0x29,1,{0x58}},	
 {0x2A,1,{0x12}},	
 {0x2B,1,{0x01}},	
 {0x4B,1,{0x06}},	
 {0x4C,1,{0x11}},	
 {0x4D,1,{0x20}},	
 {0x4E,1,{0x02}},	
 {0x4F,1,{0x02}},	
 {0x50,1,{0x20}},	
 {0x51,1,{0x61}},	
 {0x52,1,{0x01}},	
 {0x53,1,{0x63}},	
 {0x54,1,{0x77}},	
 {0x55,1,{0xED}},	
 {0x5B,1,{0x00}},	
 {0x5C,1,{0x00}},	
 {0x5D,1,{0x00}},	
 {0x5E,1,{0x00}},	
 {0x5F,1,{0x15}},	
 {0x60,1,{0x75}},	
 {0x61,1,{0x00}},	
 {0x62,1,{0x00}},	
 {0x63,1,{0x00}},	
 {0x64,1,{0x00}},	
 {0x65,1,{0x00}},	
 {0x66,1,{0x00}},	
 {0x67,1,{0x00}},	
 {0x68,1,{0x04}},	
 {0x69,1,{0x00}},	
 {0x6A,1,{0x00}},	
 {0x6C,1,{0x40}},	
 {0x75,1,{0x01}},	
 {0x76,1,{0x01}},	
 {0x7A,1,{0x80}},	
 {0x7B,1,{0xA3}},	//Modified VBP, VFP Dummy Lines
 {0x7C,1,{0xD8}},	
 {0x7D,1,{0x60}},	
 {0x7F,1,{0x15}},	//Modified MUX Pulse Width
 {0x80,1,{0x81}},	
 {0x83,1,{0x05}},	
 {0x93,1,{0x08}},	
 {0x94,1,{0x10}},	
 {0x8A,1,{0x00}},	
 {0x9B,1,{0x0F}},	
 {0xFF,1,{0x01}},	
 {0xFB,1,{0x01}},	
 {0x75,1,{0x01}},	
 {0x76,1,{0x6A}},	
 {0x77,1,{0x01}},	
 {0x78,1,{0x6C}},	
 {0x79,1,{0x01}},	
 {0x7A,1,{0x71}},	
 {0x7B,1,{0x01}},	
 {0x7C,1,{0x75}},	
 {0x7D,1,{0x01}},	
 {0x7E,1,{0x7A}},	
 {0x7F,1,{0x01}},	
 {0x80,1,{0x7F}},	
 {0x81,1,{0x01}},	
 {0x82,1,{0x83}},	
 {0x83,1,{0x01}},	
 {0x84,1,{0x88}},	
 {0x85,1,{0x01}},	
 {0x86,1,{0x8C}},	
 {0x87,1,{0x01}},	
 {0x88,1,{0x9D}},	
 {0x89,1,{0x01}},	
 {0x8A,1,{0xAD}},	
 {0x8B,1,{0x01}},	
 {0x8C,1,{0xCA}},	
 {0x8D,1,{0x01}},	
 {0x8E,1,{0xE5}},	
 {0x8F,1,{0x02}},	
 {0x90,1,{0x14}},	
 {0x91,1,{0x02}},	
 {0x92,1,{0x40}},	
 {0x93,1,{0x02}},	
 {0x94,1,{0x41}},	
 {0x95,1,{0x02}},	
 {0x96,1,{0x6C}},	
 {0x97,1,{0x02}},	
 {0x98,1,{0xA0}},	
 {0x99,1,{0x02}},	
 {0x9A,1,{0xC4}},	
 {0x9B,1,{0x02}},	
 {0x9C,1,{0xF8}},	
 {0x9D,1,{0x03}},	
 {0x9E,1,{0x1E}},	
 {0x9F,1,{0x03}},	
 {0xA0,1,{0x65}},	
 {0xA2,1,{0x03}},	
 {0xA3,1,{0x7F}},	
 {0xA4,1,{0x03}},	
 {0xA5,1,{0x89}},	
 {0xA6,1,{0x03}},	
 {0xA7,1,{0x8F}},	
 {0xA9,1,{0x03}},	
 {0xAA,1,{0x93}},	
 {0xAB,1,{0x03}},	
 {0xAC,1,{0x9F}},	
 {0xAD,1,{0x03}},	
 {0xAE,1,{0xA7}},	
 {0xAF,1,{0x03}},	
 {0xB0,1,{0xB8}},	
 {0xB1,1,{0x03}},	
 {0xB2,1,{0xCB}},	
 {0xB3,1,{0x01}},	
 {0xB4,1,{0x6A}},	
 {0xB5,1,{0x01}},	
 {0xB6,1,{0x6C}},	
 {0xB7,1,{0x01}},	
 {0xB8,1,{0x71}},	
 {0xB9,1,{0x01}},	
 {0xBA,1,{0x75}},	
 {0xBB,1,{0x01}},	
 {0xBC,1,{0x7A}},	
 {0xBD,1,{0x01}},	
 {0xBE,1,{0x7F}},	
 {0xBF,1,{0x01}},	
 {0xC0,1,{0x83}},	
 {0xC1,1,{0x01}},	
 {0xC2,1,{0x88}},	
 {0xC3,1,{0x01}},	
 {0xC4,1,{0x8C}},	
 {0xC5,1,{0x01}},	
 {0xC6,1,{0x9D}},	
 {0xC7,1,{0x01}},	
 {0xC8,1,{0xAD}},	
 {0xC9,1,{0x01}},	
 {0xCA,1,{0xCA}},	
 {0xCB,1,{0x01}},	
 {0xCC,1,{0xE5}},	
 {0xCD,1,{0x02}},	
 {0xCE,1,{0x14}},	
 {0xCF,1,{0x02}},	
 {0xD0,1,{0x40}},	
 {0xD1,1,{0x02}},	
 {0xD2,1,{0x41}},	
 {0xD3,1,{0x02}},	
 {0xD4,1,{0x6C}},	
 {0xD5,1,{0x02}},	
 {0xD6,1,{0xA0}},	
 {0xD7,1,{0x02}},	
 {0xD8,1,{0xC4}},	
 {0xD9,1,{0x02}},	
 {0xDA,1,{0xF8}},	
 {0xDB,1,{0x03}},	
 {0xDC,1,{0x1E}},	
 {0xDD,1,{0x03}},	
 {0xDE,1,{0x65}},	
 {0xDF,1,{0x03}},	
 {0xE0,1,{0x7F}},	
 {0xE1,1,{0x03}},	
 {0xE2,1,{0x89}},	
 {0xE3,1,{0x03}},	
 {0xE4,1,{0x8F}},	
 {0xE5,1,{0x03}},	
 {0xE6,1,{0x93}},	
 {0xE7,1,{0x03}},	
 {0xE8,1,{0x9F}},	
 {0xE9,1,{0x03}},	
 {0xEA,1,{0xA7}},	
 {0xEB,1,{0x03}},	
 {0xEC,1,{0xB8}},	
 {0xED,1,{0x03}},	
 {0xEE,1,{0xCB}},	
 {0xEF,1,{0x01}},	
 {0xF0,1,{0x0B}},	
 {0xF1,1,{0x01}},	
 {0xF2,1,{0x0F}},	
 {0xF3,1,{0x01}},	
 {0xF4,1,{0x16}},	
 {0xF5,1,{0x01}},	
 {0xF6,1,{0x1D}},	
 {0xF7,1,{0x01}},	
 {0xF8,1,{0x25}},	
 {0xF9,1,{0x01}},	
 {0xFA,1,{0x2C}},	
 {0xFF,1,{0x02}},	
 {0xFB,1,{0x01}},	
 {0x00,1,{0x01}},	
 {0x01,1,{0x33}},	
 {0x02,1,{0x01}},	
 {0x03,1,{0x39}},	
 {0x04,1,{0x01}},	
 {0x05,1,{0x40}},	
 {0x06,1,{0x01}},	
 {0x07,1,{0x59}},	
 {0x08,1,{0x01}},	
 {0x09,1,{0x6F}},	
 {0x0A,1,{0x01}},	
 {0x0B,1,{0x98}},	
 {0x0C,1,{0x01}},	
 {0x0D,1,{0xBB}},	
 {0x0E,1,{0x01}},	
 {0x0F,1,{0xF6}},	
 {0x10,1,{0x02}},	
 {0x11,1,{0x29}},	
 {0x12,1,{0x02}},	
 {0x13,1,{0x2A}},	
 {0x14,1,{0x02}},	
 {0x15,1,{0x5A}},	
 {0x16,1,{0x02}},	
 {0x17,1,{0x8F}},	
 {0x18,1,{0x02}},	
 {0x19,1,{0xB2}},	
 {0x1A,1,{0x02}},	
 {0x1B,1,{0xE2}},	
 {0x1C,1,{0x03}},	
 {0x1D,1,{0x02}},	
 {0x1E,1,{0x03}},	
 {0x1F,1,{0x2E}},	
 {0x20,1,{0x03}},	
 {0x21,1,{0x3A}},	
 {0x22,1,{0x03}},	
 {0x23,1,{0x48}},	
 {0x24,1,{0x03}},	
 {0x25,1,{0x58}},	
 {0x26,1,{0x03}},	
 {0x27,1,{0x68}},	
 {0x28,1,{0x03}},	
 {0x29,1,{0x7A}},	
 {0x2A,1,{0x03}},	
 {0x2B,1,{0x8B}},	
 {0x2D,1,{0x03}},	
 {0x2F,1,{0x98}},	
 {0x30,1,{0x03}},	
 {0x31,1,{0xCB}},	
 {0x32,1,{0x01}},	
 {0x33,1,{0x0B}},	
 {0x34,1,{0x01}},	
 {0x35,1,{0x0F}},	
 {0x36,1,{0x01}},	
 {0x37,1,{0x16}},	
 {0x38,1,{0x01}},	
 {0x39,1,{0x1D}},	
 {0x3A,1,{0x01}},	
 {0x3B,1,{0x25}},	
 {0x3D,1,{0x01}},	
 {0x3F,1,{0x2C}},	
 {0x40,1,{0x01}},	
 {0x41,1,{0x33}},	
 {0x42,1,{0x01}},	
 {0x43,1,{0x39}},	
 {0x44,1,{0x01}},	
 {0x45,1,{0x40}},	
 {0x46,1,{0x01}},	
 {0x47,1,{0x59}},	
 {0x48,1,{0x01}},	
 {0x49,1,{0x6F}},	
 {0x4A,1,{0x01}},	
 {0x4B,1,{0x98}},	
 {0x4C,1,{0x01}},	
 {0x4D,1,{0xBB}},	
 {0x4E,1,{0x01}},	
 {0x4F,1,{0xF6}},	
 {0x50,1,{0x02}},	
 {0x51,1,{0x29}},	
 {0x52,1,{0x02}},	
 {0x53,1,{0x2A}},	
 {0x54,1,{0x02}},	
 {0x55,1,{0x5A}},	
 {0x56,1,{0x02}},	
 {0x58,1,{0x8F}},	
 {0x59,1,{0x02}},	
 {0x5A,1,{0xB2}},	
 {0x5B,1,{0x02}},	
 {0x5C,1,{0xE2}},	
 {0x5D,1,{0x03}},	
 {0x5E,1,{0x02}},	
 {0x5F,1,{0x03}},	
 {0x60,1,{0x2E}},	
 {0x61,1,{0x03}},	
 {0x62,1,{0x3A}},	
 {0x63,1,{0x03}},	
 {0x64,1,{0x48}},	
 {0x65,1,{0x03}},	
 {0x66,1,{0x58}},	
 {0x67,1,{0x03}},	
 {0x68,1,{0x68}},	
 {0x69,1,{0x03}},	
 {0x6A,1,{0x7A}},	
 {0x6B,1,{0x03}},	
 {0x6C,1,{0x8B}},	
 {0x6D,1,{0x03}},	
 {0x6E,1,{0x98}},	
 {0x6F,1,{0x03}},	
 {0x70,1,{0xCB}},	
 {0x71,1,{0x00}},	
 {0x72,1,{0x00}},	
 {0x73,1,{0x00}},	
 {0x74,1,{0x0A}},	
 {0x75,1,{0x00}},	
 {0x76,1,{0x2A}},	
 {0x77,1,{0x00}},	
 {0x78,1,{0x41}},	
 {0x79,1,{0x00}},	
 {0x7A,1,{0x56}},	
 {0x7B,1,{0x00}},	
 {0x7C,1,{0x69}},	
 {0x7D,1,{0x00}},	
 {0x7E,1,{0x7A}},	
 {0x7F,1,{0x00}},	
 {0x80,1,{0x8A}},	
 {0x81,1,{0x00}},	
 {0x82,1,{0x98}},	
 {0x83,1,{0x00}},	
 {0x84,1,{0xCB}},	
 {0x85,1,{0x00}},	
 {0x86,1,{0xF5}},	
 {0x87,1,{0x01}},	
 {0x88,1,{0x38}},	
 {0x89,1,{0x01}},	
 {0x8A,1,{0x6F}},	
 {0x8B,1,{0x01}},	
 {0x8C,1,{0xC5}},	
 {0x8D,1,{0x02}},	
 {0x8E,1,{0x09}},	
 {0x8F,1,{0x02}},	
 {0x90,1,{0x0A}},	
 {0x91,1,{0x02}},	
 {0x92,1,{0x43}},	
 {0x93,1,{0x02}},	
 {0x94,1,{0x7E}},	
 {0x95,1,{0x02}},	
 {0x96,1,{0xA3}},	
 {0x97,1,{0x02}},	
 {0x98,1,{0xD7}},	
 {0x99,1,{0x02}},	
 {0x9A,1,{0xFD}},	
 {0x9B,1,{0x03}},	
 {0x9C,1,{0x39}},	
 {0x9D,1,{0x03}},	
 {0x9E,1,{0x53}},	
 {0x9F,1,{0x03}},	
 {0xA0,1,{0x85}},	
 {0xA2,1,{0x03}},	
 {0xA3,1,{0xA1}},	
 {0xA4,1,{0x03}},	
 {0xA5,1,{0xBF}},	
 {0xA6,1,{0x03}},	
 {0xA7,1,{0xC7}},	
 {0xA9,1,{0x03}},	
 {0xAA,1,{0xC9}},	
 {0xAB,1,{0x03}},	
 {0xAC,1,{0xCA}},	
 {0xAD,1,{0x03}},	
 {0xAE,1,{0xCB}},	
 {0xAF,1,{0x00}},	
 {0xB0,1,{0x00}},	
 {0xB1,1,{0x00}},	
 {0xB2,1,{0x0A}},	
 {0xB3,1,{0x00}},	
 {0xB4,1,{0x2A}},	
 {0xB5,1,{0x00}},	
 {0xB6,1,{0x41}},	
 {0xB7,1,{0x00}},	
 {0xB8,1,{0x56}},	
 {0xB9,1,{0x00}},	
 {0xBA,1,{0x69}},	
 {0xBB,1,{0x00}},	
 {0xBC,1,{0x7A}},	
 {0xBD,1,{0x00}},	
 {0xBE,1,{0x8A}},	
 {0xBF,1,{0x00}},	
 {0xC0,1,{0x98}},	
 {0xC1,1,{0x00}},	
 {0xC2,1,{0xCB}},	
 {0xC3,1,{0x00}},	
 {0xC4,1,{0xF5}},	
 {0xC5,1,{0x01}},	
 {0xC6,1,{0x38}},	
 {0xC7,1,{0x01}},	
 {0xC8,1,{0x6F}},	
 {0xC9,1,{0x01}},	
 {0xCA,1,{0xC5}},	
 {0xCB,1,{0x02}},	
 {0xCC,1,{0x09}},	
 {0xCD,1,{0x02}},	
 {0xCE,1,{0x0A}},	
 {0xCF,1,{0x02}},	
 {0xD0,1,{0x43}},	
 {0xD1,1,{0x02}},	
 {0xD2,1,{0x7E}},	
 {0xD3,1,{0x02}},	
 {0xD4,1,{0xA3}},	
 {0xD5,1,{0x02}},	
 {0xD6,1,{0xD7}},	
 {0xD7,1,{0x02}},	
 {0xD8,1,{0xFD}},	
 {0xD9,1,{0x03}},	
 {0xDA,1,{0x39}},	
 {0xDB,1,{0x03}},	
 {0xDC,1,{0x53}},	
 {0xDD,1,{0x03}},	
 {0xDE,1,{0x85}},	
 {0xDF,1,{0x03}},	
 {0xE0,1,{0xA1}},	
 {0xE1,1,{0x03}},	
 {0xE2,1,{0xBF}},	
 {0xE3,1,{0x03}},	
 {0xE4,1,{0xC7}},	
 {0xE5,1,{0x03}},	
 {0xE6,1,{0xC9}},	
 {0xE7,1,{0x03}},	
 {0xE8,1,{0xCA}},	
 {0xE9,1,{0x03}},	
 {0xEA,1,{0xCB}},	
 {0xFF,1,{0x01}},	
 {0xFB,1,{0x01}},	
 {0xFF,1,{0x02}},	
 {0xFB,1,{0x01}},	
 {0xFF,1,{0x04}},	
 {0xFB,1,{0x01}},	
 {0xFF,1,{0x00}},	
 {0xD3,1,{0x05}},	//VBP:  3  Vsync_low: 2   VFP: 4
 {0xD4,1,{0x04}},	


    {0x11,0,{0x00}}, 
    {REGFLAG_DELAY, 120, {0}}, 
    {0X29,0,{0X00}},     
    {REGFLAG_DELAY, 120, {0}}, 
    {REGFLAG_END_OF_TABLE, 0x00, {0}} ,
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
unsigned int data_array[16];

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
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = BURST_VDO_MODE;
		//params->dsi.mode   = BURST_VDO_MODE;
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
		params->dsi.packet_size=256;
		params->dsi.compatibility_for_nvk=0;
		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=1080*3;	
		
	params->dsi.vertical_sync_active	= 1;
	params->dsi.vertical_backporch		= 4;
	params->dsi.vertical_frontporch		= 3;
	params->dsi.vertical_active_line	= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active	= 3;
	params->dsi.horizontal_backporch	= 60;
	params->dsi.horizontal_frontporch	= 94;
	params->dsi.horizontal_active_pixel	= FRAME_WIDTH;


    params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
    params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4	

   params->dsi.fbk_div =23;   //20 // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)

}
static unsigned int lcm_compare_id(void);
static void lcm_init(void)
{

mt_set_gpio_mode(126,0);
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,1);  //iovcc  vcc  power on
MDELAY(50);
//lcm_compare_id();
	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50); 
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50);    
	//dsi_set_cmdq_V3(lcm_initialization_setting_V3, sizeof(lcm_initialization_setting_V3) / sizeof(LCM_setting_table_V3), 1); 
push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
//init_lcm_registers();
//lcm_compare_id();
mt_set_gpio_mode(129,0);   //BL on
mt_set_gpio_dir(129,1);
mt_set_gpio_out(129,1);
}


static void lcm_suspend(void)
{
 mt_set_gpio_mode(129,0);   //BL on
mt_set_gpio_dir(129,1);
mt_set_gpio_out(129,0);
    //push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
   SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120); 	
mt_set_gpio_mode(126,0);
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,0);  //iovcc  vcc  power on
MDELAY(50);
}


static void lcm_resume(void)
{	 
lcm_init();
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
//********************************************************************************************************
//******1: you can read register as same as  below sentence
//******2: we advice you to  call  "lcm_compare_id"  in  "lcm_resume"
//******3: use adb tool to print: adb shell "cat /proc/kmsg |grep  XXX(your signed) "
//********************************************************************************************************
static unsigned int lcm_compare_id(void)
{char  buffer[3];
char  buffer1[3];
char  buffer2[32];
char   i;
int   array[4];

/*
	array[0] = 0x00043902;
	array[1] = 0x9483FFB9;
	dsi_set_cmdq(&array, 2, 1);
	MDELAY(10);

	array[0] = 0x00033902;                         
	array[1] = 0x008373BA;
	dsi_set_cmdq(&array, 2, 1);
	MDELAY(5);

	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);  //dsi_set_cmdq(array, 3, 1);
	read_reg_v2(0x04, buffer, 3);
read_reg_v2(0x0a, buffer1, 1);

#ifdef BUILD_LK
	printf("%s, LK hx8394d id = 0x%x%x%x\n", __func__, buffer[0],buffer[1],buffer[2]);
#else
	printk("kevin: hx8394d id 0x04= 0x%x%x%x\n",buffer[0],buffer[1],buffer[2]);
printk("kevin: hx8394d 0x0a = 0x%x\n",buffer1[0]);

#endif
*/
		return 1;	
}


//********************************************************************************************************
//******1:pls read 0x0a reg to get success value
//******2:we make "DSP on " bit to be false, in order to enter ESD recover
//********************************************************************************************************
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
//********************************************************************************************************
LCM_DRIVER nt35596_auo55_dsi_vdo_lcm_drv = 
{
    .name			= "nt35596_auo55_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	//.esd_check = lcm_esd_check,
	//.esd_recover = lcm_esd_recover,		
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };


