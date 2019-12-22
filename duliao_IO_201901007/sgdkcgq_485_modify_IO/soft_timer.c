/*
 * soft_timer.c
 *
 * Created: 2013/6/25 13:47:12
 * Author: xiong
 *
 * ���Դ���Ϊ0����ִ����һ�κ󣬶�ʱ������رա�
 * ���Դ���Ϊ-1����һֱִ����ȥ����ʱ�����رգ�����ִ�йرպ�����
 *
 */ 
 
//////////////////////////////////////////////
#include "typedef_user.h"
#include "string.h"
//////////////////////////////////////////////
//�������ͬʱ���ж�ʱ��������������Ŀ����Ҫ�޸ġ�
//��Ҳ����̫��ռ���ڴ��CPUִ��ʱ��
#define SOFT_TIMER_NUM		8
#define SOFT_TIMER_ERROR	0
//
#define FALSE               0
#define TRUE				1
//////////////////////////////////////////////
//
extern void OnTimer(unsigned short id);
//
void timer_run(void);
void init_soft_timer(void);
unsigned short SetTimer(unsigned short id,unsigned long ms,signed char retry);
unsigned short KillTimer(unsigned short id);
unsigned char timer_is_running(unsigned short id);
//
/////////////////////////////////////////////
volatile TIMER_PARA_PKT soft_timer[SOFT_TIMER_NUM];
volatile unsigned int soft_timer_id;
/////////////////////////////////////////////
//
//extern LCD_BUF_PACKET lcd_dis_buf;
extern unsigned long boot_time;
//
/////////////////////////////////////////////
unsigned short SetTimer(unsigned short id,unsigned long ms,signed char retry)
{	
	//�����ʱ��ϸ��������ʱ�Ӷ�ʱ��ϸ���Ⱦ����������ʱ����ʱʱ�����
	//ʵ�����õ�ʱ�䣬��һ�㲻�����һ����ʱ�Ӷ�ʱ����ϸ���ȡ��籾��Ŀ
	//�У������ʱ���Ķ�ʱʱ��һ�㲻�������ֵ��6.25ms��
	unsigned int i;
	//
	if(SOFT_TIMER_ERROR == id)
		return SOFT_TIMER_ERROR;
	if(0 == ms)
		return SOFT_TIMER_ERROR;
	//
	//����id��ƥ��������ʱ��
	for(i=0; i<SOFT_TIMER_NUM; i++)
	{
		if(soft_timer[i].id == id)
		{
			soft_timer[i].enable = TRUE;
			soft_timer[i].ms = (ms+9)/10;//boot_timeһ��ֵ����10ms
			soft_timer[i].overtime = soft_timer[i].ms + boot_time;
			soft_timer[i].retry = retry;
			//
			return id;
		}
	}
	//
	//id��ƥ��������ʱ��û���ҵ����������е������ʱ��
	for(i=0; i<SOFT_TIMER_NUM; i++)
	{
		if(FALSE == soft_timer[i].enable)
		{
			soft_timer[i].enable = TRUE;
			soft_timer[i].id = id;
			soft_timer[i].ms = (ms+9)/10;//boot_timeһ��ֵ����10ms
			soft_timer[i].overtime = soft_timer[i].ms + boot_time;
			soft_timer[i].retry = retry;
			//
			return id;			
		}
	}
	//
	return SOFT_TIMER_ERROR;
}
/////////////////////////////////////////////
unsigned short KillTimer(unsigned short id)
{
	unsigned int i;
	//
	if(SOFT_TIMER_ERROR == id)
		return SOFT_TIMER_ERROR;
	//
	for(i=0; i<SOFT_TIMER_NUM; i++)
	{
		//����ָ���Ķ�ʱ�����ر���
		if(soft_timer[i].id == id)
		{
			soft_timer[i].enable = FALSE;
			return id;
		}
	}
	//
	return SOFT_TIMER_ERROR;
}
/////////////////////////////////////////////
void timer_run(void)
{
	int id;
	//
	//�����ʼ���쳣
	soft_timer_id %= SOFT_TIMER_NUM;
	//
	for(id=0; id<SOFT_TIMER_NUM; id++)
	{
		if(TRUE == soft_timer[soft_timer_id].enable)
		{
			if(boot_time >= soft_timer[soft_timer_id].overtime)
			{
				//�������Բ����ж��Ƿ�رն�ʱ��
				if(-1 != soft_timer[soft_timer_id].retry)
				{
					//���Դ���Ϊ-1����һֱ����
					if(soft_timer[soft_timer_id].retry > 0)
					{
						//�����Դ���
						soft_timer[soft_timer_id].retry--;
					}
					else
					{
						//�رն�ʱ��
						soft_timer[soft_timer_id].enable = FALSE;
					}					
				}
				//
				//��ִ�ж�ʱ���������Ĺ������ܹ��ٴ����ö�ʱ��				
				OnTimer(soft_timer[soft_timer_id].id);//��ʱ���������
				//
				//��ʱ�������������ʱ�䲻���ڶ�ʱʱ����
				if(TRUE == soft_timer[soft_timer_id].enable)
				{
					//��ʱ�������У����ö�ʱ��ʱ��
					soft_timer[soft_timer_id].overtime = soft_timer[soft_timer_id].ms + boot_time;
				}
				//
				soft_timer_id++;
				soft_timer_id %= SOFT_TIMER_NUM;
				//
				return;//ִ����һ����Ч�Ķ�ʱ������˳����ȴ��´�ɨ��
			}
		}
		//
		soft_timer_id++;
		soft_timer_id %= SOFT_TIMER_NUM;	
	}		
}
/////////////////////////////////////////////
void init_soft_timer(void)
{
	boot_time=0;
	soft_timer_id = 0;
	//
	memset((char*)&soft_timer,0x00,sizeof(soft_timer));
}
/////////////////////////////////////////////
unsigned char timer_is_running(unsigned short id)
{	
	unsigned int i;
	//
	if(SOFT_TIMER_ERROR == id)
		return SOFT_TIMER_ERROR;
	//
	//����id��ƥ��������ʱ��
	for(i=0; i<SOFT_TIMER_NUM; i++)
	{
		if(soft_timer[i].id == id)
		{
			return soft_timer[i].enable;
		}
	}
	//
	return FALSE;
}
/////////////////////////////////////////////








