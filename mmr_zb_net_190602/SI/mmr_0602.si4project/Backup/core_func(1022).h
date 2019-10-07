/*
 * core_func.h
 *
 * Created: 2014/11/13 18:11:43
 *  Author: xiong
 */


#ifndef CORE_FUNC_H_
#define CORE_FUNC_H_

/////////////////////////////////////////////////////////////////
extern void fmotor_vel(void);
extern void fmotor_vel_a(void);
extern void read_para(unsigned char);
extern void mmotor_start(void);
extern void mmotor_stop(void);
extern void roll_in(void);
extern void roll_out(void);
extern void roll_lock(void);
extern void roll_unlock(void);
extern void fmotor_start(void);
extern void fmotor_stop(void);
extern void auto_run(void);
extern void manual_run(void);
extern void key_manual_run(void);
extern void key_auto_run(void);
extern void reset_set_status(void);
extern void all_stop(void);
extern void part_stop(void);
extern void enable_wait_rollin(void);
extern void deal_wait_rollin(void);
extern void material_2_vel(unsigned short);
extern unsigned char eeprom_read_byte_user(const uint8_t *p);
extern void eeprom_write_byte_user(uint8_t *p,unsigned char val);
extern unsigned short eeprom_read_word_user(const uint16_t *p);
extern void eeprom_write_word_user(uint16_t *p,unsigned short val);
extern void uart_tx(void);
extern void save_para(void);
extern void runed_time(void);
extern void run_in_remote_ctrl(void);
extern void wdt_reset_mcu(void);
extern unsigned short get_mb_counter_60s(void);
extern unsigned char get_mb_counter_3s(void);
extern unsigned short get_dw_counter_60s(void);
extern unsigned char get_dw_counter_3s(void);
extern void mb_signal_handling(void);
extern void dw_signal_handling(void);
extern void lw_signal_handling(void);
extern void qr_ql_signal_handling(void);
/////////////////////////////////////////////////////////////////
/////////////在显示板上显示的灯状态
extern volatile U8_BIT led_buf[14];
#define bit_led_auto        BIT(led_buf[0].bit,0)//led defination
#define bit_led_manual      BIT(led_buf[0].bit,1)
#define bit_led_mmotor      BIT(led_buf[0].bit,3)
#define bit_led_xl          BIT(led_buf[0].bit,2)
#define bit_led_remote      BIT(led_buf[0].bit,4)
#define bit_led_hz          BIT(led_buf[0].bit,5)//合闸
#define bit_led_lz          BIT(led_buf[0].bit,6)//离闸
#define bit_led_over_i      BIT(led_buf[0].bit,7)//08

#define bit_led_cssd        BIT(led_buf[7].bit,0)//O9//参数设定
#define bit_led_dw          BIT(led_buf[7].bit,1)//低位
#define bit_led_gw          BIT(led_buf[7].bit,2)//高位
#define bit_led_dl          BIT(led_buf[7].bit,3)//堵料
#define bit_led_up          BIT(led_buf[7].bit,4)//第一屏
#define bit_led_down        BIT(led_buf[7].bit,5)//第二屏
////////////////
extern volatile unsigned char bit_auto;
extern volatile unsigned char bit_manual;
extern volatile unsigned char bit_mmotor;
extern volatile unsigned char bit_fmotor;
extern volatile unsigned char bit_remote;
extern volatile unsigned char bit_h_mtl;
extern volatile unsigned char bit_l_mtl;
extern volatile unsigned char bit_over_i;
//
extern volatile unsigned char bit_duliao;
extern volatile unsigned char bit_current_1st;
extern volatile unsigned char bit_current_2nd;
//
/////////////
extern volatile unsigned char  bit_setmode_1;
extern volatile unsigned char  bit_setmode_2;//内部参数
extern volatile unsigned char  bit_wait_rollin;
extern volatile unsigned char  bit_mm_control;//主电机电流连锁控制
extern volatile unsigned char  bit_comm_error;
extern volatile unsigned char  bit_setmode_password;
extern volatile unsigned char  bit_setmode_delay;
extern volatile unsigned char  bit_used_no_time;
//
extern volatile unsigned char  bit_xieliao;
extern volatile unsigned char  bit_alarm_mode;
extern volatile unsigned char  material_2_vel_enable;
extern volatile unsigned char  bit_setmode_3;
extern volatile unsigned char  bit_setmode;
extern volatile unsigned char  bit_setmode_pwd_time;
extern volatile unsigned char  bit_lock;
extern volatile unsigned char  bit_roll_in;
//
extern volatile unsigned char  bit_qr_ql_error;
extern volatile unsigned char  bit_monitor_mode;
extern volatile unsigned char  bit_material_disturbance;
extern volatile unsigned char  bit_soft_uart_rx;
extern volatile unsigned char  bit_soft_uart_tx;
extern volatile unsigned char  soft_uart_frame_head_ok;
extern volatile unsigned char  timer0_overflow_count;
//
extern volatile unsigned char  uart_frame_head_ok;
extern volatile unsigned char  uart_frame_ok;
extern volatile unsigned char  mb_is_invalid;
//
////////////////////////////////////////////////////////////////
//时间设置进入密码
extern const unsigned char const_password[6];
//密码保护参数进入密码
extern const unsigned char arg_password[6];
extern unsigned char password[6];
//
//内存变量定义
//WINAVR GCC用的是Little-endian结构,和VC相同
extern unsigned char ms1_counter,ms10_counter,ms100_counter;
extern unsigned char comm_error_delay;
extern unsigned short ad_counter[8];
extern UCHAR4 ad_add_value[8];
extern unsigned char uart_rx_counter;
extern unsigned char uart_rx_buf[10],uart_tx_buf[10];
extern unsigned char uart_tx_addr;
//
extern unsigned char key_value;
extern UCHAR2 deal_key_buf[3];
extern UCHAR2 key_value_bak;
extern unsigned short m_cur_d1,m_cur_d2,m_material,m_material_value,m_cur_cal;
extern unsigned short m_material_value_monitor;
extern unsigned short m_cur_monitor;
extern UCHAR2 m_material_add_value;
extern unsigned char m_material_counter;
extern unsigned short auto_vel;
//
extern unsigned char cur_overflow_delay;
//主电机启动,停止,合轧,离轧信号为脉冲,高电平时间为2秒
extern unsigned char mmotor_delay,roll_delay,password_delay;
extern unsigned long long_pressed_cssd_delay,long_pressed_csqr_delay;
extern unsigned char pagenumber,key_delay,key_delay_set_in,cur_d1_round,cur_d2_round,m_cur_round;
//等待合轧的计时(100ms)
extern unsigned char wait_rollin,set_idle_wait;
extern unsigned char enable_mb_delay,mb_invalid_delay,mb_valid_counter;
extern unsigned short m_set_dis_value;//设置值的显示
extern unsigned short mm_cur_overflow_stop,mmotor_cur1_cmp;
#if __D2_USED__ == TRUE
extern unsigned short mmotor_cur2_cmp;
#endif
//使用时间限制
extern unsigned short m_used_1s,m_used_12h;
//
extern unsigned char m_cgq_cmd;
//
extern unsigned char arg_set_process;
extern unsigned short m_material_stop_modify,m_material_start_modify;
extern unsigned char m_cgq_ack,m_cgq_ack_delay;
extern unsigned char part_stop_bmb;
//
extern unsigned short mb_counter_3s,dw_counter_3s;
extern unsigned short mb_counter_60s,dw_counter_60s;
extern unsigned short mb_counter_tmp,dw_counter_tmp;
//
//EEPROM 变量定义
extern NVM_DATA m_ePara[5];
//
extern NVM_DATA m_rPara;
//
extern unsigned short m_vel_alarm;//用于告警判断
//恢复默认设置(设置值为8时，所有的参数恢复出厂默认值)
extern volatile unsigned char set_default;
//使用天数
extern volatile unsigned short limited_days;
////////////////////////////////////////////////////////////////


#endif /* CORE_FUNC_H_ */