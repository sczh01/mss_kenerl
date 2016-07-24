#ifndef BUILD_LK


#include <mach/mt_gpio.h>
#include "otp.h"


#include "lcm_drv.h"


#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/earlysuspend.h>
#include <linux/kthread.h>
#include <linux/rtpm_prio.h>
#include <linux/vmalloc.h>
#include <linux/disp_assert_layer.h>
#include <linux/semaphore.h>
#include <linux/xlog.h>
#include <linux/mutex.h>
#include <linux/leds-mt65xx.h>

#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/mach-types.h>
#include <asm/cacheflush.h>
#include <asm/io.h>

#include <mach/dma.h>

#include <mach/irqs.h>
#include <linux/dma-mapping.h>

#include "mach/mt_boot.h"
#include "mach/mt_clkmgr.h"


#include <linux/delay.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/mutex.h>


void DSI_set_cmdq_V3(LCM_setting_table_V3 *para_tbl, unsigned int size, unsigned char force_update);
void DSI_set_cmdq_V2(unsigned cmd, unsigned char count, unsigned char *para_list, unsigned char force_update);
void DSI_set_cmdq(unsigned int *pdata, unsigned int queue_size, unsigned char force_update);
void DSI_write_lcm_cmd(unsigned int cmd);
void DSI_write_lcm_regs(unsigned int addr, unsigned int *para, unsigned int nums);
UINT32 DSI_read_lcm_reg(void);
UINT32 DSI_dcs_read_lcm_reg(unsigned char);
UINT32 DSI_dcs_read_lcm_reg_v2(UINT8 cmd, UINT8 *buffer, UINT8 buffer_size);



//********************************************************************************************************
//******press vol-& vol+  to enter otp working mode  on the same time   
//******and then, vol- is dec key; vol+ is inc key 
//******1:modify  vcom adjust register  in "lcm_adjust_vcom[]"  by  yourself
//******2:otp_direction=1, to do increase vcomoffset
//******3:otp_direction=0, to do decrease vcomoffset
//******4:otp_value is the final vcom value,
//******you should update it for vcom register while you do otp_confirm
//******5:vcom_value must be value of  initia code vcom setting
//******6:wrote by kevin on 2015.03.20.pm11,    
//********************************************************************************************************
extern unsigned int  otp_vcomoffset;   // offsetvalue
extern char otp_direction;    //case 1 : increase,   case 0: decrease 

//unsigned char LowPower_cmd_flag=1;
static unsigned int otp_value; // the final vcom value
static unsigned int vcom_value=0x6a;  // the default value from initial code


unsigned int Visual_Debug(void)
{
#ifdef   VISUAL_DEBUG
return 1;
#else
return 0;
#endif
}


void otp_adjust_vcom(void)
{
unsigned int data_array[16];

switch (otp_direction)
	{
       	case 0xff:
		otp_value=vcom_value+otp_vcomoffset;
		#ifndef BUILD_LK
		printk("otp:otp_adjust_vcom+++++\n");
		printk("otp:otp_vcomoffset=%x\n",otp_vcomoffset);
		#endif
		otp_direction=0;
		otp_vcomoffset=0;
		break;

	case 0xee:
		otp_value=vcom_value-otp_vcomoffset;
		#ifndef BUILD_LK
		printk("otp:otp_adjust_vcom-----\n");
		printk("otp:otp_vcomoffset=%x\n",otp_vcomoffset);
		#endif
		otp_direction=0;
		otp_vcomoffset=0;
		break;
	default:
		otp_direction=0;
		otp_vcomoffset=0;
	}
msleep(1);
data_array[0] = 0x0003390a;
data_array[1] = 0x000000b6| ((otp_value<<16)&0xff0000)| ((otp_value<<8)&0xff00);
DSI_set_cmdq(&data_array, 2, 1);
msleep(1);

}
//********************************************************************************************
//otp_confirm_vcom  will be wrote by your setting  
//we will do below sentence after press down menu key
//********************************************************************************************
void Otp_Setting(void)
{

//reference your IC datasheet do  otp command  write 
//otp_adjust_vcom(); //this is vcom confirm sentence only
//reference your IC datasheet do  otp command  write 

}

//********************************************************************************************************
//******1:pls read 0x0a reg to get success value
//******2:we make "DSP on " bit to be false, in order to enter ESD recover
//********************************************************************************************************
void Otp_Burning(void)
{

//reference your IC datasheet do  otp command  write 
//otp_adjust_vcom(); //this is vcom confirm sentence only
//reference your IC datasheet do  otp command  write 

}
unsigned int  Otp_Judge_Value;
unsigned int Otp_Judge(void)
{
//low bit : for reg1
// case 0:  times 0    display :REG1 OTP FAIL
// case 1:  times 1    display :REG1 1th  OTP code is OK
//case  2:  times 2    display :REG1 2th  OTP code is OK
//case  3:  times 3    display :REG1 3th  OTP code is OK
//case  10:  don't judge times, display:  REG1 OK

//mid bit   for reg2
// case 0:  times 0    display :REG2 OTP FAIL
// case 1:  times 1    display :REG2 1th  OTP code is OK
//case  2:  times 2    display :REG2  OTP code is OK
//case  3:  times 3    display :REG2  OTP code is OK
//case  10:  don't judge times, display:  REG1 OK

//high bit   for all judge
// case 0:  times 0    display : OTP FAIL
// case 1:  times 1    display : OTP OK

Otp_Judge_Value=1;
printk("kevin:Otp_Judge=%x\n",Otp_Judge_Value);
return 1;

}

void Otp_Power_On(void)
{

//reference your IC datasheet do  otp command  write 
//otp_adjust_vcom(); //this is vcom confirm sentence only
//reference your IC datasheet do  otp command  write 

}
void Otp_Power_Off(void)
{

//reference your IC datasheet do  otp command  write 
//otp_adjust_vcom(); //this is vcom confirm sentence only
//reference your IC datasheet do  otp command  write 

}
extern void lcm_init(void);
extern char otp_keyvalue;
void Otp_Opreration(void)
{
#ifdef  OTP_Surport
if (otp_keyvalue==0xff)    //this is for confirm ok key
	{
	otp_keyvalue=0;


mt_set_gpio_mode(126,0);
mt_set_gpio_dir(126,1);
mt_set_gpio_out(126,1);  //iovcc  vcc  power on
msleep(10);
	//

mt_set_gpio_mode(131,0);
mt_set_gpio_dir(131,1);
mt_set_gpio_out(131,1);
msleep(10);

mt_set_gpio_mode(131,0);
mt_set_gpio_dir(131,1);
mt_set_gpio_out(131,0);
msleep(10);

mt_set_gpio_mode(131,0);
mt_set_gpio_dir(131,1);
mt_set_gpio_out(131,1);
msleep(120);   //reset

/*
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);   //lcm power on
*/

printk("kevin: Otp_Opreration\n");

printk("kevin:Otp_Judge_Value1=%x\n",Otp_Judge_Value);
	Otp_Setting();
	Otp_Power_On();
	Otp_Burning();
	Otp_Power_Off();
	lcm_init();
	Otp_Judge();
printk("kevin:Otp_Judge_Value2=%x\n",Otp_Judge_Value);
	}
#endif

}

unsigned int Otp_Mipi_Mode(void)
{
#ifdef LCM_MIPI_LP&&OTP_Surport

return 1;

#else
return 0;

#endif

}



#endif










