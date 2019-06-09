/*
 * modbus_rtu.h
 *
 * Created: 2014/11/22 10:52:21
 *  Author: xiong
 */ 


#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_


extern void modbus_task(void);
extern void modbus_init(void);
extern void modbus_rtu_rx_frame(unsigned char val);


extern SYSTEM_STATUS_MODBUS modbus_status;


#endif /* MODBUS_RTU_H_ */