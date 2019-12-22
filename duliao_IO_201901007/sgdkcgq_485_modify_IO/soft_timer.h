/*
 * soft_timer.h
 *
 * Created: 2014/6/25 13:47:12
 *  Author: xiong
 */ 



#ifndef __SOFT_TIMER_H__
#define __SOFT_TIMER_H__



extern void timer_run(void);
extern void init_soft_timer(void);
extern unsigned short SetTimer(unsigned short id,unsigned long ms,signed char retry);
extern unsigned short KillTimer(unsigned short id);
extern unsigned char timer_is_running(unsigned short id);



#endif

