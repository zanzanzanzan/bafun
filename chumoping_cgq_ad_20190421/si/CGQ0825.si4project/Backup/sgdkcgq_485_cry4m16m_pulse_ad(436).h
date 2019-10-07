///////////////////////////////////
#ifndef __SGDKCGQ__
#define __SGDKCGQ__
////////////////////////////////////////////////////////////////
#include "typedef_user.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
////////////////////////////////////////////////////////////////
//Ϊ�˲�����,��������궨��,��ô�ߵ���λ�궨��ť�����pwm�����ƽ
//���Ӻͼ��ٰ�ť��.
//#define __TEST__
//
//Ϊ�˲��Բɼ�����.(�ɼ��������ݻ�Ӵ����ͳ�)
//��������������Ͳ��ܺ���������,����ͨ�Ż���ִ���
//����ʹ�õ�ʱ��,����궨�����ȡ��
//#define USART_TEST TRUE
#define USART_TEST FALSE
//
//�����õ��������ں���(8n1,38400)
#if USART_TEST == TRUE
#define DEBUG_DATA(p,len) send_uart_to_computer((unsigned char*)p,len)
#define DEBUG_STRING(p) send_uart_to_computer((unsigned char *)p,strlen(p))
#define DEBUG_VALUE(value) send_uart_to_computer(0,value)
#define DEBUG_VALUE_TEXT(value) send_ulong_ascii(value)
#endif
////////////////////////////////////////////////////////////////
//485ͨ�ŵ�ַ
#define ADDR_ZB             1
#define ADDR_XSB            2
#define ADDR_CGQ            3
////////////////////////////////////////////////////////////////
//��λ����������
#define XUYAOSHUJU          51
#define GAOLIAOWEI          52
#define DILIAOWEI           53
#define SHUAXIANSHUJU       54

#define GAOLIAOWEI_SET      57
#define DILIAOWEI_SET       56
#define SMOOTHING_SET   	58


////////////////////////////////////////////////////////////////
const unsigned short version_h=0x0619; //19060509
const unsigned short version_l=0x0905;
////////////////////////////////////////////////////////////////
//���Ͷ���
typedef struct 
{ 
	unsigned char bit0 :    1 ;
	unsigned char bit1 :    1 ;
	unsigned char bit2 :    1 ;
	unsigned char bit3 :    1 ;
	unsigned char bit4 :    1 ;
	unsigned char bit5 :    1 ;
	unsigned char bit6 :    1 ;
	unsigned char bit7 :    1 ;
}BIT_FIELD, *PBIT_FIELD; 

typedef union _UCHAR2
{
	unsigned char mc[2];
	unsigned short ms;
}UCHAR2;
typedef union _UCHAR4
{
	unsigned char mc[4];
	unsigned short ms[2];
	unsigned long ml;
}UCHAR4;
typedef union _UCHAR8
{
	unsigned char mc[8];
	unsigned short ms[4];
	unsigned long ml[2];
	unsigned long long mll;
}UCHAR8;

#define __BIT(x,b)   (*(volatile BIT_FIELD*)(&x)).bit##b   
#define BIT(x,b)    __BIT(x,b) 
////////////////////////////////////////////////////////////////
//�ܽŶ���
#define b7555out_f BIT(PIND,PD3)  //int1,   1//�������,�½���
#define b485en     BIT(PORTD,PD7) //out,    11
#define bpwm       OC1A           //pb1,out,13//�������ѹ���
#define b7555trg   OC1B           //pb2,out,14//7555��������
#define bkmh       BIT(PINC,PC2)  //in,     25
#define bkml       BIT(PINC,PC3)  //in,     26
#define bled       BIT(PORTC,PC4) //out,    27
#define bled_run   BIT(PORTC,PC5) //out,    28
#define b485ro     BIT(PIND,PD0)  //in,     30
#define b485di     BIT(PORTD,PD1) //out,    31
#define b7555out_r BIT(PIND,PD2)  //int0,   32//�������,������
////////////////////////////////////////////////////////////////
#define adsc       BIT(ADCSRA,ADSC)
////////////////////////////////////////////////////////////////
//��������
void softward_init(void);
void hardward_init(void);
void deal_key(unsigned char);
#if USART_TEST == TRUE
void send_uart_to_computer(unsigned char *p,unsigned short len);
void send_ulong_ascii(unsigned long n);
#endif
void uart_tx(void);
void read_para(void);
void scan_key(void);
unsigned short eeprom_read_word_user(const uint16_t *p);
void eeprom_write_word_user(uint16_t *p,unsigned short val);
void turn_on_led_ms(unsigned short ms);
void liaowei_sample_ok_handling(void);
void int1_handling(unsigned char val);
////////////////////////////////////////////////////////////////
//�ڴ��������
//WINAVR GCC�õ���Little-endian�ṹ,��VC��ͬ
unsigned char ms1_counter,ms10_counter,ms100_counter,s1_counter;
volatile unsigned short m_ms, m_minute;
volatile unsigned long boot_time;
volatile unsigned char boot_time_sub;
unsigned char uart_rx_counter;
unsigned char uart_tx_buf[10],uart_rx_buf[10];
volatile unsigned char uart_tx_addr;
volatile unsigned char comm_error_delay;
//
volatile unsigned short m_mh_set,m_ml_set;//��λ�ߵ�
volatile UCHAR2 m_mcur,m_cur_pwm;//��ǰ��λֵ
#if USED_AD == TRUE
volatile UCHAR2 m_mcur_ad,m_cur_per_ad;
volatile unsigned short m_ad_counter;
volatile UCHAR4 m_ad_value;
UCHAR8 m_ad_value_bak;
#endif
//
volatile unsigned char m_tc2h;//��ʱ��2���������.
volatile unsigned short m_pulse_counter;
volatile UCHAR4 m_pulse_value;
unsigned short m_pulse_value_rc;
unsigned short m_ad_value_rc;
//
#define PULSE_VALUE_NUM	64
volatile unsigned short pulse_value[PULSE_VALUE_NUM];
volatile unsigned char pulse_value_counter, pulse_value_write_id, pulse_value_read_id;
//
volatile unsigned char m_cgq_ack;//�궨Ӧ��
//
UCHAR8 m_pulse_value_bak;
//
volatile unsigned char uart_frame_head_ok;
volatile unsigned char command_gaoliaowei;
volatile unsigned char command_diliaowei;
volatile unsigned char uart_send_enable;
////////////////////////////////////////////////////////////////
//EEPROM ��������
EEPROM_DATA m_para[5] __attribute__((section(".eeprom"))) =
{
	{{0x37},0x0419,0x1816,PULSE_TOP,PULSE_BOTTOM},
	{{0x37},0x0419,0x1816,PULSE_TOP,PULSE_BOTTOM},
	{{0x37},0x0419,0x1816,PULSE_TOP,PULSE_BOTTOM},
	{{0x37},0x0419,0x1816,PULSE_TOP,PULSE_BOTTOM},
	{{0x37},0x0419,0x1816,PULSE_TOP,PULSE_BOTTOM}
};
////////////////////////////////////////////////////////////////
#endif
///////////////////////////////////
