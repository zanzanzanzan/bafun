
#ifndef __TYPEDEF_USER_H__
#define __TYPEDEF_USER_H__
//
///////////////////////////////////////////////////////////////
//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include  <avr/pgmspace.h>
#define F_CPU 4000000UL
#include <util/delay.h>
#include <avr/sleep.h>
//
#include "macro_user.h"
//
///////////////////////////////////////////////////////////////
//
//���Ͷ���
typedef union _UCHAR2_
{
	unsigned char mc[2];
	unsigned short ms;
}UCHAR2;
//
typedef union _UCHAR4_
{
	unsigned char mc[4];
	unsigned short ms[2];
	unsigned long ml;
}UCHAR4;
//
typedef union _UCHAR8_
{
	unsigned char mc[8];
	unsigned short ms[4];
	unsigned long ml[2];
	unsigned long long mll;
}UCHAR8;
//
typedef struct _BIT_FIELD_
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
//
#define __BIT(x,b)   (*(volatile BIT_FIELD*)(&x)).bit##b
#define BIT(x,b)    __BIT(x,b)
//
typedef union _U8_BIT_
{
	//�����������ȫ��Ϊ������GCC�°汾�Բ�ͬ����ָ��ת���ľ���
	unsigned char ch;
	BIT_FIELD bit;
}U8_BIT;
//
typedef struct __IP_PARA__
{
	//12�ֽ�
	unsigned char ip_addr[4];
	unsigned char net_mask[4];
	unsigned char gateway[4];
}IP_PARA;
//
/*typedef struct __MAC_PARA__
{
	//8�ֽ�
	unsigned short mac_flag;
	unsigned short mac_h;
	unsigned long mac_l;
}MAC_PARA;*/
//
typedef struct __MAC_PARA__
{
	//8�ֽ�
	unsigned short mac_flag;
	unsigned char mac[6];
}MAC_PARA;
//
typedef struct __NET_PARA__
{
	MAC_PARA mac_para;
	IP_PARA ip_para;
}NET_PARA;
//
//ARP֡�ṹ
typedef struct __ARP_FRAME_FORMAT__
{
	unsigned char mac_dec_1[6];//��̫��Ŀ�ĵ�ַ
	unsigned char mac_src[6];//��̫��Դ��ַ
	unsigned short fr_type;//֡����
	unsigned short hw_type;//Ӳ������
	unsigned short pr_type;//Э������
	unsigned char hw_addr_len;//Ӳ����ַ����
	unsigned char pr_addr_len;//Э���ַ����
	unsigned short op_type;//��������
	unsigned char mac_send[6];//���Ͷ���̫����ַ
	unsigned char ip_send[4];//���Ͷ�ip��ַ
	unsigned char mac_dec_2[6];//Ŀ����̫����ַ
	unsigned char ip_dec[4];//Ŀ��ip��ַ
}ARP_FRAME_FORMAT;
//
//Ŀ�ĵ�ַ��
typedef struct __DEC_ADDR_PACKET__
{
	unsigned char mac[6];
	unsigned char ip[4];
	unsigned short port;
}DEC_ADDR_PACKET;
//
//��̫���ײ�
typedef struct __ETH_HEAD__
{
	unsigned char mac_dec[6];
	unsigned char mac_src[6];
	unsigned short eth_type;
}ETH_HEAD;
//
//IP�ײ�
typedef struct __IP_HEAD__
{
	unsigned char ver:4;
	unsigned char len:4;
	unsigned char tos;
	unsigned short total_len;
	unsigned short id;
	unsigned short flag:3;
	unsigned short offset:13;
	unsigned char ttl;
	unsigned char pr;//Э��
	unsigned short sum;//�����
	unsigned char ip_src[4];
	unsigned char ip_dec[4];
}IP_HEAD;
//
//UDP�ײ�
typedef struct __UDP_HEAD__
{
	unsigned short src_port;
	unsigned short dec_port;
	unsigned short len;//�ײ��������ܳ���
	unsigned short sum;//�����
}UDP_HEAD;
//
//UDPα�ײ�
typedef struct __UDP_FAKE_HEAD__
{
	unsigned char ip_src[4];
	unsigned char ip_dec[4];
	unsigned char rev;
	unsigned char pr;
	unsigned short len;
}UDP_FAKE_HEAD;
//
//UDP֡�ṹ
typedef struct __UDP_FRAME_HEAD__
{
	ETH_HEAD eth;
	IP_HEAD ip;
	UDP_HEAD udp;
}UDP_FRAME_HEAD;
//
#define EEPROM_DATA_BYTE_SIZE (sizeof(NVM_DATA)/sizeof(uint8_t))
#define EEPROM_DATA_WORD_SIZE (sizeof(NVM_DATA)/sizeof(uint16_t))
//
typedef union __NVM_DATA__ //Ҫ�������ʧ�洢���ڵ�����
{
	//Ԥ�����ֿռ䣬Ϊ�Ժ�ͨ�ţ��洢���������׼����
	//���Ӳ���ʱע��32λ���룬�������λ������洢��һ�¡�
	//bear��20160625
	//
	unsigned char buf[80];
	struct  
	{
		unsigned short version_h;
		unsigned short version_l;
		//
		////////////////////////////////�ⲿ����
		//��ߺ����ת��(40-300ת/��)
		unsigned short max_vel;//1//2
		unsigned short min_vel;//2//4
		//����������(10-100A)
		unsigned char mmotor_cur1;//3//5
		//������ʱʱ��(0.2-20s,2-200�ٺ���)
		unsigned char rollin_delay;//4//6
		////////////////////////////////�ڲ�����
		//��λ���������������
		unsigned char material_full;//5//7
		//�ϵ�Զ�غ��ϵ���̿���ѡ��
		unsigned char is_remote;//6//8
		//��������ر���ͣ������
		unsigned char mm_cur_overflow;//7//9
		//�Ƿ�������������������������
		unsigned char mm_control;//8//10
		//��������������
		unsigned char cur1_full;//9//11
		//��λ������ֵ�ɼ����趨,Ĭ��ΪAD��������,����Ϊ2ʱ��485�ڽ�������
		unsigned char material_port;//10//12
		////////////////////////////////�ֶ�����
		//�ֶ�ת��
		unsigned short manual_vel;//11//14
		////////////////////////////////ʹ��ʱ������
		//ʹ������
		unsigned short limited_hours;//12//16
		//������
		unsigned char all_stop_id;//13//17
		///////////////////////////////���뱣������
		//������λֵ
		unsigned char material_start;//14//18
		//ͣ����λֵ
		unsigned char material_stop;//15//19
		//��������ֵ
		unsigned char current1_modify;//16//20
		//�����Ѿ����ñ�־��û�иñ�־��������Ѳ����Զ�����ΪĬ��ֵ
		unsigned short para_seted_flag;//17,22
		//MODBUSͨ�Ŵӻ���ַ��ͨ�Ų�����
		unsigned char addr_modbus;//18,23
		unsigned char baud_modbus;//19,24//1:9600Bd,2:19200Bd,3:38400Bd
		//MB��DW����ѡ��1����ƽ��2�����壩
		unsigned char mb_level_pulse;//20//25
		unsigned char dw_level_pulse;//21//26
		//
		unsigned char mb_div;//22//27
		//��������ֵ
		unsigned char current2_modify;//22//28
		//ʹ������
		unsigned short used_hours;//23//30
		//
		//����������(10-100A)
		unsigned char mmotor_cur2;//24//31
		//��������������
		unsigned char cur2_full;//25//32
		//
		NET_PARA w5500_net_para;//26//52
		//
		unsigned int zuo_ling;
        unsigned int zuo_qi;
        unsigned int you_ling;
        unsigned int you_qi;
		unsigned char set_yuzhi;

		/****

		1  ����ת�ٳ�ʼֵ(��ͺ����ת�ٵĳ�ʼֵ)
        2  �����л�����(Ĭ��ֵԭ�����)
        3  ��������������(���ת��-���ת��/12)
        4  ��������ֵ����(12MMA)
		**/
		unsigned char set_default_zhuansu;
		unsigned char set_model;//0 0��ԭ����ģʽ      1 ��PIDģʽ
		unsigned char set_wuliao_PID_P;
		unsigned char set_qiwang_value;  //��������5 ��19Ma

		
        
		//
		//�����Ӳ�����ע��32λ����
	}para;
}NVM_DATA;
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
//��Ҫ��ȷ��λ
typedef union __DATE_TIME_TYPE__
{
	unsigned char buf[6];
	struct
	{
		unsigned char year;
		unsigned char month;
		unsigned char day;
		unsigned char hour;
		unsigned char minute;
		unsigned char second;
	}dt;
}DATE_TIME_TYPE;
//
typedef struct __DS1302_TIME_FORMAT__
{
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
	unsigned char date;
	unsigned char month;
	unsigned char week_day;
	unsigned char year;
}DS1302_TIME_FORMAT;
//
typedef union __DS1302_TIME__
{
	unsigned char buf[7];
	DS1302_TIME_FORMAT tf;
}DS1302_TIME;
//
typedef union __READ_MAINTAIN_REG_FRAME__
{
	unsigned char buf[8];
	struct
	{
		unsigned char addr_modbus;//addr_modbus�Ǵӻ���ַ
		unsigned char code;//������
		unsigned char addr_sh;//Ҫ���ļĴ�����ʼ��ַ�����͸��ֽڣ�
		unsigned char addr_sl;
		unsigned char num_h;//Ҫ���ļĴ������������͸��ֽڣ�
		unsigned char num_l;
		unsigned char crc_l;//���͵��ֽ�
		unsigned char crc_h;
	}frame;	
}READ_MAINTAIN_REG_FRAME;
//
typedef struct __WENDU_HANDLING__
{
	unsigned char gun[2]; //1,2 //������ҹ�
	unsigned char zhou1[2];//3,4 //����1������1
	unsigned char zhou2[2];//5,6 //����2������2
	unsigned char rev[2];//7,8
}WENDU_HANDLING;
//
typedef struct __SYSTEM_STATUS_MODBUS_SUB__
{
	unsigned short m_cur_i;//2//��ǰ����
	unsigned short m_rpm;//4//ת��
	unsigned short m_lw;//6//��λ
	//
	unsigned char m_yk:1;//7:1//Զ��
	unsigned char m_zd:1;//7:2//�Զ�
	unsigned char m_sd:1;//7:3//�ֶ�
	unsigned char m_zdjq:1;//7:4//�������
	unsigned char m_wlgdjq:1;//7:5//ι�Ϲ������
	unsigned char m_lzhz:1;//7:6//��������
	//
	unsigned char m_i_over:1;//7:7//��������
	unsigned char m_comm_error:1;//7:8//ͨ�Ŵ���
	//
	unsigned char m_qr_ql_error:1;//8:1//��������λ�ô���
	//20170514
	unsigned char m_rolllocked:1; //8:2//����/����״̬���������������������
	unsigned char m_duliao:1;//8:3//����״̬��ĥ�Ŷ��ϣ�����������
	unsigned char m_mb_validity:1;//8:4//mb�ź��Ƿ���Ч��ι��������ι�Ϲ��ϣ�
	//
	//20170702
	unsigned char m_mcu_reset:1; //8:5 //��Ƭ����λ�����λ�ñ�־��һ���Ӻ����
	//
	//20170529
	WENDU_HANDLING m_wendu;
	//20170618
	unsigned long m_BootTime; //17-20
	unsigned char m_ResetCause; //21
	unsigned char rev[3];
	
}SYSTEM_STATUS_MODBUS_SUB;
//
typedef union __SYSTEM_STATUS_MODBUS__
{
	unsigned char buf[48]; //Ԥ��һ���ֱ����Ժ����Ӳ���
	SYSTEM_STATUS_MODBUS_SUB status;
}SYSTEM_STATUS_MODBUS;
//
typedef struct __PARA_COMM_PKT__ //����ͨ�Ű���ʽ
{
	NVM_DATA para;
	unsigned char para_advanced_modify_enable;//�߼������޸�ʹ��
	unsigned char rev[7];
}PARA_COMM_PKT;
//
typedef union __WRITE_MULTI_REG_FRAME__
{
	unsigned char buf[8+sizeof(PARA_COMM_PKT)+4];
	struct
	{
		unsigned char addr_modbus;//addr_modbus�Ǵӻ���ַ
		unsigned char code;//������
		unsigned char addr_sh;//Ҫ���ļĴ�����ʼ��ַ�����͸��ֽڣ�
		unsigned char addr_sl;
		unsigned char num_h;//Ҫ���ļĴ������������͸��ֽڣ�
		unsigned char num_l;
		unsigned char byte_size;//����������
		unsigned char rev1;//Ԥ����Ϊ��32λ����
		PARA_COMM_PKT data;
		unsigned char rev2;//Ϊ������MODBUS֡Ҫ��byte_sizeΪż��
		unsigned char rev3;//Ϊ��32λ����
		unsigned char crc_l;//���͵��ֽ�
		unsigned char crc_h;
	}frame;
}WRITE_MULTI_REG_FRAME;
//
typedef struct __APP_HEAD__
{
	unsigned short frame_flag;
	unsigned char cmd_type;
	unsigned char cmd;
	unsigned char frame_id;
	unsigned char len;
	struct
	{
		unsigned char crc8;//���ݵ�crc8У��
		unsigned char sum8;//Ӧ��ͷ��У��ͣ�8λУ��Ͳ��룩
	}verify;
}APP_HEAD;
//
typedef union __SGDK_UDP_READ_MAINTAIN_REG_FRAME__
{
	unsigned char buf[sizeof(APP_HEAD)+sizeof(READ_MAINTAIN_REG_FRAME)];
	struct
	{
		APP_HEAD head;
		READ_MAINTAIN_REG_FRAME val;
	}frame;
}SGDK_UDP_READ_MAINTAIN_REG_FRAME;
//
typedef union __SGDK_UDP_READ_PARA_FRAME__
{
	unsigned char buf[sizeof(APP_HEAD)+sizeof(READ_MAINTAIN_REG_FRAME)];
	struct
	{
		APP_HEAD head;
		READ_MAINTAIN_REG_FRAME val;
	}frame;
}SGDK_UDP_READ_PARA_FRAME;
//
typedef union __SGDK_UDP_WRITE_PARA_FRAME__
{
	unsigned char buf[sizeof(APP_HEAD)+sizeof(WRITE_MULTI_REG_FRAME)];
	struct
	{
		APP_HEAD head;
		WRITE_MULTI_REG_FRAME val;
	}frame;
}SGDK_UDP_WRITE_PARA_FRAME;
//
////////////////////////////////////////////////////////////////
//
#include "pin_def.h"
//
////////////////////////////////////////////////////////////////
#endif


