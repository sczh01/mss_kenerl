#include "hwdrv_v0.h"
#include <mach/mt_gpio.h>
#include <mach/mt_gpt.h>
#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/fs.h>

#define GPIO_InitIO(dir,pin) 	 mt_set_gpio_dir(pin,dir)
#define GPIO_WriteIO(level,pin)  mt_set_gpio_out(pin,level)
#define GPIO_ReadIO(pin)	        mt_get_gpio_out(pin)
#define AW2013_SDA_PIN				214//66                //I2C data 配置
#define AW2013_SCK_PIN				215//67                //I2C clock 配置
#define IIC_ADDRESS_WRITE			0x8a             //I2C 写地址
#define IIC_ADDRESS_READ			0x8b             //I2C 读地址
#define AW2013_I2C_MAX_LOOP 		50   
#define I2C_delay 		2    //可根据平台调整,保证I2C速度不高于400k

//以下为调节呼吸效果的参数
#define Imax          0x02   //LED最大电流配置,0x00=omA,0x01=5mA,0x02=10mA,0x03=15mA,
#define Rise_time   0x02   //LED呼吸上升时间,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define Hold_time   0x01   //LED呼吸到最亮时的保持时间0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s
#define Fall_time     0x02   //LED呼吸下降时间,0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define Off_time      0x01   //LED呼吸到灭时的保持时间0x00=0.13s,0x01=0.26s,0x02=0.52s,0x03=1.04s,0x04=2.08s,0x05=4.16s,0x06=8.32s,0x07=16.64s
#define Delay_time   0x00   //LED呼吸启动后的延迟时间0x00=0s,0x01=0.13s,0x02=0.26s,0x03=0.52s,0x04=1.04s,0x05=2.08s,0x06=4.16s,0x07=8.32s,0x08=16.64s
#define Period_Num  0x00   //LED呼吸次数0x00=无限次,0x01=1次,0x02=2次.....0x0f=15次

/*
static ssize_t aw2013_store_led(struct device* cd, struct device_attribute *attr,const char* buf, size_t len);
static ssize_t aw2013_get_reg(struct device* cd,struct device_attribute *attr, char* buf);
static ssize_t aw2013_set_reg(struct device* cd, struct device_attribute *attr,const char* buf, size_t len);

static DEVICE_ATTR(led, S_IRUGO | S_IWUSR, NULL, aw2013_store_led);
static DEVICE_ATTR(reg, S_IRUGO | S_IWUGO,aw2013_get_reg,  aw2013_set_reg);
*/

//Begin-Bee-20140417
struct i2c_client *aw2013_i2c_client;
//End-Bee-20140417

void AW2013_delay_1us(U16 wTime);   //
BOOL AW2013_i2c_write_reg(unsigned char reg,unsigned char data);
//*******************************AW2013呼吸灯程序***********************************///
void aw2013_breath_all(int led0,int led1,int led2)  //led on=0x01   ledoff=0x00
{  

	//write_reg(0x00, 0x55);				// Reset 
	AW2013_i2c_write_reg(0x01, 0x01);		// enable LED 不使用中断		

	AW2013_i2c_write_reg(0x31, Imax|0x70);	//config mode, IMAX = 5mA	
	AW2013_i2c_write_reg(0x32, Imax|0x70);	//config mode, IMAX = 5mA	
	AW2013_i2c_write_reg(0x33, Imax|0x70);	//config mode, IMAX = 5mA	

	AW2013_i2c_write_reg(0x34, 0xff);	// LED0 level,
	AW2013_i2c_write_reg(0x35, 0xff);	// LED1 level,
	AW2013_i2c_write_reg(0x36, 0xff);	// LED2 level,
											
	AW2013_i2c_write_reg(0x37, Rise_time<<4 | Hold_time);	//led0  上升时间，保持时间设定							
	AW2013_i2c_write_reg(0x38, Fall_time<<4 | Off_time);	       //led0 下降时间，关闭时间设定
	AW2013_i2c_write_reg(0x39, Delay_time<<4| Period_Num);   //led0  呼吸延迟时间，呼吸周期设定

	AW2013_i2c_write_reg(0x3a, Rise_time<<4 | Hold_time);	//led1上升时间，保持时间设定								
	AW2013_i2c_write_reg(0x3b, Fall_time<<4 | Off_time);	       //led1 下降时间，关闭时间设定
	AW2013_i2c_write_reg(0x3c, Delay_time<<4| Period_Num);   //led1  呼吸延迟时间，呼吸周期设定

	AW2013_i2c_write_reg(0x3d, Rise_time<<4 | Hold_time);	//led2  上升时间，保持时间设定				
	AW2013_i2c_write_reg(0x3e, Fall_time<<4 | Off_time);	       //led2 下降时间，关闭时间设定
	AW2013_i2c_write_reg(0x3f, Delay_time<<4| Period_Num);    //呼吸延迟时间，呼吸周期设定

	AW2013_i2c_write_reg(0x30, led2<<2|led1<<1|led0);	       //led on=0x01 ledoff=0x00	
	AW2013_delay_1us(8);//需延时5us以上
}


/*
void led_flash_aw2013( unsigned int id )   //id = 0/1/2，分别对应LED0 LED1 LED2
{
	unsigned char reg_data;
	unsigned int	reg_buffer[8];
	int j=0;
	//while(1)
		{
	//write_reg(0x00, 0x55);				// Reset 
	AW2013_i2c_write_reg(0x01, 0x01);		// enable LED 不使用中断			

	AW2013_i2c_write_reg(0x31+id, 0x70|Imax);	//config mode, IMAX 	
	AW2013_i2c_write_reg(0x34+id, 0xff);	// LED0 level,
											
	AW2013_i2c_write_reg(0x37+id*3, Rise_time<<4 |Hold_time);	//led0  t_rise=0.52s  && t_hold=1.04s								
	AW2013_i2c_write_reg(0x38+id*3, Fall_time<<4 |Off_time);	//led0  t_fall=0.52s  && t_off=4.16s
	AW2013_i2c_write_reg(0x39+id*3, Delay_time<<4 |Period_Num);	//led0  t_Delay=0s && cnt=无穷大
					
	//read_reg(0x30,1,reg_buffer);
	//reg_data = reg_buffer[0] | (0x1 << id);
	AW2013_i2c_write_reg(0x30, 1<<id);	//led on	
		for (j=0; j < 50000; j++);//需延时5us以上

		}
}
*/
#if 0
void led_off_aw2013()//( unsigned int id )
{
	unsigned char reg_data;
	unsigned int	reg_buffer[8];

	AW2013_i2c_write_reg(0x30, 0);				//led off	
	AW2013_i2c_write_reg(0x01,0);
}
#endif
	

void AW2013_delay_1us(U16 wTime)   //
{
	udelay(wTime);
}

// Bee-20140417
static BOOL AW2013_i2c_write_reg_org(unsigned char reg,unsigned char data)
{
	BOOL ack=0;
	unsigned char ret;
	unsigned char wrbuf[2];

	wrbuf[0] = reg;
	wrbuf[1] = data;

	ret = i2c_master_send(aw2013_i2c_client, wrbuf, 2);
	if (ret != 2) {
		dev_err(&aw2013_i2c_client->dev,
		"%s: i2c_master_recv() failed, ret=%d\n",
		__func__, ret);
		ack = 1;
	}

	return ack;
}

BOOL AW2013_i2c_write_reg(unsigned char reg,unsigned char data)
{
	BOOL ack=0;
	unsigned char i;
	for (i=0; i<AW2013_I2C_MAX_LOOP; i++) {
		ack = AW2013_i2c_write_reg_org(reg,data);
		if (ack == 0) // ack success
			break;
		}
	return ack;
}

unsigned char AW2013_i2c_read_reg(unsigned char regaddr) 
{
	unsigned char rdbuf[1], wrbuf[1], ret, i;

	wrbuf[0] = regaddr;

	for (i=0; i<AW2013_I2C_MAX_LOOP; i++) {
		ret = i2c_master_send(aw2013_i2c_client, wrbuf, 1);
		if (ret == 1)
			break;
	}
	ret = i2c_master_recv(aw2013_i2c_client, rdbuf, 1);
	if (ret != 1)
		dev_err(&aw2013_i2c_client->dev,
			"%s: i2c_master_recv() failed, ret=%d\n",
			__func__, ret);

    	return rdbuf[0];
}
//#endif //aw2013

#if 0//for s5 only 20130220
extern void tpd_toggle_hand_using(int left_right_hand);
#endif

#if 1
static unsigned char LED_ON_FLAG = 0x0;
static unsigned char LED_ON_FLAG_BACKUP = 0x0;

#define TST_BIT(flag,bit)	(flag & bit)	//(flag & (0x1 << bit))
#define CLR_BIT(flag,bit)	(flag &= (~(bit)))	//(flag &= (~(0x1 << bit)))
#define SET_BIT(flag,bit)	(flag |= bit)	//(flag |= (0x1 << bit))

#define MSG_FLAG_BIT				(0x1)
#define CALL_FLAG_BIT				(0x2)
#define BAT_FLAG_BIT				(0x4)
#define CHARGING_FLAG_BIT			(0x8)
#define CHARGING_FULL_FLAG_BIT	(0x10)
#define POWER_ON_FLAG_BIT			(0x20)
#define STANDBY_FLAG_BIT			(0x40)
#define VIDEO_RECORD_FLAG_BIT		(0x80)

#define TEST_FLAG_BIT		(0x1)

#ifdef HWCTL_DBG
#define DBG_PRINT(x...)	printk(KERN_ERR x)
#else
#define DBG_PRINT(x...)
#endif
static HW_DEVICE g_hwctl_device={.init_stage=DEV_NONE_STAGE,};
void Flush_led_data(void);
void Suspend_led(void);

extern struct i2c_adapter * get_mt_i2c_adaptor(int);
int breathlight_master_send(u16 addr, char * buf ,int count)
{
	unsigned char ret;
	ret = i2c_master_send(aw2013_i2c_client, buf, count);
	if (ret != count) {
		dev_err(&aw2013_i2c_client->dev,
			"%s: i2c_master_recv() failed, ret=%d\n",
			__func__, ret);
	}
	return ret;
}

/////////////////////////////for aw2013
void led_flash_aw2013_test( unsigned int id )
{
	char buf[2];
	
	DBG_PRINT("led_flash_aw2013_test");
	
//	buf[0]=0x00;
//	buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x32;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x35;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);	
}

void led_off_aw2013_test(void)
{
	char buf[2];
	buf[0]=0x30;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013( unsigned int id )
{
	char buf[2];

	DBG_PRINT("led_flash_aw2013");
	
	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x53;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x55;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013_power_low(void)/////red led
{
	unsigned int id =1;/////red led
	char buf[2];

	DBG_PRINT("led_flash_aw2013_power_low");

	//buf[0]=0x00;
	//buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x06;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013_charging_full(void)
{
	unsigned int id =2;/////green led
	char buf[2];

	DBG_PRINT("led_flash_aw2013_charging_full");

//	buf[0]=0x00;
//	buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013_standby(void)
{
	unsigned int id =2;/////green led
	char buf[2];

	DBG_PRINT("led_flash_aw2013_standby");

//	buf[0]=0x00;
//	buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x32;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x35;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);	
}

#include <mach/mt_boot.h>
void led_flash_aw2013_standby_direct_enable(void)
{
if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())//add for factory mode 2014-07-16
    return;
else
{
	SET_BIT(LED_ON_FLAG,STANDBY_FLAG_BIT);
	Suspend_led();
	Flush_led_data();
}
}
void led_flash_aw2013_standby_direct_disable(void)
{
if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())//add for factory mode 2014-07-16
    return;
else
{
	CLR_BIT(LED_ON_FLAG,STANDBY_FLAG_BIT);
	Suspend_led();
	Flush_led_data();
}
}	

void led_flash_aw2013_charging(void)
{
	unsigned int id =1;/////red led

	char buf[2];

	DBG_PRINT("led_flash_aw2013_charging");

//	buf[0]=0x00;
//	buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x32;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x35;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013_unanswer_message_incall(void)/////blue led
{
	unsigned int id =0;////blue led
	char buf[2];

	DBG_PRINT("led_flash_aw2013_unanswer_message_incall");

//	buf[0]=0x00;
//	buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x32;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x35;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013_video_record(void)/////blue led
{
	unsigned int id =0;////blue led
	char buf[2];

	DBG_PRINT("led_flash_aw2013_video_record");

//	buf[0]=0x00;
//	buf[1]=0x54;/////reset led module
//breathlight_master_send(0x45,buf,2);

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x32;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x35;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x00;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}

void led_flash_aw2013_power_on(void)/////
{
	char buf[2];
	unsigned int id =0;////0 blue led ,1 red,2 green,

	DBG_PRINT("led_flash_aw2013_power_on");

	buf[0]=0x01;
	buf[1]=0x01;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x31+id;
	buf[1]=0x71;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x34+id;
	buf[1]=0xff;//0xc8;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x37+id*3;
	buf[1]=0x34;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x38+id*3;
	buf[1]=0x35;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x39+id*3;
	buf[1]=0x03;
	breathlight_master_send(0x45,buf,2);

	buf[0]=0x30;
	buf[1]=1<<id;
	breathlight_master_send(0x45,buf,2);
}


void led_off_aw2013(void)
{
	char buf[2];
	
	buf[0]=0x30;
	buf[1]=0x00;
breathlight_master_send(0x45,buf,2);
	buf[0]=0x01;
	buf[1]=0x00;
breathlight_master_send(0x45,buf,2);

}

void Flush_led_data(void)
{
	static unsigned int color = 2;

	if(TST_BIT(LED_ON_FLAG_BACKUP,TEST_FLAG_BIT))
	{
		led_flash_aw2013_test(color);
		return;
	}
	//first if it's charging situation, we skip the other actions
	if(TST_BIT(LED_ON_FLAG,CHARGING_FLAG_BIT))
	{
		led_flash_aw2013_charging();
		return;
	}
	if(TST_BIT(LED_ON_FLAG,CHARGING_FULL_FLAG_BIT))
	{
		led_flash_aw2013_charging_full();
		return;
	}
	//second the bat infor is the priority
	if(TST_BIT(LED_ON_FLAG,BAT_FLAG_BIT))
	{
		led_flash_aw2013_power_low();
		return;
	}
	//then msg and call
	if(TST_BIT(LED_ON_FLAG,CALL_FLAG_BIT))
	{
		led_flash_aw2013_unanswer_message_incall();
		return;
	}
	if(TST_BIT(LED_ON_FLAG,MSG_FLAG_BIT))
	{
		led_flash_aw2013_unanswer_message_incall();
		return;
	}	
	//*****led flag bit
	if(TST_BIT(LED_ON_FLAG,POWER_ON_FLAG_BIT))
	{
		led_flash_aw2013_power_on();
		return;
	}	
	if(TST_BIT(LED_ON_FLAG,VIDEO_RECORD_FLAG_BIT))
	{
		led_flash_aw2013_video_record();
		return;
	}	
	if(TST_BIT(LED_ON_FLAG,STANDBY_FLAG_BIT))
	{
		led_flash_aw2013_standby();
		return;
	}	

	color = ((++color) % 3);
}

void Suspend_led(void)
{
	//first if it's charging situation, we skip the other actions
	led_off_aw2013();
}
#endif

static long hwctl_unlock_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	DBG_PRINT("hwctl_ioctl cmd=%d",cmd);

    switch(cmd)
    {
    	case HW_CTL_IO_TEST:
			DBG_PRINT("this is test and this module has been tested");
			SET_BIT(LED_ON_FLAG_BACKUP,TEST_FLAG_BIT);
			break;
		case HW_CTL_IO_ENB_KBD:			
			DBG_PRINT("[HWCTL]Enable keyboard");
			break;
		case HW_CTL_IO_DIS_KBD:
			DBG_PRINT("[HWCTL]Disable keyboard");
			break;
		case HW_CTL_IO_EN_MSG_NTF:
			SET_BIT(LED_ON_FLAG,MSG_FLAG_BIT);
			break;
		case HW_CTL_IO_DIS_MSG_NTF:			
			CLR_BIT(LED_ON_FLAG,MSG_FLAG_BIT);
			break;
		case HW_CTL_IO_EN_CALL_NTF:			
			SET_BIT(LED_ON_FLAG,CALL_FLAG_BIT);
			break;
		case HW_CTL_IO_DIS_CALL_NTF:			
			CLR_BIT(LED_ON_FLAG,CALL_FLAG_BIT);
			break;
		case HW_CTL_IO_EN_BAT_NTF:			
			SET_BIT(LED_ON_FLAG,BAT_FLAG_BIT);
			break;
		case HW_CTL_IO_DIS_BAT_NTF:			
			CLR_BIT(LED_ON_FLAG,BAT_FLAG_BIT);
			break;
		case HW_CTL_IO_CHARGING_EN_NTF:	
			SET_BIT(LED_ON_FLAG,CHARGING_FLAG_BIT);
			break;
		case HW_CTL_IO_CHARGING_FULL_EN_NTF:	
			SET_BIT(LED_ON_FLAG,CHARGING_FULL_FLAG_BIT);
			break;	
		case HW_CTL_IO_CHARGING_FULL_DIS_NTF:	
			CLR_BIT(LED_ON_FLAG,CHARGING_FULL_FLAG_BIT);
			break;
		case HW_CTL_IO_CHARGING_DIS_NTF:	
			CLR_BIT(LED_ON_FLAG,CHARGING_FLAG_BIT);
			break;
		//*****************************************************
		case HW_CTL_IO_POWER_ON_EN_NTF:			
			SET_BIT(LED_ON_FLAG,POWER_ON_FLAG_BIT);
			break;
		case HW_CTL_IO_POWER_ON_DIS_NTF:			
			CLR_BIT(LED_ON_FLAG,POWER_ON_FLAG_BIT);
			break;
		case HW_CTL_IO_STANDBY_EN_NTF:	
			SET_BIT(LED_ON_FLAG,STANDBY_FLAG_BIT);
			break;
		case HW_CTL_IO_STANDBY_DIS_NTF:	
			CLR_BIT(LED_ON_FLAG,STANDBY_FLAG_BIT);
			break;	
		case HW_CTL_IO_VIDEO_RECORD_EN_NTF:	
			SET_BIT(LED_ON_FLAG,VIDEO_RECORD_FLAG_BIT);
			break;
		case HW_CTL_IO_VIDEO_RECORD_DIS_NTF:	
			CLR_BIT(LED_ON_FLAG,VIDEO_RECORD_FLAG_BIT);
			break;
		//*****************************************************
		case HW_CTL_IO_LEFT_HAND_NTF:	////added by liyunpen20130219
			//tpd_toggle_hand_using(0);
			//SET_BIT(LED_ON_FLAG,CHARGING_FLAG_BIT);
			break;
		case HW_CTL_IO_RIGHT_HAND_NTF:	////added by liyunpen20130219
			//tpd_toggle_hand_using(1);
			//CLR_BIT(LED_ON_FLAG,CHARGING_FLAG_BIT);
			break;	

		case HW_CTL_IO_FORCE_REFRESH_LEDS:	//close all
			LED_ON_FLAG=0x00;
			LED_ON_FLAG_BACKUP=0x00;
			//Suspend_led();
			//Flush_led_data();
			break;
		default:
			break;
	}
	Suspend_led();
	Flush_led_data();
	
	return 0;
}

static int hwctl_open(struct inode *inode, struct file *file)
{ 
   	return 0;
}

static int hwctl_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t hwctl_read(struct file * fp, char __user * to, size_t read_size, loff_t * pos)
{
#define VERSION_NO	"Hardware direct control driver :89.0 by david wang"

	ssize_t copy_size = strlen(VERSION_NO);

	DBG_PRINT("hwctl_read ops");

	return copy_to_user(to,VERSION_NO,copy_size);
}

static struct file_operations hwctl_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= hwctl_unlock_ioctl,
	.open		= hwctl_open,
	.release	= hwctl_release,
	.read		= hwctl_read,
};

#if defined(CONFIG_HAS_EARLYSUSPEND)
static void hwdctl_early_suspend(struct early_suspend *h)
{
	Flush_led_data();
}

static void hwdctl_late_resume(struct early_suspend *h)
{
	Suspend_led();
}
#endif


void hwctl_shut_charging_leds_and_dojobs()
{
	Suspend_led();
	CLR_BIT(LED_ON_FLAG,CHARGING_FLAG_BIT);
	CLR_BIT(LED_ON_FLAG,CHARGING_FULL_FLAG_BIT);

	Flush_led_data();
}

/*
//begin-Bee-20140417
static int aw2013_create_sysfs(struct i2c_client *client)
{
	int err;
	struct device *dev = &(client->dev);

	TS_DBG("%s", __func__);
	
	err = device_create_file(dev, &dev_attr_led);
	err = device_create_file(dev, &dev_attr_reg);
	return err;
}
*/
static int __devinit aw2013_i2c_probe(struct i2c_client *client,
				      const struct i2c_device_id *id)
{
	DBG_PRINT("aw2013_i2c_probe:OK");


	aw2013_i2c_client = client;

	//led_flash_aw2013(0);
	//led_flash_aw2013_test();
	//led_flash_aw2013_charging();//
	//led_flash_aw2013_power_low();//
	//led_flash_aw2013_power_on();
	//led_flash_aw2013_unanswer_message_incall();
	//led_flash_aw2013_charging_full();
	
	return 0;
}

static int __devexit aw2013_i2c_remove(struct i2c_client *client)
{
	aw2013_i2c_client = NULL;
	return 0;
}

static const struct i2c_device_id AW2013_i2c_id[] = {
	{ "AW2013", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, AW2013_i2c_id);
//static struct i2c_board_info __initdata aw2013_i2c_hw = { I2C_BOARD_INFO("AW2013", 0x45), };

static struct i2c_driver aw2013_i2c_driver = {
        .driver = {
                .owner  = THIS_MODULE,
                .name   = "AW2013",
        },

        .probe          = aw2013_i2c_probe,
        .remove         =  __devexit_p(aw2013_i2c_remove),
        .id_table       = AW2013_i2c_id,
};
//end-Bee-20140417

static int __init hwctl_driver_init(void) {
	int ret;
	//i2c_register_board_info(1, aw2013_i2c_hw, ARRAY_SIZE(aw2013_i2c_hw));
		 
	ret = i2c_add_driver(&aw2013_i2c_driver);

	DBG_PRINT("hwctl_driver_init:start");
	printk("hwctl_driver_init:start \n");
	if(0!=alloc_chrdev_region(&g_hwctl_device.hwctl_dev_no,HW_DEVICE_MINOR,HW_DEVICE_COUNT,HW_DEVICE_NAME))
	{
		DBG_PRINT("hwctl_driver_alloc chrdev region:fail");
		goto init_error;
		goto init_error;
		goto init_error;
		goto init_error;
	}	
	DBG_PRINT("hwctl_driver_alloc chrdev region:OK");
	g_hwctl_device.init_stage = DEV_ALLOC_REGION;
	g_hwctl_device.hw_cdev = cdev_alloc();
    g_hwctl_device.hw_cdev->owner = THIS_MODULE;
    g_hwctl_device.hw_cdev->ops = &hwctl_fops;
    ret = cdev_add(g_hwctl_device.hw_cdev, g_hwctl_device.hwctl_dev_no, 1);
	if(ret)
	{
		DBG_PRINT("hwctl_driver add cdev error\n");		
		goto init_error;
	}
	g_hwctl_device.init_stage = DEV_ADD_CDEV;
	g_hwctl_device.hw_class = class_create(THIS_MODULE, HW_DEVICE_NAME);
	
	g_hwctl_device.init_stage = DEV_ALLOC_CLASS;

	// if we want auto creat device node, we must call this
	g_hwctl_device.hw_device = device_create(g_hwctl_device.hw_class, NULL, g_hwctl_device.hwctl_dev_no, NULL, HW_DEVICE_NAME); 
	g_hwctl_device.init_stage = DEV_INIT_ALL;

#ifdef CONFIG_HAS_EARLYSUSPEND
	g_hwctl_device.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 20;
	g_hwctl_device.early_suspend.suspend = hwdctl_early_suspend;
	g_hwctl_device.early_suspend.resume = hwdctl_late_resume;
	register_early_suspend(&g_hwctl_device.early_suspend);
#endif
	return 0;

init_error:
	if(g_hwctl_device.init_stage == DEV_ALLOC_REGION)
		unregister_chrdev_region(g_hwctl_device.hwctl_dev_no,1);
	return (-1);
}

/* should never be called */
static void __exit hwctl_driver_exit(void) {
	if(g_hwctl_device.init_stage == DEV_INIT_ALL)
	{
		device_del(g_hwctl_device.hw_device);
		class_destroy(g_hwctl_device.hw_class);
		cdev_del(g_hwctl_device.hw_cdev);
		unregister_chrdev_region(g_hwctl_device.hwctl_dev_no,1);
	}
		i2c_del_driver(&aw2013_i2c_driver);
}

module_init(hwctl_driver_init);
module_exit(hwctl_driver_exit);
MODULE_DESCRIPTION("Linux HW direct control driver");
MODULE_AUTHOR("David.wang(softnow@live.cn)");
MODULE_LICENSE("GPL");

