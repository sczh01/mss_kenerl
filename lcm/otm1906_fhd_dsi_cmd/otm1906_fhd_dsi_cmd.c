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
#include <linux/init.h>
#include <linux/kernel.h>
//#include <linux/module.h>  
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
//#include <linux/delay.h>
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

#define REGFLAG_RD                      0xF80
#define REGFLAG_PWR                     0xFF0

#define REGFLAG_IOVCC                   0xFF8
#define REGFLAG_VCI                     0xFF9
#define REGFLAG_VSP                     0xFFA
#define REGFLAG_VSN                     0xFFB
#define REGFLAG_RST                     0xFFC
#define REGFLAG_BL                      0xFFD
#define REGFLAG_DELAY                   0XFFE
#define REGFLAG_END_OF_TABLE            0xFFF   // END OF REGISTERS MARKER


#define LCM_ID_NT35590 (0x90)

// ---------------------------------------------------------------------------
//  Local Variables and Local Functions
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define dsi_set_cmdq_V2(cmd,count,ppara,force_update)   lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)					lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)		lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)					lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

#define   LCM_DSI_CMD_MODE				1
#define FORMAT_V1 0x00
#define FORMAT_V2 0x01

#ifndef BUILD_LK
static char  buf[1]="1";
static struct task_struct *my_thread=NULL;
static struct file *fp;
static mm_segment_t fs;
static loff_t pos;

int thread_func(void *data){

	while(!kthread_should_stop()){
		fs=get_fs();
		set_fs(KERNEL_DS);
		vfs_write(fp,buf,sizeof(buf),&pos);
		if(buf[0]<'9'){
			buf[0]++;
		}else{
			buf[0]='1';
		}
		
		set_fs(fs);
		MDELAY(3);
	}
	filp_close(fp,NULL);
	return 0;
}


static int  hello_init(void){

    printk("Enther %s.\n", __func__ );

	fp=filp_open("/root/output.txt",O_RDWR|O_CREAT,0644);

	if(IS_ERR(fp)){
		printk("Create /root/output.txt error\n");

        fp=filp_open("/sdcard/output.txt",O_RDWR|O_CREAT,0644);

        if(IS_ERR(fp)){
            printk("Create /sdcard/output.txt error\n");
            return -1;
        }else{
            printk("Create /sdcard/output.txt OK: output.txt.\n");  
        }

	}else{
        printk("Create /root/output.txt OK.\n");  
    }
	
	my_thread=kthread_run(thread_func,NULL,"my_thread");
	return 0;
}

 
static void hello_exit(void) {
	if(my_thread){
		printk("Enther %s stop mythread\n", __func__ );
		kthread_stop(my_thread);
	}
}
#endif

static struct LCM_setting_table
{
    unsigned int cmd;
    unsigned int count;
    unsigned char para_list[80];
};

static struct LCM_setting_table_V2
{
    unsigned int cmd;
    unsigned int count;
    unsigned int para_list[20];
};

static struct LCM_cfg
{
    unsigned int mode;
    unsigned int exe_num;
};

static union LCM_code_table
{
    struct LCM_setting_table tbl_v1;
    struct LCM_setting_table_V2 tbl_v2;
    unsigned char buf[sizeof(struct LCM_setting_table_V2)];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
{0xff0,10,{0x14,0x00,0x10,0x01,0x12,0x01,0x13,0x01,0x14,0x01}},
{0x00,1,{0x00}},
{0xFF,3,{0x19,0x06,0x01}},
{0x00,1,{0x80}},
{0xFF,2,{0x19,0x06}},
{0x00,1,{0x80}},
{0xA4,7,{0x00,0x00,0x00,0x02,0x00,0x82,0x00}},
{0x00,1,{0x80}},
{0xA5,4,{0x0F,0x00,0x01,0x08}},
{0x00,1,{0x80}},
{0xC0,14,{0x00,0x80,0x00,0x04,0x06,0x00,0x80,0x04,0x06,0x00,0x80,0x00,0x04,0x06}},
{0x00,1,{0xA0}},
{0xC0,7,{0x00,0x05,0x00,0x05,0x02,0x1B,0x04}},
{0x00,1,{0xD0}},
{0xC0,7,{0x00,0x05,0x00,0x05,0x02,0x1B,0x04}},
{0x00,1,{0x80}},
{0xC1,2,{0x11,0x11}},
{0x00,1,{0x80}},
{0xC2,12,{0x84,0x01,0x40,0x40,0x82,0x01,0x17,0x18,0x00,0x00,0x00,0x00}},
{0x00,1,{0x90}},
{0xC2,12,{0x00,0x00,0x00,0x00,0x02,0x01,0x08,0x0F,0x02,0x01,0x08,0x0F}},
{0x00,1,{0xB0}},
{0xC2,15,{0x84,0x01,0x00,0x08,0x00,0x82,0x03,0x00,0x08,0x00,0x83,0x02,0x00,0x08,0x00}},
{0x00,1,{0xC0}},
{0xC2,15,{0x81,0x04,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xD0}},
{0xC2,15,{0x82,0x00,0x01,0x02,0x81,0x84,0x02,0x03,0x00,0x01,0x33,0x33,0x00,0x37,0x00}},
{0x00,1,{0xA0}},
{0xC3,12,{0x84,0x01,0x40,0x40,0x82,0x01,0x17,0x18,0x00,0x00,0x00,0x00}},
{0x00,1,{0xB0}},
{0xC3,14,{0x00,0x00,0x00,0x00,0x84,0x01,0x00,0x08,0x00,0x82,0x03,0x00,0x08,0x00}},
{0x00,1,{0xC0}},
{0xC3,15,{0x83,0x02,0x00,0x08,0x00,0x81,0x04,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xE0}},
{0xC3,4,{0x33,0x33,0x00,0x37}},
{0x00,1,{0x80}},
{0xCB,11,{0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0x90}},
{0xCB,15,{0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xCB,15,{0x00,0x00,0x00,0xBF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xB0}},
{0xCB,12,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x77,0x77,0x00,0x00}},
{0x00,1,{0xC0}},
{0xCB,15,{0x15,0x11,0x15,0x15,0x15,0x15,0x15,0x15,0x01,0x01,0x01,0x01,0x01,0x01,0x01}},
{0x00,1,{0xD0}},
{0xCB,15,{0x01,0x3D,0x01,0xFF,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x11,0x01,0x11}},
{0x00,1,{0xE0}},
{0xCB,12,{0x01,0x11,0x00,0x01,0x00,0x01,0x00,0x01,0x77,0x77,0x00,0x00}},
{0x00,1,{0xF0}},
{0xCB,11,{0xF3,0xFF,0x55,0x55,0x7F,0xC0,0x03,0x33,0x03,0x00,0x02}},
{0x00,1,{0x80}},
{0xCC,12,{0x08,0x09,0x18,0x19,0x0C,0x0D,0x0E,0x0F,0x07,0x07,0x07,0x07}},
{0x00,1,{0x90}},
{0xCC,12,{0x18,0x09,0x08,0x19,0x0F,0x0E,0x0D,0x0C,0x07,0x07,0x07,0x07}},
{0x00,1,{0xA0}},
{0xCC,15,{0x14,0x15,0x16,0x17,0x1C,0x1D,0x1E,0x1F,0x20,0x07,0x07,0x07,0x07,0x07,0x00}},
{0x00,1,{0xB0}},
{0xCC,8,{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x07}},
{0x00,1,{0xC0}},
{0xCC,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x45,0x45,0x00}},
{0x00,1,{0xD0}},
{0xCC,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x33,0x33,0x33}},
{0x00,1,{0xE0}},
{0xCC,12,{0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0x80}},
{0xCD,15,{0x01,0x11,0x12,0x07,0x08,0x04,0x18,0x1A,0x03,0x09,0x23,0x23,0x23,0x1D,0x1E}},
{0x00,1,{0x90}},
{0xCD,3,{0x1F,0x23,0x23}},
{0x00,1,{0xA0}},
{0xCD,15,{0x01,0x11,0x12,0x05,0x06,0x04,0x18,0x17,0x03,0x09,0x23,0x23,0x23,0x1D,0x1E}},
{0x00,1,{0xB0}},
{0xCD,3,{0x1F,0x23,0x23}},
{0x00,1,{0x91}},
{0xC5,2,{0x14,0x1E}},
{0x00,1,{0xA1}},
{0xC5,2,{0x14,0x1E}},
{0x00,1,{0x00}},
{0xD9,8,{0x00,0xBD,0x00,0xBD,0x00,0x90,0x00,0x90}},
{0x00,1,{0x00}},
{0xD8,2,{0x1B,0x1B}},
{0x00,1,{0x00}},
{0xE1,24,{0x00,0x2C,0x34,0x3F,0x47,0x4F,0x5A,0x6A,0x74,0x83,0x8C,0x94,0x67,0x62,0x5E,0x52,0x43,0x32,0x26,0x20,0x17,0x0B,0x03,0x03}},
{0x00,1,{0x00}},
{0xE2,24,{0x00,0x2C,0x34,0x3F,0x47,0x4F,0x5A,0x6A,0x74,0x83,0x8C,0x94,0x67,0x62,0x5E,0x52,0x43,0x32,0x26,0x20,0x17,0x0B,0x03,0x03}},
{0x00,1,{0x00}},
{0xE3,24,{0x00,0x2C,0x34,0x3F,0x47,0x4F,0x5A,0x6A,0x74,0x83,0x8C,0x94,0x67,0x62,0x5E,0x52,0x43,0x32,0x26,0x20,0x17,0x0B,0x03,0x03}},
{0x00,1,{0x00}},
{0xE4,24,{0x00,0x2C,0x34,0x3F,0x47,0x4F,0x5A,0x6A,0x74,0x83,0x8C,0x94,0x67,0x62,0x5E,0x52,0x43,0x32,0x26,0x20,0x17,0x0B,0x03,0x03}},
{0x00,1,{0x00}},
{0xE5,24,{0x00,0x2C,0x34,0x3F,0x47,0x4F,0x5A,0x6A,0x74,0x83,0x8C,0x94,0x67,0x62,0x5E,0x52,0x43,0x32,0x26,0x20,0x17,0x0B,0x03,0x03}},
{0x00,1,{0x00}},
{0xE6,24,{0x00,0x2C,0x34,0x3F,0x47,0x4F,0x5A,0x6A,0x74,0x83,0x8C,0x94,0x67,0x62,0x5E,0x52,0x43,0x32,0x26,0x20,0x17,0x0B,0x03,0x03}},
{0x00,1,{0xB3}},
{0xC0,1,{0x88}},
{0x00,1,{0xA0}},
{0xB3,7,{0x33,0x04,0x38,0x07,0x80,0x00,0x20}},
{0x00,1,{0xE1}},
{0xF5,1,{0x16}},
{0x00,1,{0x81}},
{0xF5,1,{0x16}},
{0x00,1,{0x83}},
{0xF5,1,{0x16}},
{0x00,1,{0x80}},
{0xC4,1,{0x81}},
{0x00,1,{0x81}},
{0xA5,1,{0x01}},
{0x00,1,{0xA0}},
{0xC1,3,{0x00,0xC0,0x11}},
{0x00,1,{0xB0}},
{0xC5,4,{0x00,0x00,0x01,0x00}},
{0x00,1,{0x00}},
{0xFF,3,{0x00,0x00,0x00}},
{0x51,1,{0xFF}},
{0x53,1,{0x2C}},
{0x55,1,{0x01}},
{0x36,1,{0x08}},
{0x29,0,{0x00}},
{REGFLAG_DELAY, 10, {0}},
{0x11,0,{0x00}},
{0xffd,1,{0x08}},
{REGFLAG_DELAY, 120, {0}},
{REGFLAG_END_OF_TABLE, 00, {}},
};

static struct LCM_setting_table lcm_sleep_in_setting[] =
{
	// Display off sequence
	{0x28, 0, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	// Sleep Mode On
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
    {0xff0,6,{0x14,0x00,0x10,0x00,0x15,0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] =
{
	// Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 20, {}},	
    // Sleep Out
	{0x11, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

#ifndef BUILD_LK


//static struct task_struct *my_thread=NULL;
#define MAX_CMD             500
#define COMMAND_BUF_SIZE    (69*MAX_CMD*4)


//static struct LCM_setting_table lcm_code[MAX_CMD];
//static struct LCM_setting_table_V2 lcm_code_V2[MAX_CMD];

static union LCM_code_table code_table[MAX_CMD];
static struct LCM_cfg lcm_cfg;
static unsigned char   cmd_buf[COMMAND_BUF_SIZE];

char *FileName = "/sdcard/cmd.txt";
char *FileNameRD = "/sdcard/cmdrd.txt";
char *FileNameConfig = "/sdcard/CFG.txt";

char* str_sep(char** ppbuf, unsigned char seperate_char){

    char* pstr = *ppbuf;
    char* pstr_return = *ppbuf;

    if ( *ppbuf == 0 ) {
        return 0;
    }

    while ((*pstr != seperate_char) && (*pstr != 0)) {
        pstr++;
    }

    if (*pstr==0) {
        *ppbuf = 0;
    }
    else{
        *pstr = 0;
        *ppbuf = ++pstr;
    }

    return pstr_return;
}

int process_cfg_line(char* strcmd, struct LCM_cfg* lcm_cfg )
{
    if(strstr(strcmd,"cmd_mode")){
        sscanf( strcmd, "cmd_mode:%x", &lcm_cfg->mode );
        printk("Code mode = %x\n", lcm_cfg->mode );
        return lcm_cfg->mode;
    }else if( strstr(strcmd,"exe_cmd_num")){
        sscanf( strcmd, "exe_cmd_num:%x", &lcm_cfg->exe_num );
        printk("exe_cmd_num = %x\n", lcm_cfg->exe_num );
        return lcm_cfg->exe_num;
    }
}

int process_cmd_line(char* strcmd, union LCM_code_table* lcm_tbl, struct LCM_cfg* lcm_cfg  )
{
    char *str, *pstr, str_param[1000];
    
    int i=0,j=0;
    pstr = str_param;

    if( 0== lcm_cfg->mode&0x0f ){
        sscanf( strcmd, "{%x,%d,{%s}},", &lcm_tbl->tbl_v1.cmd,&lcm_tbl->tbl_v1.count, pstr );
    //    printk("Param list = %s\n",str_param);

        while( 0!=( str = str_sep(&pstr,',')) )
        {
            printk("param %d = %s\n", i, str);
            if (0 != *str) {
                sscanf(str, "0x%x", &lcm_tbl->tbl_v1.para_list[i]);
                i++;
                if (i == lcm_tbl->tbl_v1.count) {
                    break;
                }
            }
            else
                break;
        }

    } else if( 1 == lcm_cfg->mode&0x0f ){
        sscanf( strcmd, "{%x,%s}", &lcm_tbl->tbl_v2.cmd, pstr );
        i = 1;
        printk("pstr=%s\nLeave str=",pstr);
        while( 0!=( str = str_sep(&pstr,',')) )
        {
            if (0 != *str) {
                printk("%s ",str);
                sscanf(str, "%x", &lcm_tbl->tbl_v2.para_list[i]);
                i++;
            }
            else
                break;
        }
        lcm_tbl->tbl_v2.count = i;
        lcm_tbl->tbl_v2.para_list[0] = lcm_tbl->tbl_v2.cmd;

        printk("\ncmd=%x,",lcm_tbl->tbl_v2.cmd);
        printk("param len=%d\npara_list[]=",lcm_tbl->tbl_v2.count);

        for ( j=0; j<i-1; j++) {
            printk("0x%08x ",lcm_tbl->tbl_v2.para_list[j]);
        }
        printk("\n");
    }

    return lcm_tbl->tbl_v2.cmd;
}

int parser_cfg_file(char* p_buf, int buf_len, struct LCM_cfg* lcm_cfg )
{
    char* str;
    int i=0;
    while( 0!=( str = str_sep(&p_buf,'\n')) )
    {
        printk("line %d = %s\n", i, str);

        process_cfg_line(str, lcm_cfg);
        i++;
    }

    return i;
}

int parser_cmd_file(char* p_buf, int buf_len,union LCM_code_table* lcm_code[], struct LCM_cfg* lcm_cfg )
{
    char* str;
    int i=0;

    if ( strlen(p_buf)!=buf_len ) {
        printk("MSS_error: Buf length is not same to buffer.\n");
    }
    while (0 != (str = str_sep(&p_buf, '\n')))
    {
        printk("line %d = %s", i, str);
        //process_cmd_line(str, &lcm_code[i], lcm_cfg);
        process_cmd_line(str, &code_table[i], lcm_cfg);
        i++;
    }

    return i;
}


static int load_file( char *FileName, unsigned char* buf )
{
    static struct inode     *inode = NULL;
    static struct file      *fp;
    static mm_segment_t     fs;
    static loff_t           pos=0;

    int                     iSize, cmd_num,i;

    fs=get_fs();
    set_fs(KERNEL_DS);

    fp = filp_open( FileName, O_RDONLY, 0644);

	if(IS_ERR(fp)){
		printk("open file error\n");
		return -1;
	}

    inode = fp->f_dentry->d_inode;
    iSize = inode->i_size;
    printk("file(%s) size=%d\n", FileName, iSize);
    iSize = (iSize < COMMAND_BUF_SIZE) ? iSize:COMMAND_BUF_SIZE;
    printk("file(%s) size=%d\n", FileName, iSize);

    pos = 0;
    i = vfs_read(fp,(char*)buf,iSize,&pos);
    printk("Read file pos =%d,read num=%d\n", pos, i  );

    pos = i;

    if ( i < iSize ) {
        i = vfs_read(fp,(char*)buf+i,iSize-i,&pos);
        printk("Continue Read file pos =%d,read num=%d\n", pos, i  );
    }

    filp_close(fp, 0);
    set_fs(fs);

    buf[iSize] = 0;

	return iSize;
}
#endif

#define CTRL_PWR_NUM        6

static void ctrl_RST( unsigned char ctrl, unsigned char delay )
{
    #if defined(BUILD_LK)
    printf("r69338 %s\n", __func__);
    #else
    printk("r69338 %s\n", __func__);
    #endif
    mt_set_gpio_mode(131, 0);   //lcm reset control
    mt_set_gpio_dir(131,1);
    mt_set_gpio_out(131,ctrl);   
    lcm_util.mdelay(delay*10+5);    
}

static void ctrl_VSP( unsigned char ctrl, unsigned char delay )
{
    #if defined(BUILD_LK)
    printf("r69338 %s\n", __func__);
    #else
    printk("r69338 %s\n", __func__);
    #endif
	mt_set_gpio_mode(125,0);   //VSP  on
	mt_set_gpio_dir(125,1);
	mt_set_gpio_out(125,ctrl); 
    lcm_util.mdelay(delay*10+5);   
}

static void ctrl_VSN( unsigned char ctrl, unsigned char delay  )
{
    #if defined(BUILD_LK)
    printf("r69338 %s\n", __func__);
    #else
    printk("r69338 %s\n", __func__);
    #endif
	mt_set_gpio_mode(117,0);   //VSN  on
	mt_set_gpio_dir(117,1);
	mt_set_gpio_out(117,ctrl);
    lcm_util.mdelay(delay*10+5);   
}

static void ctrl_BL( unsigned char ctrl, unsigned char delay  )
{
    #if defined(BUILD_LK)
    printf("r69338 %s\n", __func__);
    #else
    printk("r69338 %s\n", __func__);
    #endif
	mt_set_gpio_mode(174,0);   //BL power
	mt_set_gpio_dir(174,1);
	mt_set_gpio_out(174,ctrl);   
    lcm_util.mdelay(5);
	mt_set_gpio_mode(129,0);   //BL enable
	mt_set_gpio_dir(129,1);
	mt_set_gpio_out(129,ctrl); 
    lcm_util.mdelay(delay*10+5);
}



static void ctrl_IOVCC( unsigned char ctrl, unsigned char delay  )
{
    #if defined(BUILD_LK)
    printf("r69338 %s\n", __func__);
    #else
    printk("r69338 %s\n", __func__);
    #endif
	mt_set_gpio_mode(126,0);   //vci +iovcc on
	mt_set_gpio_dir(126,1);
	mt_set_gpio_out(126,ctrl);   //
    lcm_util.mdelay(delay*10+5);
}

static void ctrl_VCI( unsigned char ctrl, unsigned char delay  )
{
    #if defined(BUILD_LK)
    printf("r69338 %s\n", __func__);
    #else
    printk("r69338 %s\n", __func__);
    #endif
    mt_set_gpio_mode(126,0);   //vci +iovcc on
	mt_set_gpio_dir(126,1);
	mt_set_gpio_out(126,ctrl);   //
    lcm_util.mdelay(delay*10+5);
}

static void (*ctrl_pwr[])(unsigned char ctrl, unsigned char delay)={
    ctrl_IOVCC,ctrl_VCI,ctrl_VSP,ctrl_VSN,ctrl_RST,ctrl_BL
};

unsigned int lcm_read_id(void);

static void reg_read(unsigned short cmd_num, unsigned char * buf)
{
#ifndef BUILD_LK
    unsigned int id = 0,i,j,cmd,cmd_re;
    unsigned char buffer[130];
    unsigned char str[1024],sstr[1024];
    unsigned int array[16];    

    fp=filp_open(FileNameRD,O_RDWR|O_CREAT,0644);

    if(IS_ERR(fp)){
        printk("Create %s error\n", FileNameRD );
        return -1;
    }else{
        printk("Create or open %s OK.\n", FileNameRD);  
    }

    fs=get_fs();
    set_fs(KERNEL_DS);
    pos = 0;

    sprintf(str,"Command reading... \n");

    for (i=0; i<cmd_num; i+=2) { //buf[i]=0xmm,0xnn: mm is number, nn is command
        cmd = buf[i+1];
        cmd_re = buf[i];
        array[0] = 0x00003700|((cmd_re<<16)&0xff0000);
        dsi_set_cmdq(array, 1, 1);

        read_reg_v2(cmd, buffer, cmd_re);

        sprintf(sstr,"CMD = 0x%x,Read Len=%d, Value=", cmd,cmd_re);
        strcat(str,sstr);

        for( j=0;j<cmd_re;j++ ){
            sprintf(sstr,"0x%x,", buffer[j]);
            strcat(str,sstr);
        }
        strcat(str,"\n");

        #if defined(BUILD_LK)
        printf("r69338 %s, id1 = 0x%s\n", __func__, str);
        #else
        printk("r69338 %s, id1 = 0x%s\n", __func__, str);
        #endif
        vfs_write(fp,str,strlen(str),&pos);
    }

    set_fs(fs);
    MDELAY(3);

    filp_close(fp,NULL);
#endif
}

static void push_table(union LCM_code_table *table, unsigned int count, struct LCM_cfg* lcm_cfg, unsigned char force_update)
{
    unsigned int i,j,k,l;

    unsigned int cmd;
    unsigned char pwr_index,pwr_ctrl,pwr_delay;

    #if defined(BUILD_LK)
    printf("r69338 %s,%d\n", __func__,count);
    #else
    printk("r69338 %s,%d\n", __func__,count);
    #endif

    for(i = 0; i < count; i++)
    {

        if( FORMAT_V2 == lcm_cfg->mode){
            k=table[i].tbl_v2.para_list[2];
            l = table[i].tbl_v2.para_list[1];
            cmd = table[i].tbl_v2.cmd;

            #if defined(BUILD_LK)
            printf("%s r69338 Format V2,cmd=%x,param[0]=%x\n", __func__,cmd,table[i].tbl_v2.para_list[0]);
            #else
            printk("%s r69338 Format V2,cmd=%x,param[0]=%x\n", __func__,cmd,table[i].tbl_v2.para_list[0]);
            #endif
        }
        else if( FORMAT_V1 == lcm_cfg->mode){
            k=table[i].tbl_v1.para_list[0];
            l=table[i].tbl_v1.count;
            cmd = table[i].tbl_v1.cmd;
            #if defined(BUILD_LK)
            printf("%s r69338 Format V1,cmd=%x\n", __func__,cmd);
            #else
            printk("%s r69338 Format V1,cmd=%x\n", __func__,cmd);
            #endif
        }


        switch (cmd)
        {
        case REGFLAG_RD:
            if( FORMAT_V2 == lcm_cfg->mode){
                reg_read(table[i].tbl_v2.para_list[1], table[i].tbl_v2.para_list+2);
            }
            else if( FORMAT_V1 == lcm_cfg->mode){
                reg_read(table[i].tbl_v1.count, table[i].tbl_v1.para_list);
            }
            break;

        case REGFLAG_PWR:
            if( FORMAT_V2 == lcm_cfg->mode){
                k=2;
                l = table[i].tbl_v2.para_list[1];
            }
            else if( FORMAT_V1 == lcm_cfg->mode){
                k=0;
                l=table[i].tbl_v1.count;
            }

            for (j=k;j<l+k;j+=2) { //0xmn 0xll n is index, m is delay, ll is ctrl
                if( FORMAT_V2 == lcm_cfg->mode){
                    pwr_index= table[i].tbl_v2.para_list[j]&0x0f;
                    pwr_ctrl = table[i].tbl_v2.para_list[j+1]&0xff;
                    pwr_delay = (table[i].tbl_v2.para_list[j]>>4)&0x0f;

                    #if defined(BUILD_LK)
                    printf("r69338 %s,%d,%d,%d\n", __func__,pwr_index,pwr_ctrl,pwr_delay);
                    #else
                    printk("r69338 %s,%x,%x,%d,%d,%d\n", __func__,table[i].tbl_v2.para_list[j],table[i].tbl_v2.para_list[j+1],pwr_index,pwr_ctrl,pwr_delay);
                    #endif
                }
                else if( FORMAT_V1 == lcm_cfg->mode){
                    pwr_index= table[i].tbl_v1.para_list[j]&0x0f;
                    pwr_ctrl = table[i].tbl_v1.para_list[j+1]&0xff;
                    pwr_delay = (table[i].tbl_v1.para_list[j]>>4)&0x0f;

                    #if defined(BUILD_LK)
                    printf("r69338 %s,%d,%d,%d\n", __func__,pwr_index,pwr_ctrl,pwr_delay);
                    #else
                    printk("r69338 %s,%x,%x,%d,%d,%d\n", __func__,table[i].tbl_v1.para_list[j],table[i].tbl_v1.para_list[j+1],pwr_index,pwr_ctrl,pwr_delay);
                    #endif
                }

                if ( ctrl_pwr[pwr_index] ) {
                    (*ctrl_pwr[pwr_index])(pwr_ctrl, pwr_delay);
                }
            }

            break;
        case REGFLAG_VSP:
            ctrl_VSP(l,k);        
            break;
        case REGFLAG_VSN:
            ctrl_VSN(l,k);           
            break;
        case REGFLAG_IOVCC:
            ctrl_IOVCC(l,k);            
            break;
        case REGFLAG_VCI:
            ctrl_VCI(l,k);            
            break;
        case REGFLAG_BL:
            ctrl_BL(l,k);            
            break;
        case REGFLAG_RST:
            ctrl_RST(l,k);            
            break;

        case REGFLAG_DELAY :
            lcm_util.mdelay(l);
            break;

        case REGFLAG_END_OF_TABLE :
            break;

        default:
            if ( FORMAT_V1 == lcm_cfg->mode ) {
                dsi_set_cmdq_V2(cmd, table[i].tbl_v1.count, table[i].tbl_v1.para_list, force_update);
            }else if( FORMAT_V2 == lcm_cfg->mode ){
                dsi_set_cmdq(table[i].tbl_v2.para_list,table[i].tbl_v2.count-1,force_update);
            }
        }
    }
}


static void init_lcm_registers(void)
{
    //memcpy(  );
    union LCM_code_table *tbl= (union LCM_code_table *)lcm_initialization_setting;
    struct LCM_cfg cfg;
    cfg.mode = FORMAT_V1;

	push_table( tbl, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), &cfg, 1);
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

	//enable tearing-free
	//params->dbi.te_mode 		= LCM_DBI_TE_MODE_VSYNC_ONLY;
	//params->dbi.te_edge_polarity	= LCM_POLARITY_RISING;

	#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
	#else
	params->dsi.mode   = BURST_VDO_MODE; //SYNC_EVENT_VDO_MODE;  //SYNC_PULSE_VDO_MODE;SYNC_EVENT_VDO_MODE; 
	#endif

	// DSI
	/* Command mode setting */
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
	//because DSI/DPI HW design change, this parameters 
	//should be 0 when video mode in MT658X; or memory leakage
	params->dsi.intermediat_buffer_num = 0;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;


	params->dsi.vertical_sync_active	= 0x1;//
	params->dsi.vertical_backporch		= 3;// VBP(lcm)=vertical_sync_active+vertical_backporch
	params->dsi.vertical_frontporch		= 4; // VFP
	params->dsi.vertical_active_line	= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active	= 28;// 50  2
	params->dsi.horizontal_backporch	= 40;
	params->dsi.horizontal_frontporch	= 220;
	params->dsi.horizontal_active_pixel	= FRAME_WIDTH;
	params->dsi.pll_select=0;	//0: MIPI_PLL; 1: LVDS_PLL

	params->dsi.compatibility_for_nvk=0;
	params->dsi.HS_PRPR=6;
	params->dsi.LPX=8; 
	params->dsi.HS_PRPR=5;
	params->dsi.HS_TRAIL=13;
	params->dsi.CLK_TRAIL = 10;
	//Bit rate calculation
	//1 Every lane speed
	params->dsi.pll_div1=0;//0;// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
	params->dsi.pll_div2=1;//1;// div2=0,1,2,3;div1_real=1,2,4,4	
	params->dsi.fbk_div =25;//72;58;48;34;19;0x12 fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

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
	printf("r69338 %s, id1 = 0x%x\n", __func__, id);
	#else
	printk("r69338 %s, id1 = 0x%x\n", __func__, id);
	#endif
	return 1;
}

static struct LCM_cfg cfg;
//********************************************************
void lcm_init(void)
{
    union LCM_code_table *tbl= (union LCM_code_table *)lcm_initialization_setting;
    cfg.mode = FORMAT_V1;

#ifndef BUILD_LK
    int cmd_num;
    cmd_num = load_file(FileNameConfig, cmd_buf );

    if ( cmd_num ) {
        cmd_num = parser_cfg_file( cmd_buf, cmd_num, &lcm_cfg  );
        printk("LCM code format = %d,exe_command_num=%x\n", lcm_cfg.mode, lcm_cfg.exe_num );
    }

    cmd_num = load_file( FileName, cmd_buf ); //return is filename size

    if ( cmd_num ) {
        cmd_num = parser_cmd_file( cmd_buf, cmd_num, code_table, &lcm_cfg ); //return is command number
        printk("cmd num = %d\n", cmd_num );

    }
    //cmd_num = -1;

    //cfg.mode = FORMAT_V1;
    if ( -1 == cmd_num) {
        printk("R69338: enter %s: No command file, use inside only.\n", __func__);
        push_table(tbl, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), &cfg, 1);
    }
    else if ( cmd_num > 0 ) {
        cmd_num = cmd_num<lcm_cfg.exe_num?cmd_num:lcm_cfg.exe_num;
        push_table( (union LCM_code_table *)code_table, cmd_num, &lcm_cfg, 1);
        printk("R69338: Send command from external file, cmd = %d.\n", cmd_num );
    } else {
        printk("R69338: unknow error, cmd = %d.\n", cmd_num );
    }

#else
    push_table(tbl, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), &cfg, 1);  
#endif
}

static void lcm_suspend(void)
{
    union LCM_code_table *tbl= (union LCM_code_table *)lcm_sleep_in_setting;
    struct LCM_cfg cfg;
    cfg.mode = FORMAT_V1;

	#ifndef BUILD_LK
	printk("r69338 enter %s\n", __func__);
	#endif
	push_table(tbl, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table),&cfg, 1);
	//init_lcm_registers();

	#ifndef BUILD_LK
	//hello_exit();
	#endif
}

static unsigned int lcm_compare_id(void);
static void lcm_resume(void)
{	
    #ifndef BUILD_LK
    printk("r69338 enter %s\n", __func__);
    #endif
	lcm_init( );
    #ifndef BUILD_LK
    //hello_init();
    #endif 
}
         
#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
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
	printf("r69338 %s, id1 = 0x%x\n", __func__, id);
	#else
	printk("r69338 %s, id1 = 0x%x\n", __func__, id);
	#endif

	return 1;
}

static unsigned int lcm_esd_check(void)
{
	#ifndef BUILD_LK
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
	printk("kevin: lcm_esd_recover, and enter LCM_init function");
    lcm_init( );
#endif
	return TRUE;
}


LCM_DRIVER otm1906_fhd_dsi_cmd_lcm_drv = 
{
	.name		= "otm1906_fhd_dsi_cmd",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id   = lcm_compare_id,
	//.esd_check 	= lcm_esd_check,
	//.esd_recover 	= lcm_esd_recover,		
	#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
	#endif
};
