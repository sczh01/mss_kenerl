#ifndef _CODE_CODE_H
#define _CODE_CODE_H

#include "lcm_debug.h"

extern LCM_UTIL_FUNCS lcm_util;

#ifndef BUILD_LK
static char  buf[1]="1";
static struct task_struct *my_thread=NULL;
static struct file *fp;
static mm_segment_t fs;
static loff_t pos;

//static struct task_struct *my_thread=NULL;

union LCM_code_table    lcm_code_table[MAX_CMD];
union LCM_code_table    lcm_code_sleep_in_table[LPWG_CMD];
union LCM_code_table    lcm_code_sleep_out_table[LPWG_CMD];

struct LCM_cfg          lcm_cfg;
unsigned char           lcm_cmd_buf[COMMAND_BUF_SIZE];

char *lcm_FileName = "/sdcard/cmd.txt";
char *lcm_FileNameRD = "/sdcard/cmdrd.txt";
char *lcm_FileNameConfig = "/sdcard/CFG.txt";
char *lcm_FileName_sleep_in = "/sdcard/cmd_sleep_in.txt";
char *lcm_FileName_sleep_out = "/sdcard/cmd_sleep_out.txt";

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
        return (lcm_cfg->mode);
    }else if( strstr(strcmd,"exe_cmd_num")){
        sscanf( strcmd, "exe_cmd_num:%x", &lcm_cfg->exe_num );
        printk("exe_cmd_num = %x\n", lcm_cfg->exe_num );
        return lcm_cfg->exe_num;
    }else if( strstr(strcmd,"hfp_vfp")){
        sscanf( strcmd, "hfp_vfp:%x", &lcm_cfg->hfp_vfp );
        printk("hfp_vfp = %x\n", lcm_cfg->hfp_vfp );
        return lcm_cfg->hfp_vfp;
    }else if( strstr(strcmd,"hbp_vbp")){
        sscanf( strcmd, "hbp_vbp:%x", &lcm_cfg->hbp_vbp );
        printk("hbp_vbp = %x\n", lcm_cfg->hbp_vbp );
        return lcm_cfg->hbp_vbp;
    }else if( strstr(strcmd,"hsw_vsw")){
        sscanf( strcmd, "hsw_vsw:%x", &lcm_cfg->hsw_vsw );
        printk("hsw_vsw = %x\n", lcm_cfg->hsw_vsw );
        return lcm_cfg->hsw_vsw;
    }else if( strstr(strcmd,"freq_pll")){
        sscanf( strcmd, "freq_pll:%x", &lcm_cfg->freq_pll );
        printk("freq_pll = %x\n", lcm_cfg->freq_pll );
        return lcm_cfg->freq_pll;
    }else if( strstr(strcmd,"dsi_set0")){
        sscanf( strcmd, "dsi_set0:%x", &lcm_cfg->dsi_set0 );
        printk("dsi_set0 = %x\n", lcm_cfg->dsi_set0 );
        return lcm_cfg->dsi_set0;
    }
}

int process_cmd_line(char* strcmd, union LCM_code_table* lcm_tbl, struct LCM_cfg* lcm_cfg  )
{
    char *str, *pstr, str_param[1000];
    
    int i=0,j=0;
    pstr = str_param;

    if( 0== (lcm_cfg->mode&0x0f) ){
        sscanf( strcmd, "{%x,%d,{%s}},", &lcm_tbl->tbl_v1.cmd,&lcm_tbl->tbl_v1.count, pstr );
        printk("Format V1: Param list = %s\n",str_param);

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

        printk("cmd=%x,",lcm_tbl->tbl_v1.cmd);
        printk("param len=%d\n",lcm_tbl->tbl_v1.count);

        for ( i=0; i<lcm_tbl->tbl_v1.count; i++) {
            printk("%02x ",lcm_tbl->tbl_v1.para_list[i]);
        }

    } else if( 1 == (lcm_cfg->mode&0x0f) ){
        sscanf( strcmd, "{%x,%s}", &lcm_tbl->tbl_v2.cmd, pstr );
        i = 1;
        printk("\nFormat V2: pstr=%s,Leave str=",pstr);
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

        printk("\ncmd=%x,param len=%d\n",lcm_tbl->tbl_v2.cmd,lcm_tbl->tbl_v2.count);
        printk("para_list[]=");
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

int parser_cmd_file(char* p_buf, int buf_len,union LCM_code_table* lcm_code, struct LCM_cfg* lcm_cfg )
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
        process_cmd_line(str, lcm_code+i, lcm_cfg);
        i++;
    }

    return i;
}


int load_file( char *FileName, unsigned char* buf )
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

void ctrl_RST( unsigned char ctrl, unsigned char delay )
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

void ctrl_VSP( unsigned char ctrl, unsigned char delay )
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

void ctrl_VSN( unsigned char ctrl, unsigned char delay  )
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

void ctrl_BL( unsigned char ctrl, unsigned char delay  )
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



void ctrl_IOVCC( unsigned char ctrl, unsigned char delay  )
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

void ctrl_VCI( unsigned char ctrl, unsigned char delay  )
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

void (*ctrl_pwr[])(unsigned char ctrl, unsigned char delay)={
    ctrl_IOVCC,ctrl_VCI,ctrl_VSP,ctrl_VSN,ctrl_RST,ctrl_BL
};

unsigned int lcm_read_id(void);

void reg_read(unsigned short cmd_num, unsigned char * buf)
{
#ifndef BUILD_LK
    unsigned int id = 0,i,j,cmd,cmd_re;
    unsigned char buffer[130];
    unsigned char str[1024],sstr[1024];
    unsigned int array[16];    

    fp=filp_open(lcm_FileNameRD,O_RDWR|O_CREAT,0644);

    if(IS_ERR(fp)){
        printk("Create %s error\n", lcm_FileNameRD );
        return -1;
    }else{
        printk("Create or open %s OK.\n", lcm_FileNameRD);  
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

int host_read(unsigned char cmd_head, unsigned char re_len, unsigned char packet_type, char *read_buf )
{
#ifndef BUILD_LK
    unsigned int id = 0,i,j,cmd,cmd_re;
    unsigned char buffer[130];
    unsigned int array[16];    

    printk("r69338 enter %s.\n", __func__);
    cmd = cmd_head;
    cmd_re = re_len > 10?10:re_len;

    array[0] = 0x00003700 | ((cmd_re << 16) & 0xff0000);

    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(cmd, buffer, cmd_re);

    printk("CMD = 0x%x,Read Len=%d, Value=", cmd,cmd_re);
    for( j=0;j<cmd_re;j++ ){
        printk("0x%x,", buffer[j]);
        
        read_buf[j] = buffer[j];
    }
    printk("\n", buffer[j]);
    return cmd_re;

#endif
}

void push_table(union LCM_code_table *table, unsigned int count, struct LCM_cfg* lcm_cfg, unsigned char force_update)
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

        if( FORMAT_V2 == (lcm_cfg->mode&0x0f)){
            k=table[i].tbl_v2.para_list[2];
            l = table[i].tbl_v2.para_list[1];
            cmd = table[i].tbl_v2.cmd;

            #if defined(BUILD_LK)
            printf("%s r69338 Format V2,cmd=%x,param[0]=%x\n", __func__,cmd,table[i].tbl_v2.para_list[0]);
            #else
            printk("%s r69338 Format V2,cmd=%x,param[0]=%x,count=%x\n", __func__,cmd,table[i].tbl_v2.para_list[0],table[i].tbl_v2.count);
            #endif
        }
        else if( FORMAT_V1 == (lcm_cfg->mode&0x0f)){
            k=table[i].tbl_v1.para_list[0];
            l=table[i].tbl_v1.count;
            cmd = table[i].tbl_v1.cmd;
            #if defined(BUILD_LK)
            printf("%s r69338 Format V1,cmd=%x\n", __func__,cmd);
            #else
            printk("%s r69338 Format V1,cmd=%x,count=%x\n", __func__,cmd, table[i].tbl_v1.count);
            #endif
        }


        switch (cmd)
        {
        case REGFLAG_RD:
            if( FORMAT_V2 == (lcm_cfg->mode&0x0f)){
                reg_read(table[i].tbl_v2.para_list[1], table[i].tbl_v2.para_list+2);
            }
            else if( FORMAT_V1 == (lcm_cfg->mode&0x0f)){
                reg_read(table[i].tbl_v1.count, table[i].tbl_v1.para_list);
            }
            break;

        case REGFLAG_PWR:
            if( FORMAT_V2 == (lcm_cfg->mode&0x0f)){
                k=2;
                l = table[i].tbl_v2.para_list[1];
            }
            else if( FORMAT_V1 == (lcm_cfg->mode&0x0f)){
                k=0;
                l=table[i].tbl_v1.count;
            }

            for (j=k;j<l+k;j+=2) { //0xmn 0xll n is index, m is delay, ll is ctrl
                if( FORMAT_V2 == (lcm_cfg->mode&0x0f)){
                    pwr_index= table[i].tbl_v2.para_list[j]&0x0f;
                    pwr_ctrl = table[i].tbl_v2.para_list[j+1]&0xff;
                    pwr_delay = (table[i].tbl_v2.para_list[j]>>4)&0x0f;

                    #if defined(BUILD_LK)
                    printf("r69338 %s,%d,%d,%d\n", __func__,pwr_index,pwr_ctrl,pwr_delay);
                    #else
                    printk("r69338 %s,%x,%x,%d,%d,%d\n", __func__,table[i].tbl_v2.para_list[j],table[i].tbl_v2.para_list[j+1],pwr_index,pwr_ctrl,pwr_delay);
                    #endif
                }
                else if( FORMAT_V1 == (lcm_cfg->mode&0x0f)){
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
            if ( FORMAT_V1 == (lcm_cfg->mode&0x0f) ) {
                dsi_set_cmdq_V2(cmd, table[i].tbl_v1.count, table[i].tbl_v1.para_list, force_update);
            }else if( FORMAT_V2 == (lcm_cfg->mode&0x0f) ){
                dsi_set_cmdq(table[i].tbl_v2.para_list,table[i].tbl_v2.count-1,force_update);
            }
        }
    }
}

#endif
