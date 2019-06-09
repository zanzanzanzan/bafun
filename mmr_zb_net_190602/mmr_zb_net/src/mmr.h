
#if USED_W5500 == TRUE
////////////////////////////////////////////////////////////////

#ifndef __SIGUNDIANKONGZHUBAN__
#define __SIGUNDIANKONGZHUBAN__

////////////////////////////////////////////////////////////////
#include "typedef_user.h"
#include "soft_timer.h"
#include "core_func.h"
#include "display.h"
#include "scan_key.h"
#include "modbus_rtu.h"
#include "uart_to_pc.h"
#include "w5500_manager.h"
////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
//��������
void softward_init(void);
void hardward_init(void);
void OnTimer(unsigned short id);
void pin_init(void);
void ad_value_handling(void);
void timer0_overflow_handling(void);
void hardware_uart_receive(unsigned char val);
void display_current_value(void);
void check_bootloader(void);
void base_task(void);
void low_speed_task(void);
void poll_task(void);
void net_task(void);
//
#define UART0_RX_BUF_SIZE   128
#define UART0_RX_BUF_MASK   (UART0_RX_BUF_SIZE-1)
volatile unsigned char hard_uart_rx_buf[UART0_RX_BUF_SIZE+4];//Ӳ������FIFO����
volatile unsigned char hard_uart_rx_addr,hard_uart_load_addr;
volatile unsigned char hard_uart_byte_count;
volatile unsigned char reset_cause;
//
//�Զ����
#define APLCT __attribute__((section(".apflag")))//�Զ���apflag��
volatile const APLCT unsigned char apli_id[8]={'B','E','A','R','_','X','L','F'};//�����������ѯID��
//
////////////////////////////////////////////////////////////////

#endif //__SIGUNDIANKONGZHUBAN__
#endif //USED_W5500
