/*
 * soft_timer.c
 *
 * Created: 2013/6/25 13:47:12
 * Author: xiong
 *
 * 重试次数为0代表执行完一次后，定时器立马关闭。
 * 重试次数为-1代表一直执行下去，定时器不关闭（除非执行关闭函数）
 *
 */ 
 
//////////////////////////////////////////////
#include "typedef_user.h"
#include "string.h"
//////////////////////////////////////////////
//允许最多同时运行定时器个数，根据项目的需要修改。
//但也不能太大，占用内存和CPU执行时间
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
	//软件定时器细粒度由主时钟定时器细粒度决定，软件定时器定时时间大于
	//实际设置的时间，但一般不会多于一个主时钟定时器的细粒度。如本项目
	//中，软件定时器的定时时间一般不会比设置值多6.25ms。
	unsigned int i;
	//
	if(SOFT_TIMER_ERROR == id)
		return SOFT_TIMER_ERROR;
	if(0 == ms)
		return SOFT_TIMER_ERROR;
	//
	//搜索id号匹配的软件定时器
	for(i=0; i<SOFT_TIMER_NUM; i++)
	{
		if(soft_timer[i].id == id)
		{
			soft_timer[i].enable = TRUE;
			soft_timer[i].ms = (ms+9)/10;//boot_time一个值代表10ms
			soft_timer[i].overtime = soft_timer[i].ms + boot_time;
			soft_timer[i].retry = retry;
			//
			return id;
		}
	}
	//
	//id号匹配的软件定时器没有找到，搜索空闲的软件定时器
	for(i=0; i<SOFT_TIMER_NUM; i++)
	{
		if(FALSE == soft_timer[i].enable)
		{
			soft_timer[i].enable = TRUE;
			soft_timer[i].id = id;
			soft_timer[i].ms = (ms+9)/10;//boot_time一个值代表10ms
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
		//搜索指定的定时器并关闭它
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
	//避免初始化异常
	soft_timer_id %= SOFT_TIMER_NUM;
	//
	for(id=0; id<SOFT_TIMER_NUM; id++)
	{
		if(TRUE == soft_timer[soft_timer_id].enable)
		{
			if(boot_time >= soft_timer[soft_timer_id].overtime)
			{
				//根据重试参数判断是否关闭定时器
				if(-1 != soft_timer[soft_timer_id].retry)
				{
					//重试次数为-1代表一直运行
					if(soft_timer[soft_timer_id].retry > 0)
					{
						//减重试次数
						soft_timer[soft_timer_id].retry--;
					}
					else
					{
						//关闭定时器
						soft_timer[soft_timer_id].enable = FALSE;
					}					
				}
				//
				//在执行定时器服务程序的过程中能够再次设置定时器				
				OnTimer(soft_timer[soft_timer_id].id);//定时器溢出处理
				//
				//定时器服务程序运行时间不算在定时时间内
				if(TRUE == soft_timer[soft_timer_id].enable)
				{
					//定时继续运行，重置定时器时间
					soft_timer[soft_timer_id].overtime = soft_timer[soft_timer_id].ms + boot_time;
				}
				//
				soft_timer_id++;
				soft_timer_id %= SOFT_TIMER_NUM;
				//
				return;//执行了一次有效的定时服务后退出，等待下次扫描
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
	//搜索id号匹配的软件定时器
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








