/*//////////////////////////////////////////////////////////
*                                                          *
*     �Ĺ���ش��������Ƴ���                               *
*     MCU:ATMega8(L)                                       *
*     CRY:�ⲿ����4MHz                                     *
*     ʱ��:20120501                                        *
*     ���ӵ������ݱ���                                     *
//////////////////////////////////////////////////////////*/
/*
��˿λ����ע������(������):
1,	  ���ܹ�ѡ'���Ź���ʱ��һֱ����'
2,    ��ѡ'ʹ��ISP���'
3,    ���ܹ�ѡ'�����ⲿ��λ,PC6������ͨIO'
4,    ��ѡ'ʹ�ܵ�����'��Ӧ�ù�ѡ'�������ƽΪ4.0v'
5,    ��ѡ'�ⲿ����(3-8MHz),����ʱ��16CK+4ms(��65ms)'
*/
////////////////////////////////////////////////////////////
#include "sgdkcgq_485_cry4m16m_pulse_ad.h"
#include "math.h"
#include "soft_timer.h"
////////////////////////////////////////////////////////////
//
unsigned int send_time_count = 0;
unsigned int smoothingDate =0;
//
int main()
{    
    cli();
    _delay_ms(50);
    hardward_init();
    softward_init();
    _delay_ms(100);
    read_para();
    sei();//����ȫ���ж�
    //
	#if USART_TEST == TRUE
	{
		DEBUG_STRING("USART TEST\r\n\0");
	}
	#endif
	//
	turn_on_led_ms(1500);
    //
	SetTimer(MT_SCAN_KEY, 50, -1); //����ɨ��
	SetTimer(MT_LED_RUN, 1000, -1); //���е���˸
	//	
	wdt_enable(WDTO_2S);//�������Ź���ʱ��
	//
    for(;;)
    {
        wdt_reset();//ι��
        //
        //���յ�����ȷ�����ݺ�,����Ӧ��֡
        if(1 == uart_send_enable)
        {
            uart_send_enable = 0;
			//
#if IS_MMR_USED == FALSE
            uart_tx();
#endif
        }
		//
		if(pulse_value_counter > 0)
		{
			pulse_value_counter--;
			//
			int1_handling(pulse_value[pulse_value_read_id]);
			pulse_value_read_id++;
			pulse_value_read_id %= PULSE_VALUE_NUM;
		}
		//
		//����boot_time��ȡ������ֵ���жϸ���
		if(boot_time_sub > 0)
		{
			boot_time += boot_time_sub;
			boot_time_sub = 0;
		}
		//
		#if 1
		{
			//��λ������
			#define UPDATE_EEPROM_DATA_INTERVAL	60
			//
			if(m_minute > UPDATE_EEPROM_DATA_INTERVAL)
			{
				//ˢ��һ���ڴ���Ĳ�����ͬʱҲ�����eeprom��Ĵ��������������ִ���Ļ���
				//
				read_para();
				//
				m_minute = 0;
			}
		}
		#endif
		//
        //485ͨ���жϵ�ʱ���Ƴ���
		#if 1
		{
			if(comm_error_delay > 200)//2��
			{
				#if USART_TEST == FALSE
				{
					bled = !bled;
					comm_error_delay = 0;
				}
				#endif
			}
		}
		#endif
		//
		timer_run();
    }
}
/////////////////////////////////////////////////////////////
void hardward_init(void)
{
	//Ӳ����ʼ������,���ù̶��Ķ˿ڷ���,��������,AD���Ե�
	wdt_disable();
	//I/O����
	PORTB=0;//���������еĿ�Ϊ����
	//
    DDRB = _BV(PB1)|_BV(PB2);
    PORTD = _BV(PD2)|_BV(PD3);
	DDRD = _BV(PD7);//������Ҫ���������
    PORTC = _BV(PC2)|_BV(PC3);
    DDRC = _BV(PC4)|_BV(PC5);
	//��ʱ��0
	TCCR0 = 0x03;//64��Ƶ
	#if F_CPU == F_4M
	{		
		TCNT0 = 255-62;//4MHzʱ��,����62��,Լ1����
	}
	#elif F_CPU == F_16M
	{
		TCNT0 = 255-250;//16MHzʱ��,����250��,Լ1����
	}
	#else
	{
		#error "F_CPU error"
	}
	#endif
	TIMSK |=_BV(TOIE0);//��ʱ��0����ж�ʹ��
	//��ʱ��1
	TCCR1A = 0xf2;//oc1a,oc1bƥ��ʱ��1,�ﵽTOPʱ����
	TCCR1B = 0x19;//ʱ�Ӳ���Ƶ
	ICR1 = PWM_TOP;//TOP//4kHz
    OCR1A = PWM_TOP/2;//��ѹ�����ʼռ�ձ�50%,2.5v
    OCR1B = PWM_LOW;//��������,ռ�ձ�LOW_PULSE/PWM_TOP(�͵�ƽʱ��)
	//
	#if USED_AD == TRUE
	//AD
	//ADMUX = _BV(ADLAR);//0010 0000//AREF�ο�,��8λ��ADCH
	ADMUX = 0;//0000 0000//AREF�ο�
	ADMUX |= ICM7555_AD;
	ADCSRA = 0x8f;//1000 1111//����adת��,����ת����ʽ,�����ж�,128��Ƶ
	#endif
	//
    //���ж�
    MCUCR &= 0xf0;
    MCUCR |= 0x08;//���ж�1�½��ش���
    GICR &= 0x3f;
    GICR |= _BV(INT1);//���ж�1ʹ��
	//
	#if USART_TEST == FALSE
	{
		//38461.5 Bd //0.2%
		UBRRH = 0;
		#if F_CPU == F_4M
		{
			UBRRL = 12;
		}
		#elif F_CPU == F_16M
		{
			UBRRL = 51;
		}
		#else
		{
			#error "F_CPU error"
		}
		#endif
		UCSRA = (1<<U2X);//�����ʷ�Ƶ���Ӵ�16����8
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);//���պͷ����ж�ʹ��
		UCSRC = (1<<URSEL)|(3<<UCSZ0);//8N1
		b485en = 0;//����ʹ��,���ͽ�ֹ
	}
	#else
	{
		//38461.5 Bd //0.2%
		UBRRH = 0;
		#if F_CPU == F_4M
		{
			UBRRL = 12;
		}
		#elif F_CPU == F_16M
		{
			UBRRL = 51;
		}
		#else
		{
			#error "F_CPU error"
		}
		#endif
		UCSRA = (1<<U2X);//�����ʷ�Ƶ���Ӵ�16����8
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);//�����ж�ʹ��
		UCSRC = (1<<URSEL)|(3<<UCSZ0);//8N1
		b485en = 0;//����ʹ��,���ͽ�ֹ
	}
	#endif
	//
	_delay_ms(5);
}
////////////////////////////////////////////////////////////////
void softward_init(void)
{	
    MCUCR &= 0x0f;//���߽�ֹ,����ģʽ
    //
    ms1_counter = 0;
    ms10_counter = 0;
    ms100_counter = 0;
    s1_counter = 0;
    m_ms = 0;
    m_minute = 0;
	//
    m_pulse_counter = 0;//����������
    m_pulse_value.ml = 0;//���������ֵ
    //
	m_cgq_ack=0;
    //
    m_pulse_value_bak.ms[0] = PULSE_TOP/2;
    m_pulse_value_bak.ms[1] = PULSE_TOP/2;
    m_pulse_value_bak.ms[2] = PULSE_TOP/2;
    m_pulse_value_bak.ms[3] = PULSE_TOP/2;
	//
	#if USED_AD == TRUE
    m_ad_value_bak.ms[0] = PULSE_TOP/2;
    m_ad_value_bak.ms[1] = PULSE_TOP/2;
    m_ad_value_bak.ms[2] = PULSE_TOP/2;
    m_ad_value_bak.ms[3] = PULSE_TOP/2;
	#endif

	//
	m_pulse_value_rc = PULSE_BOTTOM;
	m_ad_value_rc = PULSE_BOTTOM;
	//
	uart_frame_head_ok = 0;
	command_gaoliaowei = 0;
	command_diliaowei = 0;
	uart_send_enable = 0;
	//
	pulse_value_counter=0;
	pulse_value_write_id=0;
	pulse_value_read_id=0;
	//
	boot_time = 0;
	boot_time_sub = 0;
}
////////////////////////////////////////////////////////////////
void wdt_reset_mcu(void)
{
	//ͨ�����Ź�����λ��Ƭ��
	wdt_reset();
	wdt_disable();
	_delay_ms(50);
	cli();//���Ź���λ��Ƭ��
	wdt_enable(WDTO_120MS);
	for(;;);//�ȴ���Ƭ������
}
////////////////////////////////////////////////////////////////
void set_default_para(void)
{
	//ִ�лָ�����Ĭ�ϲ�������
	//
	cli();
	//
	//����
	eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,PULSE_TOP);
	eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,PULSE_BOTTOM);

	eeprom_write_word_user((uint16_t *)&m_para[0].smoothingDate,PARA_SETED_SMOOTHING);
	//
	//�������ñ�־
	eeprom_write_word_user((uint16_t *)&m_para[0].e_para_seted_flag,PARA_SETED_FLAG);
	#if 1
	{
		int i;
		for(i=5; i>0; i--)
		{
			_delay_ms(10);
			if(PARA_SETED_FLAG == eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
			{
				break;
			}
			else
			{
				_delay_ms(20);
				eeprom_write_word_user((uint16_t *)&m_para[0].e_para_seted_flag,PARA_SETED_FLAG);
			}
		}	
	}
	#endif
	//
	_delay_ms(100);
	wdt_reset_mcu();//����
}

void uart_tx_zan(void)
{  
    //�жϷ�ʽ������������ 
    //
    uart_tx_buf[0] = 0xaa;
	/*
	zan_m_temp.mc[0] = 1;
	zan_m_temp.mc[1] = 2;
	zan_m_temp.mc[2] = 3;
	zan_m_temp.mc[3] = 4;

	uart_tx_buf[1] = (zan_m_temp.ml >> 24)&0xff ;
	uart_tx_buf[2] = (zan_m_temp.ml >> 16)&0xff ;
	uart_tx_buf[3] = (zan_m_temp.ml >> 8)&0xff ;
	uart_tx_buf[4] = (zan_m_temp.ml >> 0)&0xff ;

	uart_tx_buf[5] = (zan_m_temp.ms[0] >> 8)&0xff ;
	uart_tx_buf[6] = (zan_m_temp.ms[0] >> 0)&0xff ;
	04 03 02 01 02 01

	zan_m_temp.mc[0] = 1;
	zan_m_temp.mc[1] = 2;
	zan_m_temp.mc[2] = 3;
	zan_m_temp.mc[3] = 4;

	zan_m_temp.ml += 257;

	uart_tx_buf[1] = (zan_m_temp.ml >> 24)&0xff ;
	uart_tx_buf[2] = (zan_m_temp.ml >> 16)&0xff ;
	uart_tx_buf[3] = (zan_m_temp.ml >> 8)&0xff ;
	uart_tx_buf[4] = (zan_m_temp.ml >> 0)&0xff ;

	//m_mcur.ms

	uart_tx_buf[5] = (zan_m_temp.ms[0] >> 8)&0xff ;
	uart_tx_buf[6] = (zan_m_temp.ms[0] >> 0)&0xff ;
	*/

	uart_tx_buf[1]  = 3;  //��ַ
 	//uart_tx_buf[2] = m_ml_set >> 8;//��ĵط��������

	//uart_tx_buf[1] |= m_e_laingcheng <<4;

	//uart_tx_buf[2] = GAOLIAOWEI;

	/*
	uart_tx_buf[3] =0;
	uart_tx_buf[4] =0;
	uart_tx_buf[5] =0;
	uart_tx_buf[6] =0;
	uart_tx_buf[7] =0;
	uart_tx_buf[8] =0;
	*/

	unsigned m_sreg = SREG;
	cli();
	if (uart_tx_buf[2] ==SMOOTHING_SET)
	{
		uart_tx_buf[3] = smoothingDate;
		//uart_tx_buf[4] = m_ml_set & 0xff;

	}
	else
	{		
		uart_tx_buf[3] = m_ml_set >> 8;
		uart_tx_buf[4] = m_ml_set & 0xff;
	}


    uart_tx_buf[5] = m_mh_set >> 8;
    uart_tx_buf[6] = m_mh_set & 0xff;

    uart_tx_buf[7] = m_mcur.ms >> 8;
	uart_tx_buf[8] = m_mcur.ms & 0xff;
	SREG = m_sreg;

    //
	
	/**
		//ADMUX |= CAV444_OUT_AD;
	ADMUX |= 1;
	ADCSRA =
		
	zan_ad_value = ADCH << 8 | ADCL;	
	*/
	{
		unsigned char i;
		//
		uart_tx_buf[9] = 0;
		for(i=0; i<9; i++)
		{
			uart_tx_buf[9] += uart_tx_buf[i];//sum    
		}
	}
    //
    //��ʼ��������
    //_delay_us(400);
	_delay_us(200);
    b485en = 1;//����ʹ��
    _delay_us(100);
    UDR = uart_tx_buf[0];
    uart_tx_addr = 1;
}
////////////////////////////////////////////////////////////////
void read_para()
{
	#if 1
	{
		int i;
		for(i=5; i>0; i--)
		{
			if(PARA_SETED_FLAG == eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
			{
				break;
			}
			else
			{
				_delay_ms(20);
			}
		}
		if(0 == i)
		{
			//�����־������������Ĭ�ϲ���
			set_default_para();
			return;//��������´���ִ�в���������Եı�ʾִ�й���
		}
	}
	#else
	{
		if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
		{
			if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
			{
				//�����־������������Ĭ�ϲ���
				set_default_para();
				return;//��������´���ִ�в���������Եı�ʾִ�й���
			}
		}		
	}
	#endif
	//
	//�汾��
	if(version_h!=eeprom_read_word_user((const uint16_t *)(&m_para[0].e_version_h)))
	{
		eeprom_write_word_user((uint16_t *)(&m_para[0].e_version_h),version_h);
	}
	if(version_l!=eeprom_read_word_user((const uint16_t *)(&m_para[0].e_version_l)))
	{
		eeprom_write_word_user((uint16_t *)(&m_para[0].e_version_l),version_l);
	}
	//
    //������ʧ����,�ߵ���λ����ֵ
    m_mh_set = eeprom_read_word_user((const uint16_t *)&m_para[0].e_mh);
    if(m_mh_set > PULSE_TOP)//���5v
	{
        m_mh_set = PULSE_TOP;
	}
    else if(m_mh_set < PULSE_BOTTOM)
	{
        m_mh_set = PULSE_BOTTOM+3000;
	}

    smoothingDate = eeprom_read_word_user((const uint16_t *)&m_para[0].smoothingDate);
    if(smoothingDate >= 10)//���5v
	{
        smoothingDate = 0;
	}
	
  	//
    m_ml_set = eeprom_read_word_user((const uint16_t *)&m_para[0].e_ml);
    if(m_ml_set > m_mh_set)
	{
        m_ml_set = m_mh_set-3000;
	}
    else if(m_ml_set < PULSE_BOTTOM)
	{
        m_ml_set = PULSE_BOTTOM;
	}
}
////////////////////////////////////////////////////////////////
unsigned short eeprom_read_word_2(const uint16_t *p)
{
	unsigned short tmp;
	//
	unsigned m_sreg = SREG;
	cli();
	wdt_reset();
	eeprom_busy_wait();
	tmp = eeprom_read_word((uint16_t *)p);
	eeprom_busy_wait();
	SREG = m_sreg;
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_word_2(const uint16_t *p, unsigned short val)
{
	unsigned m_sreg = SREG;
	cli();
	wdt_reset();
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)p, val);
	eeprom_busy_wait();
	SREG = m_sreg;
}
////////////////////////////////////////////////////////////////
unsigned short eeprom_read_word_user(const uint16_t *p1)
{
	unsigned short tmp,tmp1,tmp2,tmp3,tmp4,tmp5;
	//
	tmp = 0xff;
	//
	const uint16_t *p2 = (const uint16_t *)(p1+EEPROM_DATA_WORD_SIZE);
	const uint16_t *p3 = (const uint16_t *)(p1+2*EEPROM_DATA_WORD_SIZE);
	const uint16_t *p4 = (const uint16_t *)(p1+3*EEPROM_DATA_WORD_SIZE);
	const uint16_t *p5 = (const uint16_t *)(p1+4*EEPROM_DATA_WORD_SIZE);
	//
	tmp1 = eeprom_read_word_2(p1);
	tmp2 = eeprom_read_word_2(p2);
	tmp3 = eeprom_read_word_2(p3);
	tmp4 = eeprom_read_word_2(p4);
	tmp5 = eeprom_read_word_2(p5);
	//
	if((tmp1==tmp2) && (tmp1==tmp3)) //123
	{
		tmp = tmp1;
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp1==tmp2) && (tmp1==tmp4)) //124
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp1==tmp2) && (tmp1==tmp5)) //125
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
	}
	else if((tmp1==tmp3) && (tmp1==tmp4)) //134
	{
		tmp = tmp1;
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp1==tmp3) && (tmp1==tmp5)) //135
	{
		tmp = tmp1;
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
	}
	else if((tmp1==tmp4) && (tmp1==tmp5)) //145
	{
		tmp = tmp1;
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
		//
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
	}
	else if((tmp2==tmp3) && (tmp2==tmp4)) //234
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp5)
		{
			eeprom_write_word_2(p5,tmp);
		}
	}
	else if((tmp2==tmp3) && (tmp2==tmp5)) //235
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp4)
		{
			eeprom_write_word_2(p4,tmp);
		}
	}
	else if((tmp2==tmp4) && (tmp2==tmp5)) //245
	{
		tmp = tmp2;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp3)
		{
			eeprom_write_word_2(p3,tmp);
		}
	}
	else if((tmp3==tmp4) && (tmp3==tmp5)) //345
	{
		tmp = tmp3;
		if(tmp != tmp1)
		{
			eeprom_write_word_2(p1,tmp);
		}
		//
		if(tmp != tmp2)
		{
			eeprom_write_word_2(p2,tmp);
		}
	}
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_word_user(uint16_t *p,unsigned short val)
{
	eeprom_write_word_2((uint16_t *)p,val);
	eeprom_write_word_2((uint16_t *)(p+EEPROM_DATA_WORD_SIZE),val);
	eeprom_write_word_2((uint16_t *)(p+2*EEPROM_DATA_WORD_SIZE),val);
	eeprom_write_word_2((uint16_t *)(p+3*EEPROM_DATA_WORD_SIZE),val);
	eeprom_write_word_2((uint16_t *)(p+4*EEPROM_DATA_WORD_SIZE),val);
}
////////////////////////////////////////////////////////////////
void scan_key(void)
{
#ifndef __TEST__
	int i;
    if(0 == bkmh)
    {
        //��⵽����λ��������
		for(i=300; i>0; i--)
		{
        	_delay_ms(2);
			if(0 != bkmh)
			{
				break;
			}
		}
		if(0 == i)
		{
	        //cli();
	        if(0 == bkmh)
	        {
				#if USED_SMOOTHING == TRUE
				{
					turn_on_led_ms(2000);
				}
				#else
				{
					turn_on_led_ms(2000);
				}
				#endif
	            for(i=30; i>0; i--)
	            {
					if(1==bkmh)
					{
						break;
					}
	                _delay_ms(2);
	                wdt_reset();
	            }
				#if USED_SMOOTHING == TRUE
				{
					for(i=30; i>0; i--)
					{
						wdt_reset();
						//m_mh_set = m_mcur.ms; //�궨�����ã���ʱ��¼
						_delay_ms(50);
					}
				}
				#endif
				m_mh_set = m_mcur.ms; //�����ֹ�жϣ��������ֵ�������
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_mh, m_mh_set);
				_delay_ms(20);
	        }
	        //sei();
		}
    }
    else if(0 == bkml)
    {
        //��⵽����λ��������
		for(i=300; i>0; i--)
		{
        	_delay_ms(2);
			if(0 != bkml)
			{
				break;
			}
		}
		if(0 == i)
		{
	        //cli();
	        if(0 == bkml)
	        {
		        #if USED_SMOOTHING == TRUE
		        {
			        turn_on_led_ms(2000);
		        }
		        #else
		        {
			        turn_on_led_ms(2000);
		        }
		        #endif
	            for(i=30; i>0; i--)
	            {
		            if(1==bkml)
		            {
			            break;
		            }
		            _delay_ms(2);
		            wdt_reset();
	            }
				#if USED_SMOOTHING == TRUE
				{
					for(i=30; i>0; i--)
					{
						wdt_reset();
						//m_ml_set = m_mcur.ms;//�궨�����ã���ʱ��¼
						_delay_ms(50);
					}
				}
				#endif
				m_ml_set = m_mcur.ms; //�����ֹ�жϣ��������ֵ�������
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_ml, m_ml_set);
	            _delay_ms(20);
	        }
	        //sei();
		}
    }
    else if(1 == command_gaoliaowei)
    {
        command_gaoliaowei = 0;
		//
		#if USED_SMOOTHING == TRUE
		{
			turn_on_led_ms(3000);
		}
		#else
		{
			turn_on_led_ms(2000);
		}
		#endif
        m_mh_set = m_mcur.ms;//�궨�����ã���ʱ��¼
        //
        m_cgq_ack=GAOLIAOWEI;
		//
		#if USED_SMOOTHING == TRUE
		{
			SetTimer(MT_GAOLIAOWEI_SAVE, 2500, 0);
		}
		#else
		{
			SetTimer(MT_GAOLIAOWEI_SAVE, 2000, 0);
		}
		#endif		
    }
    else if(1 == command_diliaowei)
    {
        command_diliaowei = 0;
		//
		#if USED_SMOOTHING == TRUE
		{
			turn_on_led_ms(2500);
		}
		#else
		{
			turn_on_led_ms(2000);
		}
		#endif
        m_ml_set = m_mcur.ms;//�궨�����ã���ʱ��¼
        //
        m_cgq_ack=DILIAOWEI;
		//
		#if USED_SMOOTHING == TRUE
		{
			SetTimer(MT_DILIAOWEI_SAVE, 2500, 0);
		}
		#else
		{
			SetTimer(MT_DILIAOWEI_SAVE, 2000, 0);
		}
		#endif
    }
#else
    if(0 == bkmh)
    {
        //��ѹ���������
        _delay_ms(5);
        if(0 == bkmh)
        {
            _delay_ms(50);
            if(OCR1A > 0)
			{
                OCR1A--;
			}
        }
    }
    else if(0 == bkml)
    {
        //��ѹ����𽥽���
        _delay_ms(5);
        if(0 == bkml)
        {
            _delay_ms(50);
            if(OCR1A < PWM_TOP)
			{
                OCR1A++;
			}
        }
    }
#endif

#if IS_MMR_USED == TRUE
	if (send_time_count < 20)
	{ 	    
		send_time_count ++; //50ms
		if (send_time_count >3 && send_time_count%3 ==0)
		{
			uart_tx_zan();
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////
void turn_on_led_ms(unsigned short ms)
{
	bled = 1;
	SetTimer(MT_TURN_OFF_LED, ms, 0);
}
/////////////////////////////////////////////////////////////////
#if USART_TEST == TRUE
//
void send_uart_to_computer(unsigned char *p,unsigned short len)
{	
	b485en = 1;//����ʹ��
	_delay_ms(5);
	//
	if(NULL == p)
	{
		//�ȴ����ͻ�����Ϊ��
		while (!(UCSRA & (1<<UDRE)));
		//�����ݷ��뻺��������������
		UDR = (unsigned char)len;
	}
	else
	{
		unsigned short i;
		for(i=0; i<len; i++)
		{
			while (!(UCSRA & (1<<UDRE)));
			UDR = p[i];
		}
	}
	//
	while (!(UCSRA & (1<<UDRE)));
	//
	_delay_us(150);
	b485en = 0;
	_delay_us(300);
}
//
//
void send_ulong_ascii(unsigned long n)
{
	char tmp[13];
	int i = sizeof(tmp) - 1;

	// Convert the given number to an ASCII decimal representation.
	tmp[i] = '\0';
	tmp[--i]= '\n';
	tmp[--i]= '\r';
	do
	{
		tmp[--i] = '0' + n % 10;
		n /= 10;
	} while (n);

	// Transmit the resulting string with the given USART.
	send_uart_to_computer((unsigned char*)(tmp+i),strlen(tmp+i));
}
//
#endif
////////////////////////////////////////////////////////////////
void uart_tx(void)
{  
    //�жϷ�ʽ������������ 
    //
	UCHAR2 tmp;
    uart_tx_buf[0] = 0xaa;
    uart_tx_buf[1] = ADDR_CGQ;//Ӧ���ַ
	//
	tmp.ms = OCR1A;
    uart_tx_buf[2] = tmp.mc[0]; //���������Ƚ�ֵ
    uart_tx_buf[3] = tmp.mc[1];
    tmp.ms = PWM_TOP;
    uart_tx_buf[4] = tmp.mc[0]; //pwm����
    uart_tx_buf[5] = tmp.mc[1];
	//
    uart_tx_buf[6]= m_cgq_ack;
    //
    #if USART_TEST == FALSE
	{
		unsigned char i;
		//
		uart_tx_buf[7] = 0;
		for(i=0; i<7; i++)
		{
			uart_tx_buf[7] += uart_tx_buf[i];//sum    
		}
	}
    #else
	{
		DEBUG_STRING("m_mh_set = \0");
		DEBUG_VALUE_TEXT(m_mh_set);
		DEBUG_STRING("m_ml_set = \0");
		DEBUG_VALUE_TEXT(m_ml_set);
		DEBUG_STRING("m_mcur.ms = \0");
		DEBUG_VALUE_TEXT(m_mcur.ms);
		DEBUG_STRING("m_cur_pwm.ms = \0");
		DEBUG_VALUE_TEXT(m_cur_pwm.ms);
	}
    #endif
    //
    //��ʼ��������
    _delay_us(400);
	//_delay_us(200);
    b485en = 1;//����ʹ��
    _delay_us(100);
    UDR = uart_tx_buf[0];
    uart_tx_addr = 1;
}
/////////////////////////////////////////////////////////////////

//smoothingDate

unsigned short pulse_value_handling(unsigned short val)
{
	//������Ĳ�����ƽ������������ʱ��Ĳ��������ܷ�Ӧ��΢�ٶ�
	//
	#if USED_SMOOTHING == TRUE
	{
		unsigned short tmp;
		//
		m_pulse_value_bak.mll <<= 16;
		m_pulse_value_bak.ms[0] = val;
		//
		/*tmp = (unsigned short)(m_pulse_value_bak.ms[0]/8);
		tmp += (unsigned short)(m_pulse_value_bak.ms[1]/2);
		tmp += (unsigned short)(m_pulse_value_bak.ms[2]/4);*/
		if (smoothingDate !=0)
		{
			tmp = ((unsigned short)(m_pulse_value_bak.ms[1])/10*smoothingDate)
					+ ((unsigned short)(m_pulse_value_bak.ms[1])/10*(10 -smoothingDate));
		}
		else
		{
			tmp = m_pulse_value_bak.ms[0];

		}
		//
		return tmp;
	}
	#else
	{
		return val;
	}
	#endif
}
/////////////////////////////////////////////////////////////////
#if USED_AD == TRUE
unsigned short ad_value_handling(unsigned short val)
{
	//������Ĳ�����ƽ������������ʱ��Ĳ��������ܷ�Ӧ��΢�ٶ�
	//
	#if USED_SMOOTHING == TRUE
	{
		unsigned short tmp;
		//
		m_ad_value_bak.mll <<= 16;
		m_ad_value_bak.ms[0] = val;
		//
		tmp = (unsigned short)(m_ad_value_bak.ms[0]/8);
		tmp += (unsigned short)(m_ad_value_bak.ms[1]/2);
		tmp += (unsigned short)(m_ad_value_bak.ms[2]/4);
		tmp += (unsigned short)(m_ad_value_bak.ms[3]/8);
		//
		return tmp;
	}
	#else
	{
		return val;
	}
	#endif
}
#endif
/////////////////////////////////////////////////////////////////
void OnTimer(unsigned short id)
{
	switch(id)
	{
		case MT_TEST:
		break;
		case MT_TURN_OFF_LED:
		{
			bled = 0;
			m_cgq_ack=0;//���Ӧ������
		}
		break;
		case MT_LED_RUN:
		{
			//���е���˸
			bled_run = !bled_run;
		}
		break;
		case MT_SCAN_KEY:
		{
			scan_key();
		}
		break;
		case MT_GAOLIAOWEI_SAVE:
		{
			m_mh_set = m_mcur.ms;
			eeprom_write_word_user((uint16_t *)&m_para[0].e_mh, m_mh_set);
			_delay_ms(2);
			//
			m_cgq_ack=GAOLIAOWEI;
		}
		break;
		case MT_DILIAOWEI_SAVE:
		{
			m_ml_set = m_mcur.ms;
			eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,m_ml_set);
			_delay_ms(2);
			//
			m_cgq_ack=DILIAOWEI;
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////
void liaowei_sample_ok_handling(void)
{
	UCHAR4 m_temp;
	//
	m_mcur.ms = pulse_value_handling(m_mcur.ms);//ƽ������
	//
	if(m_mcur.ms > PULSE_TOP)
	{
		m_mcur.ms = PULSE_TOP;
	}
	//
	//OCR1A = PWM_TOP/2; //for test
	//return;
	//
	//����pwm��ѹ���
	#ifndef __TEST__
	if(m_mcur.ms <= (m_ml_set+3))
	{
		m_cur_pwm.ms = (PWM_TOP/5)-3;//150//200
		OCR1A = (unsigned short)PWM_TOP-m_cur_pwm.ms;//800,1v
	}
	else if(m_mcur.ms >= (m_mh_set-4))
	{
		m_cur_pwm.ms = PWM_TOP-1;
		OCR1A = (unsigned short)PWM_TOP-m_cur_pwm.ms;//1,5v
	}
	else
	{
		//1v <= out <= 5v
		unsigned short m_offset;
		m_offset = m_mcur.ms-m_ml_set;//��ǰ��õ�ֵ��ȥ��λ����ֵ
		m_temp.ml = m_offset;
		m_temp.ml *= (unsigned short)((PWM_TOP/5)*4);//�Ⱦ��ƽ��1v��5v
		m_offset = m_mh_set-m_ml_set;//��λ����ֵ��ȥ��λ����ֵ
		m_temp.ml += m_offset>>1;//��������
		m_temp.ml /= m_offset;
		m_temp.ms[0] += (PWM_TOP/5);//������͵�1v
		//
		if(m_temp.ms[0] >= PWM_TOP)
		{
			m_temp.ms[0] = PWM_TOP-1;
		}
		//
		#if USED_AD == TRUE //��Լ800ms
		{
			m_mcur_ad.ms = ad_value_handling(m_mcur_ad.ms);//ƽ������
			////////////////////// AD���� ///////////////////////////////////////////////
			if(m_cur_pwm.ms == m_temp.ms[0])
			{
				//ǰһ�������pwm����ֵ����μ����һ������ADֵ��΢���ж��Ƿ���Ҫ����
				//
				if(m_mcur_ad.ms != m_cur_per_ad.ms)
				{
					//ADֵ�б仯
					unsigned short m_step;
					m_step = 65535/PWM_TOP;
					//
					if(m_mcur_ad.ms > m_cur_per_ad.ms)
					{
						//ad��ǰֵ����ǰ��pwm���ʱ��ֵ
						m_offset = m_mcur_ad.ms-m_cur_per_ad.ms;
						m_offset += m_step;
						m_offset /= m_step;
						if(m_offset > 0)
						{
							m_offset = 1;
						}
					}
					else
					{
						//ad��ǰֵС��ǰ��pwm���ʱ��ֵ
						m_offset = m_cur_per_ad.ms-m_mcur_ad.ms;
						m_offset += m_step;
						m_offset /= m_step;
						if(m_offset > 0)
						{
							m_offset = 0xffff;
						}
					
					}
					//
					m_offset += m_cur_pwm.ms;
					if(m_offset >= PWM_TOP)
					{
						m_offset = PWM_TOP-1;
					}
					OCR1A=(unsigned short)PWM_TOP-m_offset;
				}
			}
			else
			{
				//���μ����pwm����ֵ��ǰ�������ֵ��ͬ����ʱ������ADֵ
				m_cur_pwm.ms = m_temp.ms[0];
				OCR1A=(unsigned short)PWM_TOP-m_cur_pwm.ms;
			}
		}
		#else
		{
			m_cur_pwm.ms = m_temp.ms[0];
			OCR1A=(unsigned short)PWM_TOP-m_cur_pwm.ms;
		}
		#endif //USED_AD
		////////////////////////////////////////////////////////////////////////////
		//
	}
	//
	#if USED_AD == TRUE
	{
		m_cur_per_ad.ms = m_mcur_ad.ms;//��������
	}
	#endif
	//
	#endif
	//
	#if USART_TEST == TRUE
	{
		uart_send_enable=1;//���ڷ���ʹ��
	}
	#endif	
}
/////////////////////////////////////////////////////////////////
void int1_handling(unsigned char val)
{
	UCHAR4 m_temp;
	//
	m_temp.ms[0] = val;
	//
	//һ��RC��ͨ�����˲�������Ƶ��10KHz����ֹƵ��1KHz
	#if 0
	{
		m_pulse_value_rc = ((m_pulse_value_rc*10+5)/11) + ((m_temp.ms[0]+5)/11);
		m_temp.ms[0] = m_pulse_value_rc;
	}
	#else
	{
		m_pulse_value_rc = ((m_pulse_value_rc<<5)-(m_pulse_value_rc<<1))>>5;
		m_temp.ms[0] = ((m_temp.ms[0]<<1)+m_temp.ms[0])>>5;
		m_pulse_value_rc += m_temp.ms[0];
		m_temp.ms[0] = m_pulse_value_rc;
	}
	#endif
	//
	m_pulse_value.ml += m_temp.ms[0];
	m_pulse_counter++;
	//
	//������õĲ�������10KHz
	if(m_pulse_counter > ((unsigned short)32*1024))//�ɼ�ˢ��ʱ��Լ2��      //32 //16 //8        //4096=2^12  //2048=2^11  //1024=2^10  //512=2^9  //256=2^8
	{
		m_pulse_value.ml <<= (6-5);                       //5   //4  //3       //16-12      //16-11      //16-10      //16-9     //16-8
		m_pulse_value.ml <<= SAMPLE_SHIFT_BIT_NUM;
		m_mcur.ms = m_pulse_value.ms[1];//������16λ,�൱�ڳ���2^16
		//
		m_pulse_counter = 0;
		m_pulse_value.ml = 0;
		//
		liaowei_sample_ok_handling();
	}
}
/////////////////////////////////////////////////////////////////
///////////////       �жϷ������       ////////////////////////
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
//
//��ʱ��0��Ϊȫ�ֶ�ʱ��,����������������ĵ���
//
ISR(TIMER0_OVF_vect/*,ISR_NOBLOCK*/)//����ISR_NOBLOCK����,���жϷ�������������ж���Ƕ
{	
	#if F_CPU == F_4M
	{
		TCNT0 = 255-62;//4MHzʱ��,64��Ƶ,����62��,��ʱԼ1����
	}
	#elif F_CPU == F_16M
	{
		TCNT0 = 255-250;//16MHzʱ��,64��Ƶ,����62��,��ʱԼ1����
	}
	#else
	{
		#error "F_CPU error"
	}
	#endif
	///////////////
	#if USED_AD == TRUE
	{
		adsc = 1;
	}
	#endif
	///////////////
	//1ms
	ms1_counter++;
    boot_time_sub++;
    //
    if(ms1_counter > 9)
    {
        ms1_counter = 0;
		//
        //10ms
        comm_error_delay++;
        if(comm_error_delay > 250)
		{
            comm_error_delay = 250;
		}
        //
        ms10_counter++;
        if(ms10_counter > 9)
        {
            ms10_counter = 0;
            //100ms
            ms100_counter++;
            if(ms100_counter > 9)
            {
                ms100_counter = 0;
                //1s
                s1_counter++;
                if(s1_counter > 59)
                {
	                //60s
	                s1_counter = 0;
	                //
	                m_minute++;
                }
            }
        }

    }
}
//
//���ж�1,�½��ش����ж�,���������Ľ�������
//
ISR(INT1_vect)
{
    pulse_value[pulse_value_write_id] = TCNT1;
	pulse_value_write_id++;
	pulse_value_write_id %= PULSE_VALUE_NUM;
	//
	pulse_value_counter++;
}
/////////////////////////////////////////////////////////////////
//
#if USED_AD == TRUE
//ADת����ɣ����ICM7555���PWM��ƽ
ISR(ADC_vect)
{
	UCHAR2 tmp;
	tmp.mc[0] = ADCL; //�����ȶ�
	tmp.mc[1] = ADCH;
	tmp.ms &= 0x03ff;
	//
	m_ad_value.ml += tmp.ms;//�ۼ�adֵ����������λ
	//
	m_ad_counter++;
	if(m_ad_counter > 128)//�ɼ�ˢ��ʱ��Լ128ms
	{
		//10λ���ݣ��ۼ�128��2^7������ֵ��������16λ
		m_ad_value.ml >>= (10+7-16);
		//
		m_mcur_ad.ms = m_ad_value.ms[0];
		//
		m_ad_counter = 0;
		m_ad_value.ml = 0;
	}
}
#endif 
//
/////////////////////////////////////////////////////////////////
//
//���ڷ����ж�
//
#if USART_TEST == FALSE
ISR(USART_TXC_vect)
{
    if(uart_tx_addr < 10)
    {
        UDR = uart_tx_buf[uart_tx_addr++];
    }
    else
    {
        _delay_us(100);
        b485en = 0;
    }
}
#endif
/////////////////////////////////////////////////////////////////
//
//���ڽ����ж�
//
ISR(USART_RXC_vect)
{
	unsigned char uart_value,sum,i,temp;
    //
	if((UCSRA&0x1c) != 0)
	{
		//���ڽ��ճ��ִ���
		uart_rx_counter = 0;
        uart_frame_head_ok = 0;
		while ( UCSRA & (1<<RXC) ) 
		{
			uart_value = UDR;
		}
	}
	else
	{
		//���ڽ�������
		uart_value = UDR;
        //
#if IS_MMR_USED == TRUE
        if(1 == uart_frame_head_ok)
        {
            uart_rx_buf[uart_rx_counter++] = uart_value;
            if(uart_rx_counter > 8)//����6���ֽ���Ч������һ���ֽ�У���
            {
                uart_rx_counter = 0;
                uart_frame_head_ok = 0;
                //
                sum = 0xaa;
                for(i=0; i<8; i++)
				{
                    sum += uart_rx_buf[i];
				}
                if(sum == uart_rx_buf[8])
                {
                    if(ADDR_CGQ == uart_rx_buf[0])
                    {
                        comm_error_delay = 0;
                        //
                        command_gaoliaowei = 0;
                        command_diliaowei = 0;
                        if(GAOLIAOWEI == uart_rx_buf[1] || GAOLIAOWEI == uart_rx_buf[2])
						{
                            command_gaoliaowei = 1;
							uart_tx_buf[2] = GAOLIAOWEI;
							send_time_count = 0;
						}
                        else if(DILIAOWEI == uart_rx_buf[1] || DILIAOWEI == uart_rx_buf[2])
						{
                            command_diliaowei = 1;
							uart_tx_buf[2] = DILIAOWEI;
							send_time_count = 0;
						}else if(SHUAXIANSHUJU == uart_rx_buf[1] || SHUAXIANSHUJU == uart_rx_buf[2])
						{
							uart_tx_buf[2] = SHUAXIANSHUJU;
							send_time_count = 0;
						}
						else if(GAOLIAOWEI_SET == uart_rx_buf[1] && GAOLIAOWEI_SET == uart_rx_buf[2])
						{

							uart_tx_buf[2] = SHUAXIANSHUJU;

							m_mh_set = uart_rx_buf[3]* 256 +uart_rx_buf[4];
							eeprom_write_word_user((uint16_t *)&m_para[0].e_mh, m_mh_set);
							_delay_ms(2);
							send_time_count = 0;
						}
						else if(DILIAOWEI_SET == uart_rx_buf[1] && DILIAOWEI_SET == uart_rx_buf[2])
						{

							uart_tx_buf[2] = SHUAXIANSHUJU;
							m_ml_set = uart_rx_buf[3]*256 +uart_rx_buf[4];
							eeprom_write_word_user((uint16_t *)&m_para[0].e_ml, m_ml_set);
							_delay_ms(2);
							send_time_count = 0;
						}
						else if(SMOOTHING_SET == uart_rx_buf[1] && SMOOTHING_SET == uart_rx_buf[2])
						{

							uart_tx_buf[2] = SMOOTHING_SET;
							temp = uart_rx_buf[3]*256 +uart_rx_buf[4];
							if(temp < 10)
							{
							    smoothingDate = temp;
								eeprom_write_word_user((uint16_t *)&m_para[0].smoothingDate, smoothingDate);
								_delay_ms(2);
							}
							send_time_count = 0;
						}
                        //
                        uart_send_enable = 1;//���ڷ���ʹ��
                    }
					comm_error_delay = 0;
                }
            }
        }
        else
        {
            if(0xaa == uart_value)
            {
				comm_error_delay = 0;

				uart_rx_counter = 0;
                uart_frame_head_ok = 1;
            }
        }
#else   //IS_MMR_USED
        if(1 == uart_frame_head_ok)
        {
	        uart_rx_buf[uart_rx_counter++] = uart_value;
	        if(uart_rx_counter > 6)//����6���ֽ���Ч������һ���ֽ�У���
	        {
		        uart_rx_counter = 0;
		        uart_frame_head_ok = 0;
		        //
		        sum = 0xaa;
		        for(i=0; i<6; i++)
		        {
			        sum += uart_rx_buf[i];
		        }
		        if(sum == uart_rx_buf[6])
		        {
			        if(ADDR_CGQ == uart_rx_buf[0])
			        {
				        comm_error_delay = 0;
				        //
				        if(GAOLIAOWEI == uart_rx_buf[1])
				        {
					        command_gaoliaowei = 1;
				        }
				        else if(DILIAOWEI == uart_rx_buf[1])
				        {
					        command_diliaowei = 1;
				        }
				        else
				        {
					        command_gaoliaowei = 0;
					        command_diliaowei = 0;
				        }
				        //
				        uart_send_enable = 1;//���ڷ���ʹ��
			        }
		        }
	        }
        }
        else
        {
	        if(0xaa == uart_value)
	        {
		        uart_rx_counter = 0;
		        uart_frame_head_ok = 1;
	        }
        }
#endif //IS_MMR_USED
	}
}
/////////////////////////////////////////////////////////////////
