/*
 * display.h
 *
 * Created: 2014/11/13 18:15:42
 *  Author: xiong
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_


extern void led_display(void);


//avr8_gcc也存在这个问题，在一个模块中，在栈上分配的内存有一定
//限制，具体到数量是多少没时间去一一确认了。
//特别注意的是，出现这种情况编译器并不会报错，但写入处理器，运行
//时会出现莫名其妙的问题。
extern unsigned char mb_60s_sample_buf[MB_60S_SAMPLE_SIZE];
extern unsigned char mb_60s_sample_addr;
extern unsigned char mb_3s_sample_buf[MB_3S_SAMPLE_SIZE];
extern unsigned char mb_3s_sample_addr;
extern unsigned char dw_60s_sample_buf[DW_60S_SAMPLE_SIZE];
extern unsigned char dw_60s_sample_addr;
extern unsigned char dw_3s_sample_buf[DW_3S_SAMPLE_SIZE];
extern unsigned char dw_3s_sample_addr;
////////////////////////////////////////////////////////////////


#endif /* DISPLAY_H_ */