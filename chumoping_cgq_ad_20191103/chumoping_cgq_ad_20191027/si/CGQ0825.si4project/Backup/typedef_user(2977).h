
#ifndef __TYPEDEF_USER_H__
#define __TYPEDEF_USER_H__


#include "string.h"



#define TRUE	1
#define FALSE	0


//ϵͳ��Ƶ
#define F_4M  4000000UL
#define F_16M 16000000UL
//#define F_CPU F_4M 
#define F_CPU F_16M

//#define USED_AD TRUE
#define USED_AD FALSE

//#define IS_MMR_USED TRUE  //MMRʹ��
#define IS_MMR_USED FALSE  //MMTʹ��


#define USED_SMOOTHING TRUE
//#define USED_SMOOTHING FALSE

#define SAMPLE_SHIFT_BIT_NUM    7 //7�����Դﵽ�ӽ�400��������������գ���8�����Դﵽ�ӽ�200�����ڼ��㣬�����Ϸֱ��ʸ��ߣ�������������գ�
#define SAMPLE_SUM_POINT_NUM	128UL//(2^SAMPLE_SHIFT_BIT_NUM)������Ӧ����256UL��ȡ128UL��ȷ�������ᳬ��200
//
#define PWM_TOP			400//1500//1000
//��������͵�ƽʱ��
#define PWM_LOW			4
#define PULSE_TOP		((PWM_TOP-100)*SAMPLE_SUM_POINT_NUM)//((PWM_TOP-((PWM_TOP*2)/100))*SAMPLE_SUM_POINT_NUM)//98%��
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
	//atmega8a��eepromһ��512�ֽ�
	//512/5 = 102
	//102-64 = 38
	//Ϊ�˶�������˹��鿴����ʱȡ96�ֽ�
	//
	unsigned short bak0[30];  //û���κ����ã�ֻ�ǲ�����ǰ���eeprom�ֽ�
	//
	unsigned short e_version_h;
	unsigned short e_version_l;
	//
	unsigned short e_mh;//2
	unsigned short e_ml;//4
	unsigned short e_para_seted_flag;//6
	unsigned short smoothingDate;
	//
	unsigned char bak1[32-6];//���ӵĲ���������䵽����
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


