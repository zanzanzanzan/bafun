
#ifndef __TYPEDEF_USER_H__
#define __TYPEDEF_USER_H__


#define FALSE               0
#define TRUE				1


//系统主频
#define F_4M  4000000UL
#define F_16M 16000000UL
#define F_CPU F_4M
//#define F_CPU F_16M


#define MT_LED_DEBUG_BLINK		5101
#define MT_LED_KEY_TURN_OFF     5102
#define MT_TURN_OFF_BP          5103
#define MT_ZK_IS_VALID          5104
#define MT_ZD_IS_VALID          5105
#define MT_HZ_OUT				5106
#define MT_MB_IN_CHECK			5107

#define SAMPLE_SHIFT_BIT_NUM    8
#define SAMPLE_SUM_POINT_NUM	128UL//(2^SAMPLE_SHIFT_BIT_NUM)，本来应该是256UL，取128UL是确认脉宽不会超过200
//
#define PWM_TOP			400//1500//1000
//触发脉冲低电平时间
#define PWM_LOW			4
#define PULSE_TOP		((PWM_TOP-((PWM_TOP*2)/100))*SAMPLE_SUM_POINT_NUM)//98%，
#define PULSE_BOTTOM	(PULSE_TOP/10)
#define PWM_1V0_4MA		((PWM_TOP/5)+5)//稍微加大一点，为了便于比较
#define PWM_2V25_9MA	((PWM_TOP/20)*9)
#define PWM_4V25_17MA	((PWM_TOP/20)*17)
#define PWM_3V75_15MA	((PWM_TOP/20)*15)

#define PARA_SETED_FLAG				0x1609 //201609



typedef struct
{
	unsigned short e_mh;//2
	unsigned short e_ml;//4
	unsigned short e_para_seted_flag;//6
	//
	unsigned char bak1[32-6];//增加的参数可以填充到这里
	unsigned char bak2[16];
}EEPROM_DATA;

typedef struct __TIMER_PARA_PKT__
{
	unsigned short id;
	unsigned long ms;
	unsigned long overtime;
	signed char retry;
	unsigned char enable;
}TIMER_PARA_PKT;

#endif


