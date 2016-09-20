#ifndef _LCM_INIT_CODE_H
#define _LCM_INIT_CODE_H


static struct LCM_setting_table lcm_sleep_in_code[] =
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

static struct LCM_setting_table lcm_sleep_out_code[] =
{
	// Display ON
    {0xff0,6,{0x14,0x00,0x10,0x01,0x14,0x01}},
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 20, {}},	
    // Sleep Out
	{0x11, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
    {0xffD,1,{0x8}},//BL
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_init_code_338_V2[] =
{

};
static struct LCM_setting_table lcm_init_code_338_V1[] =
{
{0xff0,6,{0x14,0x00,0x10,0x01,0x14,0x01}},
{0xB0,1,{0x04}},
{0xE1,10,{0x01,0x00,0x00,0x00,0x93,0x38,0x10,0x55,0x0A,0x00}},
{0xB3,7,{0x14,0x1A,0x00,0x00,0x00,0x00,0x00}},
{0xB4,1,{0x0C}},
{0xB6,2,{0x3A,0xD3}},
{0xC1,44,{0x84,0x60,0x00,0x52,0xCA,0x12,0x04,0x19,0x40,0x19,0xD9,0xD8,0x82,0xCF,0xB9,0x07,0x17,0x6B,0xF2,0x80,0x41,0x44,0x90,0x52,0x4A,0x09,0x00,0x00,0x00,0x00,0xA0,0x40,0x42,0x20,0x12,0x00,0x22,0x00,0x15,0x00,0x01,0x00,0x00,0x00}},
{0xC2,9,{0x31,0xF7,0x80,0x00,0x04,0x00,0x08,0x00,0x00}},
{0xC4,15,{0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x15,0x15,0x15,0x01}},
{0xC6,20,{0x49,0x11,0x11,0x04,0x41,0x01,0x01,0x01,0x01,0x01,0x01,0x3E,0x0A,0x01,0x01,0x01,0x01,0x02,0x12,0x03}},
{0xCB,12,{0xF8,0xF7,0xFF,0x7E,0x30,0x00,0x00,0x00,0x00,0x00,0x3C,0xCF}},
{0xCC,1,{0x0D}},
{0xD0,5,{0x10,0x91,0xBB,0x15,0x93}},
{0xD1,5,{0x25,0x00,0x1F,0x61,0x06}},
{0xD3,27,{0x0B,0x37,0x9F,0xBF,0xB7,0x33,0x33,0x17,0x00,0x01,0x00,0xA0,0xD8,0xA0,0x0D,0x53,0x53,0x33,0x3B,0xF7,0x72,0x07,0x3D,0xBF,0x99,0x21,0xFA}},
{0xD5,11,{0x06,0x00,0x00,0x01,0x28,0x01,0x28,0x00,0x00,0x00,0x00}},
{0xD6,1,{0x01}},
{0xD9,12,{0x04,0x32,0xAA,0xAA,0xAA,0xE4,0x02,0x00,0x0F,0x00,0x00,0x00}},
{0xEC,48,{0x2D,0x10,0x2C,0x55,0x01,0x00,0x00,0x00,0x68,0x13,0x13,0x00,0x00,0x00,0x00,0x00,0x07,0x10,0x31,0x11,0x0F,0x00,0x00,0x00,0x00,0x50,0x03,0x00,0x00,0x00,0x00,0x05,0xFC,0x00,0x00,0x00,0x03,0x40,0x02,0x12,0x02,0x02,0x01,0x00,0x00,0x00,0x00,0x00}},
{0xED,31,{0x41,0x13,0x13,0x03,0x24,0x11,0x02,0x90,0x00,0x03,0x00,0x00,0x00,0x03,0x21,0x24,0x00,0xA6,0x00,0xA4,0x05,0x32,0x3F,0x00,0x00,0x1D,0x00,0x00,0x00,0x00,0x00}},
{0xEE,31,{0x20,0x13,0x13,0x03,0x00,0x10,0x02,0x00,0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,0x01,0x4E,0x01,0x4C,0x0A,0x73,0x23,0x00,0x00,0x39,0x00,0x00,0x00,0x00,0x00}},
{0xEF,62,{0x41,0x13,0x13,0x03,0x24,0x11,0x02,0x90,0x00,0x03,0x00,0x00,0x00,0x03,0x21,0x24,0x00,0xA6,0x00,0xA4,0x05,0x32,0x3F,0x00,0x00,0x1D,0x00,0x00,0x00,0x00,0x00,0x41,0x13,0x13,0x03,0x24,0x11,0x02,0x90,0x00,0xF3,0x00,0x00,0x49,0x00,0x03,0x21,0x24,0x00,0xA6,0x00,0xA4,0x05,0x32,0x3F,0x00,0x1D,0x00,0x00,0x00,0x00,0x00}},
{0xC7,30,{0x00,0x18,0x20,0x2C,0x3B,0x49,0x52,0x60,0x44,0x4B,0x57,0x65,0x6F,0x76,0x7F,0x00,0x1B,0x20,0x2C,0x3B,0x49,0x52,0x60,0x44,0x4B,0x57,0x65,0x6F,0x76,0x7F}},
{0xC8,19,{0x01,0x00,0xFF,0x04,0xFE,0xFC,0xE0,0x00,0x00,0x03,0x01,0xFC,0xF0,0x00,0xFF,0xFB,0xEE,0xFC,0x11}},
{0xB0,1,{0x03}},
{0x29,0,{0x00}},
{REGFLAG_DELAY, 10, {0}},
{0x11,0,{0x00}},
{REGFLAG_DELAY, 120, {0}},
{0xffD,1,{0x8}},//BL
{REGFLAG_END_OF_TABLE, 0x00, {}},
};

static struct LCM_setting_table lcm_init_1906_V2[] =
{

};
static struct LCM_setting_table lcm_init_1906_V1[] =
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
#endif