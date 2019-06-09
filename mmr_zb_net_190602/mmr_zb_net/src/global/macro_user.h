/*
 * macro_user.h
 *
 * Created: 2014/10/4 12:19:32
 *  Author: xiong
 */ 


#ifndef MACRO_USER_H_
#define MACRO_USER_H_

////////////////////////////////////////////////////////////////
#define Bool	unsigned char
#define U32		unsigned long
#define U16		unsigned short
#define U8		unsigned char
//  
#define LSB(u16)        (((U8  *)&(u16))[0])  //!< Least significant byte of \a u16.
#define MSB(u16)        (((U8  *)&(u16))[1])  //!< Most significant byte of \a u16.

#define LSH(u32)        (((U16 *)&(u32))[0])  //!< Least significant half-word of \a u32.
#define MSH(u32)        (((U16 *)&(u32))[1])  //!< Most significant half-word of \a u32.
#define LSB0W(u32)      (((U8  *)&(u32))[0])  //!< Least significant byte of 1st rank of \a u32.
#define LSB1W(u32)      (((U8  *)&(u32))[1])  //!< Least significant byte of 2nd rank of \a u32.
#define LSB2W(u32)      (((U8  *)&(u32))[2])  //!< Least significant byte of 3rd rank of \a u32.
#define LSB3W(u32)      (((U8  *)&(u32))[3])  //!< Least significant byte of 4th rank of \a u32.
#define MSB3W(u32)      LSB0W(u32)            //!< Most significant byte of 4th rank of \a u32.
#define MSB2W(u32)      LSB1W(u32)            //!< Most significant byte of 3rd rank of \a u32.
#define MSB1W(u32)      LSB2W(u32)            //!< Most significant byte of 2nd rank of \a u32.
#define MSB0W(u32)      LSB3W(u32)            //!< Most significant byte of 1st rank of \a u32.

#define LSW(u64)        (((U32 *)&(u64))[0])  //!< Least significant word of \a u64.
#define MSW(u64)        (((U32 *)&(u64))[1])  //!< Most significant word of \a u64.
#define LSH0(u64)       (((U16 *)&(u64))[0])  //!< Least significant half-word of 1st rank of \a u64.
#define LSH1(u64)       (((U16 *)&(u64))[1])  //!< Least significant half-word of 2nd rank of \a u64.
#define LSH2(u64)       (((U16 *)&(u64))[2])  //!< Least significant half-word of 3rd rank of \a u64.
#define LSH3(u64)       (((U16 *)&(u64))[3])  //!< Least significant half-word of 4th rank of \a u64.
#define MSH3(u64)       LSH0(u64)             //!< Most significant half-word of 4th rank of \a u64.
#define MSH2(u64)       LSH1(u64)             //!< Most significant half-word of 3rd rank of \a u64.
#define MSH1(u64)       LSH2(u64)             //!< Most significant half-word of 2nd rank of \a u64.
#define MSH0(u64)       LSH3(u64)             //!< Most significant half-word of 1st rank of \a u64.
#define LSB0D(u64)      (((U8  *)&(u64))[0])  //!< Least significant byte of 1st rank of \a u64.
#define LSB1D(u64)      (((U8  *)&(u64))[1])  //!< Least significant byte of 2nd rank of \a u64.
#define LSB2D(u64)      (((U8  *)&(u64))[2])  //!< Least significant byte of 3rd rank of \a u64.
#define LSB3D(u64)      (((U8  *)&(u64))[3])  //!< Least significant byte of 4th rank of \a u64.
#define LSB4D(u64)      (((U8  *)&(u64))[4])  //!< Least significant byte of 5th rank of \a u64.
#define LSB5D(u64)      (((U8  *)&(u64))[5])  //!< Least significant byte of 6th rank of \a u64.
#define LSB6D(u64)      (((U8  *)&(u64))[6])  //!< Least significant byte of 7th rank of \a u64.
#define LSB7D(u64)      (((U8  *)&(u64))[7])  //!< Least significant byte of 8th rank of \a u64.
#define MSB7D(u64)      LSB0D(u64)            //!< Most significant byte of 8th rank of \a u64.
#define MSB6D(u64)      LSB1D(u64)            //!< Most significant byte of 7th rank of \a u64.
#define MSB5D(u64)      LSB2D(u64)            //!< Most significant byte of 6th rank of \a u64.
#define MSB4D(u64)      LSB3D(u64)            //!< Most significant byte of 5th rank of \a u64.
#define MSB3D(u64)      LSB4D(u64)            //!< Most significant byte of 4th rank of \a u64.
#define MSB2D(u64)      LSB5D(u64)            //!< Most significant byte of 3rd rank of \a u64.
#define MSB1D(u64)      LSB6D(u64)            //!< Most significant byte of 2nd rank of \a u64.
#define MSB0D(u64)      LSB7D(u64)            //!< Most significant byte of 1st rank of \a u64.
////////////////////////////////////////////////////////////////
#define SOFTWARE_VERSION	20//2.0
////////////////////////////////////////////////////////////////
#define delay_us(us) _delay_us(us)
#define delay_ms(ms) _delay_ms(ms)
//
#define min(a,b) (((a)<(b)) ? (a):(b))
#define max(a,b) (((a)>(b)) ? (a):(b))
////////////////////////////////////////////////////////////////
extern volatile unsigned long boot_time;
//
#define QR_QL_CHECH_DELAY	(20*1000)//ms
////////////////////////////////////////////////////////////////
//���ֶ�ת�Զ����ߴ��Զ�ת�ֶ��Ƿ����ͣ������
//�ô�ֻ�԰���������Ч,��Զ����Ч
#define TRUE            1
#define FALSE           0
//
//#define MUSTSTOPFIRST   TRUE//������ͣ��
#define MUSTSTOPFIRST   FALSE//������ͣ��,ֱ���л�
//
#define CONFIRM_KEY_VALUE    TRUE
//#define CONFIRM_KEY_VALUE    FALSE
////////////////////////////////////////////////////////////////
//
//#define __D2_USED__  TRUE //���õ���2
#define __D2_USED__  FALSE //���õ���2
//
//#define __TEST_USED__  TRUE //
#define __TEST_USED__  FALSE //
//
#define  LIMITEDTIME_ENABLE    1//ʹ����ʱ
//#define  LIMITEDTIME_ENABLE    0//��ֹ��ʱ
//
//#define MODBUS_TEST  TRUE //modbus��ַ�Ͳ����ʹ̶�
#define MODBUS_TEST  FALSE //modbus��ַ�Ͳ����ʸ����û����ñ仯
//
///////////////////////////////////////////////////////////////////////
#define USED_MODBUS_UART   TRUE
//#define USED_MODBUS_UART   FALSE

//#define __ZAH_JU_TIANZHEN__    TRUE 
#define __ZAH_JU_TIANZHEN__    FALSE


//
#define USED_W5500			TRUE
//#define USED_W5500		FALSE
////////////////////////////////////////////////////////////////////////
//20171005
//��Ƶ��B��ѹ���ƣ�0-10v��TRUE������2-10v��FALSE��
//#define FMOTOR_VEL_0V_10V   TRUE   //0-10v
#define FMOTOR_VEL_0V_10V   FALSE   //2-10v
//��Ƶ��A��ѹ���ƣ�0-5v��TRUE������2-5v��FALSE��
//#define FMOTOR_VEL_A_0V_10V   TRUE   //0-10v
#define FMOTOR_VEL_A_0V_10V   FALSE   //2-10v


////////////////////////////////////////////////////////////////////////
//WDRF BORF EXTRF PORF
#define RST_CAUSE_JTAG      0b00010000
#define RST_CAUSE_WDT		0b00001000 //���Ź���λ
#define RST_CAUSE_BOR		0b00000100 //���縴λ
#define RST_CAUSE_EXT		0b00000010 //�ⲿ��λ
#define RST_CAUSE_POR		0b00000001 //�ϵ縴λ
#define RST_CAUSE_OTHER     0x00
////////////////////////////////////////////////////////////////////////
//��ЩLED��ʾ����ֵ��������ı䣬�������ʾ���LED_TABһһ��Ӧ��
#define LED_DIS_0		0
#define LED_DIS_1		1
#define LED_DIS_2		2
#define LED_DIS_3		3
#define LED_DIS_4		4
#define LED_DIS_5		5
#define LED_DIS_6		6
#define LED_DIS_7		7
#define LED_DIS_8		8
#define LED_DIS_9		9
#define LED_DIS_A		10
#define LED_DIS_B		11
#define LED_DIS_C		12
#define LED_DIS_D		13
#define LED_DIS_E		14
#define LED_DIS_F		15
#define LED_DIS_H		16
#define LED_DIS_L		17
#define LED_DIS_P		18
#define LED_DIS_R		19
#define LED_DIS_SUB		20
#define LED_DIS_SPACK	21
#define LED_DIS_0DOT	22
#define LED_DIS_1DOT	23
#define LED_DIS_2DOT	24
#define LED_DIS_3DOT	25
#define LED_DIS_4DOT	26
#define LED_DIS_5DOT	27
#define LED_DIS_6DOT	28
#define LED_DIS_7DOT	29
#define LED_DIS_8DOT	30
#define LED_DIS_9DOT	31
#define LED_DIS_I		32
#define LED_DIS_M		33
#define LED_DIS_G		34
#define LED_DIS_J		35
#define LED_DIS_K		36
#define LED_DIS_N		37
#define LED_DIS_o		38
#define LED_DIS_Q		39
#define LED_DIS_S		40
#define LED_DIS_T		41
#define LED_DIS_U		42
#define LED_DIS_V		43
#define LED_DIS_W		44
#define LED_DIS_X		45
#define LED_DIS_Y		46
#define LED_DIS_Z		47
////////////////////////////////////////////////////////////////
//��������
#define KEY_ZDJQD           40//���������
//#define KEY_MGLZ            41//ĥ������
#define KEY_TJ              41//41//ͣ��
#define KEY_ZDYX            42//�Զ�����
//#define KEY_MGHZ            43//ĥ������
#define KEY_MGLZHZ          43//ĥ�������
//#define KEY_TJ              44//ͣ��
#define KEY_JS              44//44//����
#define KEY_SJ              144//44//����

#define KEY_SDYX            45//�ֶ�����
#define KEY_ZTJ             46//��ͣ��
#define KEY_XL              47//ж��
#define KEY_DOWN            48//���¼�        nothing
#define KEY_CSQR            49//����ȷ��      nothing
#define KEY_YCKZ            50//Զ�̿���      
#define KEY_UP              51//���ϼ�        nothing
//#define KEY_CSSD            52//�����趨      nothing



#define KEY_ZUO_WEI_JIA         86
#define KEY_ZUO_WEI_JIAN        87
//HZ he  lz

#define KEY_YOU_WEI_JIA         88
#define KEY_YOU_WEI_JIAN        89
//by he  zt


#define KEY_ZUO_LING        90
#define KEY_ZUO_QI        91
//HZ he  lz

#define KEY_YOU_LING        92
#define KEY_YOU_QI        93
//by he  zt


#define KEY_HE_ZHA        94
#define KEY_LI_ZHA        95
//by he  zt


#define TEST_BUTTON 100




#define SET_MAX_ZHUANSU 70
#define SET_MIN_ZHUANSU 71
#define HZ_YANCHISHIJIAN 73



#define SET_QD_LIAOWEI 74
#define SET_TJ_LIAOWEI 75

#define SET_CONGJI_DIZHI 78


#define SET_SHAGNDIAN_ZHUANGTAI 79
#define SET_DIANLIU_LIANSUO  81
#define SET_DIANLIU_BIAODING 83

#define SET_ZHUANSU_JIA 84
#define SET_ZHUANSU_JIAN 85


#define SET_ZHUDIANJI_DIANLIU 72
#define SET_ZHUDIANJI_DIANLIU1 96

#define SET_DIANLIU_GUOZAI_TINGJI 80
#define SET_DIANLIU_GUOZAI_TINGJI1 97

#define SET_DIANLIU_LIANGCHENG 82
#define SET_DIANLIU_LIANGCHENG1 98

#define SET_DIANLIU_XIUZHENG 120
#define SET_DIANLIU_XIUZHENG1 99

#define SET_MB_XINHAO  76
#define SET_DW_XINHAO  183
#define SET_DIV  184



#define SET_RIQI            66//�����趨


#define HZ_SET_ZHANJU 180


#define SET_CANSHU_DISP       181//�����趨

#define SET_YUZHI_SHEZHI       182//�����趨



//������Ч������ʹ����Ч��������Ч ����

#define SET_IP_ADDR       200//�����趨
#define SET_SLAVE_USART_ADDR       201//�����趨
#define SET_SLAVE_USART_COMM       202//�����趨


#define KEY_CJ_TEMP           203//�����趨      nothing

#define TEMP_CHANGE_TEMP      204//�����趨      nothing

#define SET_LIAOWEI_VALUE      205//�����趨     PID����   //�ӳ�ʱ������




////////////////////////////////////////////////////////////////
#define FULL_CURRENT        100
#define MAX_VEL             300
#define MIN_VEL             40
////////////////////////////////////////////////////////////////
//485ͨ�ŵ�ַ
#define ADDR_ZB             1
#define ADDR_XSB            2
#define ADDR_CGQ            3
#define ADDR_XSB_2          4

#define ADDR_XSB_1          5
#define ADDR_XSB_2_2        6
#define ADDR_XSB_3          7
#define ADDR_XSB_4          8

#define ADDR_XSB_5          9
#define ADDR_XSB_6          10
#define ADDR_XSB_7          11
#define ADDR_XSB_8          12

#define ADDR_XSB_9          13






////////////////////////////////////////////////////////////////
//��λ����������
#define XUYAOSHUJU          51
#define GAOLIAOWEI          52
#define DILIAOWEI           53
////////////////////////////////////////////////////////////////
#define PWM_TOP     1500//1000
#define PWM_MAX		(PWM_TOP-((PWM_TOP*2)/100))//98%
//��������͵�ƽʱ��
#define LOW_PULSE   100
//
#if (F_CPU/PWM_TOP) < 2048
#error "PWM_TOP����ƫ������ݾ����������"
#elif (F_CPU/PWM_TOP) > 4095
#error "PWM_TOP����ƫС������ݾ����������"
#else
//4096=2^12  //2048=2^11  //1024=2^10  //512=2^9  //256=2^8
//16-12      //16-11      //16-10      //16-9     //16-8
#define LW_COUNT    4096//��λ����ֵ��������һ��
#define LW_SHIFT    (16-12)
#endif
////////////////////////////////////////////////////////////////
#define MB_DETECTION_INTERVAL		1//s
#define MB_60S_SAMPLE_SIZE			(60/MB_DETECTION_INTERVAL)
#define MB_3S_SAMPLE_SIZE			(3/MB_DETECTION_INTERVAL)//�޸Ĳ�����ʱ��ע���չ˵��ò���
#define DW_DETECTION_INTERVAL		1//s
#define DW_60S_SAMPLE_SIZE			(60/DW_DETECTION_INTERVAL)
#define DW_3S_SAMPLE_SIZE			(3/DW_DETECTION_INTERVAL)//�޸Ĳ�����ʱ��ע���չ˵��ò���
////////////////////////////////////////////////////////////////
//�����ʱ��
#define MT_TRY					5001
#define MT_SCAN_KEY				5002
#define MT_BASETASK				5003
#define MT_PWM2AD				5004
#define MT_MB_COUNTER			5005
#define MT_DW_COUNTER			5006
#define MT_START_QR_QL_CHECK	5007
#define MT_CHECK_QR_QL			5008
#define MT_LOW_SPEED_TASK       5009
#define MT_CLEAN_MODBUS_RX_FLAG 5010
#define MT_POLL_TASK            5011
#define MT_AUTO_RUN_DELAY       5012
#define MT_NET_TASK             5013
#define MT_READ_PARA            5014
////////////////////////////////////////////////////////////////
//
#define POWER_UP_REMOTE				1//�ϵ�Զ��
#define POWER_UP_KEYPAD				2//�ϵ����
#define MATERIAL_PORT_RS485_AD		1//��AD�ɼ���λ��Ϣ����RS485���øߵ���λ
#define MATERIAL_PORT_ONLY_RS485	2//��RS485������λ��Ϣ�����øߵ���λ
#define MATERIAL_PORT_ONLY_AD		3//��AD�ɼ���λ��Ϣ��RS485����MODBUS
#define MODBUS_BAUD_9600			1
#define MODBUS_BAUD_19200			2
#define MODBUS_BAUD_38400			3
#define MB_LEVEL					1
#define MB_PULSE					2
#define DW_LEVEL					1
#define DW_PULSE					2
#define PARA_SETED_FLAG				0x1904 //201904
#define LIMITED_DAYS_MAX            2700 //���2700��
//
////////////////////////////////////////////////////////////////
//
#define STATUS_ON				0x01
#define STATUS_OFF				0x00
//
////////////////////////////////////////////////////////////////
//
#include "string.h"
//
extern void send_uart1_to_computer(unsigned char *buf,unsigned char len);
extern void send_ulong_ascii(unsigned long n,unsigned char newline);
//
#define DEBUG_TRACE TRUE
//#define DEBUG_TRACE FALSE
//
#define TRACE_DATA(p,len) send_uart1_to_computer((unsigned char*)p,len)
#define TRACE_STRING(p) send_uart1_to_computer((unsigned char *)p,strlen(p))
#define TRACE_VALUE(value) send_uart1_to_computer(0,value)
#define TRACE_VALUE_TEXT(value) send_ulong_ascii(value,TRUE)
#define TRACE_VALUE_TEXT_IN_A_ROW(value) send_ulong_ascii(value,FALSE)
//
#if DEBUG_TRACE == TRUE
#define DEBUG_DATA(p,len) send_uart1_to_computer((unsigned char*)p,len)
#define DEBUG_STRING(p) send_uart1_to_computer((unsigned char *)p,strlen(p))
#define DEBUG_VALUE(value) send_uart1_to_computer(0,value)
#define DEBUG_VALUE_TEXT(value) send_ulong_ascii(value,TRUE)
#define DEBUG_VALUE_TEXT_IN_A_ROW(value) send_ulong_ascii(value,FALSE)
#else
#define DEBUG_DATA(p,len) 
#define DEBUG_STRING(p) 
#define DEBUG_VALUE(value) 
#define DEBUG_VALUE_TEXT(value) 
#define DEBUG_VALUE_TEXT_IN_A_ROW(value) 
#endif
//
////////////////////////////////////////////////////////////////
//
#define TCNT3 _SFR_MEM16(0x94)
//
#define TCNT3L _SFR_MEM8(0x94)
#define TCNT3L0 0
#define TCNT3L1 1
#define TCNT3L2 2
#define TCNT3L3 3
#define TCNT3L4 4
#define TCNT3L5 5
#define TCNT3L6 6
#define TCNT3L7 7

#define TCNT3H _SFR_MEM8(0x95)
#define TCNT3H0 0
#define TCNT3H1 1
#define TCNT3H2 2
#define TCNT3H3 3
#define TCNT3H4 4
#define TCNT3H5 5
#define TCNT3H6 6
#define TCNT3H7 7
//
////////////////////////////////////////////////////////////////
//
#define PORT_USART	0x35
#define PORT_ENET   0x59
//
////////////////////////////////////////////////////////////////


#endif /* MACRO_USER_H_ */
