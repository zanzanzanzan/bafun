/*
 * core_func.c
 *
 * Created: 2014/11/13 18:11:25
 *  Author: xiong
 */ 
/////////////////////////////////////////////////////////////////
#include "typedef_user.h"
#include "conf_w5500.h"
#include "soft_timer.h"
#include "display.h"
#include "modbus_rtu.h"
/////////////////////////////////////////////////////////////////

extern unsigned int zan_count ;
extern unsigned int bD2_weiyi ;
extern unsigned int bD3_weiyi ;


extern unsigned int li_he_zha;
extern unsigned int zuo_gao;
extern unsigned int zuo_di;


extern unsigned int you_gao;
extern unsigned int you_di;




void fmotor_vel(void);
void fmotor_vel_a(void);
void read_para(unsigned char);
void mmotor_start(void);
void mmotor_stop(void);
inline void roll_in(void);
inline void roll_out(void);
inline void roll_lock(void);
inline void roll_unlock(void);
void fmotor_start(void);
void fmotor_stop(void);
void auto_run(void);
void manual_run(void);
void key_manual_run(void);
void key_auto_run(void);
void reset_set_status(void);
void all_stop(void);
void part_stop(void);
void enable_wait_rollin(void);
void deal_wait_rollin(void);
void material_2_vel(unsigned short);
unsigned char eeprom_read_byte_user(const uint8_t *p);
void eeprom_write_byte_user(uint8_t *p,unsigned char val);
unsigned short eeprom_read_word_user(const uint16_t *p);
void eeprom_write_word_user(uint16_t *p,unsigned short val);
void uart_tx(void);
void save_para(void);
void runed_time(void);
void run_in_remote_ctrl(void);
void wdt_reset_mcu(void);
void set_modbus_baud(unsigned char baud);
void set_default_para(void);
void write_para_to_eeprom(NVM_DATA *p,unsigned char rst_used_hours);
unsigned short get_mb_counter_60s(void);
unsigned char get_mb_counter_3s(void);
unsigned short get_dw_counter_60s(void);
unsigned char get_dw_counter_3s(void);
void qr_ql_signal_handling(void);
void mb_signal_handling(void);
void dw_signal_handling(void);
void lw_signal_handling(void);
/////////////////////////////////////////////////////////////////
//内存变量定义
//WINAVR GCC用的是Little-endian结构,和VC相同
/////////////在显示板上显示的灯状态
volatile U8_BIT led_buf[14];
#define bit_led_auto        BIT(led_buf[0].bit,0)//led defination
#define bit_led_manual      BIT(led_buf[0].bit,1)
#define bit_led_mmotor      BIT(led_buf[0].bit,3)
#define bit_led_xl          BIT(led_buf[0].bit,2)
#define bit_led_remote      BIT(led_buf[0].bit,4)
#define bit_led_hz          BIT(led_buf[0].bit,5)
#define bit_led_lz          BIT(led_buf[0].bit,6)
#define bit_led_over_i      BIT(led_buf[0].bit,7)//O8

#define bit_led_cssd        BIT(led_buf[7].bit,0)//O9//参数设定
#define bit_led_dw          BIT(led_buf[7].bit,1)//低位
#define bit_led_gw          BIT(led_buf[7].bit,2)//高位
#define bit_led_dl          BIT(led_buf[7].bit,3)//堵料
#define bit_led_up          BIT(led_buf[7].bit,4)//第一屏
#define bit_led_down        BIT(led_buf[7].bit,5)//第二屏
///////////////
const unsigned short version_h=0x0419; //19041114
const unsigned short version_l=0x1411;
///////////////
unsigned char m_rx485_tx_addr;
unsigned char ms1_counter,ms10_counter,ms100_counter;
volatile unsigned char comm_error_delay;
volatile unsigned short ad_counter[8];
volatile UCHAR4 ad_add_value[8];
unsigned char uart_rx_counter;
unsigned char uart_rx_buf[30],uart_tx_buf[40];
unsigned char temp_data[80] = {0};//温度分配公司，0-1 代表一个温度
unsigned char tempe_flag= 0;

volatile unsigned char uart_tx_addr;
//
volatile unsigned long boot_time;
///////////////////////////////////
volatile unsigned char bit_auto;
volatile unsigned char bit_manual;
volatile unsigned char bit_mmotor;
volatile unsigned char bit_fmotor;
volatile unsigned char bit_remote;
volatile unsigned char bit_h_mtl;
volatile unsigned char bit_l_mtl;
volatile unsigned char bit_over_i;
//
volatile unsigned char bit_duliao;
volatile unsigned char bit_rolllock;
volatile unsigned char bit_current_1st;
volatile unsigned char bit_current_2nd;
//
/////////////
volatile unsigned char  bit_setmode_1;
volatile unsigned char  bit_setmode_2;//内部参数
volatile unsigned char  bit_wait_rollin;
volatile unsigned char  bit_mm_control;//主电机电流连锁控制
volatile unsigned char  bit_comm_error;
volatile unsigned char  bit_setmode_password;
volatile unsigned char  bit_setmode_delay;
volatile unsigned char  bit_used_no_time;
//
volatile unsigned char  bit_xieliao;
volatile unsigned char  bit_alarm_mode;
volatile unsigned char  material_2_vel_enable;
volatile unsigned char  bit_setmode_3;
volatile unsigned char  bit_setmode;
volatile unsigned char  bit_setmode_pwd_time;
volatile unsigned char  bit_lock;
volatile unsigned char  bit_roll_in;
//
volatile unsigned char  bit_qr_ql_error;
volatile unsigned char  bit_monitor_mode;
volatile unsigned char  bit_material_disturbance;
volatile unsigned char  bit_soft_uart_rx;
volatile unsigned char  bit_soft_uart_tx;
volatile unsigned char  soft_uart_frame_head_ok;
volatile unsigned char  timer0_overflow_count;
//
volatile unsigned char  uart_frame_head_ok;
volatile unsigned char  uart_frame_ok;
volatile unsigned char  mb_is_invalid;
////////////////////////////////////////////////////////////////
//时间设置进入密码
const unsigned char const_password[6]="010101";//697278
//密码保护参数进入密码
const unsigned char arg_password[6]="444444";//234567
unsigned char password[6];
////////////////////////////////////////////////////////////////
volatile unsigned char key_value;
UCHAR2 deal_key_buf[3];
UCHAR2 key_value_bak;
volatile unsigned short m_cur_d1,m_cur_d2,m_material,m_material_value,m_cur_cal,m_cur_cal2;
volatile unsigned short m_material_value_monitor;
volatile unsigned short m_cur_monitor;
volatile UCHAR2 m_material_add_value;
volatile unsigned char m_material_counter;
volatile unsigned short auto_vel;
//
volatile unsigned char cur_overflow_delay;
//主电机启动,停止,合轧,离轧信号为脉冲,高电平时间为2秒
volatile unsigned char mmotor_delay,roll_delay,password_delay;
volatile unsigned long long_pressed_cssd_delay,long_pressed_csqr_delay;
volatile unsigned char pagenumber,key_delay,key_delay_set_in,cur_d1_round,cur_d2_round,m_cur_round;
//等待合轧的计时(100ms)
volatile unsigned char wait_rollin,set_idle_wait;
volatile unsigned char enable_mb_delay,mb_invalid_delay,mb_valid_counter;
unsigned short m_set_dis_value;//设置值的显示
unsigned short mm_cur_overflow_stop,mmotor_cur1_cmp;
#if __D2_USED__ == TRUE
unsigned short mmotor_cur2_cmp;
#endif
//使用时间限制
volatile unsigned short m_used_1s,m_used_12h;
//
volatile unsigned char m_cgq_cmd;
//
volatile unsigned char arg_set_process;
volatile unsigned short m_material_stop_modify,m_material_start_modify;
volatile unsigned char m_cgq_ack,m_cgq_ack_delay;
volatile unsigned char part_stop_bmb;
//
volatile unsigned short mb_counter_3s,dw_counter_3s;
volatile unsigned short mb_counter_60s,dw_counter_60s;
volatile unsigned short mb_counter_tmp,dw_counter_tmp;
//
extern volatile unsigned char reset_cause;
//
////////////////////////////////////////////////////////////////
//EEPROM 变量定义
NVM_DATA m_ePara[5] __attribute__((section(".eeprom")));
//
NVM_DATA m_rPara;
//
unsigned short m_vel_alarm=0;//用于告警判断
//恢复默认设置(设置值为8时，所有的参数恢复出厂默认值)
volatile unsigned char set_default;
//使用天数
volatile unsigned short limited_days;
//
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
#if FMOTOR_VEL_0V_10V == TRUE
void fmotor_vel(void)
{
	//输出电压,控制变频器
	//
	if(1 == bit_xieliao)
	{
		bpwmb = ((m_rPara.para.max_vel>>1)*9)/3;
	}
	else if(1 == bit_auto)
	{
		if(auto_vel > m_rPara.para.max_vel)
		{
			auto_vel = m_rPara.para.max_vel;
		}
		if(auto_vel <= m_rPara.para.min_vel)
		{
			auto_vel = 0;
			bpwmb = 0;
		}
		else
		{
			if(0 == bit_fmotor)
			{
				bpwmb = 0;
			}
			else
			{
				bpwmb = (auto_vel*9)/3;//9v - 300转
			}
		}
	}
	else
	{
		if(1 == bit_fmotor)
		{
			if(m_rPara.para.manual_vel > m_rPara.para.max_vel)
			{
				m_rPara.para.manual_vel = m_rPara.para.max_vel;
			}
			if(m_rPara.para.manual_vel < m_rPara.para.min_vel)
			{
				m_rPara.para.manual_vel = m_rPara.para.min_vel;
			}
			bpwmb = (m_rPara.para.manual_vel*9)/3;//9v - 300转
		}
		else
		{
			bpwmb = 0;
		}
	}
}
//
#else    ////////////////////////////////////////////////////////
//
void fmotor_vel(void)
{
	//输出电压,控制变频器，最终到达变频器的是4-20mA电流
	//
	//0-300 --> 200-1000(-200)
	//
	if(1 == bit_xieliao)
	{
		//正在卸料
		bpwmb = (((m_rPara.para.max_vel>>1)*8)/3)+200; //4-20mA
	}
	else if(1 == bit_auto)
	{
		//自动模式下
		if(auto_vel > m_rPara.para.max_vel)
		{
			//自动运行最大转速限制
			auto_vel = m_rPara.para.max_vel;
		}
		if(auto_vel <= m_rPara.para.min_vel)
		{
			//自动运行最小转速限制
			auto_vel = 0;
			bpwmb = 200; //4mA
		}
		else
		{
			if(0 == bit_fmotor)
			{
				//喂料辊已经停机
				bpwmb = 200;//4mA
			}
			else
			{
				//转速转换为控制变频器的电压
				bpwmb = ((auto_vel*8)/3)+200; //20mA - 300转
			}
		}
	}
	else
	{
		if(1 == bit_fmotor)
		{
			//喂料辊电机运行中
			if(m_rPara.para.manual_vel > m_rPara.para.max_vel)
			{
				//手动运行最大转速控制
				m_rPara.para.manual_vel = m_rPara.para.max_vel;
			}
			if(m_rPara.para.manual_vel < m_rPara.para.min_vel)
			{
				//手动运行最小转速控制
				m_rPara.para.manual_vel = m_rPara.para.min_vel;
			}
			//
			//转速转换为控制变频器的电压
			bpwmb = ((m_rPara.para.manual_vel*8)/3)+200; //20mA - 300转
		}
		else
		{
			//喂料辊停机
			bpwmb = 200; //4mA
		}
	}
}
#endif

#if FMOTOR_VEL_A_0V_10V == TRUE
void fmotor_vel_a(void)
{
	//输出电压,控制变频器
	//
	if(1 == bit_xieliao)
	{
		bpwma = ((m_rPara.para.max_vel>>1)*9)/3;
	}
	else if(1 == bit_auto)
	{
		if(auto_vel > m_rPara.para.max_vel)
		{
			auto_vel = m_rPara.para.max_vel;
		}
		if(auto_vel <= m_rPara.para.min_vel)
		{
			auto_vel = 0;
			bpwma = 0;
		}
		else
		{
			if(0 == bit_fmotor)
			{
				bpwma = 0;
			}
			else
			{
				bpwma = (auto_vel*9)/3;//9v - 300转
			}
		}
	}
	else
	{
		if(1 == bit_fmotor)
		{
			if(m_rPara.para.manual_vel > m_rPara.para.max_vel)
			{
				m_rPara.para.manual_vel = m_rPara.para.max_vel;
			}
			if(m_rPara.para.manual_vel < m_rPara.para.min_vel)
			{
				m_rPara.para.manual_vel = m_rPara.para.min_vel;
			}
			bpwma = (m_rPara.para.manual_vel*9)/3;//9v - 300转
		}
		else
		{
			bpwma = 0;
		}
	}
}
//
#else    ////////////////////////////////////////////////////////
//
void fmotor_vel_a(void)
{
	//输出电压,控制变频器，最终到达变频器的是4-20mA电流
	//
	//0-300 --> 200-1000(-200)
	//
	if(1 == bit_xieliao)
	{
		//正在卸料
		bpwma = (((m_rPara.para.max_vel>>1)*8)/3)+200; //4-20mA
	}
	else if(1 == bit_auto)
	{
		//自动模式下
		if(auto_vel > m_rPara.para.max_vel)
		{
			//自动运行最大转速限制
			auto_vel = m_rPara.para.max_vel;
		}
		if(auto_vel <= m_rPara.para.min_vel)
		{
			//自动运行最小转速限制
			auto_vel = 0;
			bpwma = 200; //4mA
		}
		else
		{
			if(0 == bit_fmotor)
			{
				//喂料辊已经停机
				bpwma = 200;//4mA
			}
			else
			{
				//转速转换为控制变频器的电压
				bpwma = ((auto_vel*8)/3)+200; //20mA - 300转
			}
		}
	}
	else
	{
		if(1 == bit_fmotor)
		{
			//喂料辊电机运行中
			if(m_rPara.para.manual_vel > m_rPara.para.max_vel)
			{
				//手动运行最大转速控制
				m_rPara.para.manual_vel = m_rPara.para.max_vel;
			}
			if(m_rPara.para.manual_vel < m_rPara.para.min_vel)
			{
				//手动运行最小转速控制
				m_rPara.para.manual_vel = m_rPara.para.min_vel;
			}
			//
			//转速转换为控制变频器的电压
			bpwma = ((m_rPara.para.manual_vel*8)/3)+200; //20mA - 300转
		}
		else
		{
			//喂料辊停机
			bpwma = 200; //4mA
		}
	}
}
#endif
/////////////////////////////////////////////////////////////////
void mmotor_start(void)
{
	// mmotor_delay = 0;
	bmmstart = 1;
	ddr_zk = 1;
	//bmmstop = 0;
	bit_mmotor = 1;
	modbus_status.status.m_zdjq = STATUS_ON;
}
/////////////////////////////////////////////////////////////////
void mmotor_stop(void)
{
	//mmotor_delay = 0;
	bmmstart = 0;
	ddr_zk = 1;
	//bmmstop = 1;
	bit_mmotor = 0;
	modbus_status.status.m_zdjq = STATUS_OFF;
}
/////////////////////////////////////////////////////////////////
void roll_in(void)
{
	//roll_delay = 0;
	//brollin = 1;
	ddr_hz = 1;
	li_he_zha = 1;
	bit_led_hz=1;
	//brollout = 0;
	modbus_status.status.m_lzhz = STATUS_ON;
	//
	//bit_qr_ql_error = 0;
	
	modbus_status.status.m_qr_ql_error = STATUS_OFF;
	//SetTimer(MT_START_QR_QL_CHECK,QR_QL_CHECH_DELAY,0);//20秒后启动QR和QL检测
}
/////////////////////////////////////////////////////////////////
void roll_out(void)
{
	//roll_delay = 0;
	//brollin = 0;
	li_he_zha = 0;
	ddr_hz = 1;
	// brollout = 1;
	bit_led_hz=0;
	modbus_status.status.m_lzhz = STATUS_OFF;
	//
	//bit_qr_ql_error = 0;
	modbus_status.status.m_qr_ql_error = STATUS_OFF;
	//KillTimer(MT_START_QR_QL_CHECK);//停止QR和QL的检测
	//KillTimer(MT_CHECK_QR_QL);
	//
	// 不管什么情况，离轧就清除等待合轧标志。bear，20160102
	bit_wait_rollin = 0;
}
/////////////////////////////////////////////////////////////////
void roll_lock(void)
{
	brolllock = 1; //解锁
	ddr_lz = 1;
	//
	bit_rolllock = 1;
}
/////////////////////////////////////////////////////////////////
void roll_unlock(void)
{
	brolllock = 0; //上锁
	ddr_lz = 1;
	//
	bit_rolllock = 0;
}
/////////////////////////////////////////////////////////////////
void fmotor_start(void)
{
	bftstart = 1;
	ddr_wg = 1;
	bit_fmotor = 1;
	modbus_status.status.m_wlgdjq = STATUS_ON;
	//
	enable_mb_delay = 0;
	mb_invalid_delay = 0;
}
/////////////////////////////////////////////////////////////////
void fmotor_stop(void)
{
	bftstart = 0;
	ddr_wg = 1;
	bit_fmotor = 0;
	modbus_status.status.m_wlgdjq = STATUS_OFF;
}
/////////////////////////////////////////////////////////////////
void all_stop(void)
{
	//总停机
	//if((1==bit_mmotor)||(1==bit_fmotor))
	{
		mmotor_stop();
		part_stop();
		mb_is_invalid = 0;
		KillTimer(MT_AUTO_RUN_DELAY);
	}
}
/////////////////////////////////////////////////////////////////
void part_stop(void)
{
	//部分停机
	fmotor_stop();
	roll_out();
	//DEBUG_STRING("fmotor stop in part_stop\r\n\0");
	//
	bit_auto = 0;
	modbus_status.status.m_zd = STATUS_OFF;
	bit_manual = 0;
	modbus_status.status.m_sd = STATUS_OFF;
	bit_xieliao = 0;
	mb_is_invalid = 0;
	//
	//清除mb报警信号，20160102
	ddr_mbbj = 1;
	//bmbbj = bit_duliao | mb_is_invalid;//该处可以写的更加简洁，为了规范还是写全较好
}
/////////////////////////////////////////////////////////////////
void key_auto_run(void)
{
	#if MUSTSTOPFIRST == TRUE
	if((0==bit_auto) && (0==bit_manual))
	#endif
	{
		bit_auto = 1;
		modbus_status.status.m_zd = STATUS_ON;
		bit_manual = 0;
		modbus_status.status.m_sd = STATUS_OFF;
		bit_xieliao = 0;
		enable_mb_delay = 0;
		//
		TRACE_STRING("key_auto_run");
		reset_set_status();
	}
}
/////////////////////////////////////////////////////////////////
void key_manual_run(void)
{
	#if MUSTSTOPFIRST == TRUE
	if((0==bit_auto) && (0==bit_manual))
	#endif
	{
		bit_manual = 1;
		modbus_status.status.m_sd = STATUS_ON;
		bit_auto = 0;
		modbus_status.status.m_zd = STATUS_OFF;
		bit_xieliao = 0;
		enable_mb_delay = 0;
		//
		fmotor_start();//喂料电机启动
		enable_wait_rollin();
		//
		TRACE_STRING("key_manual_run");
		reset_set_status();
	}
}
/////////////////////////////////////////////////////////////////
void reset_set_status(void)
{
	bit_setmode = 0;
	bit_setmode_1 = 0;
	bit_setmode_2 = 0;
	bit_setmode_3 = 0;
	bit_setmode_password = 0;
	bit_setmode_delay = 0;
	pagenumber = 0;
	set_idle_wait = 0;
    TRACE_STRING("reset_set_status");
	read_para(1);//不允许初始化上电参数
	//
	arg_set_process = 0xff;
	bit_setmode_pwd_time = 0;
	//
	set_default = 0;
}
/////////////////////////////////////////////////////////////////
void auto_run(void)
{
	//喂料电机启动信号是一个稳定电平信号
	if(1 == bit_auto)
	{
		if(1 == bftstart)
		{
			//喂料电机启动
			if(m_material < 5)//小于1v
			{
				//计算料位值的时候已经修正了停机比较值
				fmotor_stop();
				roll_out();
				//DEBUG_STRING("fmotor stop in auto_run\r\n\0");
				//
				part_stop_bmb = 0;
				//
				ddr_mbbj = 1;
				//bmbbj = bit_duliao | mb_is_invalid;
			}
		}
		else
		{
			//喂料电机停机
			//if(m_material > (unsigned short)2*12890)//大于3v
			if(m_material > (m_material_start_modify - 12890))//在料位计算过程中m_material已经减了12890
			{
				if(0 == part_stop_bmb)
				{
					fmotor_start();//喂料电机启动
					enable_wait_rollin();
				}
				else if(0 == mb_is_invalid)
				{
					part_stop_bmb = 0;
					//
					ddr_mbbj = 1;
					//bmbbj = bit_duliao | mb_is_invalid;//该处可以写的更加简洁，为了规范还是写全较好
					//
					fmotor_start();//喂料电机启动
					enable_wait_rollin();
				}
			}
		}
		//
		deal_wait_rollin();
	}
}
/////////////////////////////////////////////////////////////////
void manual_run(void)
{
	if(1==bit_manual)
	{
		deal_wait_rollin();
	}
}
/////////////////////////////////////////////////////////////////
void enable_wait_rollin(void)
{
	bit_wait_rollin = 1;
	wait_rollin=0;
}
/////////////////////////////////////////////////////////////////
void deal_wait_rollin(void)
{
	if(1 == bit_wait_rollin)
	{
		if(wait_rollin > m_rPara.para.rollin_delay)
		{
			bit_wait_rollin = 0;
			roll_in();
		}
	}
}
extern unsigned char set_date;
extern unsigned char temp1_flag_count;
/////////////////////////////////////////////////////////////////
extern char canshu_peizhi;
char xianshi_qiehua = 0;
char wendu_qiehua = 0;

void uart_tx(void)
{
	unsigned char i;
	UCHAR4 temp;
	unsigned short m_vel,m_material_value_temp;
	//

	/*if (set_date >= 90)
	{
		set_date =0;
	}


	set_date++;*/
/*	switch (set_date)
	{
		case 
		
		
	}
  */ 
	if (set_date != 0)
	{
	
		{
			uart_tx_buf[0] = 0xaa; 
			wendu_qiehua ++;
			if (wendu_qiehua%3 ==0)
			{
				uart_tx_buf[1] = ADDR_XSB_5;
				
				uart_tx_buf[2] = temp_data[2 + (set_date -1) * 20];	//936
				uart_tx_buf[3] = temp_data[3 + (set_date -1) * 20]; 


				uart_tx_buf[4] = temp_data[4 + (set_date -1) * 20];	//937
				uart_tx_buf[5] = temp_data[5 + (set_date -1) * 20];	

				uart_tx_buf[6] = temp_data[6 + (set_date -1) * 20];	//938				
				uart_tx_buf[7] = temp_data[7 + (set_date -1) * 20]; 


				uart_tx_buf[8] = set_date; 
			}else if (wendu_qiehua%3 ==1)
			{
				uart_tx_buf[1] = ADDR_XSB_6;
				
				uart_tx_buf[2] = temp_data[8 + (set_date -1) * 20];	 //939
				uart_tx_buf[3] = temp_data[9 + (set_date -1) * 20]; 


				uart_tx_buf[4] = temp_data[10 + (set_date -1) * 20];  //940
				uart_tx_buf[5] = temp_data[11 + (set_date -1) * 20];  

				uart_tx_buf[6] = temp_data[12 + (set_date -1) * 20];  //941 
				uart_tx_buf[7] = temp_data[13 + (set_date -1) * 20]; 


				uart_tx_buf[8] = 0; 
			}else
			{
				uart_tx_buf[1] = ADDR_XSB_7;
				
				uart_tx_buf[2] = temp_data[14 + (set_date -1) * 20];   //942
				uart_tx_buf[3] = temp_data[15 + (set_date -1) * 20]; 


				uart_tx_buf[4] = temp_data[16 + (set_date -1) * 20];  //943
				uart_tx_buf[5] = temp_data[17 + (set_date -1) * 20];  

				uart_tx_buf[6] = temp_data[18 + (set_date -1) * 20];  //944 
				uart_tx_buf[7] = temp_data[19 + (set_date -1) * 20]; 


				uart_tx_buf[8] = 0; 

			}
			
		}
		
		uart_tx_buf[9] = 0;
		for(i=0; i<9; i++)
		{
			uart_tx_buf[9] += uart_tx_buf[i];
		}

		
		b485en = 1;
		delay_us(100);
		UDR0 = uart_tx_buf[0];
		uart_tx_addr = 1;
	}
	else //7if (set_date > 1)
	{
		canshu_peizhi++;

		uart_tx_buf[0] = 0xaa; 

		switch(xianshi_qiehua % 6)
		{
			case 0: 
			{
				uart_tx_buf[1] = ADDR_XSB_1;
				
				uart_tx_buf[2] = m_rPara.para.max_vel / 256;  //600
				uart_tx_buf[3] = m_rPara.para.max_vel % 256;


				uart_tx_buf[4] = m_rPara.para.min_vel / 256;  //601
				uart_tx_buf[5] = m_rPara.para.min_vel  % 256;

				uart_tx_buf[6] = m_rPara.para.dw_level_pulse;	//620
				
				uart_tx_buf[7] = m_rPara.para.rollin_delay;  //602


				uart_tx_buf[8] = m_rPara.para.material_full; //603	   no use
				break;
			}
			case 1:
			{
				uart_tx_buf[1] = ADDR_XSB_2_2;
				
				uart_tx_buf[2] = m_rPara.para.is_remote;	 //604

				uart_tx_buf[3] = m_rPara.para.mmotor_cur1; //605

				uart_tx_buf[4] = m_rPara.para.mm_cur_overflow; //606

				uart_tx_buf[5] = m_rPara.para.mm_control;	 //607

				uart_tx_buf[6] = m_rPara.para.cur1_full;	 //608
				
				/*
				uart_tx_buf[15] = m_rPara.para.material_port; //609    no use

				uart_tx_buf[16] = m_rPara.para.manual_vel / 256;//610  no use
				uart_tx_buf[17] = m_rPara.para.manual_vel  % 256;

				uart_tx_buf[18] = m_rPara.para.limited_hours / 256; //611  no use
				uart_tx_buf[7] = m_rPara.para.limited_hours  % 256;

				uart_tx_buf[20] = m_rPara.para.all_stop_id; 		//612	no use
				*/

				uart_tx_buf[7] = m_rPara.para.material_start;	  //613
				
				uart_tx_buf[8] = m_rPara.para.material_stop;	//614
				 break;

			}
			case 2:
			{

				uart_tx_buf[1] = ADDR_XSB_3;
				
				uart_tx_buf[2] = m_rPara.para.current1_modify;	 //615

				/*
				uart_tx_buf[24] = m_rPara.para.para_seted_flag / 256; //616   no use
				uart_tx_buf[25] = m_rPara.para.para_seted_flag	% 256;

				uart_tx_buf[26] = m_rPara.para.addr_modbus;   //617   no use


				uart_tx_buf[27] = m_rPara.para.baud_modbus;   //618   no use
				*/
				uart_tx_buf[3] = m_rPara.para.mb_level_pulse;  //619  



				uart_tx_buf[4] = m_rPara.para.mb_div;	//621

				uart_tx_buf[5] = m_rPara.para.current2_modify; //622

				/*
				uart_tx_buf[6] = m_rPara.para.used_hours / 256;  //623	 no use
				uart_tx_buf[7] = m_rPara.para.used_hours  % 256;	
				*/
				uart_tx_buf[6] = m_rPara.para.mmotor_cur2;	//949


				uart_tx_buf[7] = m_rPara.para.cur2_full;   //625	
				uart_tx_buf[8] = m_rPara.para.material_full;
				 break;

			}

			case 3: 
			{
				uart_tx_buf[1] = ADDR_XSB_4;
				
				uart_tx_buf[2] = m_rPara.para.w5500_net_para.ip_para.ip_addr[0];  //630
				uart_tx_buf[3] = m_rPara.para.w5500_net_para.ip_para.ip_addr[1];  //631


				uart_tx_buf[4] = m_rPara.para.w5500_net_para.ip_para.ip_addr[2];  //632
				uart_tx_buf[5] = m_rPara.para.w5500_net_para.ip_para.ip_addr[3];  //633

				uart_tx_buf[6] = m_rPara.para.addr_modbus;	 //634
				
				uart_tx_buf[7] = set_date;	//温度标价			


				//uart_tx_buf[8] = temp_data[1]; //	 no use
				uart_tx_buf[8] = m_rPara.para.all_stop_id;//	 635
				break;
			}
			case 4: 
			{
				uart_tx_buf[1] = ADDR_XSB_9;
				
				uart_tx_buf[2] = m_rPara.para.set_default_zhuansu;  //990
				uart_tx_buf[3] = m_rPara.para.set_model;  			//991


				uart_tx_buf[4] = m_rPara.para.set_wuliao_PID_P;  	//992
				uart_tx_buf[5] = m_rPara.para.set_qiwang_value;  	//993

				uart_tx_buf[6] = 0;	
				
				uart_tx_buf[7] = 0;				

				uart_tx_buf[8] = 0;
				break;
			}
			default :
			{
				{

			//
			//?????????
			/*
			bit_led_auto = bit_auto;//??
			bit_led_manual = bit_manual;//??
			bit_led_mmotor = bit_mmotor;//?????
			bit_led_remote = bit_remote;//??
			//bit_led_over_i = bit_over_i|mb_is_invalid|bit_qr_ql_error|bit_comm_error;//??
			bit_led_over_i = bit_over_i;//??
			//
			bit_led_cssd = bit_setmode|bit_monitor_mode|bit_alarm_mode;//????,????,????
			bit_led_xl = bit_xieliao;//??
			bit_led_dw = bit_l_mtl;
			bit_led_gw = bit_h_mtl;
			bit_led_up = bit_current_1st;
			bit_led_down = bit_current_2nd;
			bit_led_dl = bit_duliao;
			//
			*/
			if(1==bit_xieliao)
			{
				m_vel=m_rPara.para.max_vel>>1;
			}
			else if(0==bit_fmotor)
			{
				m_vel=0;
			}
			else if(1==bit_auto)
			{
				m_vel=auto_vel;
			}
			else if(1==bit_manual)
			{
				m_vel=m_rPara.para.manual_vel;
			}
			else
			{
				m_vel=0;
			}	
			//
			uart_tx_buf[0] = 0xaa;

			if (xianshi_qiehua%2)
			{
			
				uart_tx_buf[1] = ADDR_XSB;

				if (bit_mmotor == 0)
				{
					uart_tx_buf[5] =0;
					uart_tx_buf[6] = 0;
				}
				else
				{
					uart_tx_buf[5] = m_cur_cal >> 8;
					uart_tx_buf[6] = m_cur_cal	& 0xff;

				}
			
			}
			else
			{

#if __D2_USED__  == TRUE
				uart_tx_buf[1] = ADDR_XSB_8;

				if (bit_mmotor == 0)
				{
					uart_tx_buf[5] =0;
					uart_tx_buf[6] = 0;
				}
				else
				{
					uart_tx_buf[5] = m_cur_cal2 >> 8;
					uart_tx_buf[6] = m_cur_cal2  & 0xff;

				}
#else
				uart_tx_buf[1] = ADDR_XSB;

				if (bit_mmotor == 0)
				{
					uart_tx_buf[5] =0;
					uart_tx_buf[6] = 0;
				}
				else
				{
					uart_tx_buf[5] = m_cur_cal >> 8;
					uart_tx_buf[6] = m_cur_cal	& 0xff;

				}
												
#endif
			}

			uart_tx_buf[2] =	bit_mmotor << 7 |		//??
								bit_auto << 6 | 		//??
								bit_manual << 5 |		//??
								bit_fmotor << 4 |		//???
								li_he_zha << 3 |		//??
								bit_remote<< 2 |		//??
								bit_duliao << 1 |		//??	  
								bit_lock << 0;			//??




			

	//m_vel = 100;
			uart_tx_buf[3] = m_vel >> 8;
			uart_tx_buf[4] = m_vel	& 0xff;

	//m_cur_cal = 88;


			m_material_value_temp = m_material_value>> 4;



			uart_tx_buf[7] = m_material_value_temp >> 8;
			uart_tx_buf[8] = m_material_value_temp &0xff;



			//??

			//??

			if (bit_auto ==1)
			{

				if (mb_is_invalid == 1 && auto_vel > 0)
				{
					uart_tx_buf[1] |= 1 << 6;
				}
			}

			//??

			if (bit_manual ==1)
			{

				if (mb_is_invalid == 1)
				{
					uart_tx_buf[1] |= 1 << 7;
				}
			}


		}
			}

		}
		xianshi_qiehua ++;



		uart_tx_buf[9] = 0;
		for(i=0; i<9; i++)
		{
			uart_tx_buf[9] += uart_tx_buf[i];//sum
		}
		
		b485en = 1;//????
		delay_us(100);
		UDR0 = uart_tx_buf[0];
		uart_tx_addr = 1;
	}
	/*else
	{

		if (temp1_flag_count > 50 )
		{



			temp1_flag_count  = 0;
			uart_tx_buf[0] = 0x55;
			uart_tx_buf[1] = 'A';
			uart_tx_buf[2] = 'A';
			uart_tx_buf[3] = 'A';
			uart_tx_buf[4] = 'A';

			send_uart1_to_computer(uart_tx_buf,5);

		}
	}*/

/*
	TRACE_STRING("usart  =");
	
	send_ulong_ascii(set_date ,TRUE);
	if (set_date < 16)
	{
	for(i=0; i<7; i++)
		{
	send_ulong_ascii(uart_tx_buf[i] ,TRUE);
		}
		}
*/

}


/////////////////////////////////////////////////////////////////
void material_2_vel(unsigned short m)
{
	UCHAR4 m_temp;
	m_temp.ml = m;
	if(m_temp.ms[0] > m_material_stop_modify)//12890
	{
		if(m_temp.ms[0] > 0x325a)
		{
			m_temp.ms[0] -= 0x325a;
		}
		else
		{
			m_temp.ms[0] = 1;
		}
		//料位传感器满量程输出参数的应用
		m_temp.ml *= 100;
		m_temp.ml /= m_rPara.para.material_full;
		//
		m_material = m_temp.ms[0];//用于判断在自动运行状态下是否需要改变运行动作
		//
		m_temp.ml *= m_rPara.para.max_vel-m_rPara.para.min_vel;
		auto_vel = m_temp.ml/51550;
		//
		if(auto_vel < 5)
		{
			bit_l_mtl = 1;//低位灯亮
			bit_h_mtl = 0;
			if(auto_vel < 1)
			{
				m_material = 0;//如果正在运行就应该停机(自动)
			}
		}
		else
		{
			bit_l_mtl = 0;
		}
		//
		auto_vel += m_rPara.para.min_vel;//
		//
		//判断是否需要亮高位灯
		//为高料位比较
		//比较值设定为4.6v
		//12890*3.6=46404
		if(m_material > 46404)
		{
			bit_h_mtl = 1;
			bit_l_mtl = 0;
		}
		else
		{
			bit_h_mtl = 0;
		}
	}
	else
	{
		m_material = 0;//如果正在运行就应该停机(自动)
		bit_l_mtl = 1;
		bit_h_mtl = 0;
	}
}
/////////////////////////////////////////////////////////////////
void read_para(unsigned char flag)
{
	//读非易失参数
	unsigned long m_temp;
	//
	if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.para_seted_flag))
	{ 
		delay_ms(50);
		if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.para_seted_flag))
		{
			//如果标志不正常，设置默认参数
			set_default_para();
			return;//正常情况下代码执行不到这里，显性的表示执行过程			
		}
	}
	//
	//版本号
	if(version_h!=eeprom_read_word_user((const uint16_t *)(&m_ePara[0].para.version_h)))
	{
		eeprom_write_word_user((uint16_t *)(&m_ePara[0].para.version_h),version_h);
	}
	if(version_l!=eeprom_read_word_user((const uint16_t *)(&m_ePara[0].para.version_l)))
	{
		eeprom_write_word_user((uint16_t *)(&m_ePara[0].para.version_l),version_l);
	}
	//
	//料位传感器满量程
	m_rPara.para.material_full=100;
	//上电远控&上电键盘控制
	m_rPara.para.is_remote=eeprom_read_byte_user(&m_ePara[0].para.is_remote);
	if(0 == flag)
	{
		//上电时
		if(POWER_UP_REMOTE == m_rPara.para.is_remote)
		{
			bit_remote = 1;
			modbus_status.status.m_yk = STATUS_ON;
		}
		else
		{
			m_rPara.para.is_remote = POWER_UP_KEYPAD;
			bit_remote = 0;
			modbus_status.status.m_yk = STATUS_OFF;
			eeprom_write_byte_user(&m_ePara[0].para.is_remote,m_rPara.para.is_remote);
		}
	}
	//主电机过载保护停机电流
	m_rPara.para.mm_cur_overflow=eeprom_read_byte_user(&m_ePara[0].para.mm_cur_overflow);
	if(m_rPara.para.mm_cur_overflow < 100)
	{
		m_rPara.para.mm_cur_overflow=100;
	}
	else if(m_rPara.para.mm_cur_overflow > 200)
	{
		m_rPara.para.mm_cur_overflow=200;
	}
	//连锁控制
	m_rPara.para.mm_control=eeprom_read_byte_user(&m_ePara[0].para.mm_control);
	if(0 == m_rPara.para.mm_control)
	{
		bit_mm_control=1;
	}
	else
	{
		m_rPara.para.mm_control=1;
		bit_mm_control=0;
	}
	//电流传感器量程
	m_rPara.para.cur1_full=eeprom_read_byte_user(&m_ePara[0].para.cur1_full);//50
	if(m_rPara.para.cur1_full > FULL_CURRENT)
	{
		m_rPara.para.cur1_full=FULL_CURRENT;
	}
	else if(m_rPara.para.cur1_full < 10)
	{
		m_rPara.para.cur1_full=10;
	}
	#if __D2_USED__ == TRUE
	m_rPara.para.cur2_full=eeprom_read_byte_user(&m_ePara[0].para.cur2_full);//50
	if(m_rPara.para.cur2_full > FULL_CURRENT)
	{
		m_rPara.para.cur2_full=FULL_CURRENT;
	}
	else if(m_rPara.para.cur2_full < 10)
	{
		m_rPara.para.cur2_full=10;
	}
	#else
	m_rPara.para.cur2_full=FULL_CURRENT;
	#endif
	//最高转速&最低转速
	m_rPara.para.max_vel=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.max_vel);
	if(m_rPara.para.max_vel > MAX_VEL)
	{
		m_rPara.para.max_vel=MAX_VEL;
	}
	else if(m_rPara.para.max_vel < MIN_VEL)
	{
		m_rPara.para.max_vel=MIN_VEL;
	}
	m_rPara.para.min_vel=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.min_vel);
	if(m_rPara.para.min_vel < MIN_VEL)
	{
		m_rPara.para.min_vel=MIN_VEL;
	}
	else if(m_rPara.para.min_vel > MAX_VEL)
	{
		m_rPara.para.min_vel=MAX_VEL;
	}
	//主电机额定电流
	m_rPara.para.mmotor_cur1=eeprom_read_byte_user(&m_ePara[0].para.mmotor_cur1);
	if(m_rPara.para.mmotor_cur1 > m_rPara.para.cur1_full)
	{
		m_rPara.para.mmotor_cur1=m_rPara.para.cur1_full;
	}
	else if(m_rPara.para.mmotor_cur1 < 10)
	{
		m_rPara.para.mmotor_cur1=10;
	}
	//测试用ID
	//m_rPara.para.all_stop_id=eeprom_read_byte_user(&m_ePara[0].para.all_stop_id);
	//
	#if __D2_USED__ == TRUE
	m_rPara.para.mmotor_cur2=eeprom_read_byte_user(&m_ePara[0].para.mmotor_cur2);
	if(m_rPara.para.mmotor_cur2 > m_rPara.para.cur2_full)
	{
		m_rPara.para.mmotor_cur2=m_rPara.para.cur2_full;
	}
	else if(m_rPara.para.mmotor_cur2 < 10)
	{
		m_rPara.para.mmotor_cur2=10;
	}
	#else
	m_rPara.para.mmotor_cur2=10;
	#endif
	//合轧延时时间
	m_rPara.para.rollin_delay=eeprom_read_byte_user(&m_ePara[0].para.rollin_delay);
	if(m_rPara.para.rollin_delay < 2)//100ms为单位
	{
		m_rPara.para.rollin_delay=2;
	}
	else if(m_rPara.para.rollin_delay > 200)
	{
		m_rPara.para.rollin_delay=200;
	}
	//手动转速
	m_rPara.para.manual_vel=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.manual_vel);
	if(m_rPara.para.manual_vel < m_rPara.para.min_vel)
	{
		m_rPara.para.manual_vel=m_rPara.para.min_vel;
	}
	else if(m_rPara.para.manual_vel > m_rPara.para.max_vel)
	{
		m_rPara.para.manual_vel=m_rPara.para.max_vel;
	}
	//时间限制参数
	#if LIMITEDTIME_ENABLE == 1
	{
		m_rPara.para.limited_hours=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.limited_hours);
		if(0==m_rPara.para.limited_hours)
		{
			bit_used_no_time=1;
		}
		else
		{
			limited_days = m_rPara.para.limited_hours/24;
			if(limited_days > LIMITED_DAYS_MAX)
			{
				limited_days = LIMITED_DAYS_MAX;
			}
			if(0==limited_days)
			{
				limited_days = 1;
			}
		}
	}
	#endif
	//料位数据读取端口设置
	m_rPara.para.material_port=eeprom_read_byte_user(&m_ePara[0].para.material_port);
	if((m_rPara.para.material_port>MATERIAL_PORT_ONLY_AD) || (m_rPara.para.material_port<MATERIAL_PORT_RS485_AD))
	{
		m_rPara.para.material_port=MATERIAL_PORT_RS485_AD;//默认用集成传感器
	}
	//启动料位值
	m_rPara.para.material_start=eeprom_read_byte_user(&m_ePara[0].para.material_start);
	if(m_rPara.para.material_start < 30)
	{
		m_rPara.para.material_start=30;
	}
	else if(m_rPara.para.material_start > 100)
	{
		m_rPara.para.material_start=100;
	}
	//12890*5/100 .=. 645
	m_material_start_modify = 645;
	m_material_start_modify *= m_rPara.para.material_start;
	//停止料位值
	m_rPara.para.material_stop=eeprom_read_byte_user(&m_ePara[0].para.material_stop);
	if(m_rPara.para.material_stop < 80)
	{
		m_rPara.para.material_stop = 80;
	}
	else if(m_rPara.para.material_stop > 150)
	{
		m_rPara.para.material_stop = 150;
	}
	//12890/100 .=. 129
	m_material_stop_modify = 129;
	m_material_stop_modify *= m_rPara.para.material_stop;
	//电流修正值
	m_rPara.para.current1_modify=eeprom_read_byte_user(&m_ePara[0].para.current1_modify);
	if(m_rPara.para.current1_modify < 70)
	{
		m_rPara.para.current1_modify=70;
	}
	else if(m_rPara.para.current1_modify > 130)
	{
		m_rPara.para.current1_modify=130;
	}
	#if __D2_USED__ == TRUE
	m_rPara.para.current2_modify=eeprom_read_byte_user(&m_ePara[0].para.current2_modify);
	if(m_rPara.para.current2_modify < 70)
	{
		m_rPara.para.current2_modify=70;
	}
	else if(m_rPara.para.current2_modify > 130)
	{
		m_rPara.para.current2_modify=130;
	}
	#else
	m_rPara.para.current2_modify=100;
	#endif
	//
	//MODBUS通信地址
#if MODBUS_TEST == FALSE
	m_rPara.para.addr_modbus=eeprom_read_byte_user(&m_ePara[0].para.addr_modbus);
	if(m_rPara.para.addr_modbus < 2)
	{
		m_rPara.para.addr_modbus = 2;
	}
#else
	m_rPara.para.addr_modbus = 2;//for test
#endif
	//MODBUS通信波特率
	//1:9600Bd
	//2:19200Bd
	//3:38400Bd
#if MODBUS_TEST == FALSE
	m_rPara.para.baud_modbus=eeprom_read_byte_user(&m_ePara[0].para.baud_modbus);
	if(m_rPara.para.baud_modbus < MODBUS_BAUD_9600)
	{
		m_rPara.para.baud_modbus = MODBUS_BAUD_9600;
	}
	else if(m_rPara.para.baud_modbus > MODBUS_BAUD_38400)
	{
		m_rPara.para.baud_modbus = MODBUS_BAUD_38400;
	}
#else
	m_rPara.para.baud_modbus = MODBUS_BAUD_38400;//for test
#endif
	//MB输入选择
	//1:电平	2:脉冲
	m_rPara.para.mb_level_pulse=eeprom_read_byte_user(&m_ePara[0].para.mb_level_pulse);
	if(MB_LEVEL != m_rPara.para.mb_level_pulse)
	{
		m_rPara.para.mb_level_pulse = MB_PULSE;
	}
	//DW输入选择
	//1:电平	2:脉冲
	m_rPara.para.dw_level_pulse=eeprom_read_byte_user(&m_ePara[0].para.dw_level_pulse);
	if(DW_LEVEL != m_rPara.para.dw_level_pulse)
	{
		m_rPara.para.dw_level_pulse = DW_PULSE;
	}
	//MB和转速之间的关系
	m_rPara.para.mb_div=eeprom_read_byte_user(&m_ePara[0].para.mb_div);
	if(m_rPara.para.mb_div < 1)
	{
		m_rPara.para.mb_div=1;
	}
	else if(m_rPara.para.mb_div > 99)
	{
		m_rPara.para.mb_div=99;
	}
	//设备使用时间（小时）
	m_rPara.para.used_hours = eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.used_hours);
	//
	////////////////////////////////
	//这段代码最好放在该函数的最后，因为它要用到一些函数中读出的参数，
	//为了避免某些参数没有读出而造成非预期的效果，放该函数最后，确保
	//参数都已经读出（正确初始化了）。
	//为了后面的比较做准备
	m_temp = m_rPara.para.mmotor_cur1;//35A
	m_temp *= m_rPara.para.mm_cur_overflow;//140%
	m_temp += 5;//四舍五入
	mm_cur_overflow_stop = m_temp/10;//主电机电流溢出停机阈值//490
	if(mm_cur_overflow_stop >= m_rPara.para.cur1_full*10)
	{
		//电流过载上限值大于电流传感器最大量程了，限制在最大量程内
		mm_cur_overflow_stop = m_rPara.para.cur1_full*10 - 5;
	}
	//
	mmotor_cur1_cmp = m_rPara.para.mmotor_cur1*10;//主电机溢出阈值//350
	//
	#if __D2_USED__ == TRUE
	{
		m_temp = m_rPara.para.mmotor_cur2;//35A
		m_temp *= m_rPara.para.mm_cur_overflow;//140%
		m_temp += 5;//四舍五入
		mm_cur_overflow_stop = m_temp/10;//主电机电流溢出停机阈值//490
		if(mm_cur_overflow_stop >= m_rPara.para.cur2_full*10)
		{
			//电流过载上限值大于电流传感器最大量程了，限制在最大量程内
			mm_cur_overflow_stop = m_rPara.para.cur2_full*10 - 5;
		}
		//
		mmotor_cur2_cmp = m_rPara.para.mmotor_cur2*10;//主电机溢出阈值//350
	}
	#endif

    if(0 == flag)
    {
        m_rPara.para.zuo_qi=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.zuo_qi);
        zuo_gao = m_rPara.para.zuo_qi;

        m_rPara.para.zuo_ling=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.zuo_ling);
        zuo_di = m_rPara.para.zuo_ling;

        if (m_rPara.para.zuo_ling >m_rPara.para.zuo_ling)
        {
            m_rPara.para.zuo_ling = 300;
            m_rPara.para.zuo_qi = 800;
            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.zuo_ling,m_rPara.para.zuo_ling);
            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.zuo_qi,m_rPara.para.zuo_qi);


        }

        m_rPara.para.you_qi=eeprom_read_word_user((const uint16_t  *)&m_ePara[0].para.you_qi);
        you_gao = m_rPara.para.you_qi;
        m_rPara.para.you_ling=eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.you_ling);
        you_di = m_rPara.para.you_ling;


        if (m_rPara.para.you_ling >m_rPara.para.you_ling)
        {
            m_rPara.para.you_ling = 300;
            m_rPara.para.you_qi = 800;
            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.you_ling,m_rPara.para.you_ling);
            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.you_qi,m_rPara.para.you_qi);
        }
		
		m_rPara.para.set_yuzhi=eeprom_read_byte_user(&m_ePara[0].para.set_yuzhi);

        TRACE_STRING("read eeprom");
        send_ulong_ascii(m_rPara.para.zuo_ling ,TRUE);
        send_ulong_ascii(m_rPara.para.zuo_qi ,TRUE);
        send_ulong_ascii(m_rPara.para.you_ling ,TRUE);
        send_ulong_ascii(m_rPara.para.you_qi ,TRUE);
    }



	//静态转速
	m_rPara.para.set_default_zhuansu=eeprom_read_byte_user(&m_ePara[0].para.set_default_zhuansu);
	if(m_rPara.para.set_default_zhuansu < m_rPara.para.min_vel
		|| m_rPara.para.set_default_zhuansu > m_rPara.para.max_vel)
	{
		m_rPara.para.set_default_zhuansu = m_rPara.para.min_vel >>2 + m_rPara.para.max_vel >> 2 ;
	}

    //设置模式
    m_rPara.para.set_model=eeprom_read_byte_user(&m_ePara[0].para.set_model);
	if(m_rPara.para.set_model >1 )
	{
		m_rPara.para.set_model = 0 ;
	}

	//设置比例系数
    m_rPara.para.set_wuliao_PID_P=eeprom_read_byte_user(&m_ePara[0].para.set_wuliao_PID_P);
	if(m_rPara.para.set_wuliao_PID_P ==0   )
	{
		m_rPara.para.set_wuliao_PID_P = 1 ;
	}

	if(m_rPara.para.set_wuliao_PID_P ==255   )
	{
		m_rPara.para.set_wuliao_PID_P = 1 ;
	}

	
	//设置期望值
    m_rPara.para.set_qiwang_value=eeprom_read_byte_user(&m_ePara[0].para.set_qiwang_value);
	if(m_rPara.para.set_qiwang_value < 5 
		||  m_rPara.para.set_qiwang_value >19   )
	{
		m_rPara.para.set_qiwang_value = 12 ;
	}


	
	/////////////////////////////////
}
////////////////////////////////////////////////////////////////
uint8_t eeprom_read_byte_2(const uint8_t *p)
{
	unsigned char tmp;
	unsigned char m_sreg=SREG;
	cli();
	eeprom_busy_wait();
	tmp = eeprom_read_byte((uint8_t *)p);
	eeprom_busy_wait();
	SREG = m_sreg;
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_byte_2(const uint8_t *p,unsigned char val)
{
	unsigned char m_sreg=SREG;
	cli();
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)p,val);
	eeprom_busy_wait();
	SREG = m_sreg;
}
////////////////////////////////////////////////////////////////
unsigned short eeprom_read_word_2(const uint16_t *p)
{
	unsigned short tmp;
	//
	unsigned char m_sreg=SREG;
	cli();
	eeprom_busy_wait();
	tmp = eeprom_read_word((uint16_t *)p);
	eeprom_busy_wait();
	SREG = m_sreg;
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_word_2(const uint16_t *p,unsigned short val)
{
	unsigned char m_sreg=SREG;
	cli();
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)p,val);
	eeprom_busy_wait();
	SREG = m_sreg;
}
////////////////////////////////////////////////////////////////
unsigned char eeprom_read_byte_user(const uint8_t *p1)
{
	unsigned char tmp,tmp1,tmp2,tmp3,tmp4,tmp5;
	//
	tmp = 0xff;
	//
	const uint8_t *p2 = (const uint8_t *)(p1+EEPROM_DATA_BYTE_SIZE);
	const uint8_t *p3 = (const uint8_t *)(p1+2*EEPROM_DATA_BYTE_SIZE);
	const uint8_t *p4 = (const uint8_t *)(p1+3*EEPROM_DATA_BYTE_SIZE);
	const uint8_t *p5 = (const uint8_t *)(p1+4*EEPROM_DATA_BYTE_SIZE);
	//
	tmp1 = eeprom_read_byte_2(p1);
	tmp2 = eeprom_read_byte_2(p2);
	tmp3 = eeprom_read_byte_2(p3);
	tmp4 = eeprom_read_byte_2(p4);
	tmp5 = eeprom_read_byte_2(p5);
	//
	if((tmp1==tmp2) && (tmp1==tmp3)) //123
	{
		tmp = tmp1;
		if(tmp != tmp4)
		{
			eeprom_write_byte_2(p4,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_byte_2(p5,tmp);
		}
	}
	else if((tmp1==tmp2) && (tmp1==tmp4)) //124
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_byte_2(p3,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_byte_2(p5,tmp);
		}
	}
	else if((tmp1==tmp2) && (tmp1==tmp5)) //125
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_byte_2(p3,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_byte_2(p4,tmp);
		}
	}
	else if((tmp1==tmp3) && (tmp1==tmp4)) //134
	{
		tmp = tmp1;
		if(tmp != tmp2)
		{
			eeprom_write_byte_2(p2,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_byte_2(p5,tmp);
		}
	}
	else if((tmp1==tmp3) && (tmp1==tmp5)) //135
	{
		tmp = tmp1;
		if(tmp != tmp2)
		{
			eeprom_write_byte_2(p2,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_byte_2(p4,tmp);
		}
	}
	else if((tmp1==tmp4) && (tmp1==tmp5)) //145
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_byte_2(p3,tmp);
		}
		//
		if(tmp != tmp2)
		{
			eeprom_write_byte_2(p2,tmp);
		}
	}
	else if((tmp2==tmp3) && (tmp2==tmp4)) //234
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_byte_2((uint8_t *)(p1),tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_byte_2((uint8_t *)(p5),tmp);
		}
	}
	else if((tmp2==tmp3) && (tmp2==tmp5)) //235
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_byte_2((uint8_t *)(p1),tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_byte_2((uint8_t *)(p4),tmp);
		}
	}
	else if((tmp2==tmp4) && (tmp2==tmp5)) //245
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_byte_2((uint8_t *)(p1),tmp);
		}
		//
		if(tmp != tmp3)
		{
			eeprom_write_byte_2((uint8_t *)(p3),tmp);
		}
	}
	else if((tmp3==tmp4) && (tmp3==tmp5)) //345
	{
		tmp = tmp3;
		if(tmp != tmp1)
		{
			eeprom_write_byte_2((uint8_t *)(p1),tmp);
		}
		//
		if(tmp != tmp2)
		{
			eeprom_write_byte_2((uint8_t *)(p2),tmp);
		}
	}
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_byte_user(uint8_t *p,unsigned char val)
{
	eeprom_write_byte_2((uint8_t *)p,val);
	eeprom_write_byte_2((uint8_t *)(p+EEPROM_DATA_BYTE_SIZE),val);
	eeprom_write_byte_2((uint8_t *)(p+2*EEPROM_DATA_BYTE_SIZE),val);
	eeprom_write_byte_2((uint8_t *)(p+3*EEPROM_DATA_BYTE_SIZE),val);
	eeprom_write_byte_2((uint8_t *)(p+4*EEPROM_DATA_BYTE_SIZE),val);
}
////////////////////////////////////////////////////////////////
unsigned short eeprom_read_word_user(const uint16_t *p1)
{
	unsigned short tmp,tmp1,tmp2,tmp3,tmp4,tmp5;
	//
	tmp = 0xff;
	//
	const uint16_t *p2 = (const uint16_t *)(p1+EEPROM_DATA_WORD_SIZE);
	const uint16_t *p3 = (const uint16_t *)(p1+2*EEPROM_DATA_WORD_SIZE);
	const uint16_t *p4 = (const uint16_t *)(p1+3*EEPROM_DATA_WORD_SIZE);
	const uint16_t *p5 = (const uint16_t *)(p1+4*EEPROM_DATA_WORD_SIZE);
	//
	tmp1 = eeprom_read_word_2(p1);
	tmp2 = eeprom_read_word_2(p2);
	tmp3 = eeprom_read_word_2(p3);
	tmp4 = eeprom_read_word_2(p4);
	tmp5 = eeprom_read_word_2(p5);
	//
	if((tmp1==tmp2) && (tmp1==tmp3)) //123
	{
		tmp = tmp1;
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp1==tmp2) && (tmp1==tmp4)) //124
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp1==tmp2) && (tmp1==tmp5)) //125
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
	}
	else if((tmp1==tmp3) && (tmp1==tmp4)) //134
	{
		tmp = tmp1;
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp1==tmp3) && (tmp1==tmp5)) //135
	{
		tmp = tmp1;
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
	}
	else if((tmp1==tmp4) && (tmp1==tmp5)) //145
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
		//
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
	}
	else if((tmp2==tmp3) && (tmp2==tmp4)) //234
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp2==tmp3) && (tmp2==tmp5)) //235
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
	}
	else if((tmp2==tmp4) && (tmp2==tmp5)) //245
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
	}
	else if((tmp3==tmp4) && (tmp3==tmp5)) //345
	{
		tmp = tmp3;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
	}
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_word_user(uint16_t *p,unsigned short val)
{
	eeprom_write_word_2((uint16_t *)p,val);
	eeprom_write_word_2((uint16_t *)(p+EEPROM_DATA_WORD_SIZE),val);
	eeprom_write_word_2((uint16_t *)(p+2*EEPROM_DATA_WORD_SIZE),val);
	eeprom_write_word_2((uint16_t *)(p+3*EEPROM_DATA_WORD_SIZE),val);
	eeprom_write_word_2((uint16_t *)(p+4*EEPROM_DATA_WORD_SIZE),val);
}
////////////////////////////////////////////////////////////////
void set_default_para(void)
{
	//执行恢复出厂默认参数操作
	//
	NVM_DATA tmp;
	//
	set_default = 0;
	//
	//外部参数
	//最高和最低转速(40-300转/分)
	tmp.para.max_vel=260;
	tmp.para.min_vel=100;
	//主电机额定电流(10-100A)
	tmp.para.mmotor_cur1=35;
	#if __D2_USED__ == TRUE
	tmp.para.mmotor_cur2=35;
	#endif
	//合轧延时时间(0.2-20s,2-200百毫秒)
	tmp.para.rollin_delay=20;
	//内部参数
	//料位传感器满量程输出
	tmp.para.material_full=100;
	//上电远控和上电键盘控制选择
	tmp.para.is_remote=POWER_UP_REMOTE;
	//主电机过载保护停机电流
	tmp.para.mm_cur_overflow=140;
	//是否根据主电机电流进行连锁控制
	tmp.para.mm_control=1;
	//电流传感器量程
	tmp.para.cur1_full=50;
	#if __D2_USED__ == TRUE
	tmp.para.cur2_full=50;
	#endif
	//料位传感器值采集端设定,默认为AD采样数据,设置为2时从485口接收数据，设置为3时用集成传感器
	tmp.para.material_port=MATERIAL_PORT_RS485_AD;//默认采样集成传感器
	//
	//手动参数
	//手动转数
	tmp.para.manual_vel=120;
	////////////////////////////////使用时间限制
	//使用天数
	tmp.para.limited_hours=(unsigned short)LIMITED_DAYS_MAX*24;
	//测试用
	tmp.para.all_stop_id=0;
	//
	//密码保护参数
	//启动料位值
	tmp.para.material_start=50;//40;
	//停机料位值
	tmp.para.material_stop=100;
	//电流修正值
	tmp.para.current1_modify=100;
	#if __D2_USED__ == TRUE
	tmp.para.current2_modify=100;
	#endif
	//
	//参数设置标志
	tmp.para.para_seted_flag=PARA_SETED_FLAG;
	//
	//MODBUS通信从机地址和通信波特率
	tmp.para.addr_modbus=2;//20,27
	tmp.para.baud_modbus=MODBUS_BAUD_38400;//21,28//1:9600Bd,2:19200Bd,3:38400Bd
	//MB和DW输入选择（1：电平，2：脉冲）
	tmp.para.mb_level_pulse=MB_PULSE;//MB_LEVEL;//22//29
	tmp.para.dw_level_pulse=DW_LEVEL;//23//30
	//MB和转速之间的关系
	tmp.para.mb_div=1;//24//31
	//设备使用时间
	tmp.para.used_hours=0;//26//34-32

	tmp.para.set_yuzhi=5;
	//
	tmp.para.w5500_net_para.ip_para.ip_addr[0] = W5500_CONF_IPADDR0;
	tmp.para.w5500_net_para.ip_para.ip_addr[1] = W5500_CONF_IPADDR1;
	tmp.para.w5500_net_para.ip_para.ip_addr[2] = W5500_CONF_IPADDR2;
	tmp.para.w5500_net_para.ip_para.ip_addr[3] = W5500_CONF_IPADDR3;
	//
	tmp.para.w5500_net_para.ip_para.gateway[0] = W5500_CONF_GATEWAY_ADDR0;
	tmp.para.w5500_net_para.ip_para.gateway[1] = W5500_CONF_GATEWAY_ADDR1;
	tmp.para.w5500_net_para.ip_para.gateway[2] = W5500_CONF_GATEWAY_ADDR2;
	tmp.para.w5500_net_para.ip_para.gateway[3] = W5500_CONF_GATEWAY_ADDR3;
	//
	tmp.para.w5500_net_para.ip_para.net_mask[0] = W5500_CONF_NET_MASK0;
	tmp.para.w5500_net_para.ip_para.net_mask[1] = W5500_CONF_NET_MASK1;
	tmp.para.w5500_net_para.ip_para.net_mask[2] = W5500_CONF_NET_MASK2;
	tmp.para.w5500_net_para.ip_para.net_mask[3] = W5500_CONF_NET_MASK3;

	/****
	
	1  增加转速初始值(最低和最高转速的初始值)
	2  增加切换开关(默认值原来你的)
	3  增比例参数设置(最高转速-最低转速/12)
	4  增加期望值设置(12MMA)

	
	unsigned char set_default_zhuansu;
	unsigned char set_model;
	unsigned char set_wuliao_PID_P;
	unsigned char set_qiwang_value;
	**/

	tmp.para.set_default_zhuansu = tmp.para.max_vel>>2 + tmp.para.min_vel>>2;
	tmp.para.set_model = 0;
	tmp.para.set_wuliao_PID_P = 1;
	tmp.para.set_qiwang_value = 12;
	
	
    




	write_para_to_eeprom(&tmp,TRUE);








	
	//
	delay_ms(100);
	wdt_reset_mcu();//重启单片机
}
/////////////////////////////////////////////////////////////////
void write_para_to_eeprom(NVM_DATA *p,unsigned char rst_used_hours)
{		
	wdt_reset(); //防止写EEPROM过程中看门狗复位
	//
	//外部参数
	//最高和最低转速(40-300转/分)
	eeprom_write_word_user((uint16_t *)&m_ePara[0].para.max_vel,p->para.max_vel);
	eeprom_write_word_user((uint16_t *)&m_ePara[0].para.min_vel,p->para.min_vel);
	//主电机额定电流(10-100A)
	eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur1,p->para.mmotor_cur1);
	#if __D2_USED__ == TRUE
	eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur2,p->para.mmotor_cur2);
	#endif
	//合轧延时时间(0.2-20s,2-200百毫秒)
	eeprom_write_byte_user(&m_ePara[0].para.rollin_delay,p->para.rollin_delay);
	//内部参数
	//料位传感器满量程输出
	eeprom_write_byte_user(&m_ePara[0].para.material_full,p->para.material_full);
	//上电远控和上电键盘控制选择
	eeprom_write_byte_user(&m_ePara[0].para.is_remote,p->para.is_remote);
	//主电机过载保护停机电流
	eeprom_write_byte_user(&m_ePara[0].para.mm_cur_overflow,p->para.mm_cur_overflow);
	//是否根据主电机电流进行连锁控制
	eeprom_write_byte_user(&m_ePara[0].para.mm_control,p->para.mm_control);
	//电流传感器量程
	eeprom_write_byte_user(&m_ePara[0].para.cur1_full,p->para.cur1_full);
	#if __D2_USED__ == TRUE
	eeprom_write_byte_user(&m_ePara[0].para.cur2_full,p->para.cur2_full);
	#endif
	//料位传感器值采集端设定,默认为AD采样数据,设置为2时从485口接收数据，设置为3时用集成传感器
	eeprom_write_byte_user(&m_ePara[0].para.material_port,p->para.material_port);//默认采样集成传感器
	//
	//手动参数
	//手动转数
	eeprom_write_word_user((uint16_t *)&m_ePara[0].para.manual_vel,p->para.manual_vel);
	////////////////////////////////使用时间限制
	if(FALSE != rst_used_hours)
	{
		//使用天数
		eeprom_write_word_user((uint16_t *)&m_ePara[0].para.limited_hours,p->para.limited_hours);
	}
	//测试用
	eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, p->para.all_stop_id);
	//
	//密码保护参数
	//启动料位值
	eeprom_write_byte_user(&m_ePara[0].para.material_start,p->para.material_start);
	//停机料位值
	eeprom_write_byte_user(&m_ePara[0].para.material_stop,p->para.material_stop);
	//电流修正值
	eeprom_write_byte_user(&m_ePara[0].para.current1_modify,p->para.current1_modify);
	#if __D2_USED__ == TRUE
	eeprom_write_byte_user(&m_ePara[0].para.current2_modify,p->para.current2_modify);
	#endif
	//
	//参数设置标志
	eeprom_write_word_user((uint16_t *)&m_ePara[0].para.para_seted_flag,PARA_SETED_FLAG);
	//
	//MODBUS通信从机地址和通信波特率
	eeprom_write_byte_user(&m_ePara[0].para.addr_modbus,p->para.addr_modbus);//20,27
	eeprom_write_byte_user(&m_ePara[0].para.baud_modbus,p->para.baud_modbus);//21,28//1:9600Bd,2:19200Bd,3:38400Bd
	//MB和DW输入选择（1：电平，2：脉冲）
	eeprom_write_byte_user(&m_ePara[0].para.mb_level_pulse,p->para.mb_level_pulse);//22//29
	eeprom_write_byte_user(&m_ePara[0].para.dw_level_pulse,p->para.dw_level_pulse);//23//30
	//MB和转速之间的关系
	eeprom_write_byte_user(&m_ePara[0].para.mb_div,p->para.mb_div);//24//31

	eeprom_write_byte_user(&m_ePara[0].para.set_yuzhi,p->para.set_yuzhi);//23//30
	if(FALSE != rst_used_hours)
	{
		//设备使用时间
		eeprom_write_word_user((uint16_t *)&m_ePara[0].para.used_hours,0);//26//34-32
	}
	//
	//IP和MAC
	unsigned char i;
	for(i=0; i<4; i++)
	{
		//IP地址写入EEPROM
		eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.ip_addr[i],p->para.w5500_net_para.ip_para.ip_addr[i]);
		eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.gateway[i],p->para.w5500_net_para.ip_para.gateway[i]);
		eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.net_mask[i],p->para.w5500_net_para.ip_para.net_mask[i]);
	}
	//
	#if USED_W5500 == TRUE
	{
		extern void create_mac(const IP_PARA *pIP);
		//建立MAC地址并且写入EEPROM
		create_mac(&p->para.w5500_net_para.ip_para);
	}
	#endif


    /*
	tmp.para.set_default_zhuansu = tmp.para.max_vel>>2 + tmp.para.min_vel>>2;
	tmp.para.set_model = 0;
	tmp.para.set_wuliao_PID_P = 1;
	tmp.para.set_qiwang_value = 12;
	*/
	eeprom_write_byte_user(&m_ePara[0].para.set_default_zhuansu,p->para.set_default_zhuansu);//23//30
	eeprom_write_byte_user(&m_ePara[0].para.set_model,p->para.set_model);//23//30
	eeprom_write_byte_user(&m_ePara[0].para.set_wuliao_PID_P,p->para.set_wuliao_PID_P);//23//30
	eeprom_write_byte_user(&m_ePara[0].para.set_qiwang_value,p->para.set_qiwang_value);//23//30

	
}
/////////////////////////////////////////////////////////////////
//根据页号保存参数
void save_para()
{
	if(1==bit_setmode_2)
	{
		//内部参数
		switch(pagenumber)
		{
			case 0://料位传感器满量程输出(50%-100%)
			{
				eeprom_write_byte_user(&m_ePara[0].para.material_full,m_rPara.para.material_full);
			}
			break;
			case 1://上电远控与上电键盘控制选择. 1:上电远控  2:上电键盘控制
			{
				eeprom_write_byte_user(&m_ePara[0].para.is_remote,m_rPara.para.is_remote);
			}
			break;
			case 2://主电机过载保护停机电流 (额定电流的100%-200%)
			{
				eeprom_write_byte_user(&m_ePara[0].para.mm_cur_overflow,m_rPara.para.mm_cur_overflow);
			}
			break;
			case 3://是否根据主电机电流进行连锁控制   0:连锁控制  1:不连锁控制
			{
				eeprom_write_byte_user(&m_ePara[0].para.mm_control,m_rPara.para.mm_control);
			}
			break;
			case 4://电流传感器量程(10-100A),主电机电流
			{
				eeprom_write_byte_user(&m_ePara[0].para.cur1_full,m_rPara.para.cur1_full);
			}
			break;
			case 6://设置料位数据读取端口
			{
				eeprom_write_byte_user(&m_ePara[0].para.material_port,m_rPara.para.material_port);
			}
			break;
			case 7://MB与转速之间的关系
			{
				eeprom_write_byte_user(&m_ePara[0].para.mb_div,m_rPara.para.mb_div);
			}
			break;
			#if __D2_USED__ == TRUE
			case 8://电流传感器量程(10-100A),主电机电流
			{
				eeprom_write_byte_user(&m_ePara[0].para.cur2_full,m_rPara.para.cur2_full);
			}
			break;
			#endif
		}
	}
	else if(1 == bit_setmode_delay)
	{
		switch(pagenumber)
		{
			case 0:
			{
				//限时参数
				m_rPara.para.limited_hours=limited_days*(unsigned short)24;
				eeprom_write_word_user((uint16_t *)&m_ePara[0].para.limited_hours,m_rPara.para.limited_hours);//存入的是小时
				if(m_rPara.para.limited_hours > 0)
				{
					bit_used_no_time=0;
				}
			}
			break;
		}
	}
	else if(1==bit_setmode_3)
	{
		//密码保护的参数
		switch(pagenumber)
		{
			case 0://启动料位值
			{
				eeprom_write_byte_user(&m_ePara[0].para.material_start,m_rPara.para.material_start);
			}
			break;
			case 1://停机料位值
			{
				eeprom_write_byte_user(&m_ePara[0].para.material_stop,m_rPara.para.material_stop);
			}
			break;
			case 2://电流修正值
			{
				eeprom_write_byte_user(&m_ePara[0].para.current1_modify,m_rPara.para.current1_modify);
			}
			break;
			case 3://MODBUS通信地址
			{
				eeprom_write_byte_user(&m_ePara[0].para.addr_modbus,m_rPara.para.addr_modbus);
			}
			break;
			case 4://MODBUS通信波特率
			{
				eeprom_write_byte_user(&m_ePara[0].para.baud_modbus,m_rPara.para.baud_modbus);
				//
				//将新设置的波特率应用到系统
				set_modbus_baud(m_rPara.para.baud_modbus);
			}
			break;
			case 5:
			{
				//MB输入选择
				eeprom_write_byte_user(&m_ePara[0].para.mb_level_pulse,m_rPara.para.mb_level_pulse);
				//
				//将修改应用到系统
				//PCICR &= ~_BV(PCIE2);//PCINT23禁用，检测MB脉冲
				//PCMSK2 &= ~_BV(PCINT23);
				//KillTimer(MT_MB_COUNTER);
				delay_ms(2);
				if(MB_PULSE == m_rPara.para.mb_level_pulse)
				{
					//PCICR |= _BV(PCIE2);//PCINT23使能，检测MB脉冲
					//PCMSK2 |= _BV(PCINT23);
					//检测MB脉冲
					SetTimer(MT_MB_COUNTER,MB_DETECTION_INTERVAL*1000,-1);
				}
			}
			break;
			case 6:
			{
				//DW输入选择
				eeprom_write_byte_user(&m_ePara[0].para.dw_level_pulse,m_rPara.para.dw_level_pulse);
				//
				//将修改应用到系统
				PCICR &= ~_BV(PCIE3);//PCINT31禁用，检测DW电平
				PCMSK2 &= ~_BV(PCINT31);
				KillTimer(MT_DW_COUNTER);
				delay_ms(2);
				if(DW_PULSE == m_rPara.para.dw_level_pulse)
				{
					PCICR |= _BV(PCIE3);//PCINT31使能，检测DW脉冲
					PCMSK2 |= _BV(PCINT31);
					//检测DW脉冲
					SetTimer(MT_DW_COUNTER,DW_DETECTION_INTERVAL*1000,-1);
				}
			}
			break;
			#if __D2_USED__ == TRUE 
			case 7://电流修正值
			{
				eeprom_write_byte_user(&m_ePara[0].para.current2_modify,m_rPara.para.current2_modify);
			}
			break;
			#endif
		}
	}
	else
	{
		//外部参数
		switch(pagenumber)
		{
			case 0://最高转速(40-300)
			{
				eeprom_write_word_user((uint16_t *)&m_ePara[0].para.max_vel,m_rPara.para.max_vel);
			}
			break;
			case 1://最低转速(40-300)
			{
				eeprom_write_word_user((uint16_t *)&m_ePara[0].para.min_vel,m_rPara.para.min_vel);
			}
			break;
			case 2://主电机额定电流(10-100A)
			{
				eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur1,m_rPara.para.mmotor_cur1);
			}
			break;
			case 3://合轧延时时间(0.2-20s)
			{
				eeprom_write_byte_user(&m_ePara[0].para.rollin_delay,m_rPara.para.rollin_delay);
			}
			break;
			case 4://总停机日志数据
				m_rPara.para.all_stop_id = 0;
				//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);
			break;
			case 5://恢复默认设置
			{
				if(8 == set_default)
				{
					set_default_para();
				}
			}
			break;
			#if __D2_USED__ == TRUE
			case 6://主电机额定电流(10-100A)
			{
				eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur2,m_rPara.para.mmotor_cur2);
			}
			break;
			#endif
		}
	}
}
/////////////////////////////////////////////////////////////////
void runed_time(void)
{
	if(m_used_1s > (unsigned short)3600)//1小时
	{
		m_used_1s=0;
		//
		#if LIMITEDTIME_ENABLE == 1
		{
			if(m_rPara.para.limited_hours < 2)
			{
				bit_used_no_time = 1;
			}
			else
			{
				m_rPara.para.limited_hours--;
				eeprom_write_word_user((uint16_t *)&m_ePara[0].para.limited_hours,m_rPara.para.limited_hours);
			}
		}
		#endif
		//
		//记录运行小时
		m_rPara.para.used_hours = eeprom_read_word_user((const uint16_t *)&m_ePara[0].para.used_hours);
		m_rPara.para.used_hours++;
		eeprom_write_word_user((uint16_t *)&m_ePara[0].para.used_hours,m_rPara.para.used_hours);
	}
	//
	#if LIMITEDTIME_ENABLE == 1
	{
		if(1 == bit_used_no_time)
		{
			all_stop();//限时时间到
			m_rPara.para.all_stop_id = 3;
			//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);
		}
	}
	#endif
}
/////////////////////////////////////////////////////////////////
void run_in_remote_ctrl(void)
{
	//10毫秒调用一次
	static unsigned long the_last_time_of_byq_high=0;
	static unsigned long the_last_time_of_byq_low=0;
	static unsigned long the_last_time_of_byz_byq_high=0;
	static unsigned long the_last_time_of_byz_byq_low=0;
	//
	if(1 == bit_remote)//远控模式
	{
		//扫描YQ,YZ,MB
		//远控主电机启动
		if(0 == byq)
		{
			the_last_time_of_byq_low = boot_time;
			//
			if((boot_time-the_last_time_of_byq_high) > 200)  //2秒   
			{
				if(0 == bit_mmotor)
				{
					//启动喂料辊
					roll_out();
					mmotor_start();
				}
			}
		}
		else
		{
			//byq=1
			the_last_time_of_byq_high = boot_time;
			//
			if((boot_time-the_last_time_of_byq_low) > 500)
			{
				//if((1==bit_mmotor) || (1==bit_fmotor))
				if((1==bit_mmotor))
				{
					m_rPara.para.all_stop_id = 1;
					//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);
				}
				//
				all_stop();//远控模式下
				//
				KillTimer(MT_AUTO_RUN_DELAY);
				reset_cause = RST_CAUSE_OTHER;
			}
		}
		//
		//远控自动运行
		if((0==byz) && (0==byq))
		{
			the_last_time_of_byz_byq_low = boot_time;
			//
			if((boot_time-the_last_time_of_byz_byq_high) > 20)
			{
				if(0 == bit_auto)
				{
					if(RST_CAUSE_WDT == reset_cause)
					{
						//看门狗复位
						key_auto_run();
					}
					else
					{
						//其它
						if(FALSE == timer_is_running(MT_AUTO_RUN_DELAY))
						{
							SetTimer(MT_AUTO_RUN_DELAY, 8*1000, 0);//远程状态下 YQ YZ信号同时输入等待8秒执行自动
						}					
					}
				}
			}
		}
		else if(1 == byz)
		{
			//byz和byq不全为低电平
			the_last_time_of_byz_byq_high = boot_time;
			//
			if((boot_time-the_last_time_of_byz_byq_low) > 200)
			{
				if(1 == bit_fmotor)
				{
					KillTimer(MT_AUTO_RUN_DELAY);
					reset_cause = RST_CAUSE_OTHER;
					part_stop();
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////
void wdt_reset_mcu(void)
{
	//通过看门狗来复位单片机
	is_wdt_rst = 0;
	wdt_reset(); //执行看门狗复位单片机的操作
	wdt_disable();
	delay_ms(50);
	cli();//看门狗复位单片机
	wdt_enable(WDTO_120MS);
	for(;;);//等待单片机重启
}
////////////////////////////////////////////////////////////////
unsigned short get_mb_counter_60s(void)
{
	int i;
	unsigned short tmp=0;
	for(i=0;i<MB_60S_SAMPLE_SIZE;i++)
	{
		tmp += mb_60s_sample_buf[i];
	}
	return tmp;
}
////////////////////////////////////////////////////////////////
unsigned char get_mb_counter_3s(void)
{
	int i;
	unsigned char tmp=0;
	for(i=0;i<MB_3S_SAMPLE_SIZE;i++)
	{
		tmp += mb_3s_sample_buf[i];
	}
	return tmp;
}
////////////////////////////////////////////////////////////////
unsigned short get_dw_counter_60s(void)
{
	int i;
	unsigned short tmp=0;
	for(i=0;i<DW_60S_SAMPLE_SIZE;i++)
	{
		tmp += dw_60s_sample_buf[i];
	}
	return tmp;
}
////////////////////////////////////////////////////////////////
unsigned char get_dw_counter_3s(void)
{
	int i;
	unsigned char tmp=0;
	for(i=0;i<DW_3S_SAMPLE_SIZE;i++)
	{
		tmp += dw_3s_sample_buf[i];
	}
	return tmp;
}
////////////////////////////////////////////////////////////////
void qr_ql_signal_handling(void)
{
	if((1==bqr) || (1==bql))
	{
		//左右合轧信号不全，报警
		bit_qr_ql_error = 1;
		modbus_status.status.m_qr_ql_error = STATUS_ON;
		bcomerr = 1;
	}
	else
	{
		//合轧信号正确，清除报警
		bit_qr_ql_error = 0;
		modbus_status.status.m_qr_ql_error = STATUS_OFF;
		if((0==bit_over_i) && (0==bit_comm_error))
		{
			//没有通信错误也没有电流过载错误
			bcomerr = 0;
		}
		else
		{
			bcomerr = 1;
		}
	}
}
////////////////////////////////////////////////////////////
void lw_signal_handling(void)
{
	static unsigned long the_last_time_of_blw_high=0;
	//
	if(0 == blw)
	{
		//连续检测LW信号1秒，一直为低的话，确认LW信号有效
		if((boot_time-the_last_time_of_blw_high) > 500)
		{
			part_stop();
			//
			bit_duliao = 1;
			//
			ddr_mbbj = 1;  //输出报警BY端子
			//bmbbj = 1;
			//记录总停机操作日志（哪个位置执行的总停机操作）
			//eeprom_write_byte_user(&m_ePara[0].para.e_id_test,7);
			//
			return;//已经判断LW信号是有效的，堵料状态确认
		}
	}
	else
	{
		the_last_time_of_blw_high = boot_time;
	}
	//
	//LW信号无效，不存在堵料
	bit_duliao = 0;
	ddr_mbbj = 1;
	//bmbbj = bit_duliao | mb_is_invalid;//该处可以写的更加简洁，为了规范还是写全较好
}
////////////////////////////////////////////////////////////
void dw_signal_handling(void)
{
	//磨辊转速脉冲输入信号处理
	//
	if(DW_LEVEL == m_rPara.para.dw_level_pulse)
	{
		//暂时不做处理
		asm volatile("nop");//占位代码，没有实际作用
	}
	else if(DW_PULSE == m_rPara.para.dw_level_pulse)
	{
		//暂时不做处理
		dw_counter_3s;//占位代码，没有实际作用
	}
}
////////////////////////////////////////////////////////////
void mb_signal_handling(void)
{
	//mb信号处理函数
	//mb输入信号为电平或者脉冲的处理方式重复度很高，但这样
	//便于维护，以后哪种方式有问题，针对修改该方式的代码即可
	//不需要担心会影响别的方式。
	//
	if(MB_LEVEL == m_rPara.para.mb_level_pulse)
	{
		//MB输入信号为电平
		if (1 == bftstart)
		{
			//喂料电机已经启动
			if((1==bmb) && (enable_mb_delay > 20))//100ms一个值，2s
			{
				if(mb_invalid_delay > 200)//10ms，2s
				{
					mb_invalid_delay = 0;
					//
					//确认mb信号已经无效
					mb_is_invalid = 1;
					mb_valid_counter = 0;
					//
					enable_mb_delay = 0;
					part_stop_bmb = 1; //mb电平输入
					//
					ddr_mbbj = 1;
					//bmbbj = 1;
					//
					fmotor_stop();
					roll_out();
					//DEBUG_STRING("fmotor stop in mb_signal_handling(MB_LEVEL)\r\n\0");
				}
			}
			else
			{
				//mb信号是有效的，清除mb无效计数的延时
				mb_invalid_delay = 0;
				mb_is_invalid = 0;
			}
		}
		else if(1 == part_stop_bmb)
		{
			//喂料电机没有启动
			if(0 == bmb)
			{
				mb_invalid_delay = 0;
				//
				mb_valid_counter++;
				if(mb_valid_counter > 2)
				{
					mb_valid_counter = 0;
					//
					mb_is_invalid = 0;
				}
			}
			else
			{
				if(mb_invalid_delay > 100)
				{
					mb_invalid_delay = 0;
					//
					mb_valid_counter = 0;
				}
			}
		}
	}
	else if(MB_PULSE == m_rPara.para.mb_level_pulse)
	{
		//MB输入信号为脉冲
		if (1 == bftstart)
		{
			//mb_div>0
			if((mb_counter_3s < (3*m_rPara.para.mb_div-1)) && (enable_mb_delay > 20))//100ms，2s
			{
				if(mb_invalid_delay > 200)//2s
				{
					mb_invalid_delay = 0;
					//
					//确认mb信号已经无效
					mb_is_invalid = 1;
					mb_valid_counter = 0;
					//
					enable_mb_delay=0;
					part_stop_bmb = 1; //mb脉冲输入
					//
					ddr_mbbj = 1;
					//bmbbj = 1;
					//
					fmotor_stop();
					roll_out();
					//DEBUG_STRING("fmotor stop in mb_signal_handling(MB_PULSE)\r\n\0");
				}
			}
			else
			{
				//mb信号是有效的，清除mb无效计数的延时
				mb_invalid_delay = 0;
				mb_is_invalid = 0;
			}
		}
		else if(1 == part_stop_bmb)
		{
			//mb_div>0
			if(mb_counter_3s > (3*m_rPara.para.mb_div-2))
			{
				mb_invalid_delay = 0;
				//
				mb_valid_counter++;
				if(mb_valid_counter > 2)
				{
					mb_valid_counter = 0;
					//
					mb_is_invalid = 0;
				}
			}
			else
			{
				if(mb_invalid_delay > 100)
				{
					mb_invalid_delay = 0;
					//
					mb_valid_counter = 0;
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////
