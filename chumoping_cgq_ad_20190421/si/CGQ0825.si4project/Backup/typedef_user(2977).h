
#ifndef __TYPEDEF_USER_H__
#define __TYPEDEF_USER_H__


#include "string.h"



#define TRUE	1
#define FALSE	0


//系统主频
#define F_4M  4000000UL
#define F_16M 16000000UL
//#define F_CPU F_4M 
#define F_CPU F_16M

//#define USED_AD TRUE
#define USED_AD FALSE

//#define IS_MMR_USED TRUE  //MMR使用
#define IS_MMR_USED FALSE  //MMT使用


#define USED_SMOOTHING TRUE
//#define USED_SMOOTHING FALSE

#define SAMPLE_SHIFT_BIT_NUM    7 //7：可以达到接近400（不存在溢出风险）；8：可以达到接近200（便于计算，理论上分辨率更高，但存在溢出风险）
#define SAMPLE_SUM_POINT_NUM	128UL//(2^SAMPLE_SHIFT_BIT_NUM)，本来应该是256UL，取128UL是确认脉宽不会超过200
//
#define PWM_TOP			400//1500//1000
//触发脉冲低电平时间
#define PWM_LOW			4
#define PULSE_TOP		((PWM_TOP-100)*SAMPLE_SUM_POINT_NUM)//((PWM_TOP-((PWM_TOP*2)/100))*SAMPLE_SUM_POINT_NUM)//98%，
#define PULSE_BOTTOM	((PULSE_TOP/12)*2)//(PULSE_TOP/10)

#define PARA_SETED_FLAG				0x1906 //201906
#define PARA_SETED_SMOOTHING        0

#define ICM7555_AD	0    //AD0
//#define ICM7555_AD	1//AD1
//#define ICM7555_AD	7//AD7

#define MT_TEST				1201
#define MT_GAOLIAOWEI_SAVE	1204
#define MT_DILIAOWEI_SAVE	1205
#define MT_SCAN_KEY         1206
#define MT_TURN_OFF_LED     1207
#define MT_LED_RUN          1208

#define EEPROM_DATA_WORD_SIZE (sizeof(EEPROM_DATA)/sizeof(uint16_t))

typedef struct __EEPROM_DATA__
{
	//atmega8a的eeprom一共512字节
	//512/5 = 102
	//102-64 = 38
	//为了对齐便于人工查看，暂时取96字节
	//
	unsigned short bak0[30];  //没有任何作用，只是不用最前面的eeprom字节
	//
	unsigned short e_version_h;
	unsigned short e_version_l;
	//
	unsigned short e_mh;//2
	unsigned short e_ml;//4
	unsigned short e_para_seted_flag;//6
	unsigned short smoothingDate;
	//
	unsigned char bak1[32-6];//增加的参数可以填充到这里
}EEPROM_DATA;
//
typedef struct __TIMER_PARA_PKT__
{
	unsigned short id;
	unsigned long ms;
	unsigned long overtime;
	signed char retry;
	unsigned char enable;
}TIMER_PARA_PKT;
//
#endif


