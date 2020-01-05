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
//avr8_gccҲ����������⣬��һ��ģ���У���ջ�Ϸ�����ڴ���һ��
//���ƣ����嵽�����Ƕ���ûʱ��ȥһһȷ���ˡ�
//�ر�ע����ǣ�����������������������ᱨ����д�봦����������
//ʱ�����Ī����������⡣
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
//�������ʾֵ
//Ϊ��ʾ����֯�������ʾ����ֵ,��ʾ��õ���Щֱֵ�������������ʾ
//���԰�����ܵ�����ֱ����������Ƶ�
void led_display(void)
{
	unsigned short m_vel,m_cur;
	//
	//�ж��������������Ƿ�ﵽͣ��Ҫ��
	if(m_cur_d1 < mm_cur_overflow_stop)
	{
		//������Ļ���ÿ���Զ���һ
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
		//����ģʽ���������ʾ
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
		modbus_status.status.m_rpm = m_vel;//ת��  

        
		//m_vel = 123;//ת��



        led_addr_switch_3bits((unsigned char*)&led_buf[4].ch,m_vel);
		//
		//���������Ҫһֱ��ʾ,���ܼ����ϵ������������ť�Ƿ���,��Ϊ�п�
		//���ڱ�ĵط����������
		m_cur=m_cur_cal;
		//ȷ��С�����λ��
		modbus_status.status.m_cur_i = m_cur;//�г�10��Ч��


        //m_cur = 456;//�г�10��Ч��
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
		led_buf[11].ch = led_buf[4].ch;//ת��
		led_buf[12].ch = led_buf[5].ch;
		led_buf[13].ch = led_buf[6].ch;
		led_buf[8].ch = LED_DIS_R;//R
		led_buf[9].ch = LED_DIS_P;//P
		led_buf[10].ch = LED_DIS_M;//M
		//
		if(1==bit_mmotor)
		{
		led_buf[4].ch = led_buf[3].ch;//����
		led_buf[3].ch = led_buf[2].ch;
		led_buf[2].ch = led_buf[1].ch;	
		}
		else
		{
			led_buf[4].ch = LED_DIS_0;//����
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
		//����ģʽ�µ��������ʾ
		display_page();
		//		
		bit_current_1st = 0;
		bit_current_2nd = 0;
	}
}
/////////////////////////////////////////////////////////////////
//����ҳ����ʾ�˵�ҳ
void display_page(void)
{
	unsigned short dis_value;
	unsigned char i;
	//
	if(1 == bit_monitor_mode)
	{
		//���������ʾģʽ
		dis_value=0;
		led_buf[1].ch=LED_DIS_P;//p
		led_buf[2].ch=LED_DIS_SUB;//-
		led_buf[3].ch=monitor_mode_page+1;
		//
		switch(monitor_mode_page)
		{
			case 0:
			{
				//��ǰ������λ�������źŵ�ѹֵ
				//(m_material_value_monitor/300)*1000/250
				dis_value = m_material_value_monitor/75;//4-20mA
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_L;//��λ��ѹֵ
				led_buf[10].ch = LED_DIS_W;
				led_buf[11].ch = LED_DIS_D;
				led_buf[12].ch = LED_DIS_Y;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			case 1:
			{
				//��ǰ���������������źŵ�ѹֵ
				//(m_cur_monitor*16)/(cur_full*10)+4
				dis_value = (((m_cur_monitor*16)/m_rPara.para.cur1_full)/10) + 4;
				//
				led_buf[8].ch = LED_DIS_D;//������ѹֵ1
				led_buf[9].ch = LED_DIS_L;
				led_buf[10].ch = LED_DIS_D;
				led_buf[11].ch = LED_DIS_Y;
				led_buf[12].ch = LED_DIS_Z;
				led_buf[13].ch = LED_DIS_1;
			}
			break;
			case 2:
			{
				//ι�Ϲ���ǰÿ��������������(MB)
				dis_value = mb_counter_60s;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_W;//ι�Ϲ�����
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_G;
				led_buf[12].ch = LED_DIS_M;
				led_buf[13].ch = LED_DIS_C;
			}
			break;
			case 3:
			{
				//ĥ����ǰÿ��������������(DW)
				dis_value = dw_counter_60s;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_M;//ĥ������
				led_buf[10].ch = LED_DIS_G;
				led_buf[11].ch = LED_DIS_M;
				led_buf[12].ch = LED_DIS_C;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
			case 4:
			{
				//�ۼ�����ʱ��(��ʾΪ������
				dis_value = m_rPara.para.used_hours/24;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_Y;//����ʱ��
				led_buf[10].ch = LED_DIS_X;
				led_buf[11].ch = LED_DIS_S;
				led_buf[12].ch = LED_DIS_J;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
			case 5:
			{
				//����汾��
				dis_value = SOFTWARE_VERSION;//1.0
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_R;//����汾��
				led_buf[10].ch = LED_DIS_J;
				led_buf[11].ch = LED_DIS_B;
				led_buf[12].ch = LED_DIS_B;
				led_buf[13].ch = LED_DIS_H;
			}
			break;
			#if __D2_USED__ == TRUE
			case 6:
			{
				//��ǰ���������������źŵ�ѹֵ
				//(m_cur_monitor*16)/(cur_full*10)+4
				dis_value = (((m_cur_monitor*16)/m_rPara.para.cur2_full)/10) + 4;
				//
				led_buf[8].ch = LED_DIS_D;//������ѹֵ2
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
		//�澯������ʾģʽ
		dis_value=0;
		led_buf[1].ch=LED_DIS_A;//A
		led_buf[2].ch=LED_DIS_SUB;//-
		led_buf[3].ch=alarm_mode_page+1;
		//
		switch(alarm_mode_page)
		{
			case 0:
			{
				//�������������ͣ��
				dis_value = (1==bit_over_i) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_Z;//����������
				led_buf[10].ch = LED_DIS_D;
				led_buf[11].ch = LED_DIS_L;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			case 1:
			{
				//ι�Ϲ�����ͣ��
				dis_value = (1==part_stop_bmb) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_W;//ι�Ϲ�ͣ��
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_G;
				led_buf[12].ch = LED_DIS_T;
				led_buf[13].ch = LED_DIS_J;
			}
			break;
			case 2:
			{
				//��λ���ȶ�����λ��������ʱ���ڴ�Χ������
				dis_value = (1==bit_material_disturbance) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_L;//��λ����
				led_buf[10].ch = LED_DIS_W;
				led_buf[11].ch = LED_DIS_B;
				led_buf[12].ch = LED_DIS_W;
				led_buf[13].ch = LED_DIS_SPACK;
			}
			break;
			case 3:
			{
				//���Ͽڶ�����LW��������ʱ��
				dis_value = (0==blw) ? 1:0; //1:����
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_X;//���Ͽڶ���
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_K;
				led_buf[12].ch = LED_DIS_D;
				led_buf[13].ch = LED_DIS_S;
			}
			break;
			case 4:
			{
				//ι�Ϲ�ת�ٲ����趨��Χ�ڣ�MB���������/3������ߺ����ֵ�ڣ�
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
				led_buf[9].ch = LED_DIS_W;//ι�Ϲ�����
				led_buf[10].ch = LED_DIS_L;
				led_buf[11].ch = LED_DIS_G;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			/*case 5:
			{
				//ĥ����ߺ�������λ��QLû����ʱ��
				dis_value = (1==bql) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_M;//ĥ�������
				led_buf[10].ch = LED_DIS_G;
				led_buf[11].ch = LED_DIS_Z;
				led_buf[12].ch = LED_DIS_G;
				led_buf[13].ch = LED_DIS_Z;
			}
			break;
			case 6:
			{
				//ĥ���ұߺ�������λ(QRû����ʱ)
				dis_value = (1==bqr) ? 1:0;
				//
				led_buf[8].ch = LED_DIS_SPACK;
				led_buf[9].ch = LED_DIS_M;//ĥ���ҹ���
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
		//�����������
		if(pagenumber > 5)
		{
			//�����������,��Ҫ�Ƚ������Ƿ���ȷ
			if(1 == bit_setmode_pwd_time)
			{
				//ʹ��ʱ����������
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
					bit_setmode_delay=1;//������ʱ����
					bit_setmode_pwd_time=0;
					pagenumber=0;
				}
				else
				{
			        TRACE_STRING("bit_setmode_password1");
					reset_set_status();//�˳�����
				}
			}
			else
			{
				//���뱣��������������
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
					bit_setmode_3=1;//������ʱ����
					pagenumber=0;
				}
				else
				{
					TRACE_STRING("bit_setmode_password2");
					reset_set_status();//�˳�����
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
		//�������ý���
		dis_value=0;
		led_buf[2].ch=LED_DIS_SUB;//-
		led_buf[3].ch=pagenumber+1;
		//
		if(1 == bit_setmode_delay)
		{
			//���ÿ��õ�ʱ��
			led_buf[1].ch=LED_DIS_4;
			//
			switch(pagenumber)
			{
				case 0://ʱ������
				{
					if(limited_days > 999) //��Ҫ�޸�
					{
						dis_value = 999;
					}
					else
					{
						dis_value=limited_days;
					}					
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_S;//ʱ������
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
			//�����ڲ�����
			led_buf[1].ch=LED_DIS_2;
			//
			switch(pagenumber)
			{
				case 0://��λ���������������(50%-100%)
				{
					dis_value=m_rPara.para.material_full;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_L;//��λ������
					led_buf[10].ch = LED_DIS_W;
					led_buf[11].ch = LED_DIS_M;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_C;
				}
				break;
				case 1://�ϵ�Զ�����ϵ���̿���ѡ��. 1:�ϵ�Զ��  2:�ϵ���̿���
				{
					dis_value=m_rPara.para.is_remote;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_S;//�ϵ�Զ��
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_Y;
					led_buf[12].ch = LED_DIS_K;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 2://��������ر���ͣ������ (�������100%-200%)
				{
					dis_value=m_rPara.para.mm_cur_overflow;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Z;//���������
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_J;
					led_buf[12].ch = LED_DIS_G;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				case 3://�Ƿ�������������������������   0:��������  1:����������
				{
					dis_value=m_rPara.para.mm_control;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_L;//��������
					led_buf[10].ch = LED_DIS_S;
					led_buf[11].ch = LED_DIS_K;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 4://��������������(10-100A),���������
				{
					dis_value=m_rPara.para.cur1_full;
					//
					led_buf[8].ch = LED_DIS_Z;//���������1
					led_buf[9].ch = LED_DIS_D;
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_1;
				}
				break;
				case 5://������λ�������ߵ���λ
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
				case 6://������λ���ݶ�ȡ�˿�	  1:AD 2:485 3:LC
				{
					dis_value=m_rPara.para.material_port;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_L;//��λ�˿�
					led_buf[10].ch = LED_DIS_W;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_K;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 7://MB��ת��֮��ı�����ϵ
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
				case 8://��������������(10-100A),���������
				{
					dis_value=m_rPara.para.cur2_full;
					//
					led_buf[8].ch = LED_DIS_Z;//���������2
					led_buf[9].ch = LED_DIS_D;
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_2;
				}
				break;
				#endif
				default://�˳�����ģʽ,���빤��ģʽ
				{
					TRACE_STRING("jingruguozuomoshi");
					reset_set_status();
				}
				break;
			}
		}
		else if(1==bit_setmode_3)
		{
			//��Ҫ�������Ĳ�������
			led_buf[1].ch=LED_DIS_3;
			//
			switch(pagenumber)
			{
				case 0://������λֵ
				{
					dis_value=m_rPara.para.material_start;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Q;//������λֵ
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_L;
					led_buf[12].ch = LED_DIS_W;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				case 1://ͣ����λֵ
				{
					dis_value=m_rPara.para.material_stop;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_T;//ͣ����λֵ
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_L;
					led_buf[12].ch = LED_DIS_W;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				case 2://��������ֵ
				{
					dis_value=m_rPara.para.current1_modify;
					//
					led_buf[8].ch = LED_DIS_D;//��������ֵ
					led_buf[9].ch = LED_DIS_L;
					led_buf[10].ch = LED_DIS_X;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_1;
				}
				break;
				case 3://MODBUSͨ�Ŵӻ���ַ
				{
					dis_value=m_rPara.para.addr_modbus;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_C;//�ӻ���ַ
					led_buf[10].ch = LED_DIS_J;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 4://MODBUSͨ�Ų�����
				{
					dis_value=m_rPara.para.baud_modbus;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_T;//ͨ�Ų�����
					led_buf[10].ch = LED_DIS_X;
					led_buf[11].ch = LED_DIS_B;
					led_buf[12].ch = LED_DIS_T;
					led_buf[13].ch = LED_DIS_L;
				}
				break;
				case 5://MB����ѡ�񣨵�ƽ�������壩
				{
					dis_value = m_rPara.para.mb_level_pulse;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_M;//MB����
					led_buf[10].ch = LED_DIS_B;
					led_buf[11].ch = LED_DIS_S;
					led_buf[12].ch = LED_DIS_R;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 6://DW����ѡ�񣨵�ƽ�������壩
				{
					dis_value = m_rPara.para.dw_level_pulse;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_D;//DW����
					led_buf[10].ch = LED_DIS_W;
					led_buf[11].ch = LED_DIS_S;
					led_buf[12].ch = LED_DIS_R;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				#if __D2_USED__ == TRUE
				case 7://��������ֵ
				{
					dis_value=m_rPara.para.current2_modify;
					//
					led_buf[8].ch = LED_DIS_D;//��������ֵ2
					led_buf[9].ch = LED_DIS_L;
					led_buf[10].ch = LED_DIS_X;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_Z;
					led_buf[13].ch = LED_DIS_2;
				}
				break;
				#endif
				default://�˳�����ģʽ,���빤��ģʽ
				{
					TRACE_STRING("jingruguozuomoshi2");
					reset_set_status();
				}
				break;
			}
		}
		else
		{
			//�����ⲿ����
			led_buf[1].ch=LED_DIS_1;
			//
			switch(pagenumber)
			{
				case 0://���ת��(40-300)
				{
					dis_value=m_rPara.para.max_vel;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Z;//���ת��
					led_buf[10].ch = LED_DIS_G;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_S;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 1://���ת��(40-300)
				{
					dis_value=m_rPara.para.min_vel;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_Z;//���ת��
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_Z;
					led_buf[12].ch = LED_DIS_S;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 2://����������(10-100A)
				{
					dis_value=m_rPara.para.mmotor_cur1;
					//
					led_buf[8].ch = LED_DIS_Z;//�������1
					led_buf[9].ch = LED_DIS_E;
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_1;
				}
				break;
				case 3://������ʱʱ��(0.2-20s)
				{
					dis_value=m_rPara.para.rollin_delay;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_H;//������ʱ
					led_buf[10].ch = LED_DIS_Z;
					led_buf[11].ch = LED_DIS_Y;
					led_buf[12].ch = LED_DIS_S;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 4://����������ʾ
				{
					//dis_value = eeprom_read_byte_user(&m_ePara[0].para.all_stop_id);
					dis_value = m_rPara.para.all_stop_id;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_C;//��������
					led_buf[10].ch = LED_DIS_S;
					led_buf[11].ch = LED_DIS_S;
					led_buf[12].ch = LED_DIS_J;
					led_buf[13].ch = LED_DIS_SPACK;
				}
				break;
				case 5://�ָ�Ĭ�ϲ���
				{
					dis_value=set_default;
					//
					led_buf[8].ch = LED_DIS_SPACK;
					led_buf[9].ch = LED_DIS_H;//�ָ�Ĭ��ֵ
					led_buf[10].ch = LED_DIS_F;
					led_buf[11].ch = LED_DIS_M;
					led_buf[12].ch = LED_DIS_R;
					led_buf[13].ch = LED_DIS_Z;
				}
				break;
				#if __D2_USED__ == TRUE
				case 6://����������(10-100A)
				{
					dis_value=m_rPara.para.mmotor_cur2;
					//
					led_buf[8].ch = LED_DIS_Z;//�������2
					led_buf[9].ch = LED_DIS_E;
					led_buf[10].ch = LED_DIS_D;
					led_buf[11].ch = LED_DIS_D;
					led_buf[12].ch = LED_DIS_L;
					led_buf[13].ch = LED_DIS_2;
				}
				break;
				#endif
				default://�˳�����ģʽ,���빤��ģʽ
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
		p[0]=LED_DIS_SPACK;//����ʾ
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
		p[0]=LED_DIS_SPACK;//����ʾ
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