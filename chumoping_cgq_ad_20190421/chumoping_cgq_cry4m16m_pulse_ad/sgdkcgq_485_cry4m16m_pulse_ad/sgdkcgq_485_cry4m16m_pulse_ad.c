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
1,	  不能勾选'看门狗定时器一直启用'
2,    勾选'使能ISP编程'
3,    不能勾选'禁用外部复位,PC6用作普通IO'
4,    勾选'使能掉电检测'则应该勾选'掉电检测电平为4.0v'
5,    勾选'外部晶振(3-8MHz),启动时间16CK+4ms(或65ms)'
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
    sei();//启动全局中断
    //
	#if USART_TEST == TRUE
	{
		DEBUG_STRING("USART TEST\r\n\0");
	}
	#endif
	//
	turn_on_led_ms(1500);
    //
	SetTimer(MT_SCAN_KEY, 50, -1); //按键扫描
	SetTimer(MT_LED_RUN, 1000, -1); //运行灯闪烁
	//	
	wdt_enable(WDTO_2S);//启动看门狗定时器
	//
    for(;;)
    {
        wdt_reset();//喂狗
        //
        //接收到了正确的数据后,发送应答帧
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
		//避免boot_time读取过程中值被中断更新
		if(boot_time_sub > 0)
		{
			boot_time += boot_time_sub;
			boot_time_sub = 0;
		}
		//
		#if 1
		{
			//单位：分钟
			#define UPDATE_EEPROM_DATA_INTERVAL	60
			//
			if(m_minute > UPDATE_EEPROM_DATA_INTERVAL)
			{
				//刷新一下内存里的参数，同时也会更新eeprom里的错误参数（如果出现错误的话）
				//
				read_para();
				//
				m_minute = 0;
			}
		}
		#endif
		//
        //485通信中断的时候红灯常亮
		#if 1
		{
			if(comm_error_delay > 200)//2秒
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
	//硬件初始化程序,设置固定的端口方向,串口属性,AD属性等
	wdt_disable();
	//I/O设置
	PORTB=0;//先设置所有的口为高阻
	//
    DDRB = _BV(PB1)|_BV(PB2);
    PORTD = _BV(PD2)|_BV(PD3);
	DDRD = _BV(PD7);//根据需要设置输出口
    PORTC = _BV(PC2)|_BV(PC3);
    DDRC = _BV(PC4)|_BV(PC5);
	//定时器0
	TCCR0 = 0x03;//64分频
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
	ICR1 = PWM_TOP;//TOP//4kHz
    OCR1A = PWM_TOP/2;//电压输出初始占空比50%,2.5v
    OCR1B = PWM_LOW;//触发脉冲,占空比LOW_PULSE/PWM_TOP(低电平时间)
	//
	#if USED_AD == TRUE
	//AD
	//ADMUX = _BV(ADLAR);//0010 0000//AREF参考,高8位在ADCH
	ADMUX = 0;//0000 0000//AREF参考
	ADMUX |= ICM7555_AD;
	ADCSRA = 0x8f;//1000 1111//允许ad转换,单次转换方式,允许中断,128分频
	#endif
	//
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
    ms1_counter = 0;
    ms10_counter = 0;
    ms100_counter = 0;
    s1_counter = 0;
    m_ms = 0;
    m_minute = 0;
	//
    m_pulse_counter = 0;//检测脉冲个数
    m_pulse_value.ml = 0;//检测脉冲宽度值
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
	//通过看门狗来复位单片机
	wdt_reset();
	wdt_disable();
	_delay_ms(50);
	cli();//看门狗复位单片机
	wdt_enable(WDTO_120MS);
	for(;;);//等待单片机重启
}
////////////////////////////////////////////////////////////////
void set_default_para(void)
{
	//执行恢复出厂默认参数操作
	//
	cli();
	//
	//脉冲
	eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,PULSE_TOP);
	eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,PULSE_BOTTOM);

	eeprom_write_word_user((uint16_t *)&m_para[0].smoothingDate,PARA_SETED_SMOOTHING);
	//
	//参数设置标志
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
	wdt_reset_mcu();//重启
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
    //开始发送数据
    //_delay_us(400);
	_delay_us(200);
    b485en = 1;//发送使能
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
			//如果标志不正常，设置默认参数
			set_default_para();
			return;//正常情况下代码执行不到这里，显性的表示执行过程
		}
	}
	#else
	{
		if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
		{
			if(PARA_SETED_FLAG != eeprom_read_word_user((const uint16_t *)&m_para[0].e_para_seted_flag))
			{
				//如果标志不正常，设置默认参数
				set_default_para();
				return;//正常情况下代码执行不到这里，显性的表示执行过程
			}
		}		
	}
	#endif
	//
	//版本号
	if(version_h!=eeprom_read_word_user((const uint16_t *)(&m_para[0].e_version_h)))
	{
		eeprom_write_word_user((uint16_t *)(&m_para[0].e_version_h),version_h);
	}
	if(version_l!=eeprom_read_word_user((const uint16_t *)(&m_para[0].e_version_l)))
	{
		eeprom_write_word_user((uint16_t *)(&m_para[0].e_version_l),version_l);
	}
	//
    //读非易失参数,高低料位设置值
    m_mh_set = eeprom_read_word_user((const uint16_t *)&m_para[0].e_mh);
    if(m_mh_set > PULSE_TOP)//输出5v
	{
        m_mh_set = PULSE_TOP;
	}
    else if(m_mh_set < PULSE_BOTTOM)
	{
        m_mh_set = PULSE_BOTTOM+3000;
	}

    smoothingDate = eeprom_read_word_user((const uint16_t *)&m_para[0].smoothingDate);
    if(smoothingDate >= 10)//输出5v
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
        //检测到高料位按键操作
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
						//m_mh_set = m_mcur.ms; //标定起作用，延时记录
						_delay_ms(50);
					}
				}
				#endif
				m_mh_set = m_mcur.ms; //如果禁止中断，这个采样值不会更新
	            eeprom_write_word_user((uint16_t *)&m_para[0].e_mh, m_mh_set);
				_delay_ms(20);
	        }
	        //sei();
		}
    }
    else if(0 == bkml)
    {
        //检测到低料位按键操作
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
						//m_ml_set = m_mcur.ms;//标定起作用，延时记录
						_delay_ms(50);
					}
				}
				#endif
				m_ml_set = m_mcur.ms; //如果禁止中断，这个采样值不会更新
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
        m_mh_set = m_mcur.ms;//标定起作用，延时记录
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
        m_ml_set = m_mcur.ms;//标定起作用，延时记录
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
	UCHAR2 tmp;
    uart_tx_buf[0] = 0xaa;
    uart_tx_buf[1] = ADDR_CGQ;//应答地址
	//
	tmp.ms = OCR1A;
    uart_tx_buf[2] = tmp.mc[0]; //输出的脉宽比较值
    uart_tx_buf[3] = tmp.mc[1];
    tmp.ms = PWM_TOP;
    uart_tx_buf[4] = tmp.mc[0]; //pwm周期
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
    //开始发送数据
    _delay_us(400);
	//_delay_us(200);
    b485en = 1;//发送使能
    _delay_us(100);
    UDR = uart_tx_buf[0];
    uart_tx_addr = 1;
}
/////////////////////////////////////////////////////////////////

//smoothingDate

unsigned short pulse_value_handling(unsigned short val)
{
	//对输入的波形做平滑处理，消除短时间的波动，可能反应稍微迟钝
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
	//对输入的波形做平滑处理，消除短时间的波动，可能反应稍微迟钝
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
			m_cgq_ack=0;//清除应答命令
		}
		break;
		case MT_LED_RUN:
		{
			//运行灯闪烁
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
	m_mcur.ms = pulse_value_handling(m_mcur.ms);//平滑处理
	//
	if(m_mcur.ms > PULSE_TOP)
	{
		m_mcur.ms = PULSE_TOP;
	}
	//
	//OCR1A = PWM_TOP/2; //for test
	//return;
	//
	//计算pwm电压输出
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
		m_offset = m_mcur.ms-m_ml_set;//当前测得的值减去低位设置值
		m_temp.ml = m_offset;
		m_temp.ml *= (unsigned short)((PWM_TOP/5)*4);//等距的平分1v到5v
		m_offset = m_mh_set-m_ml_set;//高位设置值减去低位设置值
		m_temp.ml += m_offset>>1;//四舍五入
		m_temp.ml /= m_offset;
		m_temp.ms[0] += (PWM_TOP/5);//加入最低的1v
		//
		if(m_temp.ms[0] >= PWM_TOP)
		{
			m_temp.ms[0] = PWM_TOP-1;
		}
		//
		#if USED_AD == TRUE //大约800ms
		{
			m_mcur_ad.ms = ad_value_handling(m_mcur_ad.ms);//平滑处理
			////////////////////// AD补充 ///////////////////////////////////////////////
			if(m_cur_pwm.ms == m_temp.ms[0])
			{
				//前一次输出的pwm脉冲值跟这次计算的一样，用AD值做微调判断是否需要调整
				//
				if(m_mcur_ad.ms != m_cur_per_ad.ms)
				{
					//AD值有变化
					unsigned short m_step;
					m_step = 65535/PWM_TOP;
					//
					if(m_mcur_ad.ms > m_cur_per_ad.ms)
					{
						//ad当前值大于前次pwm输出时的值
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
						//ad当前值小于前次pwm输出时的值
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
				//本次计算的pwm脉冲值跟前次输出的值不同，暂时不考虑AD值
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
		m_cur_per_ad.ms = m_mcur_ad.ms;//保留备用
	}
	#endif
	//
	#endif
	//
	#if USART_TEST == TRUE
	{
		uart_send_enable=1;//串口发送使能
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
	//一阶RC低通数字滤波，采样频率10KHz，截止频率1KHz
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
	//软件中用的采样率是10KHz
	if(m_pulse_counter > ((unsigned short)32*1024))//采集刷新时间约2秒      //32 //16 //8        //4096=2^12  //2048=2^11  //1024=2^10  //512=2^9  //256=2^8
	{
		m_pulse_value.ml <<= (6-5);                       //5   //4  //3       //16-12      //16-11      //16-10      //16-9     //16-8
		m_pulse_value.ml <<= SAMPLE_SHIFT_BIT_NUM;
		m_mcur.ms = m_pulse_value.ms[1];//丢弃低16位,相当于除以2^16
		//
		m_pulse_counter = 0;
		m_pulse_value.ml = 0;
		//
		liaowei_sample_ok_handling();
	}
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
//外中断1,下降沿触发中断,输出正脉冲的结束边沿
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
//AD转换完成，检测ICM7555输出PWM电平
ISR(ADC_vect)
{
	UCHAR2 tmp;
	tmp.mc[0] = ADCL; //必须先读
	tmp.mc[1] = ADCH;
	tmp.ms &= 0x03ff;
	//
	m_ad_value.ml += tmp.ms;//累加ad值，丢弃低两位
	//
	m_ad_counter++;
	if(m_ad_counter > 128)//采集刷新时间约128ms
	{
		//10位数据，累加128（2^7）个数值，保留高16位
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
	unsigned char uart_value,sum,i,temp;
    //
	if((UCSRA&0x1c) != 0)
	{
		//串口接收出现错误
		uart_rx_counter = 0;
        uart_frame_head_ok = 0;
		while ( UCSRA & (1<<RXC) ) 
		{
			uart_value = UDR;
		}
	}
	else
	{
		//串口接收正常
		uart_value = UDR;
        //
#if IS_MMR_USED == TRUE
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
#else   //IS_MMR_USED
        if(1 == uart_frame_head_ok)
        {
	        uart_rx_buf[uart_rx_counter++] = uart_value;
	        if(uart_rx_counter > 6)//接收6个字节有效参数和一个字节校验和
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
				        uart_send_enable = 1;//串口发送使能
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
