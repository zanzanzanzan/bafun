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
////////////////////////////////////////////////////////////////
//485ͨ�ŵ�ַ
#define ADDR_ZB             1
#define ADDR_XSB            2
#define ADDR_CGQ            4
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
#define bpwm       OC1A           //pb1,out,13//�������ѹ���
#define b7555trg   OC1B           //pb2,out,14//7555��������
#define bkmh       BIT(PINC,PC2)  //in,     25
#define bkml       BIT(PINC,PC3)  //in,     26
#define bled       BIT(PORTC,PC4) //out,    27
#define b7555out_r BIT(PIND,PD2)  //int0,   32//�������,������
//
#define led_debug_out 	BIT(PORTC,PC1) //���Ե�
//
#define push_up_disable BIT(SFIOR,PUD)
//
#define pin_bp_out			BIT(PORTD,PD4)//pd4
#define pin_bp_out_bak		BIT(PORTD,PD5)
#define pin_hz_out		0	//BIT(PORTC,PC5)//pc5
#define pin_hz_out_bak	0	//BIT(PORTD,PD0)
//
#define pd_bp_out			BIT(DDRD,PD4)//pd4
#define pd_bp_out_bak		BIT(DDRD,PD1)
#define pd_hz_out			BIT(DDRC,PC5)//pc5
#define pd_hz_out_bak		BIT(DDRD,PD0)
//
#define zd_in				BIT(PIND,PD5)//pd5
#define mb_in				BIT(PIND,PD6)//pd6
#define zk_in				BIT(PIND,PD7)//pd7
#define bk_in				BIT(PINB,PB0)//pb0
////////////////////////////////////////////////////////////////
#define adsc       BIT(ADCSRA,ADSC)
////////////////////////////////////////////////////////////////
//��������
void softward_init(void);
void hardward_init(void);
void deal_key(unsigned char);
void uart_tx(void);
void read_para(void);
void scan_key(void);
unsigned short eeprom_read_word_user(const uint16_t *p);
void eeprom_write_word_user(uint16_t *p,unsigned short val);
void OnTimer(unsigned short id);
void check_pin_function(void);
void check_liaowei_and_output_bp_hz(void);
void app_task(void);
void turn_on_bp_out(void);
void turn_off_bp_out(void);
void turn_on_hz_out(void);
void turn_off_hz_out(void);
unsigned char bp_is_out(void);
unsigned char hz_is_out(void);
unsigned char bp_and_hz_is_out(void);
unsigned short get_mcur_value(unsigned short val);
////////////////////////////////////////////////////////////////
//�ڴ��������
//WINAVR GCC�õ���Little-endian�ṹ,��VC��ͬ
unsigned char ms1_counter,ms10_counter,ms100_counter;
volatile unsigned short m_ms;
unsigned char uart_rx_counter;
unsigned char uart_tx_buf[10],uart_rx_buf[10];
volatile unsigned char uart_tx_addr;

volatile unsigned short m_mh_set,m_ml_set;//��λ�ߵ�
volatile unsigned short m_mcur;
volatile unsigned short m_cur_pwm;//��ǰ��λֵ

volatile unsigned char m_tc2h;//��ʱ��2���������.
volatile unsigned short m_pulse_counter;
volatile UCHAR4 m_pulse_value;

volatile unsigned char m_cgq_ack,m_cgq_ack_delay;//�궨Ӧ��

volatile unsigned long boot_time;

volatile unsigned char uart_frame_head_ok;
volatile unsigned char task_scheduling_ok;
//
volatile UCHAR8 m_pulse_value_bak;
//
////////////////////////////////////////////////////////////////
//EEPROM ��������
EEPROM_DATA m_para[3] __attribute__((section(".eeprom"))) =
{
	{PULSE_TOP,PULSE_BOTTOM},
	{PULSE_TOP,PULSE_BOTTOM},
	{PULSE_TOP,PULSE_BOTTOM}
};
//���ڸ�λ�ã���map�ļ�����eeprom����������ǰ��
unsigned short e_bak[128] __attribute__((section(".eeprom"))) = {0x37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//û���κ�����
////////////////////////////////////////////////////////////////
#endif
///////////////////////////////////
