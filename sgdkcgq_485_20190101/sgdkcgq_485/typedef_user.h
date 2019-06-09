
#ifndef __TYPEDEF_USER_H__
#define __TYPEDEF_USER_H__

#define TRUE	1
#define FALSE	0



#define PARA_SETED_FLAG				0x0319 //201901
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


#if (F_4M ==F_CPU)
#define PWM_TOP		4000/4//1500//1000
#define PWM_TOP1	4000/4//1500//1000
//触发脉冲低电平时间
#define PWM_LOW			40/4

//4兆晶振比例系数调节
#define PWM_BILI 8


#endif


#if (F_16M ==F_CPU)
#define PWM_TOP		4000//1500//1000
#define PWM_TOP1	4000//1500//1000
//触发脉冲低电平时间
#define PWM_LOW			40

//8兆晶振比例系数调节

#define PWM_BILI 10


#endif


/*

#define PWM_TOP		4000/4//1500//1000
#define PWM_TOP1	4000/4//1500//1000
//触发脉冲低电平时间
#define PWM_LOW			40/4
*/



#define PULSE_TOP		((PWM_TOP-((PWM_TOP*2)/100))*SAMPLE_SUM_POINT_NUM)//98%，
#define PULSE_BOTTOM	(PULSE_TOP/10)

#endif


