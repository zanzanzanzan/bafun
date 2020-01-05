/*
 * uart_to_pc.h
 *
 * Created: 2016/5/4 19:55:43
 *  Author: xiong
 */ 


#ifndef UART_TO_PC_H_
#define UART_TO_PC_H_

extern void uart1_task(void);
extern void uart1_init(void);
extern void set_modbus_baud(unsigned char baud);

#endif /* UART_TO_PC_H_ */