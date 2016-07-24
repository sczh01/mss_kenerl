/*
 * MD218A voice coil motor driver
 *
 *
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include "CN3927AF.h"
#include "../camera/kd_camera_hw.h"

#define LENS_I2C_BUSNUM 1
//static struct i2c_board_info __initdata kd_lens_dev={ I2C_BOARD_INFO("CN3927AF", 0x18)};


#define CN3927AF_DRVNAME "CN3927AF"
#define CN3927AF_VCM_WRITE_ID           0x18

#define CN3927AF_DEBUG
#ifdef CN3927AF_DEBUG
#define CN3927AFDB printk
#else
#define CN3927AFDB(x,...)
#endif

static spinlock_t g_CN3927AF_SpinLock;

static struct i2c_client * g_pstCN3927AF_I2Cclient = NULL;

static dev_t g_CN3927AF_devno;
static struct cdev * g_pCN3927AF_CharDrv = NULL;
static struct class *actuator_class = NULL;

static int  g_s4CN3927AF_Opened = 0;
static long g_i4MotorStatus = 0;
static long g_i4Dir = 0;
static unsigned long g_u4CN3927AF_INF = 0;
static unsigned long g_u4CN3927AF_MACRO = 1023;
static unsigned long g_u4TargetPosition = 0;
static unsigned long g_u4CurrPosition   = 0;

static int g_sr = 3;

extern s32 mt_set_gpio_mode(u32 u4Pin, u32 u4Mode);
extern s32 mt_set_gpio_out(u32 u4Pin, u32 u4PinOut);
extern s32 mt_set_gpio_dir(u32 u4Pin, u32 u4Dir);


static int s4CN3927AF_ReadReg(unsigned short * a_pu2Result)
{
    int  i4RetValue = 0;
    char pBuff[2];

    i4RetValue = i2c_master_recv(g_pstCN3927AF_I2Cclient, pBuff , 2);

    if (i4RetValue < 0) 
    {
        CN3927AFDB("[CN3927AF] I2C read failed!! \n");
        return -1;
    }

    *a_pu2Result = (((u16)pBuff[0]) << 4) + (pBuff[1] >> 4);

    return 0;
}

static int s4CN3927AF_WriteReg(u16 a_u2Data)
{
    int  i4RetValue = 0;

    char puSendCmd[2] = {(char)(a_u2Data >> 4) , (char)(((a_u2Data & 0xF) << 4)+g_sr)};

    //CN3927AFDB("[CN3927AF] g_sr %d, write %d \n", g_sr, a_u2Data);
    g_pstCN3927AF_I2Cclient->ext_flag |= I2C_A_FILTER_MSG;
    i4RetValue = i2c_master_send(g_pstCN3927AF_I2Cclient, puSendCmd, 2);
	
    if (i4RetValue < 0) 
    {
        CN3927AFDB("[CN3927AF] I2C send failed!! \n");
        return -1;
    }

    return 0;
}

inline static int getCN3927AFInfo(__user stCN3927AF_MotorInfo * pstMotorInfo)
{
    stCN3927AF_MotorInfo stMotorInfo;
    stMotorInfo.u4MacroPosition   = g_u4CN3927AF_MACRO;
    stMotorInfo.u4InfPosition     = g_u4CN3927AF_INF;
    stMotorInfo.u4CurrentPosition = g_u4CurrPosition;
    stMotorInfo.bIsSupportSR      = TRUE;

	if (g_i4MotorStatus == 1)	{stMotorInfo.bIsMotorMoving = 1;}
	else						{stMotorInfo.bIsMotorMoving = 0;}

	if (g_s4CN3927AF_Opened >= 1)	{stMotorInfo.bIsMotorOpen = 1;}
	else						{stMotorInfo.bIsMotorOpen = 0;}

    if(copy_to_user(pstMotorInfo , &stMotorInfo , sizeof(stCN3927AF_MotorInfo)))
    {
        CN3927AFDB("[CN3927AF] copy to user failed when getting motor information \n");
    }

    return 0;
}

inline static int moveCN3927AF(unsigned long a_u4Position)
{
    int ret = 0;
    
    if((a_u4Position > g_u4CN3927AF_MACRO) || (a_u4Position < g_u4CN3927AF_INF))
    {
        CN3927AFDB("[CN3927AF] out of range \n");
        return -EINVAL;
    }

    if (g_s4CN3927AF_Opened == 1)
    {
        unsigned short InitPos;
        ret = s4CN3927AF_ReadReg(&InitPos);
	    
        spin_lock(&g_CN3927AF_SpinLock);
        if(ret == 0)
        {
            CN3927AFDB("[CN3927AF] Init Pos %6d \n", InitPos);
            g_u4CurrPosition = (unsigned long)InitPos;
        }
        else
        {		
            g_u4CurrPosition = 0;
        }
        g_s4CN3927AF_Opened = 2;
        spin_unlock(&g_CN3927AF_SpinLock);
    }

    if (g_u4CurrPosition < a_u4Position)
    {
        spin_lock(&g_CN3927AF_SpinLock);	
        g_i4Dir = 1;
        spin_unlock(&g_CN3927AF_SpinLock);	
    }
    else if (g_u4CurrPosition > a_u4Position)
    {
        spin_lock(&g_CN3927AF_SpinLock);	
        g_i4Dir = -1;
        spin_unlock(&g_CN3927AF_SpinLock);			
    }
    else										{return 0;}

    spin_lock(&g_CN3927AF_SpinLock);    
    g_u4TargetPosition = a_u4Position;
    spin_unlock(&g_CN3927AF_SpinLock);	

    //CN3927AFDB("[CN3927AF] move [curr] %d [target] %d\n", g_u4CurrPosition, g_u4TargetPosition);

            spin_lock(&g_CN3927AF_SpinLock);
            g_sr = 3;
            g_i4MotorStatus = 0;
            spin_unlock(&g_CN3927AF_SpinLock);	
		
            if(s4CN3927AF_WriteReg((unsigned short)g_u4TargetPosition) == 0)
            {
                spin_lock(&g_CN3927AF_SpinLock);		
                g_u4CurrPosition = (unsigned long)g_u4TargetPosition;
                spin_unlock(&g_CN3927AF_SpinLock);				
            }
            else
            {
                CN3927AFDB("[CN3927AF] set I2C failed when moving the motor \n");			
                spin_lock(&g_CN3927AF_SpinLock);
                g_i4MotorStatus = -1;
                spin_unlock(&g_CN3927AF_SpinLock);				
            }

    return 0;
}

inline static int setCN3927AFInf(unsigned long a_u4Position)
{
    spin_lock(&g_CN3927AF_SpinLock);
    g_u4CN3927AF_INF = a_u4Position;
    spin_unlock(&g_CN3927AF_SpinLock);	
    return 0;
}

inline static int setCN3927AFMacro(unsigned long a_u4Position)
{
    spin_lock(&g_CN3927AF_SpinLock);
    g_u4CN3927AF_MACRO = a_u4Position;
    spin_unlock(&g_CN3927AF_SpinLock);	
    return 0;	
}

////////////////////////////////////////////////////////////////
static long CN3927AF_Ioctl(
struct file * a_pstFile,
unsigned int a_u4Command,
unsigned long a_u4Param)
{
    long i4RetValue = 0;

    switch(a_u4Command)
    {
        case CN3927AFIOC_G_MOTORINFO :
            i4RetValue = getCN3927AFInfo((__user stCN3927AF_MotorInfo *)(a_u4Param));
        break;

        case CN3927AFIOC_T_MOVETO :
            i4RetValue = moveCN3927AF(a_u4Param);
        break;
 
        case CN3927AFIOC_T_SETINFPOS :
            i4RetValue = setCN3927AFInf(a_u4Param);
        break;

        case CN3927AFIOC_T_SETMACROPOS :
            i4RetValue = setCN3927AFMacro(a_u4Param);
        break;
		
        default :
      	    CN3927AFDB("[CN3927AF] No CMD \n");
            i4RetValue = -EPERM;
        break;
    }

    return i4RetValue;
}

//Main jobs:
// 1.check for device-specified errors, device not ready.
// 2.Initialize the device if it is opened for the first time.
// 3.Update f_op pointer.
// 4.Fill data structures into private_data
//CAM_RESET
static int CN3927AF_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    CN3927AFDB("[CN3927AF] CN3927AF_Open - Start\n");

    spin_lock(&g_CN3927AF_SpinLock);

    if(g_s4CN3927AF_Opened)
    {
        spin_unlock(&g_CN3927AF_SpinLock);
        CN3927AFDB("[CN3927AF] the device is opened \n");
        return -EBUSY;
    }

    g_s4CN3927AF_Opened = 1;
		
    spin_unlock(&g_CN3927AF_SpinLock);

    CN3927AFDB("[CN3927AF] CN3927AF_Open - End\n");

    return 0;
}

//Main jobs:
// 1.Deallocate anything that "open" allocated in private_data.
// 2.Shut down the device on last close.
// 3.Only called once on last time.
// Q1 : Try release multiple times.
static int CN3927AF_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    CN3927AFDB("[CN3927AF] CN3927AF_Release - Start\n");

    if (g_s4CN3927AF_Opened)
    {
        CN3927AFDB("[CN3927AF] feee \n");
        g_sr = 5;
	    s4CN3927AF_WriteReg(200);
        msleep(10);
	    s4CN3927AF_WriteReg(100);
        msleep(10);
            	            	    	    
        spin_lock(&g_CN3927AF_SpinLock);
        g_s4CN3927AF_Opened = 0;
        spin_unlock(&g_CN3927AF_SpinLock);

    }

    CN3927AFDB("[CN3927AF] CN3927AF_Release - End\n");

    return 0;
}

static const struct file_operations g_stCN3927AF_fops = 
{
    .owner = THIS_MODULE,
    .open = CN3927AF_Open,
    .release = CN3927AF_Release,
    .unlocked_ioctl = CN3927AF_Ioctl
};

inline static int Register_CN3927AF_CharDrv(void)
{
    struct device* vcm_device = NULL;

    CN3927AFDB("[CN3927AF] Register_CN3927AF_CharDrv - Start\n");

    //Allocate char driver no.
    if( alloc_chrdev_region(&g_CN3927AF_devno, 0, 1,CN3927AF_DRVNAME) )
    {
        CN3927AFDB("[CN3927AF] Allocate device no failed\n");

        return -EAGAIN;
    }

    //Allocate driver
    g_pCN3927AF_CharDrv = cdev_alloc();

    if(NULL == g_pCN3927AF_CharDrv)
    {
        unregister_chrdev_region(g_CN3927AF_devno, 1);

        CN3927AFDB("[CN3927AF] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(g_pCN3927AF_CharDrv, &g_stCN3927AF_fops);

    g_pCN3927AF_CharDrv->owner = THIS_MODULE;

    //Add to system
    if(cdev_add(g_pCN3927AF_CharDrv, g_CN3927AF_devno, 1))
    {
        CN3927AFDB("[CN3927AF] Attatch file operation failed\n");

        unregister_chrdev_region(g_CN3927AF_devno, 1);

        return -EAGAIN;
    }

    actuator_class = class_create(THIS_MODULE, "actuatordrv3");
    if (IS_ERR(actuator_class)) {
        int ret = PTR_ERR(actuator_class);
        CN3927AFDB("Unable to create class, err = %d\n", ret);
        return ret;            
    }

    vcm_device = device_create(actuator_class, NULL, g_CN3927AF_devno, NULL, CN3927AF_DRVNAME);

    if(NULL == vcm_device)
    {
        return -EIO;
    }
    
    CN3927AFDB("[CN3927AF] Register_CN3927AF_CharDrv - End\n");    
    return 0;
}

inline static void Unregister_CN3927AF_CharDrv(void)
{
    CN3927AFDB("[CN3927AF] Unregister_CN3927AF_CharDrv - Start\n");

    //Release char driver
    cdev_del(g_pCN3927AF_CharDrv);

    unregister_chrdev_region(g_CN3927AF_devno, 1);
    
    device_destroy(actuator_class, g_CN3927AF_devno);

    class_destroy(actuator_class);

    CN3927AFDB("[CN3927AF] Unregister_CN3927AF_CharDrv - End\n");    
}

//////////////////////////////////////////////////////////////////////

static int CN3927AF_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int CN3927AF_i2c_remove(struct i2c_client *client);
static const struct i2c_device_id CN3927AF_i2c_id[] = {{CN3927AF_DRVNAME,0},{}};   
struct i2c_driver CN3927AF_i2c_driver = {                       
    .probe = CN3927AF_i2c_probe,                                   
    .remove = CN3927AF_i2c_remove,                           
    .driver.name = CN3927AF_DRVNAME,                 
    .id_table = CN3927AF_i2c_id,                             
};  

#if 0 
static int CN3927AF_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) {         
    strcpy(info->type, CN3927AF_DRVNAME);                                                         
    return 0;                                                                                       
}      
#endif 
static int CN3927AF_i2c_remove(struct i2c_client *client) {
    return 0;
}

/* Kirby: add new-style driver {*/
static int CN3927AF_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int i4RetValue = 0;

    CN3927AFDB("[CN3927AF] CN3927AF_i2c_probe\n");

    /* Kirby: add new-style driver { */
    g_pstCN3927AF_I2Cclient = client;
    
 //   g_pstCN3927AF_I2Cclient->addr = g_pstCN3927AF_I2Cclient->addr >> 1;
	g_pstCN3927AF_I2Cclient->addr = CN3927AF_VCM_WRITE_ID >> 1;
    
    //Register char driver
    i4RetValue = Register_CN3927AF_CharDrv();

    if(i4RetValue){

        CN3927AFDB("[CN3927AF] register char device failed!\n");

        return i4RetValue;
    }

    spin_lock_init(&g_CN3927AF_SpinLock);

    CN3927AFDB("[CN3927AF] Attached!! \n");

    return 0;
}

static int CN3927AF_probe(struct platform_device *pdev)
{
    return i2c_add_driver(&CN3927AF_i2c_driver);
}

static int CN3927AF_remove(struct platform_device *pdev)
{
    i2c_del_driver(&CN3927AF_i2c_driver);
    return 0;
}

static int CN3927AF_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

static int CN3927AF_resume(struct platform_device *pdev)
{
    return 0;
}

// platform structure
static struct platform_driver g_stCN3927AF_Driver = {
    .probe		= CN3927AF_probe,
    .remove	= CN3927AF_remove,
    .suspend	= CN3927AF_suspend,
    .resume	= CN3927AF_resume,
    .driver		= {
        .name	= "lens_actuator3",
        .owner	= THIS_MODULE,
    }
};

static int __init CN3927AF_i2C_init(void)
{
//    i2c_register_board_info(LENS_I2C_BUSNUM, &kd_lens_dev, 1);
	
    if(platform_driver_register(&g_stCN3927AF_Driver)){
        CN3927AFDB("failed to register CN3927AF driver\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit CN3927AF_i2C_exit(void)
{
	platform_driver_unregister(&g_stCN3927AF_Driver);
}

module_init(CN3927AF_i2C_init);
module_exit(CN3927AF_i2C_exit);

MODULE_DESCRIPTION("CN3927AF lens module driver");
MODULE_AUTHOR("KY Chen <ky.chen@Mediatek.com>");
MODULE_LICENSE("GPL");


