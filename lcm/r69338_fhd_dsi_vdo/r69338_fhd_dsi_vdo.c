/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
#ifndef BUILD_LK
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/slab.h>  
#include <linux/kobject.h>  
#include <linux/platform_device.h> 
#endif

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

#include "lcm_drv.h"
#include "lcm_debug.h"
#include "code_code.h"
#include "lcm_init_code.h"

static LCM_PARAMS *g_lcm_params=0;
LCM_UTIL_FUNCS lcm_util = {0};

#ifndef BUILD_LK

static struct dsi_debug debug;
static unsigned int lcm_compare_id(void);
static void set_lcm_params( LCM_PARAMS *params, struct LCM_cfg* lcm_cfg );
static int mdss_dsi_panel_cmds_send( enum read_write rw)
{
	return 1;
}

static int parse_input(const char *buf, union LCM_code_table *plcm_code, unsigned char *pre_buf, int *len )
{
	int retval;
	int index = 0,i,j;
    unsigned int ch0,ch1,ch2,ch3;
	char *input = (char *)buf;
	char *token;
	unsigned long value;
	printk("r69338 enter %s, buf=%s\n", __func__, buf );
	input[strlen(input)] = '\0';

    while( 0!=( token = str_sep(&input,' ')) )
    {
        if (0 != *token) {
            sscanf(token, "%x", &debug.buffer[index]);
        }
        else
            break;

        printk("param %d = %s, hex value = %x\n", index, token,debug.buffer[index]);
        index++;
        if (index >= BUFFER_LENGTH) {
            break;
        }
    }

    printk("index = %d \n", index);

    if (index>1) { //fill command packet
        debug.length = index - 1;
        if(0x05 == debug.buffer[0]) {// 05 29  ==> 0x00290500     
            ch0 = debug.buffer[1];
            debug.lcm_code_table.tbl_v2.para_list[0]= 0x0500|((ch0<<16)&0xff0000);
            debug.lcm_code_table.tbl_v2.count = 1;
            debug.lcm_code_table.tbl_v2.cmd = debug.lcm_code_table.tbl_v2.para_list[0];
            printk("ch0=%x,para_list[0]=%x.\n",ch0,debug.lcm_code_table.tbl_v2.para_list[0] ); 
        }else if(0x15 == debug.buffer[0]) {// 15 36 80 ==> 0x80361500
            ch0 = debug.buffer[1];
            ch1 = debug.buffer[2];
            debug.lcm_code_table.tbl_v2.para_list[0]= 0x1500|((ch0<<16)&0xff0000)|((ch1<<24)&0xff000000);
            debug.lcm_code_table.tbl_v2.count = 1;
            debug.lcm_code_table.tbl_v2.cmd = debug.lcm_code_table.tbl_v2.para_list[0];
            printk("ch0=%x,para_list[0]=%x.\n",ch0,debug.lcm_code_table.tbl_v2.para_list[0] );       
        }else if(0x39 == debug.buffer[0]) {// 39 2A 00 01 02 03 ==> 0x00003902,0x0201002A,0x00000003
            debug.lcm_code_table.tbl_v2.para_list[0] = 0x3902|((index-1)<<16);
            j=1;
            if ( index >4 ) {
                for (i = 1; i < index; i += 4) {
                    ch0 = debug.buffer[i];
                    ch1 = debug.buffer[i+1];
                    ch2 = debug.buffer[i+2];
                    ch3 = debug.buffer[i+3];
                    debug.lcm_code_table.tbl_v2.para_list[j]=(ch3<<24)|(ch2<<16)|(ch1<<8)|ch0;
                    printk("para_list[%d]=%x.\n",j,debug.lcm_code_table.tbl_v2.para_list[j] );
                    j++;
                }

                if ((index-1)%4) {
                    ch0 = debug.buffer[i-4];
                    ch1 = debug.buffer[i-3];
                    ch2 = debug.buffer[i-2];
                    ch3 = debug.buffer[i-1];
                    debug.lcm_code_table.tbl_v2.para_list[j]=(ch3<<24)|(ch2<<16)|(ch1<<8)|ch0;
                    debug.lcm_code_table.tbl_v2.count = j+1;
                    printk("para_list[%d]=%x.\n",j,debug.lcm_code_table.tbl_v2.para_list[j] );
                }
                else{
                    debug.lcm_code_table.tbl_v2.count = j;
                }
            }
            else {
                ch0 = debug.buffer[1];
                ch1 = debug.buffer[2];
                ch2 = debug.buffer[3];
                ch3 = debug.buffer[4];
                debug.lcm_code_table.tbl_v2.para_list[1]=(ch3<<24)|(ch2<<16)|(ch1<<8)|ch0;
                debug.lcm_code_table.tbl_v2.count = 2;
                printk("para_list[%d]=%x.\n",1,debug.lcm_code_table.tbl_v2.para_list[1] );
            }

            debug.lcm_code_table.tbl_v2.cmd = debug.lcm_code_table.tbl_v2.para_list[0];
            printk("Total count = %d.\n", debug.lcm_code_table.tbl_v2.count );
        }else if(0x23 == debug.buffer[0]) {// 29 2A 00 01 02 03 ==> 0x04B02300
            ch0 = debug.buffer[1];
            ch1 = debug.buffer[2];
            debug.lcm_code_table.tbl_v2.para_list[0]= 0x2300|((ch0<<16)&0xff0000)|((ch1<<24)&0xff000000);
            debug.lcm_code_table.tbl_v2.count = 1;
            debug.lcm_code_table.tbl_v2.cmd = debug.lcm_code_table.tbl_v2.para_list[0];
            printk("ch0=%x,para_list[0]=%x.\n",ch0,debug.lcm_code_table.tbl_v2.para_list[0] );                
        }else if(0x29 == debug.buffer[0]) {// 29 2A 00 01 02 03 ==> 0x00002902,0x0201002A,0x00000003,                       
            debug.lcm_code_table.tbl_v2.para_list[0] = 0x2902|((index-1)<<16);
            j=1;
            if ( index >4 ) {
                for (i = 1; i < index; i += 4) {
                    ch0 = debug.buffer[i];
                    ch1 = debug.buffer[i+1];
                    ch2 = debug.buffer[i+2];
                    ch3 = debug.buffer[i+3];
                    debug.lcm_code_table.tbl_v2.para_list[j]=(ch3<<24)|(ch2<<16)|(ch1<<8)|ch0;
                    printk("para_list[%d]=%x.\n",j,debug.lcm_code_table.tbl_v2.para_list[j] );
                    j++;
                }

                if ((index-1)%4) {
                    ch0 = debug.buffer[i-4];
                    ch1 = debug.buffer[i-3];
                    ch2 = debug.buffer[i-2];
                    ch3 = debug.buffer[i-1];
                    debug.lcm_code_table.tbl_v2.para_list[j]=(ch3<<24)|(ch2<<16)|(ch1<<8)|ch0;
                    debug.lcm_code_table.tbl_v2.count = j+1;
                    printk("para_list[%d]=%x.\n",j,debug.lcm_code_table.tbl_v2.para_list[j] );
                }
                else{
                    debug.lcm_code_table.tbl_v2.count = j;
                }
            }
            else {
                ch0 = debug.buffer[1];
                ch1 = debug.buffer[2];
                ch2 = debug.buffer[3];
                ch3 = debug.buffer[4];
                debug.lcm_code_table.tbl_v2.para_list[1]=(ch3<<24)|(ch2<<16)|(ch1<<8)|ch0;
                debug.lcm_code_table.tbl_v2.count = 2;
                printk("para_list[%d]=%x.\n",1,debug.lcm_code_table.tbl_v2.para_list[1] );
            }

            debug.lcm_code_table.tbl_v2.cmd = debug.lcm_code_table.tbl_v2.para_list[0];
            printk("Total count = %d.\n", debug.lcm_code_table.tbl_v2.count );
        }else if( 0x14 == debug.buffer[0] ){
            //data type = 0x14 / 24, command = 0xc7, parameter = 0x00, return packet type = long, read length = 30 bytes
            //# echo 14 c7 00 1 1e > read      
            debug.lcm_code_table.tbl_v2.cmd = debug.buffer[1];
            debug.lcm_code_table.tbl_v2.count = debug.buffer[4];
            debug.rlength = debug.buffer[4];
        }else if( 0x06 == debug.buffer[0] ){
            debug.lcm_code_table.tbl_v2.cmd = debug.buffer[1];
            debug.lcm_code_table.tbl_v2.count = debug.buffer[4];
            debug.rlength = debug.buffer[4];
        }

        printk("cmd=%x,count=%x,buf[0]=%x,len=%d\n",debug.lcm_code_table.tbl_v2.cmd,debug.lcm_code_table.tbl_v2.count, debug.buffer[0],debug.length );   
    }
    else{
        printk("%s: Failed to convert from string to hex number\n", __func__);
    }

    return index;
}

static int do_read(void)
{
	printk("r69338 enter %s.\n", __func__);
    debug.rlength=host_read(debug.lcm_code_table.tbl_v2.cmd, debug.lcm_code_table.tbl_v2.count, 1, debug.read_buffer );
    return debug.rlength;
}

static int do_write(void)
{
    unsigned int temp;
	printk("r69338 enter %s.\n", __func__);
    printk("cmd=%x,count=%x.\n",debug.lcm_code_table.tbl_v2.para_list[0],debug.lcm_code_table.tbl_v2.count );
    //debug.buffer
    if (0xff == debug.buffer[0]) {
        g_host_cfg.input_mode = debug.buffer[1]; //0x00 is default, 0x01 is from host control
        if ( g_host_cfg.input_mode ) {
            g_host_cfg.input_board_ver= debug.buffer[2];
            g_host_cfg.input_lpwg= debug.buffer[3];
            g_host_cfg.input_panel_vendor= debug.buffer[4];
            g_host_cfg.input_cmd_format= debug.buffer[5];

            if ( g_host_cfg.input_board_ver ) {
                g_lcm_cfg.mode |= 0x4000; 
            }else
                g_lcm_cfg.mode &= 0xBfff;

            
            if ( g_host_cfg.input_lpwg ) {
                g_lcm_cfg.mode |= 0x0100; 
            }else
                g_lcm_cfg.mode &= 0xFEff;

            temp = debug.buffer[4]<<4+debug.buffer[5];
            temp |=0xff00;
            g_lcm_cfg.mode &=temp; 

            printk("input mode %x,temp=%x \n", g_lcm_cfg.mode, temp );
        }
    } else
    dsi_set_cmdq(debug.lcm_code_table.tbl_v2.para_list, debug.lcm_code_table.tbl_v2.count, 1);

	return 1;
}

static ssize_t sysfs_show_read(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	unsigned char ii;
	unsigned char cnt;
	unsigned char count=0;
	printk("r69338 enter %s, buf=%s\n", __func__, buf);
	for (ii = 0; ii < debug.rlength - 1; ii++) {
		cnt = snprintf(buf, PAGE_SIZE - count, "%02x ",
				debug.read_buffer[ii]);
		buf += cnt;
		count += cnt;
	}

	cnt = snprintf(buf, PAGE_SIZE - count, "%02x\n",
			debug.read_buffer[ii]);
	buf += cnt;
	count += cnt;

	return count;
}

static ssize_t sysfs_store_read(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int retval = -EINVAL;
	printk("r69338 enter %s, buf=%s\n", __func__, buf);
	retval = parse_input(buf, &debug.lcm_code_table, debug.buffer, &debug.length);

    if ( retval ) {
        retval = do_read();
    }

	debug.length = 0;
    memset( debug.buffer, 0, BUFFER_LENGTH );

	return count;
}

static ssize_t sysfs_store_write(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int retval = -EINVAL;
	printk("r69338 enter %s, buf=%s, count=%d\n", __func__, buf, count); // 15 36 80, 29 B0 04, 39 2a 01 00 02 04,
	parse_input(buf, &debug.lcm_code_table, debug.buffer, &debug.length);

	if (debug.length)
		retval = do_write();
	else
		printk("%s: No valid command to send\n", __func__);

	debug.length = 0;
    memset( debug.buffer, 0, BUFFER_LENGTH );

	if (retval < 0)
		return retval;
	else
		return count;
}

static DEVICE_ATTR(read, (S_IRUGO | S_IWUGO),sysfs_show_read, sysfs_store_read);
static DEVICE_ATTR(write, S_IWUGO,NULL, sysfs_store_write);

static struct attribute *sysfs_attrs[] = {
	&dev_attr_read.attr,
	&dev_attr_write.attr,
	NULL,
};

static struct attribute_group sysfs_attr_group = {
	.attrs = sysfs_attrs,
};
#endif

static void init_lcm_registers(void)
{
    union LCM_code_table *tbl= (union LCM_code_table *)lcm_init_code_338_V1;
    local_cfg.mode |= FORMAT_V1;

	push_table( tbl, sizeof(lcm_init_code_338_V1) / sizeof(struct LCM_setting_table), &local_cfg, 1);
}

//********************************************************
void lcm_init(void)
{
#ifndef BUILD_LK
    int ret;  
    printk("R69338 enter lcm_init.\n");
	debug.sysfs_dir = kobject_create_and_add(SYSFS_FOLDER, NULL);
       

    if(debug.sysfs_dir == NULL){  
        ret = -ENOMEM;  
        goto kobj_err;  
    }  
  
	ret = sysfs_create_group(debug.sysfs_dir, &sysfs_attr_group);
    if(ret < 0){  
        goto file_err;  
    }  
    printk("R69338 quit, and load debug interface successfully.\n");
    return 0;  
  
file_err:  
     kobject_del(debug.sysfs_dir);    
kobj_err:  
    printk("R69338 quit, and load debug interface fail.\n");
    return ret;  

#else
    union LCM_code_table *tbl= (union LCM_code_table *)lcm_init_code_338_V1;
    int sleep_in_num=0;

    local_cfg.mode |= FORMAT_V1;
    //memcpy( local_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );
    memcpy( local_cfg.ctrl_index, ctrl_fn_v1, sizeof(ctrl_fn_v1) );
    push_table(tbl, sizeof(lcm_init_code_338_V1) / sizeof(struct LCM_setting_table), &local_cfg, 1);
#endif
}


static void lcm_resume(void)
{	

    #ifndef BUILD_LK
    //hello_init();
    #endif 

    union LCM_code_table *tbl= (union LCM_code_table *)lcm_init_code_338_V1;
#ifndef BUILD_LK
    int ret;  
    int cmd_num=0, sleep_in_num=0,sleep_out_num=0;

    local_cfg.mode |= FORMAT_V1;
    memcpy( local_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );

    printk("r69338 enter %s\n", __func__);
    if ( debug.sysfs_dir == NULL ) {
        debug.sysfs_dir = kobject_create_and_add(SYSFS_FOLDER, NULL);
        if(debug.sysfs_dir == NULL){  
            printk("Create the debug interface fail(sysfs_dir).\n");
        }       
    	ret = sysfs_create_group(debug.sysfs_dir, &sysfs_attr_group);
        if(ret < 0){  
            printk("Create the debug interface fail(create group).\n");
        }  
        printk("Load debug interface successfully.\n");   
    }else{
        printk("LCM Driver have created debug interface.\n");
    }

    cmd_num = load_file(lcm_FileNameConfig, lcm_cmd_buf );

    if ( cmd_num >0 ) {

        if ( 0 == g_host_cfg.input_mode ){
            cmd_num = parser_cfg_file( lcm_cmd_buf, cmd_num, &g_lcm_cfg  );
            if ( g_lcm_params ) {
                printk("%s reset lcm params\n", __func__);
                set_lcm_params(g_lcm_params, &g_lcm_cfg);
            }
        }

        if(0x400 == (g_lcm_cfg.mode & 0xC00)){
            memcpy( g_lcm_cfg.ctrl_index, ctrl_fn_v1, sizeof(ctrl_fn_v1) );
            memcpy( local_cfg.ctrl_index, ctrl_fn_v1, sizeof(ctrl_fn_v1) );
        }else if( 0x000 == (g_lcm_cfg.mode & 0xC00)){
            memcpy( g_lcm_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );
        }else{
            memcpy( g_lcm_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );
        }

        printk("LCM code format = %d,exe_command_num=%x\n", g_lcm_cfg.mode, g_lcm_cfg.exe_num );
    }
    else{
        memcpy( g_lcm_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );
    }

    cmd_num = load_file( lcm_FileName, lcm_cmd_buf ); //return is filename size
    if ( cmd_num > 0  ) {
        cmd_num = parser_cmd_file( lcm_cmd_buf, cmd_num, lcm_code_table, &g_lcm_cfg ); //return is command number
        printk("cmd num = %d\n", cmd_num );
    }

    sleep_in_num = load_file( lcm_FileName_sleep_in, lcm_cmd_buf ); //return is filename size
    if ( sleep_in_num >0  ) {
        sleep_in_num = parser_cmd_file( lcm_cmd_buf, sleep_in_num, lcm_code_sleep_in_table, &g_lcm_cfg ); //return is command number
        printk("slee in cmd num = %d\n", sleep_in_num );
    }else{
        printk("read sleep in file error!\n" );
    }

    sleep_out_num = load_file( lcm_FileName_sleep_out, lcm_cmd_buf ); //return is filename size
    if ( sleep_out_num > 0 ) {
        sleep_out_num = parser_cmd_file( lcm_cmd_buf, sleep_out_num, lcm_code_sleep_out_table, &g_lcm_cfg ); //return is command number
        printk("sleep out cmd num = %d\n", sleep_out_num );
    }else{
        printk("read sleep out file error!\n" );
    }

    if ( -1 == cmd_num) {
        printk("enter %s: No command file, use inside only.\n", __func__);
        push_table(tbl, sizeof(lcm_init_code_338_V1) / sizeof(struct LCM_setting_table), &local_cfg, 1);
    }else if ( cmd_num > 0 ) {
        if (0x0000 == (g_lcm_cfg.mode & 0x300)) { // LPWG off, follow normall sequence   
            if (0x10 == (g_lcm_cfg.mode & 0xf0)) { //
                push_table(tbl, sizeof(lcm_init_code_338_V1) / sizeof(struct LCM_setting_table), &g_lcm_cfg, 1);  
                printk("enter %s: use inside R69338 only.\n", __func__);
            }else if(0x00==(g_lcm_cfg.mode&0xf0)){
                cmd_num = cmd_num < g_lcm_cfg.exe_num ? cmd_num : g_lcm_cfg.exe_num;
                push_table( (union LCM_code_table *)lcm_code_table, cmd_num, &g_lcm_cfg, 1);
                printk("Send command from external file, cmd = %d.\n", cmd_num );
            }else if(0x20==(g_lcm_cfg.mode&0xf0)){
                push_table((union LCM_code_table *)lcm_init_1906_V1, sizeof(lcm_init_1906_V1) / sizeof(struct LCM_setting_table), &g_lcm_cfg, 1);  
                printk("enter %s: use inside OTM1906 only.\n", __func__);
            }else{
                printk("don't support this format mode.\n", __func__);
            }
        }else if (0x0100 == (g_lcm_cfg.mode & 0x300)){  //LPWG on, follow LPWG sequence
            if (sleep_out_num) {
                push_table((union LCM_code_table *)lcm_code_sleep_out_table, sleep_out_num, &g_lcm_cfg, 1);
                printk("enter %s: use LPWG sequency.\n", __func__);
            }else{
                printk("No code exe in LPWG mode.\n", __func__);
            }
        }
 
    } else {
        printk("unknow error, cmd = %d.\n", cmd_num );
    }

#else
    push_table(tbl, sizeof(lcm_init_code_338_V1) / sizeof(struct LCM_setting_table), &local_cfg, 1);  
#endif
    #ifndef BUILD_LK
    printk("r69338 quit %s\n", __func__);
    #endif
}

static void lcm_suspend(void)
{

#ifndef BUILD_LK
	printk("r69338 enter %s\n", __func__);
    union LCM_code_table *tbl= (union LCM_code_table *)lcm_sleep_in_code;
    int sleep_in_num=0;

    local_cfg.mode |= FORMAT_V1;
    memcpy( local_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );

    sleep_in_num = load_file(lcm_FileNameConfig, lcm_cmd_buf );

    if ( sleep_in_num >0  ) {
        if ( 0x00==g_host_cfg.input_mode ){
            sleep_in_num = parser_cfg_file( lcm_cmd_buf, sleep_in_num, &g_lcm_cfg  );
            if ( g_lcm_params ) {
                printk("%s reset lcm params\n", __func__);
                set_lcm_params(g_lcm_params, &g_lcm_cfg);
            }
        }
        printk("LCM code format = %d,exe_command_num=%x\n g_host_cfg.input_mode\n", g_lcm_cfg.mode, g_lcm_cfg.exe_num,g_host_cfg.input_mode );
    }
    else{
        printk("read file error!\n" );
    }

    if(0x400 == (g_lcm_cfg.mode & 0xC00)){
            memcpy( g_lcm_cfg.ctrl_index, ctrl_fn_v1, sizeof(ctrl_fn_v1) );
            memcpy( local_cfg.ctrl_index, ctrl_fn_v1, sizeof(ctrl_fn_v1) );
    }else{
            memcpy( g_lcm_cfg.ctrl_index, ctrl_fn_v0, sizeof(ctrl_fn_v0) );
    }

    sleep_in_num = load_file( lcm_FileName_sleep_in, lcm_cmd_buf ); //return is filename size
    if ( sleep_in_num >0  ) {
        sleep_in_num = parser_cmd_file( lcm_cmd_buf, sleep_in_num, lcm_code_sleep_in_table, &g_lcm_cfg ); //return is command number
        printk("slee in cmd num = %d\n", sleep_in_num );
    }
    else{
        printk("read file error!\n" );
    }

	

    if (  -1 == sleep_in_num ) {
        printk("No sleep in command file, use internal sleep sequence.\n");
        push_table(tbl, sizeof(lcm_sleep_in_code) / sizeof(struct LCM_setting_table), &local_cfg, 1);
    }
    else if ( 0x0000 == (g_lcm_cfg.mode & 0x300) ) {
        push_table(tbl, sizeof(lcm_sleep_in_code) / sizeof(struct LCM_setting_table), &local_cfg, 1);
        printk("enter %s: use interanl sleep code.\n", __func__);
    }
    else if (0x0100 == (g_lcm_cfg.mode & 0x300)){  //LPWG on, follow LPWG sequence
        if (sleep_in_num) {
            push_table((union LCM_code_table *)lcm_code_sleep_in_table, sleep_in_num, &g_lcm_cfg, 1);
            printk("enter %s: use LPWG off sequency.\n", __func__);
        }
    }  
	printk("r69338 quit %s\n", __func__);
#endif
    #ifndef BUILD_LK
	//hello_exit();
	#endif
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

char LCDTiming[256]={0x04,0x38,0x07,0x80,0x01,0x04,4,12,15,20,70,70,1,1,25};   //FHD 1080*1920

static void set_lcm_params( LCM_PARAMS *params, struct LCM_cfg* lcm_cfg )
{
    printk("r69338 enter %s\n", __func__);
    if ( lcm_cfg && params ) {
        printk("Reset LCM MIPI timing!\n");
        params->dsi.vertical_sync_active	= lcm_cfg->hsw_vsw & 0xffff;
        params->dsi.vertical_backporch		= lcm_cfg->hbp_vbp&0xffff;
        params->dsi.vertical_frontporch		= lcm_cfg->hfp_vfp&0xffff; 

        params->dsi.horizontal_sync_active	= (lcm_cfg->hsw_vsw>>16)&0xffff;
        params->dsi.horizontal_backporch	= (lcm_cfg->hbp_vbp>>16)&0xffff;
        params->dsi.horizontal_frontporch	= (lcm_cfg->hfp_vfp>>16)&0xffff;

        params->dsi.HS_PRPR=6;
        params->dsi.LPX=8; 
        params->dsi.HS_PRPR=5;
        params->dsi.HS_TRAIL=13;
        params->dsi.CLK_TRAIL = 10;
        params->dsi.pll_div1=0;
        params->dsi.pll_div2=1;	
        params->dsi.fbk_div =36;
    }

}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
    g_lcm_params = params;
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

	params->dsi.horizontal_sync_active	= 12;// 50  2
	params->dsi.horizontal_backporch	= 56;
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
	params->dsi.fbk_div =36;//72;58;48;34;19;0x12 fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

}

unsigned int lcm_read_id(void)
{
	unsigned char buffer[5];
	unsigned int array[16];

	array[0] = 0x00053700;// read id return 5 byte
	dsi_set_cmdq(array, 1, 1);

	array[0] = 0x04B02300;// unlock for reading ID
	dsi_set_cmdq(array, 1, 1);
	MDELAY(50);

	read_reg_v2(0xBF, buffer, 5);

	#if defined(BUILD_LK)
	printf("%s,device id = 0x%x\n", __func__, buffer[2],buffer[3]);
	#else
	printk("%s,device id = 0x%x\n", __func__, buffer[2],buffer[3]);
	#endif
	return 1;
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
    //lcm_init( );
#endif
	return TRUE;
}

LCM_DRIVER r69338_fhd_dsi_vdo_lcm_drv = 
{
	.name		= "r69338_fhd_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id   = lcm_compare_id,
	.esd_check 	= lcm_esd_check,
	//.esd_recover 	= lcm_esd_recover,		
	#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
	#endif
};
