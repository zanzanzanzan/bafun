/*
 * display.h
 *
 * Created: 2014/11/13 18:15:42
 *  Author: xiong
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_


extern void led_display(void);


//avr8_gccҲ����������⣬��һ��ģ���У���ջ�Ϸ�����ڴ���һ��
//���ƣ����嵽�����Ƕ���ûʱ��ȥһһȷ���ˡ�
//�ر�ע����ǣ�����������������������ᱨ����д�봦����������
//ʱ�����Ī����������⡣
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