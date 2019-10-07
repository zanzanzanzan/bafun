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
2,��ѡ'ʹ��ISP���'
3,���ܹ�ѡ'�����ⲿ��λ,PC6������ͨIO'
4,�����ѡ'ʹ�ܵ�����'��Ӧ�ù�ѡ'�������ƽΪ4.0v'
5,��ѡ'�ⲿ����(3-8MHz),����ʱ��16CK+4ms(��65ms)'
*/
////////////////////////////////////////////////////////////
#include "sgdkcgq_485_modify_IO.h"
#include "math.h"
#include "soft_timer.h"
////////////////////////////////////////////////////////////

volatile unsigned char command_gaoliaowei =0;
volatile unsigned char command_diliaowei =0;

volatile unsigned char uart_tx_addr;
unsigned int send_time_count = 0;
unsigned char uart_tx_buf[10],uart_rx_buf[10];
#define b485en     BIT(PORTC,PC5) //out,    11
unsigned char uart_rx_counter;
volatile unsigned char uart_frame_head_ok;

#define ADDR_CGQ            4

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



    uart_tx_buf[5] = 0;
    uart_tx_buf[6] = 0;

    uart_tx_buf[7] = 0;
	uart_tx_buf[8] = 0;
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

int main()
{    
    cli();
    hardward_init();
    softward_init();
    read_para();
    sei();//����ȫ���ж�
    //
    bled = 1;
	SetTimer(MT_LED_KEY_TURN_OFF,1500,0);
    //
	SetTimer(MT_LED_DEBUG_BLINK,500,-1);//����˸
	//
    for(;;)
    {
        wdt_reset();//ι��
        //
		//10ms
		if(TRUE == task_scheduling_ok)
		{
			task_scheduling_ok = FALSE;
			//
			app_task();
		}
		//
        /////////////////////////////////////////
        MCUCR |= 0x80;//����ʹ��
        asm volatile("sleep");
        MCUCR &= 0x7f;//���߽�ֹ
        /////////////////////////////////////////
		if (send_time_count < 20)
		{
			send_time_count ++; //50ms
			if (send_time_count >3 && send_time_count%3 ==0)
			{
				uart_tx_zan();
			}
		}
    }
}
/////////////////////////////////////////////////////////////
void app_task(void)
{
#if 0      //�����������ǲ���ͬʱ�õģ��õ�һ����ֵΪ1���õڶ�����ֵΪ0
	check_pin_function();
#else	
	check_liaowei_and_output_bp_hz();
#endif
	//
    scan_key();
	//
	timer_run();
}
////////////////////////////////////////////////////////////////
void check_liaowei_and_output_bp_hz(void)
{
	//OCR1A = (unsigned short)PWM_TOP-m_cur_pwm
	//0 < OCR1A < PWM_TOP
	//OCR1A��ֵԽ�������ѹԽС
	//
	if(FALSE == bp_is_out())
	{
		//BPû�����
		if(m_cur_pwm > PWM_4V25_17MA)
		{
			//����17mA�����bp��hz
			turn_on_bp_out();
			turn_on_hz_out();
		}
	}
	else
	{
		//BP�Ѿ������״̬
		if(m_cur_pwm < PWM_3V75_15MA)
		{
			turn_off_bp_out();
			turn_off_hz_out();
		}
	}	
}
////////////////////////////////////////////////////////////////
void check_pin_function(void)
{
	//OCR1A = (unsigned short)PWM_TOP-m_cur_pwm
	//0 < OCR1A < PWM_TOP
	//OCR1A��ֵԽ�������ѹԽС
	//
	if(FALSE == bp_is_out())
	{
		//BPû�����
		if((0 == zk_in) && (0 == zd_in) && (m_cur_pwm > PWM_2V25_9MA))
		{
			//zk��zdͬʱ�����룬��4-20mA�������9mA
			turn_on_bp_out();
			turn_off_hz_out();
			//
			SetTimer(MT_HZ_OUT,1000,0);//�ȴ�ʱ������HZ
		}
		else
		{
			//��������δͬʱ���㣬�ر����
			turn_off_bp_out();
			turn_off_hz_out();
			//
			KillTimer(MT_MB_IN_CHECK);
		}
	}
	else
	{
		//BP�Ѿ������״̬
		if((1 == zk_in) || (1 == zd_in) || (m_cur_pwm <= PWM_1V0_4MA))
		{
			turn_off_bp_out();
			turn_off_hz_out();
			//
			KillTimer(MT_MB_IN_CHECK);
		}
	}
}
////////////////////////////////////////////////////////////////
void OnTimer(unsigned short id)
{
	switch(id)
	{
	case MT_LED_DEBUG_BLINK:
		if(1 == led_debug_out)
		{
			led_debug_out = 0;
		}
		else
		{
			led_debug_out = 1;
		}
		break;

	case MT_LED_KEY_TURN_OFF:
		bled = 0;//�رհ�����
		break;

	case MT_HZ_OUT:
		if(TRUE == bp_is_out())
		{
			turn_on_hz_out();
			//
			SetTimer(MT_MB_IN_CHECK,3000,0);
		}
		break;

	case MT_MB_IN_CHECK:
		if(TRUE == bp_and_hz_is_out())
		{
			//������bp��hz���Ѿ����
			//
			if(1 == mb_in)//���mb����
			{
				//û��mb�ź�
				turn_off_bp_out();
				turn_off_hz_out();
				//
				KillTimer(MT_MB_IN_CHECK);//�رռ�⣨��ʵ���Զ��رգ��ô�ֻ�����ֵĹر�һ�Σ�
			}
			else
			{
				//��ǰmb�ź�����Ч�ģ������ٴμ��MB��ʱ��
				SetTimer(MT_MB_IN_CHECK,200,0);
			}
		}
		break;
	}
}
////////////////////////////////////////////////////////////////
void hardward_init(void)
{
	//Ӳ����ʼ������,���ù̶��Ķ˿ڷ���,��������,AD���Ե�
	wdt_disable();
	//I/O����
	DDRB=0;
	PORTB=0;//���������еĿ�Ϊ����
	DDRC=0;
	PORTC=0;
	DDRD=0;
	PORTD=0;
	//
	PORTB = _BV(PB0);
    DDRB = _BV(PB1)|_BV(PB2);
	//
    PORTD = _BV(PD0)|_BV(PD1)|_BV(PD2)|_BV(PD3)|_BV(PD4)|_BV(PD5)|_BV(PD6)|_BV(PD7);
	DDRD = _BV(PD0)|_BV(PD1)|_BV(PD4)|_BV(PD5);
	//
    PORTC = _BV(PC2)|_BV(PC3)|_BV(PC5);

	DDRC = _BV(PC5);//485EN
	
    DDRC = _BV(PC1)|_BV(PC4)|_BV(PC5);
	//
	//��ʱ��0
	TCCR0 = 0x03;//4MHzʱ��,64��Ƶ
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
    //���ж�
    MCUCR &= 0xf0;
    MCUCR |= 0x08;//���ж�1�½��ش���
    GICR &= 0x3f;
    GICR |= _BV(INT1);//���ж�1ʹ��
	
	
	
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
    m_pulse_value_bak.ms[0] = PWM_TOP/2;
    m_pulse_value_bak.ms[1] = PWM_TOP/2;
    m_pulse_value_bak.ms[2] = PWM_TOP/2;
    m_pulse_value_bak.ms[3] = PWM_TOP/2;
	//
	m_cgq_ack=0;
	m_cgq_ack_delay=0;
	task_scheduling_ok = FALSE;
	boot_time = 0;
    //
	turn_off_bp_out();
	turn_off_hz_out();
	//
	init_soft_timer();
	//
	wdt_enable(WDTO_2S);//�������Ź���ʱ��
}
////////////////////////////////////////////////////////////////
void turn_on_bp_out(void)
{
	pin_bp_out = 0;//���bp�ź�
	//pd_bp_out = 1;
	pin_bp_out_bak = 0;
	//pd_bp_out_bak = 1;
}
////////////////////////////////////////////////////////////////
void turn_off_bp_out(void)
{
	pin_bp_out = 1;//�ر�bp�ź�
	//pd_bp_out = 0;
	pin_bp_out_bak = 1;
	//pd_bp_out_bak = 0;
}
////////////////////////////////////////////////////////////////
void turn_on_hz_out(void)
{
	//pin_hz_out = 0;//���hz�ź�
	//pd_hz_out = 1;
	//pin_hz_out_bak = 0;
	//pd_hz_out_bak = 1;
}
////////////////////////////////////////////////////////////////
void turn_off_hz_out(void)
{
	//pin_hz_out = 1;//�ر�hz�ź�
	//pd_hz_out = 0;
	//pin_hz_out_bak = 1;
	//pd_hz_out_bak = 0;
}
////////////////////////////////////////////////////////////////
unsigned char bp_is_out(void)
{
	if((0 == pin_bp_out) && (0 == pin_bp_out_bak))
		return TRUE;
	else
		return FALSE;
}
////////////////////////////////////////////////////////////////
unsigned char hz_is_out(void)
{
	if((0 == pin_hz_out) && (0 == pin_hz_out_bak))
		return TRUE;
	else
		return FALSE;
}
////////////////////////////////////////////////////////////////
unsigned char bp_and_hz_is_out(void)
{
	if((0 == pin_bp_out) && (0 == pin_bp_out_bak) && 
	   (0 == pin_hz_out) && (0 == pin_hz_out_bak))
	{
		return TRUE;
	}
	else
		return FALSE;
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
	//����
	eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,PULSE_TOP);
	eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,PULSE_BOTTOM);
	//
	//�������ñ�־
	eeprom_write_word_user((uint16_t *)&m_para[0].e_para_seted_flag,PARA_SETED_FLAG);
	//
	_delay_ms(100);
	wdt_reset_mcu();//����
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
	//
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

	if (command_gaoliaowei == 1)
	{
		command_gaoliaowei =0;
		cli();

		m_mh_set = m_mcur;
		eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,m_mh_set);
		//
		SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//����ʱ

		m_pulse_counter = 0;
		m_pulse_value.ml = 0;
		sei();

	}
	
	if (command_diliaowei == 1)
	{
		cli();

		m_ml_set = m_mcur;
		eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,m_ml_set);
		//
		SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//����ʱ

		m_pulse_counter = 0;
		m_pulse_value.ml = 0;
		sei();

	}

	

    if(0 == bkmh )
    {
		command_gaoliaowei =0;
        //��⵽����λ��������
		for(i=50; i>0; i--)
		{
        	_delay_ms(1);
			if(0 != bkmh)
				break;
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
	            m_mh_set = m_mcur;
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,m_mh_set);
				//
	            SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//����ʱ
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
				break;
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
	            m_ml_set = m_mcur;
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,m_ml_set);
				//
				SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//����ʱ
	        }
	        m_pulse_counter = 0;
	        m_pulse_value.ml = 0;
	        sei();
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
                OCR1A--;
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
                OCR1A++;
        }
    }
#endif
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
	ms1_counter++;
    m_ms++;
    //
    if(ms1_counter > 9)
    {
        ms1_counter = 0;
        //10ms
		boot_time++;
		task_scheduling_ok = TRUE;
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
		m_mcur = m_pulse_value.ms[1];//������16λ,�൱�ڳ���2^16
		//
		m_mcur = pulse_value_handling(m_mcur);//ƽ������
		//
		if(m_mcur > PULSE_TOP)
		{
			m_mcur = PULSE_TOP;
		}
		m_pulse_counter = 0;
		m_pulse_value.ml = 0;
		//
		//OCR1A = PWM_TOP/2; //for test
		//return;
		//
		//����pwm��ѹ���
		#ifndef __TEST__
		if(m_mcur < m_ml_set)
		{
			m_cur_pwm = (PWM_TOP/5)-2;//150//200
			OCR1A = (unsigned short)PWM_TOP-m_cur_pwm;//800,1v
		}
		else if(m_mcur > m_mh_set)
		{
			m_cur_pwm = PWM_TOP-1;
			OCR1A = (unsigned short)PWM_TOP-m_cur_pwm;//1,5v
		}
		else
		{
			//1v <= out <= 5v
			unsigned short m_offset;
			m_offset = m_mcur-m_ml_set;//��ǰ��õ�ֵ��ȥ��λ����ֵ
			m_temp.ml = m_offset;
			m_temp.ml *= (unsigned short)((PWM_TOP/5)*4);//�Ⱦ��ƽ��1v��5v
			m_offset = m_mh_set-m_ml_set;//��λ����ֵ��ȥ��λ����ֵ
			m_temp.ml += m_offset>>1;//��������
			m_temp.ml /= m_offset;
			m_temp.ms[0] += (PWM_TOP/5);//������͵�1v
			//
			if(m_temp.ms[0] > PWM_TOP)
			{
				m_temp.ms[0] = PWM_TOP-1;
			}
			m_cur_pwm=m_temp.ms[0];
			OCR1A=(unsigned short)PWM_TOP-m_cur_pwm;
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
					//	comm_error_delay = 0;
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
							//command_gaoliaowei = 0;
							//command_diliaowei = 0;
						}
						//
						//uart_send_enable = 1;//���ڷ���ʹ��
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
//////////////////////////////////////
/////////////////////////////////////////////////////////////////
