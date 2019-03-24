///////////////////////////////////
#ifndef __SGDKCGQ__
#define __SGDKCGQ__
////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>


//ϵͳ��Ƶ
#define F_4M  4000000UL
#define F_16M 16000000UL
//#define F_CPU F_4M
#define F_CPU F_16M



#include <util/delay.h>
#include "typedef_user.h"
#include "string.h"
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
////////////////////////////////////////////////////////////////
//�ڴ��������
//WINAVR GCC�õ���Little-endian�ṹ,��VC��ͬ
unsigned char ms1_counter,ms10_counter,ms100_counter;
volatile unsigned short m_ms;
unsigned char uart_rx_counter;
unsigned char uart_tx_buf[10],uart_rx_buf[10];
volatile unsigned char uart_tx_addr;
volatile unsigned char comm_error_delay;
//
volatile unsigned short m_mh_set,m_ml_set;//��λ�ߵ�
volatile UCHAR2 m_mcur,m_cur_pwm;//��ǰ��λֵ
//
volatile unsigned char m_tc2h;//��ʱ��2���������.
volatile unsigned short m_pulse_counter;
volatile UCHAR4 m_pulse_value;
//
volatile unsigned char m_led_delay;//�������������ʱ��
//
volatile unsigned char m_cgq_ack,m_cgq_ack_delay;//�궨Ӧ��
//
UCHAR8 m_pulse_value_bak;
//
volatile BIT_FIELD m_flag;
#define uart_frame_head_ok             BIT(m_flag,0)
#define scan_key_enable                BIT(m_flag,1)
#define command_gaoliaowei             BIT(m_flag,3)
#define command_diliaowei              BIT(m_flag,4)
#define uart_send_enable               BIT(m_flag,5)
////////////////////////////////////////////////////////////////
//EEPROM ��������
unsigned short e_bak[32] __attribute__((section(".eeprom"))) = {0x37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//û���κ�����
EEPROM_DATA m_para[3] __attribute__((section(".eeprom"))) =
{
	
	{{},PULSE_TOP,PULSE_BOTTOM},
	{{},PULSE_TOP,PULSE_BOTTOM},
	{{},PULSE_TOP,PULSE_BOTTOM}
};
////////////////////////////////////////////////////////////////
#endif
///////////////////////////////////
