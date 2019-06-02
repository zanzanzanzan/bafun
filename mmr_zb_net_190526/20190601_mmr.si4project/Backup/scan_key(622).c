/*
 * scan_key.c
 *
 * Created: 2014/11/13 18:14:21
 *  Author: xiong
 */ 
/////////////////////////////////////////////////////////////////
#include "typedef_user.h"
#include "core_func.h"
#include "modbus_rtu.h"
/////////////////////////////////////////////////////////////////

//unsigned char 
void deal_key(unsigned char);
void key_long_pressed_handling(unsigned char key_value);
void key_cssd_handling(void);
void key_csqr_handling(void);
void key_up_handling(void);
void key_down_handling(void);
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
unsigned int zan_count = 0;
unsigned int bD2_weiyi = 0;
unsigned int bD3_weiyi = 0;


unsigned int li_he_zha = 0;
unsigned int zuo_gao;
unsigned int zuo_di;


unsigned int you_gao;
unsigned int you_di;

unsigned char canshu_peizhi = 0;






unsigned int zan_count_max =20;
unsigned char monitor_mode_page,alarm_mode_page;
unsigned char key_status[20];
extern unsigned char tempe_flag;
//
#define KEY_ON				0x01
#define KEY_OFF				0x00
//
#define STATUS_ZDJQ_POS     1
#define STATUS_ZD_POS		2
#define STATUS_SD_POS		3
#define STATUS_XL_POS		4
#define STATUS_LHZ_POS		5
#define STATUS_TJ_POS		6
#define STATUS_CSSD_POS		7
#define STATUS_UP_POS		8
#define STATUS_DOWN_POS		9
#define STATUS_QR_POS		0
#define STATUS_YK_POS		10
#define STATUS_JS_POS		11
#define STATUS_ZTJ_POS		12
/////////////////////////////////////////////////////////////////
//��������ܹؼ�,�����������Ľ�����������ʵ��
//10ms
void delay(unsigned int ms)
{
    unsigned int zan = 0;
    for (;ms > 0; ms--)
    {
        for (zan =0; zan <5555;zan++ )
        {
             wdt_reset();//������ѭ����ι��     
        }
    }

}
unsigned char set_date= 0;
void deal_key(unsigned char key_value)
{
    unsigned char zengliang = 5;
   /* 

   if(0xff == key_value)
	{
		key_value_bak.ms=0xffff;
	}
	else if((key_value_bak.mc[0]==key_value) && (key_value_bak.mc[1]==key_value))
	{
		key_long_pressed_handling(key_value);
	}
	else
	*/
	
	{
		//��ⰴ���Ƿ���
	/*	key_value_bak.ms <<= 8;
		key_value_bak.mc[0] = key_value;
		//
		//�ظ����һ�ΰ���
		if(key_value_bak.mc[1] != key_value_bak.mc[0])
		{
			return;
		}
		//
		memset(key_status,KEY_OFF,20);
	*/
		//
		if(1 == bit_remote)
		{
			//Զ��ģʽ
			switch(key_value)
			{
				case KEY_TJ://ͣ��
				{
					key_status[STATUS_TJ_POS] = KEY_ON;
					//
					bit_remote=0;//�˳�Զ��ģʽ,���ı�ĥ������״̬
					modbus_status.status.m_yk = STATUS_OFF;
				}
				break;
				case KEY_JS://ĥ������2014.6.6�޸�Ϊ������
				{
					key_status[STATUS_JS_POS] = KEY_ON;
					//

						bit_lock=1;
					    brolllock = 1;
 						//
						roll_lock();
						bit_led_lz = 1;


				}
				break;

				case KEY_SJ://ĥ������2014.6.6�޸�Ϊ������
				{
					key_status[STATUS_JS_POS] = KEY_ON;
					//
						bit_lock=0;
					    brolllock = 0;
						//
						roll_unlock();

				}
				break;
				
				case KEY_ZTJ://��ͣ��
				{
					key_status[STATUS_ZTJ_POS] = KEY_ON;
					//
					bit_remote=0;//�˳�Զ��ģʽ,��ִ����ͣ��
					modbus_status.status.m_yk = STATUS_OFF;
					all_stop();//Զ��ģʽ��ִ����ͣ��
					m_rPara.para.all_stop_id = 5;
					//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);
				}
				break;
				case KEY_CJ_TEMP://ͣ��
				{
					key_status[STATUS_TJ_POS] = KEY_ON;
					tempe_flag = 1;
					//

				}
				break;
			}
		}
		else
		{
			//����ģʽ,������״̬
			switch(key_value)
			{
				/*case KEY_CSSD://�����趨(�ⲿ)
				{
					key_status[STATUS_CSSD_POS] = KEY_ON;
					//
					key_cssd_handling();
				}
				break;*/
				case KEY_ZDJQD://���������
				{
					key_status[STATUS_ZDJQ_POS] = KEY_ON;
					//
					//li_he_zha = 0;

                    TRACE_STRING("set zhu dian ji qi dong ");
					if(0==bit_mmotor)
					{
                         TRACE_STRING("set zhu dian ji qi dong2 ");
                        roll_out();
						mmotor_start();
					}
				}
				break;
				case KEY_JS://ĥ������2014.6.6�޸�Ϊ������
				{
					key_status[STATUS_JS_POS] = KEY_ON;
					//

					//li_he_zha = 0;

					bit_lock=1;
					brolllock = 1;
					//
					roll_lock();
					bit_led_lz = 1;

				}
				break;

				case KEY_SJ://ĥ������2014.6.6�޸�Ϊ������
				{
					key_status[STATUS_JS_POS] = KEY_ON;
					//
	
					bit_lock=0;
					brolllock = 0;
					//
					roll_unlock();
					bit_led_lz = 0;


				}
				break;
				case KEY_ZDYX://�Զ�����
				{
					key_status[STATUS_ZD_POS] = KEY_ON;
					//
					  
					if(1==bit_mmotor)
					{
						key_auto_run();
						// li_he_zha = 1;
					}
				}
				break;
				case KEY_MGLZHZ://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					if((0==bit_mmotor)||(1==bit_manual))
					{
						li_he_zha = 0;
						if(0==bit_roll_in)
						{
							bit_roll_in=1;
							roll_in();
						}
						else
						{
							bit_roll_in=0;
							roll_out();
						}
					}
				}
				break;
                // add by zan
                case KEY_ZUO_WEI_JIA://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
					zan_count  =0;
					//if((0==bit_mmotor)||(1==bit_manual))
					{
                        /*
                        brollin = 1;
                        //while (zan_count < zan_count_max);

                        delay(zan_count_max);
                        brollin = 0;
                        */
                        if (li_he_zha == 0)
                        {
                            zuo_di +=zengliang;
                            if (zuo_di >1024)
                            {
                                zuo_di = 1024;
                            }
                        }
                        else
                        {
                            zuo_gao +=zengliang;
                            if (zuo_gao >1024)
                            {
                                zuo_gao = 1024;
                            }
                        }
					}
				}
				break;
                case KEY_ZUO_WEI_JIAN://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					zan_count  =0;
					//set_idle_wait = 0;
					//if((0==bit_mmotor)||(1==bit_manual))
					{
                        /*
                        brolllock = 1;
                        //while (zan_count < zan_count_max)
                        delay(zan_count_max);
                        brolllock = 0;*/
                        if (li_he_zha == 0)
                        {
                            zuo_di -=zengliang;
                            if (zuo_di < 20)
                            {
                                zuo_di = 20;
                            }
                        }
                        else
                        {
                            zuo_gao -=zengliang;
                            if (zuo_gao  < 20)
                            {
                                zuo_gao = 20;
                            }
                        }
                        
					}
				}
				break;
                case KEY_YOU_WEI_JIA://ĥ������2014.6.6�޸�Ϊ���բ����
				{
                    zan_count  =0;
                    key_status[STATUS_LHZ_POS] = KEY_ON;
					//set_idle_wait = 0;;
					//
					//if((0==bit_mmotor)||(1==bit_manual))
					{
					    /*bmbby = 1;
                        //while (zan_count < zan_count_max)
              
                        delay(zan_count_max);
                        bmbby = 0;*/
                        if (li_he_zha == 0)
                        {
                            you_di +=zengliang;
                            if (you_di >1024)
                            {
                                you_di = 1024;
                            }
                        }
                        else
                        {
                            you_gao +=zengliang;
                            if (you_gao >1024)
                            {
                                you_gao = 1024;
                            }
                        }
					}
				}
				break;
                case KEY_YOU_WEI_JIAN://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					zan_count  =0;
					//set_idle_wait = 0;
					//if((0==bit_mmotor)||(1==bit_manual))
					{
                        /*bmmstop = 1;
                        //while (zan_count < zan_count_max)
                                               
                        delay(zan_count_max);
                        bmmstop = 0;*/
                        if (li_he_zha == 0)
                        {
                            you_di -=zengliang;
                            if (you_di < 20)
                            {
                                you_di = 20;
                            }
                        }
                        else
                        {
                            you_gao -=zengliang;
                            if (you_gao  < 20)
                            {
                                you_gao = 20;
                            }
                        }
					}
				}
				break;


                case KEY_ZUO_LING://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//set_idle_wait = 0;
					//

                    m_rPara.para.zuo_ling = zuo_di;//�õ�����ֵ
                    //zuo_di = bD2_weiyi;
					eeprom_write_word_user((uint16_t *)&m_ePara[0].para.zuo_ling,m_rPara.para.zuo_ling);

                   TRACE_STRING("set zuoling  =");
                   send_ulong_ascii(m_rPara.para.zuo_ling  ,TRUE);

				}
				break;


                case KEY_ZUO_QI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                    m_rPara.para.zuo_qi =  zuo_gao;//�õ�����ֵ
                    //zuo_gao = bD2_weiyi;
					eeprom_write_word_user((uint16_t *)&m_ePara[0].para.zuo_qi,m_rPara.para.zuo_qi);

                   TRACE_STRING("set zuoqi  =");
                   send_ulong_ascii(m_rPara.para.zuo_qi ,TRUE);

				}
				break;

                 case KEY_YOU_LING://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                    m_rPara.para.you_ling =  you_di;//�õ�����ֵ
                    //you_di = bD3_weiyi;
					eeprom_write_word_user((uint16_t *)&m_ePara[0].para.you_ling,m_rPara.para.you_ling);

                   TRACE_STRING("set youling  =");
                   send_ulong_ascii(m_rPara.para.you_ling ,TRUE);

				}
				break;


                case KEY_YOU_QI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					
					//set_idle_wait = 0;

                    m_rPara.para.you_qi = you_gao;//�õ�����ֵ
                    //you_gao = bD3_weiyi;
					eeprom_write_word_user((uint16_t *)&m_ePara[0].para.you_qi,m_rPara.para.you_qi);

                   TRACE_STRING("set youqi  =");
                   send_ulong_ascii(m_rPara.para.you_qi ,TRUE);

				}
				break;  

                
                case KEY_HE_ZHA://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
                    //

                    li_he_zha = 1;
					bit_led_hz = 1;

					//set_idle_wait = 0;


				}
				break; 

                case KEY_LI_ZHA://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
                    li_he_zha = 0;
					bit_led_hz = 0;
					//set_idle_wait = 0;

				}
				break; 

                case TEST_BUTTON://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;          		//

                    		uart_tx_buf[2] = uart_rx_buf[4];

                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;




				}
				break;





                //eeprom_write_word_user((uint16_t *)&m_ePara[0].para.max_vel,m_rPara.para.max_vel);
//SET DATE
                case SET_MAX_ZHUANSU://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
							if ( (uart_rx_buf[4]*256 + uart_rx_buf[5])  > m_rPara.para.min_vel)

							{
	                            m_rPara.para.max_vel = uart_rx_buf[4]*256 + uart_rx_buf[5];
	                            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.max_vel,m_rPara.para.max_vel);
	                            uart_tx_buf[0] = 0xaa;
	                    		uart_tx_buf[1] = 5;
	                    		uart_tx_buf[2] = uart_rx_buf[4];
	                    		uart_tx_buf[3] = uart_rx_buf[5];
	                    		uart_tx_buf[4] = 0;
	                    		uart_tx_buf[5] = 0;
	                    		uart_tx_buf[6] = 0;
	                            set_date = 0;
							}
				}
				break;
                case SET_MIN_ZHUANSU://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
							if ( (uart_rx_buf[4]*256 + uart_rx_buf[5])   <  m_rPara.para.max_vel)
							{
								
                            m_rPara.para.min_vel = uart_rx_buf[4]*256 + uart_rx_buf[5];
                            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.min_vel,m_rPara.para.min_vel);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
							
							}
							
				}
				break;
                
                case SET_ZHUDIANJI_DIANLIU://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
	

                            m_rPara.para.mmotor_cur1 = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur1,m_rPara.para.mmotor_cur1);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;

						   

				}
				break;

                case HZ_YANCHISHIJIAN://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.rollin_delay = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.rollin_delay,m_rPara.para.rollin_delay);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                case SET_QD_LIAOWEI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.material_start = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.material_start,m_rPara.para.material_start);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                case SET_TJ_LIAOWEI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.material_stop = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.material_stop,m_rPara.para.material_stop);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;



                case SET_MB_XINHAO://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.mb_level_pulse = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mb_level_pulse,m_rPara.para.mb_level_pulse);
			
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;
				//TEMP_CHANGE_TEMP
				case TEMP_CHANGE_TEMP://�¶��л�
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					
					if (uart_rx_buf[4] == 0x1)
					{
						//��һ���¶�
						set_date =1;
					}else if (uart_rx_buf[4] == 0x2) 
					{
						//�ڶ����¶�
						set_date= 2;
					}
					else
					{
						//����ʾ�¶�
						set_date =0;

					}
					//set_date =1;
				}
				break;
				
				case SET_DIV://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.mb_div = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mb_div,m_rPara.para.mb_div);
			
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;
				case SET_DW_XINHAO://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.dw_level_pulse = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.dw_level_pulse,m_rPara.para.dw_level_pulse);
			
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                case SET_DIANLIU_XIUZHENG://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
					TRACE_STRING("dianliu xiuzheng1");


					m_rPara.para.current1_modify = uart_rx_buf[4] *256 + uart_rx_buf[5];
					eeprom_write_byte_user(&m_ePara[0].para.current1_modify,m_rPara.para.current1_modify);


                           
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
					TRACE_STRING("dianliu xiuzheng2");
				}
				break;

			    case SET_DIANLIU_XIUZHENG1://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
					TRACE_STRING("dianliu xiuzheng1");


					m_rPara.para.current2_modify = uart_rx_buf[4] *256 + uart_rx_buf[5];
					eeprom_write_byte_user(&m_ePara[0].para.current2_modify,m_rPara.para.current2_modify);


                           
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
					TRACE_STRING("dianliu xiuzheng2");
				}
				break;

                case SET_CONGJI_DIZHI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.addr_modbus = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.addr_modbus,m_rPara.para.addr_modbus);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                case SET_SHAGNDIAN_ZHUANGTAI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.is_remote = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.is_remote,m_rPara.para.is_remote);

                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                case SET_DIANLIU_GUOZAI_TINGJI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.mm_cur_overflow = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mm_cur_overflow,m_rPara.para.mm_cur_overflow);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

			    case SET_DIANLIU_GUOZAI_TINGJI1://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.mm_cur_overflow = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mm_cur_overflow,m_rPara.para.mm_cur_overflow);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                 case SET_DIANLIU_LIANSUO://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.mm_control = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mm_control,m_rPara.para.mm_control);
							set_date = 0;
                           /* uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;*/
				}
				break;

                case SET_DIANLIU_LIANGCHENG://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.cur1_full = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.cur1_full,m_rPara.para.cur1_full);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

				case SET_DIANLIU_LIANGCHENG1://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.cur2_full = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.cur2_full,m_rPara.para.cur2_full);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
				}
				break;

                case SET_DIANLIU_BIAODING://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					set_date = 0;
					//
					//set_idle_wait = 0;
					/*	
                            m_rPara.para.current1_modify = uart_rx_buf[3] *256 + uart_rx_buf[4];
                            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.current1_modify,m_rPara.para.current1_modify);



                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[3];
                    		uart_tx_buf[3] = uart_rx_buf[4];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;*/
				}
				break;
                case HZ_SET_ZHANJU://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					unsigned short  zhaju;
					key_status[STATUS_LHZ_POS] = KEY_ON;

                    zhaju = uart_rx_buf[4] *256 + uart_rx_buf[5];

					if (uart_rx_buf[6] == 0)
					{
						zuo_di = zhaju;
						
                   		TRACE_STRING("set zuolingzhi");
                  
					}
					else if (uart_rx_buf[6] == 1)
					{
						zuo_gao= zhaju;
						TRACE_STRING("set zuoqizhi");
					}
					else if(uart_rx_buf[6] == 2)
					{
						you_di = zhaju;
						TRACE_STRING("set youklingzhi");
					}
					else if(uart_rx_buf[6] == 3)
					{
						you_gao = zhaju;
						TRACE_STRING("set youkqizhi");
					}	


					send_ulong_ascii(zhaju ,TRUE);

                    uart_tx_buf[0] = 0xaa;
            		uart_tx_buf[1] = 5;
            		uart_tx_buf[2] = uart_rx_buf[4];
            		uart_tx_buf[3] = uart_rx_buf[5];
            		uart_tx_buf[4] = 0;
            		uart_tx_buf[5] = 0;
            		uart_tx_buf[6] = 0;
                    set_date = 0;
				}
				break;


                case SET_ZHUANSU_JIA://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.manual_vel += 5;
							set_date = 0;

                    
                           
                            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.manual_vel,m_rPara.para.manual_vel);
 				}
				break;

                case SET_ZHUANSU_JIAN://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                            m_rPara.para.manual_vel -= 5;
							set_date = 0;
                           
                            eeprom_write_word_user((uint16_t *)&m_ePara[0].para.manual_vel,m_rPara.para.manual_vel);
 				}
				break;


                case SET_RIQI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;


                            if ((uart_rx_buf[4] == 0) && (uart_rx_buf[5] == 8) )
                            {
                                set_default_para();
                            }

                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;
 				}
				break;



				case SET_CANSHU_DISP://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;


					canshu_peizhi = 0;
					set_date = 0;

				}
				break;
				
					
				case SET_YUZHI_SHEZHI://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                    m_rPara.para.set_yuzhi = uart_rx_buf[4] *256 + uart_rx_buf[5];
                    eeprom_write_byte_user(&m_ePara[0].para.set_yuzhi,m_rPara.para.set_yuzhi);
                    uart_tx_buf[0] = 0xaa;
            		uart_tx_buf[1] = 5;
            		uart_tx_buf[2] = uart_rx_buf[4];
            		uart_tx_buf[3] = uart_rx_buf[5];
            		uart_tx_buf[4] = 0;
            		uart_tx_buf[5] = 0;
            		uart_tx_buf[6] = 0;
                    set_date = 0;

				
				}
				break;

				case SET_IP_ADDR://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                    //m_rPara.para.set_yuzhi = uart_rx_buf[4] *256 + uart_rx_buf[5];
                    //eeprom_write_byte_user(&m_ePara[0].para.set_yuzhi,m_rPara.para.set_yuzhi);
                    uart_tx_buf[0] = 0xaa;
            		uart_tx_buf[1] = 5;
            		uart_tx_buf[2] = uart_rx_buf[3];  //ip_addrs
            		uart_tx_buf[3] = uart_rx_buf[4];
            		uart_tx_buf[4] = uart_rx_buf[5];
            		uart_tx_buf[5] = uart_rx_buf[6];
            		uart_tx_buf[6] = 0;
                    set_date = 0;



					unsigned char i;
					for(i=0; i<4; i++)
					{
						//IP��ַд��EEPROM
						eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.ip_addr[i],uart_tx_buf[2 + i]);
						if (i == 3)
						{
								eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.gateway[i],1);
						}
						else
						{
								eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.gateway[i],uart_tx_buf[2 + i]);
						}
						//eeprom_write_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.net_mask[i],p->para.w5500_net_para.ip_para.net_mask[i]);
					}
					//TRACE_STRING("para.w5500_net_para.ip_para.ip_addr");

					#if USED_W5500 == TRUE
					{
						wdt_reset();//������ѭ����ι��
						//��ʼ������оƬ��ָ����ʱʱ��(ms)		
						w5500_read_para();
						if(TRUE == init_w5500(10)) //100
						{
							TRACE_STRING("w5500 set ok\r\n\0");
						}
						else
						{
							TRACE_STRING("w5500 set fail\r\n\0");
						}
					}
					#endif



					
					//while(1);
				
				}
				break;

				case SET_SLAVE_USART_ADDR://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                    m_rPara.para.addr_modbus = uart_rx_buf[5];
                    eeprom_write_byte_user(&m_ePara[0].para.addr_modbus,m_rPara.para.addr_modbus);
                    uart_tx_buf[0] = 0xaa;
            		uart_tx_buf[1] = 5;
            		uart_tx_buf[2] = uart_rx_buf[4];
            		uart_tx_buf[3] = uart_rx_buf[5];
            		uart_tx_buf[4] = 0;
            		uart_tx_buf[5] = 0;
            		uart_tx_buf[6] = 0;
                    set_date = 0;

				
				}
				break;

				
				case SET_SLAVE_USART_COMM://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;

                    m_rPara.para.set_yuzhi = uart_rx_buf[4] *256 + uart_rx_buf[5];
                    eeprom_write_byte_user(&m_ePara[0].para.set_yuzhi,m_rPara.para.set_yuzhi);
                    uart_tx_buf[0] = 0xaa;
            		uart_tx_buf[1] = 5;
            		uart_tx_buf[2] = uart_rx_buf[4];
            		uart_tx_buf[3] = uart_rx_buf[5];
            		uart_tx_buf[4] = 0;
            		uart_tx_buf[5] = 0;
            		uart_tx_buf[6] = 0;
                    set_date = 0;

				
				}
				break;

                case SET_ZHUDIANJI_DIANLIU1://ĥ������2014.6.6�޸�Ϊ���բ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;
					//
					//set_idle_wait = 0;
	

                            m_rPara.para.mmotor_cur2 = uart_rx_buf[4] *256 + uart_rx_buf[5];
                            eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur2,m_rPara.para.mmotor_cur2);
                            uart_tx_buf[0] = 0xaa;
                    		uart_tx_buf[1] = 5;
                    		uart_tx_buf[2] = uart_rx_buf[4];
                    		uart_tx_buf[3] = uart_rx_buf[5];
                    		uart_tx_buf[4] = 0;
                    		uart_tx_buf[5] = 0;
                    		uart_tx_buf[6] = 0;
                            set_date = 0;

						   

				}
				break;


                
				case KEY_TJ://ͣ��
				{
					key_status[STATUS_TJ_POS] = KEY_ON;
					//
					if(0==bit_setmode)
					{
						part_stop();
						li_he_zha = 0;
					}
					set_date = 0;
				}
				break;

				case KEY_CJ_TEMP://ͣ��
				{
					key_status[STATUS_TJ_POS] = KEY_ON;
					tempe_flag = 1;
					set_date = 0;
					bit_remote=0;
					//

				}
				break;
				
				case KEY_SDYX://�ֶ�����
				{ 
					key_status[STATUS_SD_POS] = KEY_ON;
					set_date = 0;
					//
					if(1==bit_mmotor)
					{
						key_manual_run();
						//li_he_zha = 1;
					}
					set_date = 0;
				}
				break;
				case KEY_ZTJ://��ͣ��
				{
					key_status[STATUS_ZTJ_POS] = KEY_ON;
					//
					li_he_zha = 0;
					bit_remote=0;
					all_stop();//����ִ����ͣ��
					m_rPara.para.all_stop_id = 6;
					//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);
					set_date = 0;
				}
				break;
				case KEY_XL://ж��
				{
					key_status[STATUS_XL_POS] = KEY_ON;
					//
					//���Զ����ֶ�ģʽ�²����л���ж��
					if((0==bit_auto)&&(0==bit_manual))
					{
						fmotor_start();
						bit_xieliao=1;
					}
					set_date = 0;
				}
				break;
				case KEY_YCKZ://Զ�̿���
				{
					key_status[STATUS_YK_POS] = KEY_ON;
					//
					if (KEY_YCKZ == uart_rx_buf[3])
					{
						bit_remote=1;
						modbus_status.status.m_yk = STATUS_ON;
						bit_manual=0;
						bit_xieliao=0;
						enable_mb_delay=0;
						set_date = 0;
					}
				}
				break;
				case KEY_DOWN:
				{
					key_status[STATUS_DOWN_POS] = KEY_ON;
					//
					key_down_handling();
				}
				break;
				case KEY_UP:
				{
					key_status[STATUS_UP_POS] = KEY_ON;
					//
					key_up_handling();
				}
				break;
				case KEY_CSQR://����ȷ��
				{
					key_status[STATUS_QR_POS] = KEY_ON;
					//
					key_csqr_handling();
				}
				break;
				case SET_LIAOWEI_VALUE://��λ����ģʽ����
				{
					key_status[STATUS_LHZ_POS] = KEY_ON;

					/***
					m_rPara.para.mmotor_cur2 = uart_rx_buf[4] *256 + uart_rx_buf[5];
					eeprom_write_byte_user(&m_ePara[0].para.mmotor_cur2,m_rPara.para.mmotor_cur2);
					uart_tx_buf[0] = 0xaa;
					
					
					1  ����ת�ٳ�ʼֵ(��ͺ����ת�ٵĳ�ʼֵ)
					2  �����л�����(Ĭ��ֵԭ�����)
					3  ��������������(���ת��-���ת��/12)
					4  ��������ֵ����(12MMA)
					


					*/

					
					if (uart_rx_buf[3] == 0x1)
					{
						//Ĭ��ת��֮��
						if (uart_rx_buf[5] > m_rPara.para.min_vel 
							&&  uart_rx_buf[5] > m_rPara.para.max_vel)
						{
							m_rPara.para.set_default_zhuansu =  uart_rx_buf[5];
							eeprom_write_byte_user(&m_ePara[0].para.set_default_zhuansu,m_rPara.para.set_default_zhuansu);	

						}

						
					}else if (uart_rx_buf[3] == 0x2)
					{
						//ģʽ
						if (uart_rx_buf[5] == 0
							||  uart_rx_buf[5]  == 1)
						{
							m_rPara.para.set_model = uart_rx_buf[5];
							eeprom_write_byte_user(&m_ePara[0].para.set_model,m_rPara.para.set_model);	

						}

						
					}else if (uart_rx_buf[3] == 0x3)
					{
						//��������
						if (uart_rx_buf[5]  > 0
							&&  uart_rx_buf[5] < 255)
						{
							m_rPara.para.set_wuliao_PID_P = uart_rx_buf[5];
							eeprom_write_byte_user(&m_ePara[0].para.set_wuliao_PID_P,m_rPara.para.set_wuliao_PID_P);	

						}

						
					}else if (uart_rx_buf[3] == 0x4)
					{
						if (uart_rx_buf[5]	> 4
							&&	uart_rx_buf[5] < 20)
						{
							m_rPara.para.set_qiwang_value = uart_rx_buf[5];
							eeprom_write_byte_user(&m_ePara[0].para.set_qiwang_value,m_rPara.para.set_qiwang_value);	
						
						}
						
					}
					
					//set_date =1;
				}
				break;
			}
			//�������ʱ�䳬��1s��,˵���ղŰ��ļ��϶�����������ģʽ�µİ���
			if(set_idle_wait > 1)
			{
				TRACE_STRING("jingruguozuomoshi8");
				reset_set_status();
				//
				bit_monitor_mode = 0;
				bit_alarm_mode = 0;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////
void key_long_pressed_handling(unsigned char key_value)
{
	//ͬһ��������ʱ�䰴�£��ڸô�����
	//
	unsigned char key_value_set;
	//
	if((KEY_UP==key_value) || (KEY_DOWN==key_value))
	{
		//ͬһ�������ظ�ִ�еļ��ʱ��
		if(1 == bit_setmode_password)
		{
			//�������ù����У���һ���߼�һ���Ϊ250ms
			key_value_set=25;
		}
		if((1==bit_monitor_mode) || (1==bit_alarm_mode))
		{
			key_value_set = 100;
		}
		else
		{
			//��������������ֶ�ת�ٲ����������ʱ��Ϊ125ms
			key_value_set=13;
		}
		//
		//key_delayһ��ֵ����10ms
		if(key_delay > key_value_set)
		{
			//�����ֵ��������һ�ΰ���ȷ��
			key_delay = 0;
			key_value_bak.ms=0xffff;
		}
	}
	else /*if(KEY_CSSD==key_value)
	{
		//���������������
		if((boot_time-long_pressed_cssd_delay) > 5000UL)//5s
		{
			//���������������趨��5�����ϣ������������ý���
			bit_setmode=1;
			bit_setmode_password=1;
			bit_setmode_pwd_time=1;
			pagenumber=0;
			password[0]=0;
			password[1]=0;
			password[2]=0;
			password[3]=0;
			password[4]=0;
			password[5]=0;
		}
	}
	else */if((KEY_CSQR == key_value) && (0 == bit_setmode))
	{
		//�ڷ�����ģʽ�³���CSQR�����ȴ�����������ģʽ
		if((boot_time-long_pressed_csqr_delay) > 3000UL)//3s
		{
			bit_monitor_mode = 1;
			bit_alarm_mode = 0;
			monitor_mode_page = 0;
			alarm_mode_page = 0;
			//
			set_idle_wait=0;
			long_pressed_csqr_delay = boot_time;
		}
	}
	else
	{
		key_delay=0;
		key_delay_set_in=0;
	}
}
/////////////////////////////////////////////////////////////////
void key_cssd_handling(void)
{
	//���������趨�������ν��벻ͬ�Ĳ����趨���
	if(1 == arg_set_process)
	{
		//�ڲ��������ã�Ҳ���ǲ������޸ĵĲ�������ҪС��
		TRACE_STRING("jingruguozuomoshi_canshuqueren");
		reset_set_status();
		arg_set_process = 2;//�ڲ�����
		bit_setmode=1;
		bit_setmode_2=1;
		long_pressed_cssd_delay = boot_time;
	}
	else if(2 == arg_set_process)
	{
		//�Կ���������Ӱ��ϴ�Ĳ����趨�������ɹ���Ա����
		TRACE_STRING("jingruguozuomoshi_canshuqueren2");
		reset_set_status();
		arg_set_process = 3;//��Ҫ�������Ĳ���
		bit_setmode_password=1;
		bit_setmode_pwd_time=0;
		bit_setmode=1;
		password[0]=2;
		password[1]=2;
		password[2]=2;
		password[3]=2;
		password[4]=2;
		password[5]=2;
		long_pressed_cssd_delay = boot_time;
	}
	else
	{
		//�ⲿ�������ã�Ҳ������Ҫ�����޸ĵĲ�������
		TRACE_STRING("jingruguozuomoshi_canshuqueren3");
		reset_set_status();
		arg_set_process=1;//�ⲿ����
		bit_setmode_1=1;
		bit_setmode=1;
		long_pressed_cssd_delay = boot_time;
	}
}
/////////////////////////////////////////////////////////////////
void key_csqr_handling(void)
{
	if(1 == bit_setmode)
	{
		//��������ģʽ��
		if(0 == bit_setmode_password)
		{
			//���ֲ���
			save_para();
		}
		//
		set_idle_wait=0;
		pagenumber++;//��һҳ���ã�����ʾ��ʱ���ж��Ƿ��˳�����ģʽ
	}
	else if(1 == bit_monitor_mode)
	{
		//�������ģʽ�£�����CSQR�����˳����ģʽ����澯����ģʽ
		bit_monitor_mode = 0;
		bit_alarm_mode = 1;
		monitor_mode_page = 0;
		set_idle_wait=0;
	}
	else if(1 == bit_alarm_mode)
	{
		//�澯��Ϣģʽ�£�����CSQR�����˳��澯ģʽ
		bit_monitor_mode = 0;
		bit_alarm_mode = 0;
		alarm_mode_page = 0;
		set_idle_wait=0;
	}
	//
	long_pressed_csqr_delay = boot_time;
}
/////////////////////////////////////////////////////////////////
void key_up_handling(void)
{
	//���ϰ����ڹ���ģʽ������ģʽ�¶���Ҫ�õ��������Ҫ�ֱ���
	//
	if(1 == bit_monitor_mode)
	{
		//���������ʾģʽ
		set_idle_wait=0;
		if(monitor_mode_page < 5)
		{
			monitor_mode_page++;
		}
		else
		{
			monitor_mode_page = 0;
		}
	}
	else if(1 == bit_alarm_mode)
	{
		//�澯������ʾģʽ
		set_idle_wait=0;
		if(alarm_mode_page < 6)
		{
			alarm_mode_page++;
		}
		else
		{
			alarm_mode_page = 0;
		}
	}
	else if(0 == bit_setmode)
	{
		if(1==bit_manual)
		{
			m_rPara.para.manual_vel++;
			if(m_rPara.para.manual_vel > m_rPara.para.max_vel)
			{
				m_rPara.para.manual_vel=m_rPara.para.max_vel;
			}
			eeprom_write_word_user((uint16_t *)&m_ePara[0].para.manual_vel,m_rPara.para.manual_vel);
		}
		#if __TEST_USED__ == TRUE
		else if(1==bit_auto)
		{
			if(bpwma < 1000)
			{
				bpwma++;
			}
		}
		#endif
	}
	else
	{
		//����ģʽ��
		set_idle_wait=0;
		if(1 == bit_setmode_2)
		{
			//�ڲ�����
			switch(pagenumber)
			{
				case 0://��λ���������������(50%-100%)
				{
						m_rPara.para.material_full=100;
				}
				break;
				case 1://�ϵ�Զ�����ϵ���̿���ѡ��. 1:�ϵ�Զ��  2:�ϵ���̿���
				{
					if(1==m_rPara.para.is_remote)
					{
						m_rPara.para.is_remote=2;
					}
					else
					{
						m_rPara.para.is_remote=1;
					}
				}
				break;
				case 2://��������ر���ͣ������ (�������100%-200%)
				{
					m_rPara.para.mm_cur_overflow++;
					if(m_rPara.para.mm_cur_overflow > 200)
					{
						m_rPara.para.mm_cur_overflow=200;
					}
				}
				break;
				case 3://�Ƿ�������������������������   0:��������  1:����������
				{
					if(0==m_rPara.para.mm_control)
					{
						m_rPara.para.mm_control=1;
					}
				}
				break;
				case 4://��������������(10-100A),���������
				{
					m_rPara.para.cur1_full++;
					if(m_rPara.para.cur1_full > FULL_CURRENT)
					{
						m_rPara.para.cur1_full=FULL_CURRENT;
					}
				}
				break;
				case 5://����λ
				{
					m_cgq_cmd = GAOLIAOWEI;
				}
				break;
				case 6://ѡ����λ�ɼ��ź�    1��485+AD 2:485 3��LC�����ɴ������� 4:AD
				{
					if(m_rPara.para.material_port < MATERIAL_PORT_ONLY_AD)
					{
						m_rPara.para.material_port++;
					}
					else
					{
						m_rPara.para.material_port = MATERIAL_PORT_RS485_AD;
					}
				}
				break;
				case 7://MB��ת��֮��Ĺ�ϵ
				{
					if(m_rPara.para.mb_div < 99)
					{
						m_rPara.para.mb_div++;
					}
					else
					{
						m_rPara.para.mb_div = 1;
					}
				}
				break;
				#if __D2_USED__ == TRUE
				case 8://��������������(10-100A),���������
				{
					m_rPara.para.cur2_full++;
					if(m_rPara.para.cur2_full > FULL_CURRENT)
					{
						m_rPara.para.cur2_full=FULL_CURRENT;
					}
				}
				break;
				#endif
			}
		}
		else if(1==bit_setmode_password)
		{
			//��������
			password[pagenumber]++;
			if(password[pagenumber] > 9)
			{
				password[pagenumber]=9;
			}
		}
		else if(1==bit_setmode_delay)
		{
			switch(pagenumber)
			{
				case 0:
				{
					//��ʱ����
					//��ʱʱ��,��
					limited_days++;
					if(limited_days > LIMITED_DAYS_MAX)
					{
						limited_days = LIMITED_DAYS_MAX;
					}
				}
				break;
			}
		}
		else if(1==bit_setmode_3)
		{
			//���뱣���Ĳ���
			switch(pagenumber)
			{
				case 0://������λֵ
				{
					m_rPara.para.material_start++;
					if(m_rPara.para.material_start > 100)
					{
						m_rPara.para.material_start=100;
					}
				}
				break;
				case 1://ͣ����λֵ
				{
					m_rPara.para.material_stop++;
					if(m_rPara.para.material_stop > 150)
					{
						m_rPara.para.material_stop=150;
					}
				}
				break;
				case 2://��������ֵ
				{
					m_rPara.para.current1_modify++;
					if(m_rPara.para.current1_modify > 130)
					{
						m_rPara.para.current1_modify=130;
					}
				}
				break;
				case 3://MODBUS�ӻ���ַ
				{
					if(m_rPara.para.addr_modbus < 254)
					{
						m_rPara.para.addr_modbus++;
					}
				}
				break;
				case 4://MODBUSͨ�Ų�����
				{
					if(m_rPara.para.baud_modbus < MODBUS_BAUD_38400)
					{
						m_rPara.para.baud_modbus++;
					}
					else
					{
						m_rPara.para.baud_modbus = MODBUS_BAUD_9600;
					}
				}
				break;
				case 5:
				{
					//MB����ѡ��1����ƽ��2�����壩
					if(MB_LEVEL == m_rPara.para.mb_level_pulse)
					{
						m_rPara.para.mb_level_pulse = MB_PULSE;
					}
					else
					{
						m_rPara.para.mb_level_pulse = MB_LEVEL;
					}
				}
				break;
				case 6:
				{
					//DW����ѡ��1����ƽ��2�����壩
					if(DW_LEVEL == m_rPara.para.dw_level_pulse)
					{
						m_rPara.para.dw_level_pulse = DW_PULSE;
					}
					else
					{
						m_rPara.para.dw_level_pulse = DW_LEVEL;
					}
				}
				break;
				#if __D2_USED__ == TRUE
				case 7://��������ֵ
				{
					m_rPara.para.current2_modify++;
					if(m_rPara.para.current2_modify > 130)
					{
						m_rPara.para.current2_modify=130;
					}
				}
				break;
				#endif
			}
		}
		else
		{
			//�ⲿ����
			switch(pagenumber)
			{
				case 0://���ת��(40-300)
				{
					m_rPara.para.max_vel++;
					if(m_rPara.para.max_vel > MAX_VEL)
					{
						m_rPara.para.max_vel=MAX_VEL;
					}
				}
				break;
				case 1://���ת��(40-300)
				{
					m_rPara.para.min_vel++;
					if(m_rPara.para.min_vel > MAX_VEL)
					{
						m_rPara.para.min_vel=MAX_VEL;
					}
				}
				break;
				case 2://����������(10-100A)
				{
					m_rPara.para.mmotor_cur1++;
					if(m_rPara.para.mmotor_cur1 > 100)
					{
						m_rPara.para.mmotor_cur1=100;
					}
				}
				break;
				case 3://������ʱʱ��(0.2-20s)
				{
					m_rPara.para.rollin_delay++;
					if(m_rPara.para.rollin_delay > 200)
					{
						m_rPara.para.rollin_delay=200;
					}
				}
				break;
				case 4:
				{
					//��ͣ����־����
				}
				break;
				case 5://�ָ�Ĭ�ϲ���
				{
					if(set_default < 99)
					{
						set_default++;
					}
					else
					{
						set_default = 1;
					}
				}
				break;
				#if __D2_USED__ == TRUE
				case 6://����������(10-100A)
				{
					m_rPara.para.mmotor_cur2++;
					if(m_rPara.para.mmotor_cur2 > 100)
					{
						m_rPara.para.mmotor_cur2=100;
					}
				}
				break;
				#endif 
			}
		}
	}
}
/////////////////////////////////////////////////////////////////
void key_down_handling(void)
{
	//���°����ڹ���ģʽ������ģʽ�¶���Ҫ�õ��������Ҫ�ֱ���
	//
	if(1 == bit_monitor_mode)
	{
		//���������ʾģʽ
		set_idle_wait=0;
		if(monitor_mode_page > 0)
		{
			monitor_mode_page--;
		}
		else
		{
			monitor_mode_page = 5;
		}
	}
	else if(1 == bit_alarm_mode)
	{
		//�澯������ʾģʽ
		set_idle_wait=0;
		if(alarm_mode_page > 0)
		{
			alarm_mode_page--;
		}
		else
		{
			alarm_mode_page = 6;
		}
	}
	else if(0 == bit_setmode)
	{
		//����ģʽ
		if(1==bit_manual)
		{
			m_rPara.para.manual_vel--;
			if(m_rPara.para.manual_vel < m_rPara.para.min_vel)
			{
				m_rPara.para.manual_vel=m_rPara.para.min_vel;
			}
			//��¼ת��ֵ
			eeprom_write_word_user((uint16_t *)&m_ePara[0].para.manual_vel,m_rPara.para.manual_vel);
		}
		#if __TEST_USED__ == TRUE
		else if(1==bit_auto)
		{
			if(bpwma > 2)
			{
				bpwma--;
			}
		}
		#endif
	}
	else
	{
		//��������ģʽ
		set_idle_wait=0;
		if(1 == bit_setmode_2)
		{
			//�ڲ�����
			switch(pagenumber)
			{
				case 0://��λ���������������(50%-100%)
				{
					/*
					m_rPara.para.material_full--;
					if(m_rPara.para.material_full < 50)
					{
						m_rPara.para.material_full=50;
					}
					*/
				}
				break;
				case 1://�ϵ�Զ�����ϵ���̿���ѡ��. 1:�ϵ�Զ��  2:�ϵ���̿���
				{
					if(2==m_rPara.para.is_remote)
					{
						m_rPara.para.is_remote=1;
					}
					else
					{
						m_rPara.para.is_remote=2;
					}
				}
				break;
				case 2://��������ر���ͣ������ (�������100%-200%)
				{
					m_rPara.para.mm_cur_overflow--;
					if(m_rPara.para.mm_cur_overflow < 100)
					{
						m_rPara.para.mm_cur_overflow=100;
					}
				}
				break;
				case 3://�Ƿ�������������������������   0:��������  1:����������
				{
					if(1==m_rPara.para.mm_control)
					{
						m_rPara.para.mm_control=0;
					}
				}
				break;
				case 4://��������������(10-90A),���������
				{
					m_rPara.para.cur1_full--;
					if(m_rPara.para.cur1_full < 10)
					{
						m_rPara.para.cur1_full=10;
					}
				}
				break;
				case 5://����λ
				{
					m_cgq_cmd=DILIAOWEI;
				}
				break;
				case 6://ѡ����λ�ɼ��ź�	  1:485+AD 2:485 3:LC�����ɴ������� 4:AD
				{
					if(m_rPara.para.material_port > MATERIAL_PORT_RS485_AD)
					{
						m_rPara.para.material_port--;
					}
					else
					{
						m_rPara.para.material_port = MATERIAL_PORT_ONLY_AD;
					}
				}
				break;
				case 7://MB��ת��֮��Ĺ�ϵ
				{
					if(m_rPara.para.mb_div > 1)
					{
						m_rPara.para.mb_div--;
					}
					else
					{
						m_rPara.para.mb_div = 99;
					}
				}
				break;
				#if __D2_USED__ == TRUE
				case 8://��������������(10-90A),���������
				{
					m_rPara.para.cur2_full--;
					if(m_rPara.para.cur2_full < 10)
					{
						m_rPara.para.cur2_full=10;
					}
				}
				break;
				#endif 
			}
		}
		else if(1 == bit_setmode_password)
		{
			//�������루�������ý׶Σ�
			password[pagenumber]--;
			if(password[pagenumber] < 1)
			{
				password[pagenumber]=1;
			}
		}
		else if(1 == bit_setmode_delay)
		{
			switch(pagenumber)
			{
				case 0:
				{
					//��ʱ����
					//��ʱʱ��,��
					limited_days--;
					if(limited_days < 1)
					{
						limited_days=1;
					}
				}
				break;
			}
		}
		else if(1 == bit_setmode_3)
		{
			//���뱣���Ĳ���
			switch(pagenumber)
			{
				case 0://������λֵ
				{
					m_rPara.para.material_start--;
					if(m_rPara.para.material_start < 30)
					{
						m_rPara.para.material_start=30;
					}
				}
				break;
				case 1://ͣ����λֵ
				{
					m_rPara.para.material_stop--;
					if(m_rPara.para.material_stop < 80)
					{
						m_rPara.para.material_stop=80;
					}
				}
				break;
				case 2://��������ֵ
				{
					m_rPara.para.current1_modify--;
					if(m_rPara.para.current1_modify < 70)
					{
						m_rPara.para.current1_modify=70;
					}
				}
				break;
				case 3://MODBUSͨ�Ŵӻ���ַ
				{
					if(m_rPara.para.addr_modbus > 2)
					{
						m_rPara.para.addr_modbus--;
					}
				}
				break;
				case 4://MODBUSͨ�Ų�����
				{
					if(m_rPara.para.baud_modbus > MODBUS_BAUD_9600)
					{
						m_rPara.para.baud_modbus--;
					}
					else
					{
						m_rPara.para.baud_modbus = MODBUS_BAUD_38400;
					}
				}
				break;
				case 5:
				{
					//MB����ѡ��1����ƽ��2�����壩
					if(MB_LEVEL == m_rPara.para.mb_level_pulse)
					{
						m_rPara.para.mb_level_pulse = MB_PULSE;
					}
					else
					{
						m_rPara.para.mb_level_pulse = MB_LEVEL;
					}
				}
				break;
				case 6:
				{
					//DW����ѡ��1����ƽ��2�����壩
					if(DW_LEVEL == m_rPara.para.dw_level_pulse)
					{
						m_rPara.para.dw_level_pulse = DW_PULSE;
					}
					else
					{
						m_rPara.para.dw_level_pulse = DW_LEVEL;
					}
				}
				break;
				#if __D2_USED__ == TRUE
				case 7://��������ֵ
				{
					m_rPara.para.current2_modify--;
					if(m_rPara.para.current2_modify < 70)
					{
						m_rPara.para.current2_modify=70;
					}
				}
				break;
				#endif 
			}
		}
		else
		{
			//�ⲿ����
			switch(pagenumber)
			{
				case 0://���ת��(40-300)
				{
					m_rPara.para.max_vel--;
					if(m_rPara.para.max_vel < MIN_VEL)
					{
						m_rPara.para.max_vel=MIN_VEL;
					}
				}
				break;
				case 1://���ת��(40-300)
				{
					m_rPara.para.min_vel--;
					if(m_rPara.para.min_vel < MIN_VEL)
					{
						m_rPara.para.min_vel=MIN_VEL;
					}
				}
				break;
				case 2://����������(10-100A)
				{
					m_rPara.para.mmotor_cur1--;
					if(m_rPara.para.mmotor_cur1 < 10)
					{
						m_rPara.para.mmotor_cur1=10;
					}
				}
				break;
				case 3://������ʱʱ��(0.2-20s)
				{
					m_rPara.para.rollin_delay--;
					if(m_rPara.para.rollin_delay < 2)
					{
						m_rPara.para.rollin_delay=2;
					}
				}
				break;
				case 4:
				{
					//��ͣ����־����
				}
				break;
				case 5://�ָ�Ĭ�ϲ���
				{
					if(set_default > 0)
					{
						set_default--;
					}
					else
					{
						set_default = 99;
					}
				}
				break;
				#if __D2_USED__ == TRUE
				case 6://����������(10-100A)
				{
					m_rPara.para.mmotor_cur2--;
					if(m_rPara.para.mmotor_cur2 < 10)
					{
						m_rPara.para.mmotor_cur2=10;
					}
				}
				break;
				#endif 
			}
		}
	}
}
/////////////////////////////////////////////////////////////////

