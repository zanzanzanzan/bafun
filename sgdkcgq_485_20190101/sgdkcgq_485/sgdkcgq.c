/*//////////////////////////////////////////////////////////
*                                                          *
*     四辊电控传感器控制程序                               *
*     MCU:ATMega8(L)                                       *
*     CRY:外部晶振4MHz                                     *
*     时间:20120501                                        *
*     增加掉电数据保护                                     *
//////////////////////////////////////////////////////////*/
/*
熔丝位配置注意事项(基本的):
1,不能勾选'看门狗定时器一直启用'
2,    勾选'使能ISP编程'
3,不能勾选'禁用外部复位,PC6用作普通IO'
4,    勾选'使能掉电检测'则应该勾选'掉电检测电平为4.0v'
5,    勾选'外部晶振(3-8MHz),启动时间16CK+4ms(或65ms)'
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
    //中断方式发生串口数据 
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


	uart_tx_buf[1]  = 3;  //地址
 	//uart_tx_buf[2] = m_ml_set >> 8;//别的地方进行填充

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
    //开始发送数据
    //_delay_us(400);
	_delay_us(200);
    b485en = 1;//发送使能
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
    sei();//启动全局中断
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
        wdt_reset();//喂狗
        //
        //扫描键盘
        if(1 == scan_key_enable)
        {
            scan_key_enable = 0;
            scan_key();
        }
        //接收到了正确的数据后,发送应答帧
        if(1 == uart_send_enable)
        {
            uart_send_enable = 0;
            //uart_tx();
        }
        //485通信中断的时候红灯常亮
        if(comm_error_delay > 200)//2秒
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
        MCUCR |= 0x80;//休眠使能
        asm volatile("sleep");
        MCUCR &= 0x7f;//休眠禁止
        /////////////////////////////////////////
    }
}
/////////////////////////////////////////////////////////////
void hardward_init(void)
{
	//硬件初始化程序,设置固定的端口方向,串口属性,AD属性等
	wdt_disable();
	//I/O设置
	PORTB=0;//先设置所有的口为高阻
	//
    DDRB = _BV(PB1)|_BV(PB2);
    PORTD = _BV(PD2)|_BV(PD3);
	DDRD = _BV(PD7);//根据需要设置输出口
    PORTC = _BV(PC2)|_BV(PC3);
    DDRC = _BV(PC4);
	//定时器0
	TCCR0 = 0x03;//,64分频
	#if F_CPU == F_4M
	{		

		TCNT0 = 255-62;//4MHz时钟,计数62次,约1毫秒
	}
	#elif F_CPU == F_16M
	{
		TCNT0 = 255-250;//16MHz时钟,计数250次,约1毫秒
	}
	#else
	{
		#error "F_CPU error"
	}
	#endif
	TIMSK |=_BV(TOIE0);//定时器0溢出中断使能
	//定时器1
	TCCR1A = 0xf2;//oc1a,oc1b匹配时置1,达到TOP时清零
	TCCR1B = 0x19;//时钟不分频
	ICR1 = PWM_TOP1;//TOP//4kHz
    OCR1A = PWM_TOP1/2;//电压输出初始占空比50%,2.5v
    OCR1B = PWM_LOW;//触发脉冲,占空比LOW_PULSE/PWM_TOP(低电平时间)
    //外中断
    MCUCR &= 0xf0;
    MCUCR |= 0x08;//外中断1下降沿触发
    GICR &= 0x3f;
    GICR |= _BV(INT1);//外中断1使能
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
		UCSRA = (1<<U2X);//波特率分频因子从16降到8
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);//接收和发送中断使能
		UCSRC = (1<<URSEL)|(3<<UCSZ0);//8N1
		b485en = 0;//接收使能,发送禁止
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
		UCSRA = (1<<U2X);//波特率分频因子从16降到8
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);//接收中断使能
		UCSRC = (1<<URSEL)|(3<<UCSZ0);//8N1
		b485en = 0;//接收使能,发送禁止
	}
	#endif
    //
    _delay_ms(5);
}
////////////////////////////////////////////////////////////////
void softward_init(void)
{	
    MCUCR &= 0x0f;//休眠禁止,空闲模式
    //
    m_pulse_counter = 0;//检测脉冲个数
    m_pulse_value.ml = 0;//检测脉冲宽度值
    //
	m_cgq_ack=0;
	m_cgq_ack_delay=0;
    //
    m_pulse_value_bak.ms[0] = PULSE_TOP/2;
    m_pulse_value_bak.ms[1] = PULSE_TOP/2;
    m_pulse_value_bak.ms[2] = PULSE_TOP/2;
    m_pulse_value_bak.ms[3] = PULSE_TOP/2;
	//
	wdt_enable(WDTO_2S);//启动看门狗定时器
}
////////////////////////////////////////////////////////////////
void read_para()
{
	if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
	{
		//如果标志不正常，设置默认参数
		set_default_para();
		return;//正常情况下代码执行不到这里，显性的表示执行过程
	}
	//读非易失参数,高低料位设置值
    m_mh_set = eeprom_read_word_user((const uint16_t *)&m_para[0].e_mh);
    if(m_mh_set > PULSE_TOP)//输出5v
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
        //检测到高料位按键操作
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
	            m_led_delay = 150;//灯延时
	        }
	        m_pulse_counter = 0;
	        m_pulse_value.ml = 0;
	        sei();
		}
    }
    else if(0 == bkml)
    {
        //检测到低料位按键操作
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
	            m_led_delay = 150;//灯延时
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

		//将对噶两位机型反馈
		
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
        //电压输出逐渐升高
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
        //电压输出逐渐降低
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
	b485en = 1;//发送使能
	_delay_ms(5);
	//
	if(NULL == p)
	{
		//等待发送缓冲器为空
		while (!(UCSRA & (1<<UDRE)));
		//将数据放入缓冲器，发送数据
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
    //中断方式发生串口数据 
    //
    uart_tx_buf[0] = 0xaa;
    uart_tx_buf[1] = ADDR_CGQ;//应答地址
    uart_tx_buf[2] = m_cur_pwm.mc[0];
    uart_tx_buf[3] = m_cur_pwm.mc[1];
    uart_tx_buf[4] = m_mcur.mc[0];
    uart_tx_buf[5] = m_mcur.mc[1];
    uart_tx_buf[6]=m_cgq_ack;
	if(m_cgq_ack_delay > 4)//500ms
	{
		m_cgq_ack_delay=0;
		m_cgq_ack=0;//清除应答命令
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
    //开始发送数据
    _delay_us(400);
    b485en = 1;//发送使能
    _delay_us(100);
    UDR = uart_tx_buf[0];
    uart_tx_addr = 1;
}
/////////////////////////////////////////////////////////////////
unsigned short pulse_value_handling(unsigned short val)
{
	//对输入的波形做平滑处理，消除短时间的波动，可能反应稍微迟钝
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
///////////////       中断服务程序       ////////////////////////
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
//
//定时器0作为全局定时器,用来管理所有任务的调度
//
ISR(TIMER0_OVF_vect/*,ISR_NOBLOCK*/)//加入ISR_NOBLOCK属性,在中断服务程序中允许中断套嵌
{
	#if F_CPU == F_4M
	{
		TCNT0 = 255-62;//4MHz时钟,64分频,计数62次,定时约1毫秒
	}
	#elif F_CPU == F_16M
	{
		TCNT0 = 255-250;//16MHz时钟,64分频,计数62次,定时约1毫秒
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
        scan_key_enable = 1;//允许扫描按键
        comm_error_delay++;
        if(comm_error_delay > 250)
		{
            comm_error_delay = 250;
		}
        //
        m_led_delay++;
        if(m_led_delay > 249)//最长可以延时2.5秒
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
			m_cgq_ack_delay++;//传感器应答延时
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
//外中断1,下降沿触发中断,输出正脉冲的结束边沿
//
ISR(INT1_vect)
{
    UCHAR4 m_temp;
    //
    m_temp.ms[0] = TCNT1;
    m_pulse_value.ml += m_temp.ms[0];
    m_pulse_counter++;
	//
    //软件中用的采样率是10KHz
    if(m_pulse_counter > (8*1024))//采集刷新时间约1秒       //4096=2^12  //2048=2^11  //1024=2^10  //512=2^9  //256=2^8
    {
        m_pulse_value.ml <<= (6-3);                        //16-12      //16-11      //16-10      //16-9     //16-8
		m_pulse_value.ml <<= SAMPLE_SHIFT_BIT_NUM;
        m_mcur.ms = m_pulse_value.ms[1];//丢弃低16位,相当于除以2^16
		//
		m_mcur.ms = pulse_value_handling(m_mcur.ms);//平滑处理
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
        //计算pwm电压输出
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
            m_offset = m_mcur.ms-m_ml_set;//当前测得的值减去低位设置值
            m_temp.ml = m_offset;
            //m_temp.ml *= (unsigned short)((PWM_TOP/5)*4);//等距的平分1v到5v
             m_temp.ml *= (unsigned short)((PWM_TOP/PWM_BILI)*(PWM_BILI -1));//等距的平分1v到5v
            m_offset = m_mh_set-m_ml_set;//高位设置值减去低位设置值
            m_temp.ml += m_offset>>1;//四舍五入
            m_temp.ml /= m_offset;
            //m_temp.ms[0] += (PWM_TOP/5);//加入最低的1v
             m_temp.ms[0] += (PWM_TOP/PWM_BILI);//加入最低的1v
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
			uart_send_enable=1;//串口发送使能 
		}
        #endif   
    }
}
/////////////////////////////////////////////////////////////////
//
//串口发送中断
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
//串口接收中断
//
ISR(USART_RXC_vect)
{
	unsigned char uart_value,sum,i;
    //
	if((UCSRA&0x1c) != 0)
	{
		//串口接收出现错误
		uart_rx_counter = 0;
        uart_frame_head_ok = 0;
		while ( UCSRA & (1<<RXC) ) uart_value = UDR;
	}
	else
	{
		//串口接收正常
		uart_value = UDR;
        //
        if(1 == uart_frame_head_ok)
        {
            uart_rx_buf[uart_rx_counter++] = uart_value;
            if(uart_rx_counter > 8)//接收6个字节有效参数和一个字节校验和
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
                        uart_send_enable = 1;//串口发送使能
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
