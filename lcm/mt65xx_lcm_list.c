#include <lcm_drv.h>
extern LCM_DRIVER mss_debug_online_lcm_drv;
extern LCM_DRIVER hx8394d_cmi50_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8389b_lg45_dsi_vdo_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_vdo_lcm_drv;
extern LCM_DRIVER otm1283_hsd50_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8394d_boe466_dsi_vdo_lcm_drv;
extern LCM_DRIVER r61318_boe50_dsi_vdo_lcm_drv;
extern LCM_DRIVER ili9881_boe50_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt35596_auo497_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8389c_ivo594_dsi_vdo_lcm_drv;
extern LCM_DRIVER otm1283_boe50_dsi_vdo_lcm_drv;
extern LCM_DRIVER otm8009_ctc43_dsi_cmd_lcm_drv;
extern LCM_DRIVER otm1284a_boe47_dsi_vdo_lcm_drv;
extern LCM_DRIVER ili9881_cpt50_dsi_vdo_lcm_drv;
extern LCM_DRIVER ili9807_cpt50_dsi_vdo_lcm_drv;
extern LCM_DRIVER r69006_fhd_dsi_cmd_lcm_drv;
extern LCM_DRIVER r69338_fhd_dsi_vdo_lcm_drv;
extern LCM_DRIVER otm1906_fhd_dsi_cmd_lcm_drv;
//*******************************confirm************************

LCM_DRIVER* lcm_driver_list[] = 
{ 

#ifdef MSS_DEBUG_ONLINE
	&mss_debug_online_lcm_drv,
#endif	

#ifdef HX8394D_CMI50_DSI_VDO
	&hx8394d_cmi50_dsi_vdo_lcm_drv,
#endif	
#ifdef HX8389B_LG45_DSI_VDO
	&hx8389b_lg45_dsi_vdo_lcm_drv,
#endif	

#ifdef R63417_FHD_DSI_VDO
	&r63417_fhd_dsi_vdo_lcm_drv,
#endif
#ifdef OTM1283_HSD50_DSI_VDO
	&otm1283_hsd50_dsi_vdo_lcm_drv,
#endif
#ifdef HX8394D_BOE466_DSI_VDO
	&hx8394d_boe466_dsi_vdo_lcm_drv,
#endif
#ifdef R61318_BOE50_DSI_VDO
	&r61318_boe50_dsi_vdo_lcm_drv,
#endif	
#ifdef ILI9881_BOE50_DSI_VDO
	&ili9881_boe50_dsi_vdo_lcm_drv,
#endif	
#ifdef NT35596_AUO497_DSI_VDO
	&nt35596_auo497_dsi_vdo_lcm_drv,
#endif	
#ifdef HX8389C_IVO594_DSI_VDO
	&hx8389c_ivo594_dsi_vdo_lcm_drv,
#endif
#ifdef OTM1283_BOE50_DSI_VDO
	&otm1283_boe50_dsi_vdo_lcm_drv,
#endif	
#ifdef OTM8009_CTC43_DSI_CMD
	&otm8009_ctc43_dsi_cmd_lcm_drv,
#endif			
#ifdef OTM1284A_BOE47_DSI_VDO
	&otm1284a_boe47_dsi_vdo_lcm_drv,
#endif
#ifdef ILI9881_CPT50_DSI_VDO
	&ili9881_cpt50_dsi_vdo_lcm_drv,
#endif
#ifdef ILI9807_CPT50_DSI_VDO
	&ili9807_cpt50_dsi_vdo_lcm_drv,
#endif
#ifdef R69006_FHD_DSI_CMD
	&r69006_fhd_dsi_cmd_lcm_drv,
#endif
#ifdef R69338_FHD_DSI_VDO
	&r69338_fhd_dsi_vdo_lcm_drv,
#endif
#ifdef OTM1906_FHD_DSI_CMD
	&otm1906_fhd_dsi_cmd_lcm_drv,
#endif
//************************confirm ***********************

};

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition)?1:-1]

unsigned int lcm_count = sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*));
