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



//#define DUMMY_LENS_DEBUG
#ifdef DUMMY_LENS_DEBUG
#define DUMMY_LENSDB printk
#else
#define DUMMY_LENSDB(x,...)
#endif

static int __init S5K4E1FX_DUMMY_LENS_i2C_init(void)
{
    return 0;
}

static void __exit S5K4E1FX_DUMMY_LENS_i2C_exit(void)
{

}

module_init(S5K4E1FX_DUMMY_LENS_i2C_init);
module_exit(S5K4E1FX_DUMMY_LENS_i2C_exit);

MODULE_DESCRIPTION("S5K4E1FX_Dummy lens module driver");
MODULE_AUTHOR("zhanglianhong <zhang.lianhong@eastaeon.com>");
MODULE_LICENSE("GPL");


