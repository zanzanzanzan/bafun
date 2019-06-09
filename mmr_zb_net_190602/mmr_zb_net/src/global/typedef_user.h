
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
//类型定义
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
	//定义该类型完全是为了消除GCC新版本对不同类型指针转换的警告
	unsigned char ch;
	BIT_FIELD bit;
}U8_BIT;
//
typedef struct __IP_PARA__
{
	//12字节
	unsigned char ip_addr[4];
	unsigned char net_mask[4];
	unsigned char gateway[4];
}IP_PARA;
//
/*typedef struct __MAC_PARA__
{
	//8字节
	unsigned short mac_flag;
	unsigned short mac_h;
	unsigned long mac_l;
}MAC_PARA;*/
//
typedef struct __MAC_PARA__
{
	//8字节
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
//ARP帧结构
typedef struct __ARP_FRAME_FORMAT__
{
	unsigned char mac_dec_1[6];//以太网目的地址
	unsigned char mac_src[6];//以太网源地址
	unsigned short fr_type;//帧类型
	unsigned short hw_type;//硬件类型
	unsigned short pr_type;//协议类型
	unsigned char hw_addr_len;//硬件地址长度
	unsigned char pr_addr_len;//协议地址长度
	unsigned short op_type;//操作类型
	unsigned char mac_send[6];//发送端以太网地址
	unsigned char ip_send[4];//发送端ip地址
	unsigned char mac_dec_2[6];//目的以太网地址
	unsigned char ip_dec[4];//目的ip地址
}ARP_FRAME_FORMAT;
//
//目的地址包
typedef struct __DEC_ADDR_PACKET__
{
	unsigned char mac[6];
	unsigned char ip[4];
	unsigned short port;
}DEC_ADDR_PACKET;
//
//以太网首部
typedef struct __ETH_HEAD__
{
	unsigned char mac_dec[6];
	unsigned char mac_src[6];
	unsigned short eth_type;
}ETH_HEAD;
//
//IP首部
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
	unsigned char pr;//协议
	unsigned short sum;//反码和
	unsigned char ip_src[4];
	unsigned char ip_dec[4];
}IP_HEAD;
//
//UDP首部
typedef struct __UDP_HEAD__
{
	unsigned short src_port;
	unsigned short dec_port;
	unsigned short len;//首部和数据总长度
	unsigned short sum;//反码和
}UDP_HEAD;
//
//UDP伪首部
typedef struct __UDP_FAKE_HEAD__
{
	unsigned char ip_src[4];
	unsigned char ip_dec[4];
	unsigned char rev;
	unsigned char pr;
	unsigned short len;
}UDP_FAKE_HEAD;
//
//UDP帧结构
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
typedef union __NVM_DATA__ //要存入非易失存储器内的数据
{
	//预留部分空间，为以后通信，存储更多参数做准备。
	//增加参数时注意32位对齐，避免跟上位机软件存储不一致。
	//bear，20160625
	//
	unsigned char buf[80];
	struct  
	{
		unsigned short version_h;
		unsigned short version_l;
		//
		////////////////////////////////外部参数
		//最高和最低转速(40-300转/分)
		unsigned short max_vel;//1//2
		unsigned short min_vel;//2//4
		//主电机额定电流(10-100A)
		unsigned char mmotor_cur1;//3//5
		//合轧延时时间(0.2-20s,2-200百毫秒)
		unsigned char rollin_delay;//4//6
		////////////////////////////////内部参数
		//料位传感器满量程输出
		unsigned char material_full;//5//7
		//上电远控和上电键盘控制选择
		unsigned char is_remote;//6//8
		//主电机过载保护停机电流
		unsigned char mm_cur_overflow;//7//9
		//是否根据主电机电流进行连锁控制
		unsigned char mm_control;//8//10
		//电流传感器量程
		unsigned char cur1_full;//9//11
		//料位传感器值采集端设定,默认为AD采样数据,设置为2时从485口接收数据
		unsigned char material_port;//10//12
		////////////////////////////////手动参数
		//手动转数
		unsigned short manual_vel;//11//14
		////////////////////////////////使用时间限制
		//使用天数
		unsigned short limited_hours;//12//16
		//测试用
		unsigned char all_stop_id;//13//17
		///////////////////////////////密码保护参数
		//启动料位值
		unsigned char material_start;//14//18
		//停机料位值
		unsigned char material_stop;//15//19
		//电流修正值
		unsigned char current1_modify;//16//20
		//参数已经设置标志，没有该标志，软件将把参数自动设置为默认值
		unsigned short para_seted_flag;//17,22
		//MODBUS通信从机地址和通信波特率
		unsigned char addr_modbus;//18,23
		unsigned char baud_modbus;//19,24//1:9600Bd,2:19200Bd,3:38400Bd
		//MB和DW输入选择（1：电平，2：脉冲）
		unsigned char mb_level_pulse;//20//25
		unsigned char dw_level_pulse;//21//26
		//
		unsigned char mb_div;//22//27
		//电流修正值
		unsigned char current2_modify;//22//28
		//使用天数
		unsigned short used_hours;//23//30
		//
		//主电机额定电流(10-100A)
		unsigned char mmotor_cur2;//24//31
		//电流传感器量程
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

		1  增加转速初始值(最低和最高转速的初始值)
        2  增加切换开关(默认值原来你的)
        3  增比例参数设置(最高转速-最低转速/12)
        4  增加期望值设置(12MMA)
		**/
		unsigned char set_default_zhuansu;
		unsigned char set_model;//0 0是原来的模式      1 是PID模式
		unsigned char set_wuliao_PID_P;
		unsigned char set_qiwang_value;  //期望是是5 到19Ma

		
        
		//
		//新增加参数，注意32位对齐
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
//需要精确定位
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
		unsigned char addr_modbus;//addr_modbus是从机地址
		unsigned char code;//功能码
		unsigned char addr_sh;//要读的寄存器起始地址（先送高字节）
		unsigned char addr_sl;
		unsigned char num_h;//要读的寄存器个数（先送高字节）
		unsigned char num_l;
		unsigned char crc_l;//先送低字节
		unsigned char crc_h;
	}frame;	
}READ_MAINTAIN_REG_FRAME;
//
typedef struct __WENDU_HANDLING__
{
	unsigned char gun[2]; //1,2 //左辊，右辊
	unsigned char zhou1[2];//3,4 //左轴1，右轴1
	unsigned char zhou2[2];//5,6 //左轴2，右轴2
	unsigned char rev[2];//7,8
}WENDU_HANDLING;
//
typedef struct __SYSTEM_STATUS_MODBUS_SUB__
{
	unsigned short m_cur_i;//2//当前电流
	unsigned short m_rpm;//4//转速
	unsigned short m_lw;//6//料位
	//
	unsigned char m_yk:1;//7:1//远控
	unsigned char m_zd:1;//7:2//自动
	unsigned char m_sd:1;//7:3//手动
	unsigned char m_zdjq:1;//7:4//主电机启
	unsigned char m_wlgdjq:1;//7:5//喂料辊电机启
	unsigned char m_lzhz:1;//7:6//离轧合轧
	//
	unsigned char m_i_over:1;//7:7//电流过载
	unsigned char m_comm_error:1;//7:8//通信错误
	//
	unsigned char m_qr_ql_error:1;//8:1//轧的左右位置错误
	//20170514
	unsigned char m_rolllocked:1; //8:2//解锁/上锁状态，轧距上锁（轧距解锁）
	unsigned char m_duliao:1;//8:3//堵料状态，磨膛堵料（提料正常）
	unsigned char m_mb_validity:1;//8:4//mb信号是否有效，喂料正常（喂料故障）
	//
	//20170702
	unsigned char m_mcu_reset:1; //8:5 //单片机复位后会置位该标志，一分钟后清除
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
	unsigned char buf[48]; //预留一部分便于以后增加参数
	SYSTEM_STATUS_MODBUS_SUB status;
}SYSTEM_STATUS_MODBUS;
//
typedef struct __PARA_COMM_PKT__ //参数通信包格式
{
	NVM_DATA para;
	unsigned char para_advanced_modify_enable;//高级参数修改使能
	unsigned char rev[7];
}PARA_COMM_PKT;
//
typedef union __WRITE_MULTI_REG_FRAME__
{
	unsigned char buf[8+sizeof(PARA_COMM_PKT)+4];
	struct
	{
		unsigned char addr_modbus;//addr_modbus是从机地址
		unsigned char code;//功能码
		unsigned char addr_sh;//要读的寄存器起始地址（先送高字节）
		unsigned char addr_sl;
		unsigned char num_h;//要读的寄存器个数（先送高字节）
		unsigned char num_l;
		unsigned char byte_size;//不包含本身
		unsigned char rev1;//预留，为了32位对齐
		PARA_COMM_PKT data;
		unsigned char rev2;//为了满足MODBUS帧要求，byte_size为偶数
		unsigned char rev3;//为了32位对齐
		unsigned char crc_l;//先送低字节
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
		unsigned char crc8;//内容的crc8校验
		unsigned char sum8;//应用头部校验和（8位校验和补码）
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


