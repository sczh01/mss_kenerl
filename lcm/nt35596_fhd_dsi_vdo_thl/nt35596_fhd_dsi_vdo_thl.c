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


#define   LCM_DSI_CMD_MODE							0

static struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] =
{
	{0xFF ,1,{	0x05}},
	{0xFB ,1,{	0x01}},
	{0xC5 ,1,{	0x31}},
	
	{0xFF ,1,{	0xEE}},
	{0xFB ,1,{	0x01}},
	{0x24 ,1,{	0x4F}},
	{0x38 ,1,{	0xC8}},
	{0x39 ,1,{	0x27}},
	{0x1E ,1,{	0x77}},
	{0x1D ,1,{	0x0F}},
	{0x7E ,1,{	0x71}},

	{0xFF ,1,{	0x01}},
	{0xFB ,1,{	0x01}},
	{0x00 ,1,{	0x01}},
	{0x01 ,1,{	0x55}},
	{0x02 ,1,{	0x40}},
	{0x05 ,1,{	0x50}},
	{0x06 ,1,{	0x4A}},
	{0x07 ,1,{	0x24}},
	{0x08 ,1,{	0x0C}},
	{0x0B ,1,{	0x87}},
	{0x0C ,1,{	0x87}},
	{0x0E ,1,{	0xB0}},
	{0x0F ,1,{	0xB3}},
	{0x11 ,1,{	0x10}},
	{0x12 ,1,{	0x10}},
	{0x13 ,1,{	0x03}},
	{0x14 ,1,{	0x4A}},
	{0x15 ,1,{	0x12}},
	{0x16 ,1,{	0x12}},
	{0x18 ,1,{	0x00}},
	{0x19 ,1,{	0x77}},
	{0x1A ,1,{	0x55}},
	{0x1B ,1,{	0x13}},
	{0x1C ,1,{	0x00}},
	{0x1D ,1,{	0x00}},
	{0x1E ,1,{	0x13}},
	{0x1F ,1,{	0x00}},
	{0x23 ,1,{	0x00}},
	{0x24 ,1,{	0x00}},
	{0x25 ,1,{	0x00}},
	{0x26 ,1,{	0x00}},
	{0x27 ,1,{	0x00}},
	{0x28 ,1,{	0x00}},
	{0x35 ,1,{	0x00}},
	{0x66 ,1,{	0x00}},
	{0x58 ,1,{	0x82}},
	{0x59 ,1,{	0x02}},
	{0x5A ,1,{	0x02}},
	{0x5B ,1,{	0x02}},
	{0x5C ,1,{	0x82}},
	{0x5D ,1,{	0x82}},
	{0x5E ,1,{	0x02}},
	{0x5F ,1,{	0x02}},
	{0x72 ,1,{	0x31}},

	{0xFF ,1,{	0x05}},
	{0xFB ,1,{	0x01}},
	{0x00 ,1,{	0x01}},
	{0x01 ,1,{	0x0B}},
	{0x02 ,1,{	0x0C}},
	{0x03 ,1,{	0x09}},
	{0x04 ,1,{	0x0A}},
	{0x05 ,1,{	0x00}},
	{0x06 ,1,{	0x0F}},
	{0x07 ,1,{	0x10}},
	{0x08 ,1,{	0x00}},
	{0x09 ,1,{	0x00}},
	{0x0A ,1,{	0x00}},
	{0x0B ,1,{	0x00}},
	{0x0C ,1,{	0x00}},
	{0x0D ,1,{	0x13}},
	{0x0E ,1,{	0x15}},
	{0x0F ,1,{	0x17}},
	{0x10 ,1,{	0x01}},
	{0x11 ,1,{	0x0B}},
	{0x12 ,1,{	0x0C}},
	{0x13 ,1,{	0x09}},
	{0x14 ,1,{	0x0A}},
	{0x15 ,1,{	0x00}},
	{0x16 ,1,{	0x0F}},
	{0x17 ,1,{	0x10}},
	{0x18 ,1,{	0x00}},
	{0x19 ,1,{	0x00}},
	{0x1A ,1,{	0x00}},
	{0x1B ,1,{	0x00}},
	{0x1C ,1,{	0x00}},
	{0x1D ,1,{	0x13}},
	{0x1E ,1,{	0x15}},
	{0x1F ,1,{	0x17}},
	{0x20 ,1,{	0x00}},
	{0x21 ,1,{	0x03}},
	{0x22 ,1,{	0x01}},
	{0x23 ,1,{	0x40}},
	{0x24 ,1,{	0x40}},
	{0x25 ,1,{	0xED}},
	{0x29 ,1,{	0x58}},
	{0x2A ,1,{	0x12}},
	{0x2B ,1,{	0x01}},
	{0x4B ,1,{	0x06}},
	{0x4C ,1,{	0x11}},
	{0x4D ,1,{	0x20}},
	{0x4E ,1,{	0x02}},
	{0x4F ,1,{	0x02}},
	{0x50 ,1,{	0x20}},
	{0x51 ,1,{	0x61}},
	{0x52 ,1,{	0x01}},
	{0x53 ,1,{	0x63}},
	{0x54 ,1,{	0x77}},
	{0x55 ,1,{	0xED}},
	{0x5B ,1,{	0x00}},
	{0x5C ,1,{	0x00}},
	{0x5D ,1,{	0x00}},
	{0x5E ,1,{	0x00}},
	{0x5F ,1,{	0x15}},
	{0x60 ,1,{	0x75}},
	{0x61 ,1,{	0x00}},
	{0x62 ,1,{	0x00}},
	{0x63 ,1,{	0x00}},
	{0x64 ,1,{	0x00}},
	{0x65 ,1,{	0x00}},
	{0x66 ,1,{	0x00}},
	{0x67 ,1,{	0x00}},
	{0x68 ,1,{	0x04}},
	{0x69 ,1,{	0x00}},
	{0x6A ,1,{	0x00}},
	{0x6C ,1,{	0x40}},
	{0x75 ,1,{	0x01}},
	{0x76 ,1,{	0x01}},
	{0x7A ,1,{	0x80}},
	{0x7B ,1,{	0xA3}},
	{0x7C ,1,{	0xD8}},
	{0x7D ,1,{	0x60}},
	{0x7F ,1,{	0x15}},
	{0x80 ,1,{	0x81}},
	{0x83 ,1,{	0x05}},
	{0x93 ,1,{	0x08}},
	{0x94 ,1,{	0x10}},
	{0x8A ,1,{	0x00}},
	{0x9B ,1,{	0x0F}},

	{0xFF ,1,{	0x01}},
	{0xFB ,1,{	0x01}},
	{0x75 ,1,{	0x00}},
	{0x76 ,1,{	0x18}},
	{0x77 ,1,{	0x00}},
	{0x78 ,1,{	0x38}},
	{0x79 ,1,{	0x00}},
	{0x7A ,1,{	0x65}},
	{0x7B ,1,{	0x00}},
	{0x7C ,1,{	0x84}},
	{0x7D ,1,{	0x00}},
	{0x7E ,1,{	0x9B}},
	{0x7F ,1,{	0x00}},
	{0x80 ,1,{	0xAF}},
	{0x81 ,1,{	0x00}},
	{0x82 ,1,{	0xC1}},
	{0x83 ,1,{	0x00}},
	{0x84 ,1,{	0xD2}},
	{0x85 ,1,{	0x00}},
	{0x86 ,1,{	0xDF}},
	{0x87 ,1,{	0x01}},
	{0x88 ,1,{	0x11}},
	{0x89 ,1,{	0x01}},
	{0x8A ,1,{	0x38}},
	{0x8B ,1,{	0x01}},
	{0x8C ,1,{	0x76}},
	{0x8D ,1,{	0x01}},
	{0x8E ,1,{	0xA7}},
	{0x8F ,1,{	0x01}},
	{0x90 ,1,{	0xF3}},
	{0x91 ,1,{	0x02}},
	{0x92 ,1,{	0x2F}},
	{0x93 ,1,{	0x02}},
	{0x94 ,1,{	0x30}},
	{0x95 ,1,{	0x02}},
	{0x96 ,1,{	0x66}},
	{0x97 ,1,{	0x02}},
	{0x98 ,1,{	0xA0}},
	{0x99 ,1,{	0x02}},
	{0x9A ,1,{	0xC5}},
	{0x9B ,1,{	0x02}},
	{0x9C ,1,{	0xF8}},
	{0x9D ,1,{	0x03}},
	{0x9E ,1,{	0x1B}},
	{0x9F ,1,{	0x03}},
	{0xA0 ,1,{	0x46}},
	{0xA2 ,1,{	0x03}},
	{0xA3 ,1,{	0x52}},
	{0xA4 ,1,{	0x03}},
	{0xA5 ,1,{	0x62}},
	{0xA6 ,1,{	0x03}},
	{0xA7 ,1,{	0x71}},
	{0xA9 ,1,{	0x03}},
	{0xAA ,1,{	0x83}},
	{0xAB ,1,{	0x03}},
	{0xAC ,1,{	0x94}},
	{0xAD ,1,{	0x03}},
	{0xAE ,1,{	0xA3}},
	{0xAF ,1,{	0x03}},
	{0xB0 ,1,{	0xAD}},
	{0xB1 ,1,{	0x03}},
	{0xB2 ,1,{	0xCC}},
	{0xB3 ,1,{	0x00}},
	{0xB4 ,1,{	0x18}},
	{0xB5 ,1,{	0x00}},
	{0xB6 ,1,{	0x38}},
	{0xB7 ,1,{	0x00}},
	{0xB8 ,1,{	0x65}},
	{0xB9 ,1,{	0x00}},
	{0xBA ,1,{	0x84}},
	{0xBB ,1,{	0x00}},
	{0xBC ,1,{	0x9B}},
	{0xBD ,1,{	0x00}},
	{0xBE ,1,{	0xAF}},
	{0xBF ,1,{	0x00}},
	{0xC0 ,1,{	0xC1}},
	{0xC1 ,1,{	0x00}},
	{0xC2 ,1,{	0xD2}},
	{0xC3 ,1,{	0x00}},
	{0xC4 ,1,{	0xDF}},
	{0xC5 ,1,{	0x01}},
	{0xC6 ,1,{	0x11}},
	{0xC7 ,1,{	0x01}},
	{0xC8 ,1,{	0x38}},
	{0xC9 ,1,{	0x01}},
	{0xCA ,1,{	0x76}},
	{0xCB ,1,{	0x01}},
	{0xCC ,1,{	0xA7}},
	{0xCD ,1,{	0x01}},
	{0xCE ,1,{	0xF3}},
	{0xCF ,1,{	0x02}},
	{0xD0 ,1,{	0x2F}},
	{0xD1 ,1,{	0x02}},
	{0xD2 ,1,{	0x30}},
	{0xD3 ,1,{	0x02}},
	{0xD4 ,1,{	0x66}},
	{0xD5 ,1,{	0x02}},
	{0xD6 ,1,{	0xA0}},
	{0xD7 ,1,{	0x02}},
	{0xD8 ,1,{	0xC5}},
	{0xD9 ,1,{	0x02}},
	{0xDA ,1,{	0xF8}},
	{0xDB ,1,{	0x03}},
	{0xDC ,1,{	0x1B}},
	{0xDD ,1,{	0x03}},
	{0xDE ,1,{	0x46}},
	{0xDF ,1,{	0x03}},
	{0xE0 ,1,{	0x52}},
	{0xE1 ,1,{	0x03}},
	{0xE2 ,1,{	0x62}},
	{0xE3 ,1,{	0x03}},
	{0xE4 ,1,{	0x71}},
	{0xE5 ,1,{	0x03}},
	{0xE6 ,1,{	0x83}},
	{0xE7 ,1,{	0x03}},
	{0xE8 ,1,{	0x94}},
	{0xE9 ,1,{	0x03}},
	{0xEA ,1,{	0xA3}},
	{0xEB ,1,{	0x03}},
	{0xEC ,1,{	0xAD}},
	{0xED ,1,{	0x03}},
	{0xEE ,1,{	0xCC}},
	{0xEF ,1,{	0x00}},
	{0xF0 ,1,{	0x18}},
	{0xF1 ,1,{	0x00}},
	{0xF2 ,1,{	0x38}},
	{0xF3 ,1,{	0x00}},
	{0xF4 ,1,{	0x65}},
	{0xF5 ,1,{	0x00}},
	{0xF6 ,1,{	0x84}},
	{0xF7 ,1,{	0x00}},
	{0xF8 ,1,{	0x9B}},
	{0xF9 ,1,{	0x00}},
	{0xFA ,1,{	0xAF}},
	{0xFF ,1,{	0x02}},
	{0xFB ,1,{	0x01}},
	{0x00 ,1,{	0x00}},
	{0x01 ,1,{	0xC1}},
	{0x02 ,1,{	0x00}},
	{0x03 ,1,{	0xD2}},
	{0x04 ,1,{	0x00}},
	{0x05 ,1,{	0xDF}},
	{0x06 ,1,{	0x01}},
	{0x07 ,1,{	0x11}},
	{0x08 ,1,{	0x01}},
	{0x09 ,1,{	0x38}},
	{0x0A ,1,{	0x01}},
	{0x0B ,1,{	0x76}},
	{0x0C ,1,{	0x01}},
	{0x0D ,1,{	0xA7}},
	{0x0E ,1,{	0x01}},
	{0x0F ,1,{	0xF3}},
	{0x10 ,1,{	0x02}},
	{0x11 ,1,{	0x2F}},
	{0x12 ,1,{	0x02}},
	{0x13 ,1,{	0x30}},
	{0x14 ,1,{	0x02}},
	{0x15 ,1,{	0x66}},
	{0x16 ,1,{	0x02}},
	{0x17 ,1,{	0xA0}},
	{0x18 ,1,{	0x02}},
	{0x19 ,1,{	0xC5}},
	{0x1A ,1,{	0x02}},
	{0x1B ,1,{	0xF8}},
	{0x1C ,1,{	0x03}},
	{0x1D ,1,{	0x1B}},
	{0x1E ,1,{	0x03}},
	{0x1F ,1,{	0x46}},
	{0x20 ,1,{	0x03}},
	{0x21 ,1,{	0x52}},
	{0x22 ,1,{	0x03}},
	{0x23 ,1,{	0x62}},
	{0x24 ,1,{	0x03}},
	{0x25 ,1,{	0x71}},
	{0x26 ,1,{	0x03}},
	{0x27 ,1,{	0x83}},
	{0x28 ,1,{	0x03}},
	{0x29 ,1,{	0x94}},
	{0x2A ,1,{	0x03}},
	{0x2B ,1,{	0xA3}},
	{0x2D ,1,{	0x03}},
	{0x2F ,1,{	0xAD}},
	{0x30 ,1,{	0x03}},
	{0x31 ,1,{	0xCC}},
	{0x32 ,1,{	0x00}},
	{0x33 ,1,{	0x18}},
	{0x34 ,1,{	0x00}},
	{0x35 ,1,{	0x38}},
	{0x36 ,1,{	0x00}},
	{0x37 ,1,{	0x65}},
	{0x38 ,1,{	0x00}},
	{0x39 ,1,{	0x84}},
	{0x3A ,1,{	0x00}},
	{0x3B ,1,{	0x9B}},
	{0x3D ,1,{	0x00}},
	{0x3F ,1,{	0xAF}},
	{0x40 ,1,{	0x00}},
	{0x41 ,1,{	0xC1}},
	{0x42 ,1,{	0x00}},
	{0x43 ,1,{	0xD2}},
	{0x44 ,1,{	0x00}},
	{0x45 ,1,{	0xDF}},
	{0x46 ,1,{	0x01}},
	{0x47 ,1,{	0x11}},
	{0x48 ,1,{	0x01}},
	{0x49 ,1,{	0x38}},
	{0x4A ,1,{	0x01}},
	{0x4B ,1,{	0x76}},
	{0x4C ,1,{	0x01}},
	{0x4D ,1,{	0xA7}},
	{0x4E ,1,{	0x01}},
	{0x4F ,1,{	0xF3}},
	{0x50 ,1,{	0x02}},
	{0x51 ,1,{	0x2F}},
	{0x52 ,1,{	0x02}},
	{0x53 ,1,{	0x30}},
	{0x54 ,1,{	0x02}},
	{0x55 ,1,{	0x66}},
	{0x56 ,1,{	0x02}},
	{0x58 ,1,{	0xA0}},
	{0x59 ,1,{	0x02}},
	{0x5A ,1,{	0xC5}},
	{0x5B ,1,{	0x02}},
	{0x5C ,1,{	0xF8}},
	{0x5D ,1,{	0x03}},
	{0x5E ,1,{	0x1B}},
	{0x5F ,1,{	0x03}},
	{0x60 ,1,{	0x46}},
	{0x61 ,1,{	0x03}},
	{0x62 ,1,{	0x52}},
	{0x63 ,1,{	0x03}},
	{0x64 ,1,{	0x62}},
	{0x65 ,1,{	0x03}},
	{0x66 ,1,{	0x71}},
	{0x67 ,1,{	0x03}},
	{0x68 ,1,{	0x83}},
	{0x69 ,1,{	0x03}},
	{0x6A ,1,{	0x94}},
	{0x6B ,1,{	0x03}},
	{0x6C ,1,{	0xA3}},
	{0x6D ,1,{	0x03}},
	{0x6E ,1,{	0xAD}},
	{0x6F ,1,{	0x03}},
	{0x70 ,1,{	0xCC}},
	{0x71 ,1,{	0x00}},
	{0x72 ,1,{	0x18}},
	{0x73 ,1,{	0x00}},
	{0x74 ,1,{	0x38}},
	{0x75 ,1,{	0x00}},
	{0x76 ,1,{	0x65}},
	{0x77 ,1,{	0x00}},
	{0x78 ,1,{	0x84}},
	{0x79 ,1,{	0x00}},
	{0x7A ,1,{	0x9B}},
	{0x7B ,1,{	0x00}},
	{0x7C ,1,{	0xAF}},
	{0x7D ,1,{	0x00}},
	{0x7E ,1,{	0xC1}},
	{0x7F ,1,{	0x00}},
	{0x80 ,1,{	0xD2}},
	{0x81 ,1,{	0x00}},
	{0x82 ,1,{	0xDF}},
	{0x83 ,1,{	0x01}},
	{0x84 ,1,{	0x11}},
	{0x85 ,1,{	0x01}},
	{0x86 ,1,{	0x38}},
	{0x87 ,1,{	0x01}},
	{0x88 ,1,{	0x76}},
	{0x89 ,1,{	0x01}},
	{0x8A ,1,{	0xA7}},
	{0x8B ,1,{	0x01}},
	{0x8C ,1,{	0xF3}},
	{0x8D ,1,{	0x02}},
	{0x8E ,1,{	0x2F}},
	{0x8F ,1,{	0x02}},
	{0x90 ,1,{	0x30}},
	{0x91 ,1,{	0x02}},
	{0x92 ,1,{	0x66}},
	{0x93 ,1,{	0x02}},
	{0x94 ,1,{	0xA0}},
	{0x95 ,1,{	0x02}},
	{0x96 ,1,{	0xC5}},
	{0x97 ,1,{	0x02}},
	{0x98 ,1,{	0xF8}},
	{0x99 ,1,{	0x03}},
	{0x9A ,1,{	0x1B}},
	{0x9B ,1,{	0x03}},
	{0x9C ,1,{	0x46}},
	{0x9D ,1,{	0x03}},
	{0x9E ,1,{	0x52}},
	{0x9F ,1,{	0x03}},
	{0xA0 ,1,{	0x62}},
	{0xA2 ,1,{	0x03}},
	{0xA3 ,1,{	0x71}},
	{0xA4 ,1,{	0x03}},
	{0xA5 ,1,{	0x83}},
	{0xA6 ,1,{	0x03}},
	{0xA7 ,1,{	0x94}},
	{0xA9 ,1,{	0x03}},
	{0xAA ,1,{	0xA3}},
	{0xAB ,1,{	0x03}},
	{0xAC ,1,{	0xAD}},
	{0xAD ,1,{	0x03}},
	{0xAE ,1,{	0xCC}},
	{0xAF ,1,{	0x00}},
	{0xB0 ,1,{	0x18}},
	{0xB1 ,1,{	0x00}},
	{0xB2 ,1,{	0x38}},
	{0xB3 ,1,{	0x00}},
	{0xB4 ,1,{	0x65}},
	{0xB5 ,1,{	0x00}},
	{0xB6 ,1,{	0x84}},
	{0xB7 ,1,{	0x00}},
	{0xB8 ,1,{	0x9B}},
	{0xB9 ,1,{	0x00}},
	{0xBA ,1,{	0xAF}},
	{0xBB ,1,{	0x00}},
	{0xBC ,1,{	0xC1}},
	{0xBD ,1,{	0x00}},
	{0xBE ,1,{	0xD2}},
	{0xBF ,1,{	0x00}},
	{0xC0 ,1,{	0xDF}},
	{0xC1 ,1,{	0x01}},
	{0xC2 ,1,{	0x11}},
	{0xC3 ,1,{	0x01}},
	{0xC4 ,1,{	0x38}},
	{0xC5 ,1,{	0x01}},
	{0xC6 ,1,{	0x76}},
	{0xC7 ,1,{	0x01}},
	{0xC8 ,1,{	0xA7}},
	{0xC9 ,1,{	0x01}},
	{0xCA ,1,{	0xF3}},
	{0xCB ,1,{	0x02}},
	{0xCC ,1,{	0x2F}},
	{0xCD ,1,{	0x02}},
	{0xCE ,1,{	0x30}},
	{0xCF ,1,{	0x02}},
	{0xD0 ,1,{	0x66}},
	{0xD1 ,1,{	0x02}},
	{0xD2 ,1,{	0xA0}},
	{0xD3 ,1,{	0x02}},
	{0xD4 ,1,{	0xC5}},
	{0xD5 ,1,{	0x02}},
	{0xD6 ,1,{	0xF8}},
	{0xD7 ,1,{	0x03}},
	{0xD8 ,1,{	0x1B}},
	{0xD9 ,1,{	0x03}},
	{0xDA ,1,{	0x46}},
	{0xDB ,1,{	0x03}},
	{0xDC ,1,{	0x52}},
	{0xDD ,1,{	0x03}},
	{0xDE ,1,{	0x62}},
	{0xDF ,1,{	0x03}},
	{0xE0 ,1,{	0x71}},
	{0xE1 ,1,{	0x03}},
	{0xE2 ,1,{	0x83}},
	{0xE3 ,1,{	0x03}},
	{0xE4 ,1,{	0x94}},
	{0xE5 ,1,{	0x03}},
	{0xE6 ,1,{	0xA3}},
	{0xE7 ,1,{	0x03}},
	{0xE8 ,1,{	0xAD}},
	{0xE9 ,1,{	0x03}},
	{0xEA ,1,{	0xCC}},
	{0xFF ,1,{	0x01}},
	{0xFB ,1,{	0x01}},
	{0xFF ,1,{	0x02}},
	{0xFB ,1,{	0x01}},
	{0xFF ,1,{	0x04}},
	{0xFB ,1,{	0x01}},
	{0xFF ,1,{	0x00}},
	{0xD3 ,1,{	0x05}},
	{0xD4 ,1,{	0x04}},
	{0x11 ,0,{	 }},
	{REGFLAG_DELAY, 120, {}},	
	{0xFF ,1,{	0x00}},
	{0x34 ,1,{	0x00}},
	{0x35 ,1,{	0x00}},
	{0x29 ,0,{	  }},	
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
     {REGFLAG_DELAY, 60, {}},

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


//static void init_lcm_registers(void)
//{
//	unsigned int data_array[16];

//	//	{0x00,0X01,{0x00}},
////	data_array[0]= 0x00023902;	
////	data_array[1]= 0x00000000;	
////	dsi_set_cmdq(data_array, 2, 1);	
//	data_array[0]= 0x00001500;		
//	dsi_set_cmdq(data_array, 1, 1); 

//	//	{0XFF,0X03,{0X12,0X83,0x01}},
//	data_array[0]= 0x00043902;	
//	data_array[1]= 0x018312ff;	
//	dsi_set_cmdq(data_array, 2, 1);	

//	//	{0x00,0X01,{0x80}}, 
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0XFF,0X02,{0X12,0X83}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x008312ff;	
//	dsi_set_cmdq(data_array, 2, 1);	
//	
//	//	{0X00,0X01,{0X92}},
//	data_array[0]= 0x92001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0Xff,0X02,{0x30,0x02}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x000230ff;	
//	dsi_set_cmdq(data_array, 2, 1);	

//	//	{0X00,0X01,{0X90}},
//	data_array[0]= 0x90001500;		
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0Xf5,0X04,{0x02,0x11,0x02,0x11}},
//	data_array[0]= 0x00053902;	
//	data_array[1]= 0x021102f5;
//	data_array[2]= 0x00000011;
//	dsi_set_cmdq(data_array, 3, 1);	
//	
//	//	{0X00,0X01,{0X90}},
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0Xc5,0X01,{0X50}},
//	data_array[0]= 0x50c51500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0X00,0X01,{0X94}},
//	data_array[0]= 0x94001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0Xc5,0X01,{0X77}},
//	data_array[0]= 0x77c51500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0X00,0X01,{0X80}},
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0Xc0,0x09,{0x00,0x64,0x00, 0x10,0x10,0x00,0x64, 0x10,0x10}},
//	data_array[0]= 0x000a3902;	
//	data_array[1]= 0x006400c0;	
//	data_array[2]= 0x64001010;	
//	data_array[3]= 0x00001010;	
//	dsi_set_cmdq(data_array, 4, 1);
//	
//	//	{0X00,0X01,{0X90}},
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xc0,0x06,{0x00,0x5c,0x00,  0x01,0x00,0x04}},
//	data_array[0]= 0x00073902;	
//	data_array[1]= 0x005c00c0;	
//	data_array[2]= 0x00040001;	
//	dsi_set_cmdq(data_array, 3, 1);
//	
//	//	{0X00,0X01,{0Xb3}},
//	data_array[0]= 0xb3001500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	//	{0Xc0,0X02,{0X00,0X50}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x005000c0;	
//	dsi_set_cmdq(data_array, 2, 1);
//	
//	//	{0X00,0X01,{0X81}},
//	data_array[0]= 0x81001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xc1,0X01,{0X66}},
//	data_array[0]= 0x55c11500;	    //huxh
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0X00,0X01,{0Xa0}},
//	data_array[0]= 0xa0001500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	//	{0Xc4,0X0E,{0x05,0x10,0x06,| 0x02,0x05,0x15,0x10, | 0x05,0x10,0x07,0x02, | 0x05,0x15,0x10}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x061005c4;	
//	data_array[2]= 0x10150502;	
//	data_array[3]= 0x02071005;	
//	data_array[4]= 0x00101505;	
//	dsi_set_cmdq(data_array, 5, 1);
//	
//	//	{0X00,0X01,{0Xb0}},
//	data_array[0]= 0xb0001500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	//	{0Xc4,0X02,{0X00,0X00}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x000000c4;	
//	dsi_set_cmdq(data_array, 2, 1);
//	
//	//	{0X00,0X01,{0X91}},
//	data_array[0]= 0x91001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xc5,0X02,{0X46,0X40}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x004046c5;	
//	dsi_set_cmdq(data_array, 2, 1);
//	
//	//	{0X00,0X01,{0X00}},
//	data_array[0]= 0x00001500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	//	{0Xd8,0X02,{0Xbc,0Xbc}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x00bcbcd8;	
//	dsi_set_cmdq(data_array, 2, 1);

//	//	{0X00,0X01,{0X00}},
//	data_array[0]= 0x00001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xd9,0X01,{0X30}},
//	data_array[0]= 0x30d91500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0X00,0X01,{0Xb0}},
//	data_array[0]= 0xb0001500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	//	{0Xc5,0X02,{0X04,0x38}},
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x003804c5;	
//	dsi_set_cmdq(data_array, 2, 1);
//	
//	//	{0X00,0X01,{0Xbb}},
//	data_array[0]= 0xbb001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xc5,0X01,{0X80}},
//	data_array[0]= 0x80c51500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	//	{0X00,0X01,{0Xc3}},
//	data_array[0]= 0xc3001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xf5,0X01,{0X81}},
//	data_array[0]= 0x81f51500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0X00,0X01,{0X82}},
//	data_array[0]= 0x82001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xf4,0X01,{0X00}}, 
//	data_array[0]= 0x00f41500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0X00,0X01,{0X80}},
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xc6,0X01,{0X24}}, 
//	data_array[0]= 0x24c61500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0X00,0X01,{0X00}},
//	data_array[0]= 0x00001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xd0,0X01,{0X40}}, 
//	data_array[0]= 0x40d01500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0X00,0X01,{0X00}},
//	data_array[0]= 0x00001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0Xd1,0X02,{0X00,0X00}},	
//	data_array[0]= 0x00033902;	
//	data_array[1]= 0x000000d1;
//	dsi_set_cmdq(data_array, 2, 1);

////===========================================
////SSD2825_Gen_write_1A_1P(0x00,0x94);  //VCL on   
//	data_array[0]= 0x94001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
////SSD2825_Gen_write_1A_1P(0xF5,0x02);
//	data_array[0]= 0x02f51500;	
//	dsi_set_cmdq(data_array, 1, 1); 

////SSD2825_Gen_write_1A_1P(0x00,0xBA);  //VSP on  
//	data_array[0]= 0xba001500;	
//	dsi_set_cmdq(data_array, 1, 1); 

////SSD2825_Gen_write_1A_1P(0xF5,0x03);
//	data_array[0]= 0x03f51500;	
//	dsi_set_cmdq(data_array, 1, 1); 

////===========================================
//	
//	//	{0X00,0X01,{0X80}},
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0xcb,0x0b,{0x00,0x00,0x00,-0x00,0x00,0x00,0x00,-0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x000c3902;	
//	data_array[1]= 0x000000cb;	
//	data_array[2]= 0x00000000;
//	data_array[3]= 0x00000000;
//	dsi_set_cmdq(data_array, 4, 1);
//	
//	//	{0X00,0X01,{0X90}}, 
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0xCB,0x0F,{0x00,0x00,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x000000cb;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0Xa0}}, 
//	data_array[0]= 0xa0001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0xCB,0x0F,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x000000cb;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0Xb0}}, 
//	data_array[0]= 0xb0001500;	
//	dsi_set_cmdq(data_array, 1, 1);

//	
//	//	{0xCB,0x0F,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x000000cb;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0Xc0}}, 
//	data_array[0]= 0xc0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCB,0x0F,{0x05,0x00,0x05=,0x05,0x05,0x05,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x050005cb;	
//	data_array[2]= 0x00050505;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0Xd0}}, 
//	data_array[0]= 0xd0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCB,0x0F,{0x00,0x00,0x00,=0x05,0x00,0x05,0x05,=0x05,0x00,0x05,0x05,=0x05,0x05,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x000000cb;	
//	data_array[2]= 0x05050005;	
//	data_array[3]= 0x05050005;	
//	data_array[4]= 0x00000505;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0Xe0}}, 
//	data_array[0]= 0xe0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCB,0x0E,{0x00,0x00,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x05, =0x00,0x05,0x05}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cb;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x05000000;	
//	data_array[4]= 0x00050500;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0Xf0}}, 
//	data_array[0]= 0xf0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCB,0x0b,{0xff,0xff,0xff,=0xff,0xff,0xff,0xff,=0xff,0xff,0xff,0xff}},
//	data_array[0]= 0x000c3902;	
//	data_array[1]= 0xffffffcb;	
//	data_array[2]= 0xffffffff;	
//	data_array[3]= 0xffffffff;	
//	dsi_set_cmdq(data_array, 4, 1);

//	//	{0X00,0X01,{0X80}},
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCC,0x0F,{0x02,0x00,0x0a,=0x0e,0x0c,0x10,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x0a0002cc;	
//	data_array[2]= 0x00100c0e;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0X90}}, 
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCC,0x0F,{0x00,0x00,0x00,=0x06,0x00,0x2e,0x2d,=0x01,0x00,0x09,0x0d,=0x0b,0x0f,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x000000cc;	
//	data_array[2]= 0x2d2e0006;	
//	data_array[3]= 0x0d090001;	
//	data_array[4]= 0x00000f0b;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XA0}}, 
//	data_array[0]= 0xa0001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0xCC,0x0E,{0x00,0x00,0x00,=0x00,0x00,0x00,0x00=,0x00,0x00,0x00,0x05,=0x00,0x2e,0x2d}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cc;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x05000000;	
//	data_array[4]= 0x002d2e00;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XB0}}, 
//	data_array[0]= 0xb0001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0xCC,0x0F,{0x05,0x00,0x0f,=0x0b,0x0d,0x09,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x0f0005cc;	
//	data_array[2]= 0x00090d0b;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XC0}}, 
//	data_array[0]= 0xc0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCC,0x0F,{0x00,0x00,0x00,=0x01,0x00,0x2d,0x2e,=0x06,0x00,0x10,0x0c,=0x0e,0x0a,0x00,0x00}},
//	data_array[0]= 0x00103902;	
//	data_array[1]= 0x000000cc;	
//	data_array[2]= 0x2e2d0001;	
//	data_array[3]= 0x0c100006;	
//	data_array[4]= 0x00000a0e;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XD0}}, 
//	data_array[0]= 0xd0001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0xCC,0x0E,{0x00,0x00,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x02,=0x00,0x2d,0x2e}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cc;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x02000000;	
//	data_array[4]= 0x002e2d00;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0X80}}, 
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);
//	
//	//	{0xCE,0x0C,{0x87,0x03,0x18,=0x86,0x03,0x18,0x00,=0x00,0x00,0x00,0x00,=0x00}},
//	data_array[0]= 0x000d3902;	
//	data_array[1]= 0x180387ce;	
//	data_array[2]= 0x00180386;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0X90}}, 
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCE,0x0E,{0x34,0xfe,0x18,=0x34,0xff,0x18,0x00=,0x00,0x00,0x00,0x00,=0x00,0x00,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x18fe34ce;	
//	data_array[2]= 0x0018ff34;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XA0}}, 
//	data_array[0]= 0xa0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCE,0x0E,{0x38,0x03,0x05,=0x00,0x00,0x18,0x00,=0x38,0x02,0x05,0x01,=0x00,0x18,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x050338ce;	
//	data_array[2]= 0x00180000;	
//	data_array[3]= 0x01050238;	
//	data_array[4]= 0x00001800;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XB0}}, 
//	data_array[0]= 0xb0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCE,0x0E,{0x38,0x01,0x05,=0x02,0x00,0x18,0x00,=0x38,0x00,0x05,0x03,=0x00,0x18,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x050138ce;	
//	data_array[2]= 0x00180002;	
//	data_array[3]= 0x03050038;	
//	data_array[4]= 0x00001800;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XC0}}, 
//	data_array[0]= 0xc0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCE,0x0E,{0x30,0x00,0x05,=0x04,0x00,0x18,0x00,=0x30,0x01,0x05,0x05,=0x00,0x18,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x050030ce;	
//	data_array[2]= 0x00180004;	
//	data_array[3]= 0x05050130;	
//	data_array[4]= 0x00001800;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XD0}}, 
//	data_array[0]= 0xd0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCE,0x0E,{0x30,0x02,0x05,=0x06,0x00,0x18,0x00,=0x30,0x03,0x05,0x07,=0x00,0x18,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x050230ce;	
//	data_array[2]= 0x00180006;	
//	data_array[3]= 0x07050330;	
//	data_array[4]= 0x00001800;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0X80}}, 
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCF,0x0E,{0x00,0x00,0x00=,0x00,0x00,0x00,0x00,=0x00,0x00,0x00,0x00,=0x00,0x00,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cf;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0X90}}, 
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCF,0x0E,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cf;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XA0}}, 
//	data_array[0]= 0xa0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCF,0x0E,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cf;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XB0}}, 
//	data_array[0]= 0xb0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCF,0x0E,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//	data_array[0]= 0x000f3902;	
//	data_array[1]= 0x000000cf;	
//	data_array[2]= 0x00000000;	
//	data_array[3]= 0x00000000;	
//	data_array[4]= 0x00000000;	
//	dsi_set_cmdq(data_array, 5, 1);

//	//	{0X00,0X01,{0XC0}}, 
//	data_array[0]= 0xc0001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xCF,0x0A,{0x01,0x01,0x20,==0x20,0x00,0x00,0x01,==0x01,0x00,0x00}},
//	data_array[0]= 0x000b3902;	
//	data_array[1]= 0x200101cf;	
//	data_array[2]= 0x01000020;	
//	data_array[3]= 0x00000001;	
//	dsi_set_cmdq(data_array, 4, 1);

//	//	{0X00,0X01,{0XB5}}, 
//	data_array[0]= 0xb5001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0xC5,0x06,{0x37,0xf1,0xfd,=0x37,0xf1,0xfd}},
//	data_array[0]= 0x00073902;	
//	data_array[1]= 0xfdf137c5;	
//	data_array[2]= 0x00fdf137;	
//	dsi_set_cmdq(data_array, 3, 1);

//	//	{0X00,0X01,{0X00}}, 
//	data_array[0]= 0x00001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0xE1,0x10,{0x10,0x15,0x1B,=0x11,0x0A,0x1D,0x0D,=0x0D,0x01,0x05,0x02,=0x07,0x0F,0x1F,0x1C,=0x08}},
//	data_array[0]= 0x00113902;	
//	data_array[1]= 0x1b1510e1;	
//	data_array[2]= 0x0d1d0a11;	
//	data_array[3]= 0x0205010d;	
//	data_array[4]= 0x1c1f0f07;	
//	data_array[5]= 0x00000008;	
//	dsi_set_cmdq(data_array, 6, 1);

//	//	{0X00,0X01,{0X00}}, 
//	data_array[0]= 0x00001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0xE2,0x10,{0x10,0x15,0x1B,=0x11,0x0A,0x1D,0x0D,=0x0D,0x01,0x05,0x02,=0x07,0x0F,0x1F,0x1C,=0x08}},
//	data_array[0]= 0x00113902;	
//	data_array[1]= 0x1b1510e2;	
//	data_array[2]= 0x0d1d0a11;	
//	data_array[3]= 0x0205010d;	
//	data_array[4]= 0x1c1f0f07;	
//	data_array[5]= 0x00000008;	
//	dsi_set_cmdq(data_array, 6, 1);

//	//	{0X00,0X01,{0X90}}, 
//	data_array[0]= 0x90001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0XB3,0X01,{0X02}}, 
//	data_array[0]= 0x02b31500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0X00,0X01,{0X92}}, 
//	data_array[0]= 0x92001500;	
//	dsi_set_cmdq(data_array, 1, 1);		
//	
//	//	{0XB3,0X01,{0X40}}, 
//	data_array[0]= 0x40b31500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0X00,0X01,{0X80}}, 
//	data_array[0]= 0x80001500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0Xf6,0X01,{0X01}}, 
//	data_array[0]= 0x01f61500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
////=======================#ifdef DISPLAY_DIRECTION_0_MODE
////	//	{0x36, 1 ,{0x00}},// Display Direction 0
////	data_array[0]= 0x00023902;	
////	data_array[1]= 0x00000036;
////	dsi_set_cmdq(data_array, 2, 1);	
////	
////	//	{0x35, 1 ,{0x00}},// TE( Fmark ) Signal On
////	data_array[0]= 0x00023902;	
////	data_array[1]= 0x00000035;
////	dsi_set_cmdq(data_array, 2, 1);	
////	
////	//	{0x44, 2 ,{0x01,0x22}},// TE( Fmark ) Signal Output Position
////	data_array[0]= 0x00033902;	
////	data_array[1]= 0x00220144;
////	dsi_set_cmdq(data_array, 2, 1);	

////#ifdef DISPLAY_DIRECTION_180_MODE
////	{0x36, 1 ,{0xD0}},// Display Direction 180
//	data_array[0]= 0xd0361500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
////	{0x35, 1 ,{0x00}},// TE( Fmark ) Signal On
////	data_array[0]= 0x00351500;	
////	dsi_set_cmdq(data_array, 1, 1);	

////	{0x44, 2 ,{0x01,0xFF}},// TE( Fmark ) Signal Output Position
////	data_array[0]= 0x00033902;	
////	data_array[1]= 0x00ff0144;
////	dsi_set_cmdq(data_array, 2, 1);	

////======================================================end
//	//	{0X21,0X01,{0X00}}, 
//	data_array[0]= 0x00210500;	
//	dsi_set_cmdq(data_array, 1, 1);	
//	
//	//	{0X11,0X00,{}}, 
////	data_array[0]= 0x00023902;	
////	data_array[1]= 0x00000011;
////	dsi_set_cmdq(data_array, 2, 1);	
//	data_array[0]= 0x00110500;	
//	dsi_set_cmdq(data_array, 1, 1); 

//	MDELAY(200);
//	//	{REGFLAG_DELAY, 200, {}},
//	//	{0X29,0X00,{}}, 
////	data_array[0]= 0x00023902;	
////	data_array[1]= 0x00000029;
////	dsi_set_cmdq(data_array, 2, 1);	
//	data_array[0]= 0x00290500;	
//	dsi_set_cmdq(data_array, 1, 1); 

//	//	{REGFLAG_DELAY, 200, {}},
//	MDELAY(200);

//}

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

		
		params->dsi.vertical_sync_active				= 2;    
		params->dsi.vertical_backporch					= 3;// 
		params->dsi.vertical_frontporch					= 4; // 4//20
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				=10;//= 1
		params->dsi.horizontal_backporch				=118;//= 30
		params->dsi.horizontal_frontporch				=118;//= 30
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.pll_select=0;	//0: MIPI_PLL; 1: LVDS_PLL

		params->dsi.compatibility_for_nvk=1;
        //	params->dsi.HS_PRPR=6;
//	    params->dsi.LPX=8; 
		//params->dsi.HS_PRPR=5;
		//params->dsi.HS_TRAIL=13;
//		params->dsi.CLK_TRAIL = 10;
		// Bit rate calculation
		//1 Every lane speed
		params->dsi.pll_div1=1;//0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;//1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =62; // 72; // 58;//34; // 19;0x12    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

}

static void lcm_init(void)
{

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(60);
	
	SET_RESET_PIN(1);
	MDELAY(150);      

	init_lcm_registers();

}


static void lcm_suspend(void)
{
   // SET_RESET_PIN(0);
   // MDELAY(20);
   // SET_RESET_PIN(1);
	//MDELAY(20);
    push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
   SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(60);

	SET_RESET_PIN(1);
	MDELAY(250); 	

}


static void lcm_resume(void)
{
	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(60);

	SET_RESET_PIN(1);
	MDELAY(250); 	 

	init_lcm_registers();
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

//	data_array[0]= 0x00290508; //HW bug, so need send one HS packet
//	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(&data_array, 1, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{
	unsigned char lcd_id = 0;
    mt_set_gpio_mode(GPIO_LCM_ID_PIN, GPIO_LCM_ID_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_LCM_ID_PIN, GPIO_DIR_IN);	
	mt_set_gpio_pull_enable(GPIO_LCM_ID_PIN,GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_LCM_ID_PIN, GPIO_PULL_UP);
	MDELAY(1);
	lcd_id =  mt_get_gpio_in(GPIO_LCM_ID_PIN);

#ifdef BUILD_LK
	printf("thl:%s, LK nt35590 debug: nt35590 id = 0x%08x\n", __func__, lcd_id);
#else
	printk("thl:%s, kernel nt35590 horse debug: nt35590 id = 0x%08x\n", __func__, lcd_id);
#endif

	if(lcd_id)
		return 0;
	else
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
	printk("jacob test lcm_esd_check buffer[0]=0x%x\n",buffer[0]);
	if(buffer[0]==0x9c)
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
	printk("jacob test lcm_esd_recover\n");
	lcm_init();
#endif
	return TRUE;
}


LCM_DRIVER nt35596_fhd_dsi_vdo_thl_lcm_drv = 
{
    .name			= "nt35596_fhd_dsi_vdo_thl",
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
