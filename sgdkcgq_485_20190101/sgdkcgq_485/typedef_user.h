
#ifndef __TYPEDEF_USER_H__
#define __TYPEDEF_USER_H__

#define TRUE	1
#define FALSE	0



#define PARA_SETED_FLAG				0x0119 //201901
typedef struct
{
	unsigned char bak2[16];
	unsigned short e_mh;//2
	unsigned short e_ml;//4
	unsigned short e_para_seted_flag;//6
	//
	unsigned char bak1[32-6];//增加的参数可以填充到这里
	
}EEPROM_DATA;



#define SAMPLE_SHIFT_BIT_NUM    8
#define SAMPLE_SUM_POINT_NUM	128UL//(2^SAMPLE_SHIFT_BIT_NUM)，本来应该是256UL，取128UL是确认脉宽不会超过200
//
#define PWM_TOP			400//1500//1000
//触发脉冲低电平时间
#define PWM_LOW			4
#define PULSE_TOP		((PWM_TOP-((PWM_TOP*2)/100))*SAMPLE_SUM_POINT_NUM)//98%，
#define PULSE_BOTTOM	(PULSE_TOP/10)

#endif


