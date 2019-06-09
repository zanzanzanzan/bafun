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
1,���ܹ�ѡ'���Ź���ʱ��һֱ����'
2,    ��ѡ'ʹ��ISP���'
3,���ܹ�ѡ'�����ⲿ��λ,PC6������ͨIO'
4,    ��ѡ'ʹ�ܵ�����'��Ӧ�ù�ѡ'�������ƽΪ4.0v'
5,    ��ѡ'�ⲿ����(3-8MHz),����ʱ��16CK+4ms(��65ms)'
*/
////////////////////////////////////////////////////////////
#include "sgdkcgq.h"
#include "math.h"
////////////////////////////////////////////////////////////
unsigned int send_time_count = 0;
//unsigned int zan_ad_value= 0;
//UCHAR4 zan_m_temp;


////////////////////////////////////////////////////////////////
void set_default_para(void)
{
	//????????????
	//
	cli();
	//
	//??
	eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,PULSE_TOP);
	eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,PULSE_BOTTOM);
	//
	//??????
	eeprom_write_word_user((uint16_t *)&m_para[0].e_para_seted_flag,PARA_SETED_FLAG);
	//
	_delay_ms(100);
//	wdt_reset_mcu();//??
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

	uart_tx_buf[2] = GAOLIAOWEI;
	uart_tx_buf[3] =0;
	uart_tx_buf[4] =0;
	uart_tx_buf[5] =0;
	uart_tx_buf[6] =0;
	uart_tx_buf[7] =0;
	uart_tx_buf[8] =0;
	/*
	uart_tx_buf[3] = m_ml_set >> 8;
    uart_tx_buf[4] = m_ml_set & 0xff;

    uart_tx_buf[5] = m_mh_set >> 8;
    uart_tx_buf[6] = m_mh_set & 0xff;

    uart_tx_buf[7] = m_mcur.ms >> 8;
	uart_tx_buf[8] = m_mcur.ms & 0xff;

	*/
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
int main()
{    
    cli();
    hardward_init();
    softward_init();
    read_para();
    sei();//����ȫ���ж�
    //
    bled = 1;
    m_led_delay = 100;
    //
	#if USART_TEST == TRUE
	{
		DEBUG_STRING("USART TEST\r\n\0");
	}
	#endif
	//
    for(;;)
    {
        wdt_reset();//ι��
        //
        //ɨ�����
        if(1 == scan_key_enable)
        {
            scan_key_enable = 0;
            scan_key();
        }
        //���յ�����ȷ�����ݺ�,����Ӧ��֡
        if(1 == uart_send_enable)
        {
            uart_send_enable = 0;
            //uart_tx();
        }
        //485ͨ���жϵ�ʱ���Ƴ���
        if(comm_error_delay > 200)//2��
        {
            #if USART_TEST == FALSE
			{
				if(1 == bled)
				{
					bled = 0;
				}
				else
				{
					bled = 1;
				}
				m_led_delay = 0;
				comm_error_delay = 0;
			}
            #endif
        }
        /////////////////////////////////////////
        MCUCR |= 0x80;//����ʹ��
        asm volatile("sleep");
        MCUCR &= 0x7f;//���߽�ֹ
        /////////////////////////////////////////
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
    DDRC = _BV(PC4);
	//��ʱ��0
	TCCR0 = 0x03;//,64��Ƶ
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
	ICR1 = PWM_TOP1;//TOP//4kHz
    OCR1A = PWM_TOP1/2;//��ѹ�����ʼռ�ձ�50%,2.5v
    OCR1B = PWM_LOW;//��������,ռ�ձ�LOW_PULSE/PWM_TOP(�͵�ƽʱ��)
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
    m_pulse_counter = 0;//����������
    m_pulse_value.ml = 0;//���������ֵ
    //
	m_cgq_ack=0;
	m_cgq_ack_delay=0;
    //
    m_pulse_value_bak.ms[0] = PULSE_TOP/2;
    m_pulse_value_bak.ms[1] = PULSE_TOP/2;
    m_pulse_value_bak.ms[2] = PULSE_TOP/2;
    m_pulse_value_bak.ms[3] = PULSE_TOP/2;
	//
	wdt_enable(WDTO_2S);//�������Ź���ʱ��
}
////////////////////////////////////////////////////////////////
void read_para()
{
	if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
	{
		//�����־������������Ĭ�ϲ���
		set_default_para();
		return;//��������´���ִ�в���������Եı�ʾִ�й���
	}
	//������ʧ����,�ߵ���λ����ֵ
    m_mh_set = eeprom_read_word_user((const uint16_t *)&m_para[0].e_mh);
    if(m_mh_set > PULSE_TOP)//���5v
	{
        m_mh_set = PULSE_TOP;
	}
    else if(m_mh_set < PULSE_BOTTOM)
	{
        m_mh_set = PULSE_BOTTOM+10;
	}
  	//
    m_ml_set = eeprom_read_word_user((const uint16_t *)&m_para[0].e_ml);
    if(m_ml_set > m_mh_set)
	{
        m_ml_set = m_mh_set-10;
	}
    else if(m_ml_set < PULSE_BOTTOM)
	{
        m_ml_set = PULSE_BOTTOM;
	}
}
////////////////////////////////////////////////////////////////
unsigned short eeprom_read_word_user(const uint16_t *p)
{
	unsigned short tmp,tmp1,tmp2;
	//
	tmp = eeprom_read_word(p);
	tmp1 = eeprom_read_word((const uint16_t *)(p+sizeof(EEPROM_DATA)));
	tmp2 = eeprom_read_word((const uint16_t *)(p+2*sizeof(EEPROM_DATA)));
	//
	if(tmp == tmp1)
	{
		if(tmp != tmp2)
		{
			tmp2 = tmp;
			eeprom_write_word((uint16_t *)(p+2*sizeof(EEPROM_DATA)),tmp2);
		}
	}
	else if(tmp == tmp2)
	{
		tmp1 = tmp;
		eeprom_write_word((uint16_t *)(p+sizeof(EEPROM_DATA)),tmp1);
	}
	else if(tmp1 == tmp2)
	{
		tmp = tmp1;
		eeprom_write_word((uint16_t *)p,tmp);
	}
	//
	return tmp;
}
////////////////////////////////////////////////////////////////
void eeprom_write_word_user(uint16_t *p,unsigned short val)
{
	eeprom_write_word((uint16_t *)p,val);
	eeprom_write_word((uint16_t *)(p+sizeof(EEPROM_DATA)),val);
	eeprom_write_word((uint16_t *)(p+2*sizeof(EEPROM_DATA)),val);
}
////////////////////////////////////////////////////////////////
void scan_key(void)
{
#ifndef __TEST__
	int i;
    if(0 == bkmh)
    {
        //��⵽����λ��������
		for(i=50; i>0; i--)
		{
        	_delay_ms(1);
			if(0 != bkmh)
			{
				break;
			}
		}
		if(0 == i)
		{
	        cli();
	        if(0 == bkmh)
	        {
	            bled = 1;
	            for(;0 == bkmh;)
	            {
	                _delay_ms(1);
	                wdt_reset();
	            }
	            m_mh_set = m_mcur.ms;
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,m_mh_set);
	            m_led_delay = 150;//����ʱ
	        }
	        m_pulse_counter = 0;
	        m_pulse_value.ml = 0;
	        sei();
		}
    }
    else if(0 == bkml)
    {
        //��⵽����λ��������
		for(i=50; i>0; i--)
		{
        	_delay_ms(1);
			if(0 != bkml)
			{
				break;
			}
		}
		if(0 == i)
		{
	        cli();
	        if(0 == bkml)
	        {
	            bled=1;
	            for(;0 == bkml;)
	            {
	                _delay_ms(1);
	                wdt_reset();
	            }
	            m_ml_set = m_mcur.ms;
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,m_ml_set);
	            m_led_delay = 150;//����ʱ
	        }
	        m_pulse_counter = 0;
	        m_pulse_value.ml = 0;
	        sei();
		}
    }
    else if(1 == command_gaoliaowei)
    {
        command_gaoliaowei = 0;
        m_mh_set = m_mcur.ms;
        eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,m_mh_set);
        bled=1;
        m_led_delay = 150;
        m_cgq_ack=GAOLIAOWEI;
		m_cgq_ack_delay=0;

		//���Ը���λ���ͷ���
		
    }
    else if(1 == command_diliaowei)
    {
        command_diliaowei = 0;
        m_ml_set = m_mcur.ms;
        eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,m_ml_set);
        bled = 1;
        m_led_delay = 150;
		//
        m_cgq_ack=DILIAOWEI;
		m_cgq_ack_delay=0;
    }
	//uart_tx_zan();


	if (send_time_count < 20)
	{ 
	    
		send_time_count ++;
		if (send_time_count >3 && send_time_count%3 ==0)
		{
			uart_tx_zan();
		}
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
    uart_tx_buf[0] = 0xaa;
    uart_tx_buf[1] = ADDR_CGQ;//Ӧ���ַ
    uart_tx_buf[2] = m_cur_pwm.mc[0];
    uart_tx_buf[3] = m_cur_pwm.mc[1];
    uart_tx_buf[4] = m_mcur.mc[0];
    uart_tx_buf[5] = m_mcur.mc[1];
    uart_tx_buf[6]=m_cgq_ack;
	if(m_cgq_ack_delay > 4)//500ms
	{
		m_cgq_ack_delay=0;
		m_cgq_ack=0;//���Ӧ������
	}
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
    b485en = 1;//����ʹ��
    _delay_us(100);
    UDR = uart_tx_buf[0];
    uart_tx_addr = 1;
}
/////////////////////////////////////////////////////////////////
unsigned short pulse_value_handling(unsigned short val)
{
	//������Ĳ�����ƽ������������ʱ��Ĳ��������ܷ�Ӧ��΢�ٶ�
	//
	unsigned short tmp;
	//
	m_pulse_value_bak.mll <<= 16;
	m_pulse_value_bak.ms[0] = val;
	//
	tmp = (unsigned short)(m_pulse_value_bak.ms[0]/8);
	tmp += (unsigned short)(m_pulse_value_bak.ms[1]/2);
	tmp += (unsigned short)(m_pulse_value_bak.ms[2]/4);
	tmp += (unsigned short)(m_pulse_value_bak.ms[3]/8);
	//
	return tmp;
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
	ms1_counter++;
    m_ms++;
    //
    if(ms1_counter > 9)
    {
        ms1_counter = 0;
        //10ms
        scan_key_enable = 1;//����ɨ�谴��
        comm_error_delay++;
        if(comm_error_delay > 250)
		{
            comm_error_delay = 250;
		}
        //
        m_led_delay++;
        if(m_led_delay > 249)//�������ʱ2.5��
        {
            m_led_delay = 0;
            bled = 0;
        }
        //
        ms10_counter++;
        if(ms10_counter > 9)
        {
            ms10_counter = 0;
            //100ms
			m_cgq_ack_delay++;//������Ӧ����ʱ
            ms100_counter++;
            if(ms100_counter > 9)
            {
                ms100_counter = 0;
                //1s
            }
        }

    }
}
/////////////////////////////////////////////////////////////////
//
//���ж�1,�½��ش����ж�,���������Ľ�������
//
ISR(INT1_vect)
{
    UCHAR4 m_temp;
    //
    m_temp.ms[0] = TCNT1;
    m_pulse_value.ml += m_temp.ms[0];
    m_pulse_counter++;
	//
    //������õĲ�������10KHz
    if(m_pulse_counter > (8*1024))//�ɼ�ˢ��ʱ��Լ1��       //4096=2^12  //2048=2^11  //1024=2^10  //512=2^9  //256=2^8
    {
        m_pulse_value.ml <<= (6-3);                        //16-12      //16-11      //16-10      //16-9     //16-8
		m_pulse_value.ml <<= SAMPLE_SHIFT_BIT_NUM;
        m_mcur.ms = m_pulse_value.ms[1];//������16λ,�൱�ڳ���2^16
		//
		m_mcur.ms = pulse_value_handling(m_mcur.ms);//ƽ������
		//
        if(m_mcur.ms > PULSE_TOP)
		{
            m_mcur.ms = PULSE_TOP;
		}
        m_pulse_counter = 0;
        m_pulse_value.ml = 0;
        //
		//OCR1A = PWM_TOP/2; //for test
		//return;
		//
        //����pwm��ѹ���
        #ifndef __TEST__
        if(m_mcur.ms < m_ml_set)
        {
            //m_cur_pwm.ms = (PWM_TOP/5)-2;//150//200
            m_cur_pwm.ms = (PWM_TOP/PWM_BILI)-2;//150//200
            OCR1A = (unsigned short)PWM_TOP1-m_cur_pwm.ms;//800,1v
        }
        else if(m_mcur.ms > m_mh_set)
        {
            m_cur_pwm.ms = PWM_TOP-1;
            OCR1A = (unsigned short)PWM_TOP1-m_cur_pwm.ms;//1,5v
        }
        else
        {
            //1v <= out <= 5v
            unsigned short m_offset;
            m_offset = m_mcur.ms-m_ml_set;//��ǰ��õ�ֵ��ȥ��λ����ֵ
            m_temp.ml = m_offset;
            //m_temp.ml *= (unsigned short)((PWM_TOP/5)*4);//�Ⱦ��ƽ��1v��5v
             m_temp.ml *= (unsigned short)((PWM_TOP/PWM_BILI)*(PWM_BILI -1));//�Ⱦ��ƽ��1v��5v
            m_offset = m_mh_set-m_ml_set;//��λ����ֵ��ȥ��λ����ֵ
            m_temp.ml += m_offset>>1;//��������
            m_temp.ml /= m_offset;
            //m_temp.ms[0] += (PWM_TOP/5);//������͵�1v
             m_temp.ms[0] += (PWM_TOP/PWM_BILI);//������͵�1v
            //
            if(m_temp.ms[0] > PWM_TOP)
			{
                m_temp.ms[0] = PWM_TOP-1;
			}
            m_cur_pwm.ms=m_temp.ms[0];
            OCR1A=(unsigned short)PWM_TOP1-m_cur_pwm.ms;
        }
        #endif                
        //
        #if USART_TEST == TRUE
		{
			uart_send_enable=1;//���ڷ���ʹ�� 
		}
        #endif   
    }
}
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
	unsigned char uart_value,sum,i;
    //
	if((UCSRA&0x1c) != 0)
	{
		//���ڽ��ճ��ִ���
		uart_rx_counter = 0;
        uart_frame_head_ok = 0;
		while ( UCSRA & (1<<RXC) ) uart_value = UDR;
	}
	else
	{
		//���ڽ�������
		uart_value = UDR;
        //
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
                        if(GAOLIAOWEI == uart_rx_buf[1])
						{
                            command_gaoliaowei = 1;
							uart_tx_buf[2] = GAOLIAOWEI;
							send_time_count = 0;
						}
                        else if(DILIAOWEI == uart_rx_buf[1])
						{
                            command_diliaowei = 1;
							uart_tx_buf[2] = DILIAOWEI;
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
	}
}
/////////////////////////////////////////////////////////////////
