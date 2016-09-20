#ifndef _LCM_DEBUG_H
#define _LCM_DEBUG_H

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define dsi_set_cmdq_V2(cmd,count,ppara,force_update)   lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)					lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)		lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)					lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

#define   LCM_DSI_CMD_MODE				0
#define FORMAT_V1 0x00
#define FORMAT_V2 0x01
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

#define MAX_CMD                         500
#define LPWG_CMD                        100
#define COMMAND_BUF_SIZE                (69*MAX_CMD*4)

#define SYSFS_FOLDER "dsi_access"
#define BUFFER_LENGTH 258

static struct LCM_setting_table
{
    unsigned int mode;
    unsigned int cmd;
    unsigned int count;
    unsigned char para_list[80];
};

static struct LCM_setting_table_V2
{
    unsigned int mode;
    unsigned int cmd;
    unsigned int count;
    unsigned int para_list[20];
};

static struct LCM_cfg
{
    unsigned int mode;
    unsigned int exe_num;
    unsigned int freq_pll;// 0x0000mnll
    unsigned int hfp_vfp;
    unsigned int hbp_vbp;
    unsigned int hsw_vsw;
    unsigned int dsi_set0;//0xmmnnllkk, LPX/HS_PRPR/HS_TRAIL/CLK_TRAIL
    unsigned int dsi_set1;
	//params->dsi.LPX=8; 
	//params->dsi.HS_PRPR=5;
	//params->dsi.HS_TRAIL=13;
	//params->dsi.CLK_TRAIL = 10;
};

static union LCM_code_table
{
    unsigned int mode;
    struct LCM_setting_table tbl_v1;
    struct LCM_setting_table_V2 tbl_v2;
    unsigned char buf[sizeof(struct LCM_setting_table_V2)];
};

struct dsi_debug {
	bool long_rpkt;
	unsigned char length;
	unsigned char rlength;
	unsigned char buffer[BUFFER_LENGTH];
	unsigned char read_buffer[BUFFER_LENGTH];
	unsigned char command_len;
	unsigned char *command_buf;
	struct kobject *sysfs_dir;
    struct LCM_cfg *lcm_cfg;
    union LCM_code_table lcm_code_table;
};

enum read_write {
	CMD_READ = 0,
	CMD_WRITE = 1,
};

#endif
