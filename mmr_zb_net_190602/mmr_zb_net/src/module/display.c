/*
 * display.c
 *
 * Created: 2014/11/13 18:15:58
 *  Author: xiong
 */ 
/////////////////////////////////////////////////////////////////
#include "typedef_user.h"
#include "core_func.h"
#include "scan_key.h"
#include "modbus_rtu.h"
/////////////////////////////////////////////////////////////////
void led_display(void);
void display_page(void);
void led_addr_switch_3bits(unsigned char *p,unsigned short value);
void led_addr_switch_4bits(unsigned char *p,unsigned short value);
/////////////////////////////////////////////////////////////////
//avr8_gcc也存在这个问题，在一个模块中，在栈上分配的内存有一定
//限制，具体到数量是多少没时间去一一确认了。
//特别注意的是，出现这种情况编译器并不会报错，但写入处理器，运行
//时会出现莫名其妙的问题。
unsigned char mb_60s_sample_buf[MB_60S_SAMPLE_SIZE];
unsigned char mb_60s_sample_addr;
unsigned char mb_3s_sample_buf[MB_3S_SAMPLE_SIZE];
unsigned char mb_3s_sample_addr;
unsigned char dw_60s_sample_buf[DW_60S_SAMPLE_SIZE];
unsigned char dw_60s_sample_addr;
unsigned char dw_3s_sample_buf[DW_3S_SAMPLE_SIZE];
unsigned char dw_3s_sample_addr;
//
////////////////////////////////////////////////////////////////
//数码管显示值
//为显示板组织数码管显示驱动值,显示板得到这些值直接驱动数码管显示
//可以把数码管当作是直接由主板控制的
void led_display(void)
{
	unsigned short m_vel,m_cur;
	//
	//判断主电机电流溢出是否达到停机要求
	if(m_cur_d1 < mm_cur_overflow_stop)
	{
		//不清除的话，每秒自动加一
		cur_overflow_delay=0;
	}
	//
	if(GAOLIAOWEI==m_cgq_ack)
	{
		led_buf[1].ch=LED_DIS_SPACK;
		led_buf[2].ch=LED_DIS_SPACK;
		led_buf[3].ch=LED_DIS_SPACK;
		led_buf[4].ch=LED_DIS_SPACK;
		led_buf[5].ch=LED_DIS_SPACK;
		led_buf[6].ch=LED_DIS_H;
		if(m_cgq_ack_delay > 9)
		{
			m_cgq_ack=0;
			m_cgq_ack_delay=0;
		}
	}
	else if(DILIAOWEI==m_cgq_ack)
	{
		led_buf[1].ch=LED_DIS_SPACK;
		led_buf[2].ch=LED_DIS_SPACK;
		led_buf[3].ch=LED_DIS_SPACK;
		led_buf[4].ch=LED_DIS_SPACK;
		led_buf[5].ch=LED_DIS_SPACK;
		led_buf[6].ch=LED_DIS_L;
		if(m_cgq_ack_delay > 9)
		{
			m_cgq_ack=0;
			m_cgq_ack_delay=0;
		}
	}
	else if((0==bit_setmode) && (0==bit_monitor_mode) && (0==bit_alarm_mode))
	{
		//工作模式的数码管显示
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
		modbus_status.status.m_rpm = m_vel;//转速  

        
		//m_vel = 123;//转速



        led_addr_switch_3bits((unsigned char*)&led_buf[4].ch,m_vel);
		//
		//主电机电流要一直显示,不管键盘上的主电机启动按钮是否按下,因为有可
		//能在别的地方启动主电机
		m_cur=m_cur_cal;
		//确定小数点的位置
		modbus_status.status.m_cur_i = m_cur;//有乘10的效果


        //m_cur = 456;//有乘10的效果
		//
		if(m_cur < 100)
		{
			m_cur*=10;
			m_cur+=m_cur_round;
			led_addr_switch_3bits((unsigned char*)&led_buf[1].ch,m_cur);
			if(LED_DIS_SPACK != led_buf[1].ch)
			{
				led_buf[1].ch += 22;
			}
		}
		else
		{
			led_addr_switch_3bits((unsigned char*)&led_buf[1].ch,m_cur);
			led_buf[2].ch += 22;
		}
		//
		led_buf[11].ch = led_buf[4].ch;//转速
		led_buf[12].ch = led_buf[5].ch;
		led_buf[13].ch = led_buf[6].ch;
		led_buf[8].ch = LED_DIS_R;//R
		led_buf[9].ch = LED_DIS_P;//P
		led_buf[10].ch = LED_DIS_M;//M
		//
		if(1==bit_mmotor)
		{
		led_buf[4].ch = led_buf[3].ch;//电流
		led_buf[3].ch = led_buf[2].ch;
		led_buf[2].ch = led_buf[1].ch;	
		}
		else
		{
			led_buf[4].ch = LED_DIS_0;//电流
			led_buf[3].ch = LED_DIS_SPACK;
			led_buf[2].ch = LED_DIS_SPACK;
		}
		led_buf[1].ch = LED_DIS_I;//I
		//
		led_buf[5].ch = LED_DIS_SPACK;
		led_buf[6].ch = LED_DIS_SPACK;
	}
	else
	{
		//设置模式下的数码管显示
		display_page();
		//		
		bit_current_1st = 0;
		bit_current_2nd = 0;
	}
}
/////////////////////////////////////////////////////////////////
//根据页号显示菜单页
void display_page(void)
{
	unsigned short dis_value;
	unsigned char i;
	//
	if(1 == bit_monitor_mode)
	{
		//监控数据显示模式
		dis_value=0;
		led_buf[1].ch=LED_DIS_P;//p
		led_buf[2].ch=LED_DIS_SUB;//-
		led_buf[3].ch=monitor_mode_page+1;
		//
		switch(monitor_mode_page)
		{
			case 0:
			{
				//当前采样料位传感器信号电压值
				//(m_material_value_monitor/300)*1000/250
				dis_value = m_material_value_monitor/75;//4-20mA
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_L;//料位电压值
				led_buf[10].ch = LED_DIS_W;
				led_buf[11].ch = LED_DIS_D;
				led_buf[12].ch = LED_DIS_Y;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			case 1:
			{
				//当前采样电流传感器信号电压值
				//(m_cur_monitor*16)/(cur_full*10)+4
				dis_value = (((m_cur_monitor*16)/m_rPara.para.cur1_full)/10) + 4;
				//
				led_buf[8].ch = LED_DIS_D;//电流电压值1
				led_buf[9].ch = LED_DIS_L;
				led_buf[10].ch = LED_DIS_D;
				led_buf[11].ch = LED_DIS_Y;
				led_buf[12].ch = LED_DIS_Z;
				led_buf[13].ch = LED_DIS_1;
			}
			break;
			case 2:
			{
				//喂料辊当前每分钟脉冲输入数(MB)
				dis_value = mb_counter_60s;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_W;//喂料辊脉冲
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_G;
				led_buf[12].ch = LED_DIS_M;
				led_buf[13].ch = LED_DIS_C;
			}
			break;
			case 3:
			{
				//磨辊当前每分钟脉冲输入数(DW)
				dis_value = dw_counter_60s;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_M;//磨辊脉冲
				led_buf[10].ch = LED_DIS_G;
				led_buf[11].ch = LED_DIS_M;
				led_buf[12].ch = LED_DIS_C;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
			case 4:
			{
				//累计运行时间(显示为天数）
				dis_value = m_rPara.para.used_hours/24;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_Y;//运行时间
				led_buf[10].ch = LED_DIS_X;
				led_buf[11].ch = LED_DIS_S;
				led_buf[12].ch = LED_DIS_J;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
			case 5:
			{
				//软件版本号
				dis_value = SOFTWARE_VERSION;//1.0
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_R;//软件版本号
				led_buf[10].ch = LED_DIS_J;
				led_buf[11].ch = LED_DIS_B;
				led_buf[12].ch = LED_DIS_B;
				led_buf[13].ch = LED_DIS_H;
			}
			break;
			#if __D2_USED__ == TRUE
			case 6:
			{
				//当前采样电流传感器信号电压值
				//(m_cur_monitor*16)/(cur_full*10)+4
				dis_value = (((m_cur_monitor*16)/m_rPara.para.cur2_full)/10) + 4;
				//
				led_buf[8].ch = LED_DIS_D;//电流电压值2
				led_buf[9].ch = LED_DIS_L;
				led_buf[10].ch = LED_DIS_D;
				led_buf[11].ch = LED_DIS_Y;
				led_buf[12].ch = LED_DIS_Z;
				led_buf[13].ch = LED_DIS_2;
			}
			break;
			#endif
			default:
			{
				dis_value = 123;//for test
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_SPACK;
				led_buf[10].ch = LED_DIS_SPACK;
				led_buf[11].ch = LED_DIS_SPACK;
				led_buf[12].ch = LED_DIS_SPACK;
				led_buf[13].ch = LED_DIS_SPACK;				
			}
			break;
		}
		//
		led_addr_switch_3bits((unsigned char*)&led_buf[4].ch,dis_value);
	}
	else if(1 == bit_alarm_mode)
	{
		//告警数据显示模式
		dis_value=0;
		led_buf[1].ch=LED_DIS_A;//A
		led_buf[2].ch=LED_DIS_SUB;//-
		led_buf[3].ch=alarm_mode_page+1;
		//
		switch(alarm_mode_page)
		{
			case 0:
			{
				//主电机电流过载停机
				dis_value = (1==bit_over_i) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_Z;//主电流过载
				led_buf[10].ch = LED_DIS_D;
				led_buf[11].ch = LED_DIS_L;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			case 1:
			{
				//喂料辊故障停机
				dis_value = (1==part_stop_bmb) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_W;//喂料辊停机
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_G;
				led_buf[12].ch = LED_DIS_T;
				led_buf[13].ch = LED_DIS_J;
			}
			break;
			case 2:
			{
				//料位不稳定（料位传感器长时间内大范围波动）
				dis_value = (1==bit_material_disturbance) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_L;//料位不稳
				led_buf[10].ch = LED_DIS_W;
				led_buf[11].ch = LED_DIS_B;
				led_buf[12].ch = LED_DIS_W;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
			case 3:
			{
				//下料口堵塞（LW光耦输入时）
				dis_value = (0==blw) ? 1:0; //1:堵料
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_X;//下料口堵塞
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_K;
				led_buf[12].ch = LED_DIS_D;
				led_buf[13].ch = LED_DIS_S;
			}
			break;
			case 4:
			{
				//喂料辊转速不在设定范围内（MB检测脉冲数/3不在最高和最低值内）
				if((m_vel_alarm>m_rPara.para.max_vel) || (m_vel_alarm<m_rPara.para.min_vel))
				{
					dis_value = 1;
				}
				else
				{
					dis_value = 0;
				}
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_W;//喂料辊故障
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_G;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			/*case 5:
			{
				//磨辊左边合轧不到位（QL没输入时）
				dis_value = (1==bql) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_M;//磨辊左故障
				led_buf[10].ch = LED_DIS_G;
				led_buf[11].ch = LED_DIS_Z;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			case 6:
			{
				//磨辊右边合轧不到位(QR没输入时)
				dis_value = (1==bqr) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_M;//磨辊右故障
				led_buf[10].ch = LED_DIS_G;
				led_buf[11].ch = LED_DIS_Y;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;*/
			default:
			{
				dis_value = 567;//for test
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_SPACK;
				led_buf[10].ch = LED_DIS_SPACK;
				led_buf[11].ch = LED_DIS_SPACK;
				led_buf[12].ch = LED_DIS_SPACK;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
		}
		//
		led_addr_switch_3bits((unsigned char*)&led_buf[4].ch,dis_value);
	}
	else if(1 == bit_setmode_password)
	{
		//密码输入界面
		if(pagenumber > 5)
		{
			//密码输入完成,需要比较密码是否正确
			if(1 == bit_setmode_pwd_time)
			{
				//使用时间密码设置
				for(i=0; i<6; i++)
				{
					if((const_password[i]-'0') != password[i])
					{
						break;
					}
				}
				if(6 == i)
				{
					bit_setmode_password=0;
					bit_setmode_delay=1;//进入限时设置
					bit_setmode_pwd_time=0;
					pagenumber=0;
				}
				else
				{
			        TRACE_STRING("bit_setmode_password1");
					reset_set_status();//退出设置
				}
			}
			else
			{
				//密码保护参数密码设置
				for(i=0; i<6; i++)
				{
					if((arg_password[i]-'0') != password[i])
					{
						break;
					}
				}
				if(6 == i)
				{
					bit_setmode_password=0;
					bit_setmode_3=1;//进入限时设置
					pagenumber=0;
				}
				else
				{
					TRACE_STRING("bit_setmode_password2");
					reset_set_status();//退出设置
				}
			}
		}
		else
		{
			led_buf[1].ch=password[0];
			led_buf[2].ch=password[1];
			led_buf[3].ch=password[2];
			led_buf[4].ch=password[3];
			led_buf[5].ch=password[4];
			led_buf[6].ch=password[5];
			//
			led_buf[8].ch = LED_DIS_SPACK;
			led_buf[9].ch = LED_DIS_SPACK;
			led_buf[10].ch = LED_DIS_SPACK;
			led_buf[11].ch = LED_DIS_SPACK;
			led_buf[12].ch = LED_DIS_SPACK;
			led_buf[13].ch = LED_DIS_SPACK;
		}
	}
	else
	{
		//参数设置界面
		dis_value=0;
		led_buf[2].ch=LED_DIS_SUB;//-
		led_buf[3].ch=pagenumber+1;
		//
		if(1 == bit_setmode_delay)
		{
			//设置可用的时间
			led_buf[1].ch=LED_DIS_4;
			//
			switch(pagenumber)
			{
				case 0://时间限制
				{
					if(limited_days > 999) //不要修改
					{
						dis_value = 999;
					}
					else
					{
						dis_value=limited_days;
					}					
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_S;//时间限制
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_X;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				default:
				{
					TRACE_STRING("default");
					reset_set_status();
				}
				break;
			}
		}
		else if(1==bit_setmode_2)
		{
			//设置内部参数
			led_buf[1].ch=LED_DIS_2;
			//
			switch(pagenumber)
			{
				case 0://料位传感器满量程输出(50%-100%)
				{
					dis_value=m_rPara.para.material_full;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_L;//料位满量程
					led_buf[10].ch = LED_DIS_W;
					led_buf[11].ch = LED_DIS_M;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_C;
				}
				break;
				case 1://上电远控与上电键盘控制选择. 1:上电远控  2:上电键盘控制
				{
					dis_value=m_rPara.para.is_remote;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_S;//上电远控
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_Y;
					led_buf[12].ch = LED_DIS_K;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 2://主电机过载保护停机电流 (额定电流的100%-200%)
				{
					dis_value=m_rPara.para.mm_cur_overflow;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Z;//主电机过载
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_J;
					led_buf[12].ch = LED_DIS_G;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				case 3://是否根据主电机电流进行连锁控制   0:连锁控制  1:不连锁控制
				{
					dis_value=m_rPara.para.mm_control;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_L;//连锁控制
					led_buf[10].ch = LED_DIS_S;
					led_buf[11].ch = LED_DIS_K;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 4://电流传感器量程(10-100A),主电机电流
				{
					dis_value=m_rPara.para.cur1_full;
					//
					led_buf[8].ch = LED_DIS_Z;//主电机电流1
					led_buf[9].ch = LED_DIS_D;
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_1;
				}
				break;
				case 5://设置料位传感器高低料位
				{
					dis_value=0;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_SPACK;
					led_buf[10].ch = LED_DIS_SPACK;
					led_buf[11].ch = LED_DIS_SPACK;
					led_buf[12].ch = LED_DIS_SPACK;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 6://设置料位数据读取端口	  1:AD 2:485 3:LC
				{
					dis_value=m_rPara.para.material_port;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_L;//料位端口
					led_buf[10].ch = LED_DIS_W;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_K;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 7://MB和转速之间的比例关系
				{
					dis_value=m_rPara.para.mb_div;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_M;//MB
					led_buf[10].ch = LED_DIS_B;
					led_buf[11].ch = LED_DIS_SPACK;
					led_buf[12].ch = LED_DIS_SPACK;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				#if __D2_USED__ == TRUE
				case 8://电流传感器量程(10-100A),主电机电流
				{
					dis_value=m_rPara.para.cur2_full;
					//
					led_buf[8].ch = LED_DIS_Z;//主电机电流2
					led_buf[9].ch = LED_DIS_D;
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_2;
				}
				break;
				#endif
				default://退出设置模式,进入工作模式
				{
					TRACE_STRING("jingruguozuomoshi");
					reset_set_status();
				}
				break;
			}
		}
		else if(1==bit_setmode_3)
		{
			//需要密码进入的参数设置
			led_buf[1].ch=LED_DIS_3;
			//
			switch(pagenumber)
			{
				case 0://启动料位值
				{
					dis_value=m_rPara.para.material_start;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Q;//启动料位值
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_L;
					led_buf[12].ch = LED_DIS_W;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				case 1://停机料位值
				{
					dis_value=m_rPara.para.material_stop;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_T;//停机料位值
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_L;
					led_buf[12].ch = LED_DIS_W;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				case 2://电流修正值
				{
					dis_value=m_rPara.para.current1_modify;
					//
					led_buf[8].ch = LED_DIS_D;//电流修正值
					led_buf[9].ch = LED_DIS_L;
					led_buf[10].ch = LED_DIS_X;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_1;
				}
				break;
				case 3://MODBUS通信从机地址
				{
					dis_value=m_rPara.para.addr_modbus;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_C;//从机地址
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 4://MODBUS通信波特率
				{
					dis_value=m_rPara.para.baud_modbus;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_T;//通信波特率
					led_buf[10].ch = LED_DIS_X;
					led_buf[11].ch = LED_DIS_B;
					led_buf[12].ch = LED_DIS_T;
					led_buf[13].ch = LED_DIS_L;
				}
				break;
				case 5://MB输入选择（电平或者脉冲）
				{
					dis_value = m_rPara.para.mb_level_pulse;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_M;//MB输入
					led_buf[10].ch = LED_DIS_B;
					led_buf[11].ch = LED_DIS_S;
					led_buf[12].ch = LED_DIS_R;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 6://DW输入选择（电平或者脉冲）
				{
					dis_value = m_rPara.para.dw_level_pulse;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_D;//DW输入
					led_buf[10].ch = LED_DIS_W;
					led_buf[11].ch = LED_DIS_S;
					led_buf[12].ch = LED_DIS_R;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				#if __D2_USED__ == TRUE
				case 7://电流修正值
				{
					dis_value=m_rPara.para.current2_modify;
					//
					led_buf[8].ch = LED_DIS_D;//电流修正值2
					led_buf[9].ch = LED_DIS_L;
					led_buf[10].ch = LED_DIS_X;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_2;
				}
				break;
				#endif
				default://退出设置模式,进入工作模式
				{
					TRACE_STRING("jingruguozuomoshi2");
					reset_set_status();
				}
				break;
			}
		}
		else
		{
			//设置外部参数
			led_buf[1].ch=LED_DIS_1;
			//
			switch(pagenumber)
			{
				case 0://最高转速(40-300)
				{
					dis_value=m_rPara.para.max_vel;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Z;//最高转速
					led_buf[10].ch = LED_DIS_G;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_S;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 1://最低转速(40-300)
				{
					dis_value=m_rPara.para.min_vel;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Z;//最低转速
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_S;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 2://主电机额定电流(10-100A)
				{
					dis_value=m_rPara.para.mmotor_cur1;
					//
					led_buf[8].ch = LED_DIS_Z;//主额定电流1
					led_buf[9].ch = LED_DIS_E;
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_1;
				}
				break;
				case 3://合轧延时时间(0.2-20s)
				{
					dis_value=m_rPara.para.rollin_delay;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_H;//合轧延时
					led_buf[10].ch = LED_DIS_Z;
					led_buf[11].ch = LED_DIS_Y;
					led_buf[12].ch = LED_DIS_S;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 4://测试数据显示
				{
					//dis_value = eeprom_read_byte_user(&m_ePara[0].para.all_stop_id);
					dis_value = m_rPara.para.all_stop_id;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_C;//测试数据
					led_buf[10].ch = LED_DIS_S;
					led_buf[11].ch = LED_DIS_S;
					led_buf[12].ch = LED_DIS_J;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 5://恢复默认参数
				{
					dis_value=set_default;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_H;//恢复默认值
					led_buf[10].ch = LED_DIS_F;
					led_buf[11].ch = LED_DIS_M;
					led_buf[12].ch = LED_DIS_R;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				#if __D2_USED__ == TRUE
				case 6://主电机额定电流(10-100A)
				{
					dis_value=m_rPara.para.mmotor_cur2;
					//
					led_buf[8].ch = LED_DIS_Z;//主额定电流2
					led_buf[9].ch = LED_DIS_E;
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_2;
				}
				break;
				#endif
				default://退出设置模式,进入工作模式
				{
					TRACE_STRING("jingruguozuomoshi3");
					reset_set_status();
				}
				break;
			}
		}
		//
		led_addr_switch_3bits((unsigned char*)&led_buf[4].ch,dis_value);
	}
}
/////////////////////////////////////////////////////////////////
void led_addr_switch_3bits(unsigned char *p,unsigned short value)
{
	unsigned short m_temp;
	//
	if(0==value)
	{
		p[0]=LED_DIS_SPACK;//不显示
		p[1]=LED_DIS_SPACK;
		p[2]=LED_DIS_0;
	}
	else
	{
		p[0]=value/100;
		m_temp=value%100;
		p[1]=m_temp/10;
		p[2]=m_temp%10;
		//
		if(0 == p[0])
		{
			p[0] = LED_DIS_SPACK;
			//
			if(LED_DIS_0 == p[1])
			{
				p[1] = LED_DIS_SPACK;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////
void led_addr_switch_4bits(unsigned char *p,unsigned short value)
{
	unsigned short m_temp;
	//
	if(0==value)
	{
		p[0]=LED_DIS_SPACK;//不显示
		p[1]=LED_DIS_SPACK;
		p[2]=LED_DIS_SPACK;
		p[3]=LED_DIS_0;
	}
	else
	{
		p[0]=value/1000;
		m_temp=value%1000;
		p[1]=m_temp/100;
		m_temp=m_temp%100;
		p[2]=m_temp/10;
		p[3]=m_temp%10;
		//
		if(LED_DIS_0 == p[0])
		{
			p[0] = LED_DIS_SPACK;
			//
			if(LED_DIS_0 == p[1])
			{
				p[1] = LED_DIS_SPACK;
				//
				if(LED_DIS_0 == p[2])
				{
					p[2] = LED_DIS_SPACK;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////