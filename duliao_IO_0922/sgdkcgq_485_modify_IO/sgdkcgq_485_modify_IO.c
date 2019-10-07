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
2,勾选'使能ISP编程'
3,不能勾选'禁用外部复位,PC6用作普通IO'
4,如果勾选'使能掉电检测'则应该勾选'掉电检测电平为4.0v'
5,勾选'外部晶振(3-8MHz),启动时间16CK+4ms(或65ms)'
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
	SetTimer(MT_LED_KEY_TURN_OFF,1500,0);
    //
	SetTimer(MT_LED_DEBUG_BLINK,500,-1);//灯闪烁
	//
    for(;;)
    {
        wdt_reset();//喂狗
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
        MCUCR |= 0x80;//休眠使能
        asm volatile("sleep");
        MCUCR &= 0x7f;//休眠禁止
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
#if 0      //这两个函数是不能同时用的，用第一个，值为1，用第二个，值为0
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
	//OCR1A的值越大输出电压越小
	//
	if(FALSE == bp_is_out())
	{
		//BP没有输出
		if(m_cur_pwm > PWM_4V25_17MA)
		{
			//超过17mA，输出bp和hz
			turn_on_bp_out();
			turn_on_hz_out();
		}
	}
	else
	{
		//BP已经在输出状态
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
	//OCR1A的值越大输出电压越小
	//
	if(FALSE == bp_is_out())
	{
		//BP没有输出
		if((0 == zk_in) && (0 == zd_in) && (m_cur_pwm > PWM_2V25_9MA))
		{
			//zk和zd同时有输入，且4-20mA输出超过9mA
			turn_on_bp_out();
			turn_off_hz_out();
			//
			SetTimer(MT_HZ_OUT,1000,0);//等待时间后输出HZ
		}
		else
		{
			//三个条件未同时满足，关闭输出
			turn_off_bp_out();
			turn_off_hz_out();
			//
			KillTimer(MT_MB_IN_CHECK);
		}
	}
	else
	{
		//BP已经在输出状态
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
		bled = 0;//关闭按键灯
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
			//必须是bp和hz都已经输出
			//
			if(1 == mb_in)//检测mb输入
			{
				//没有mb信号
				turn_off_bp_out();
				turn_off_hz_out();
				//
				KillTimer(MT_MB_IN_CHECK);//关闭检测（其实会自动关闭，该处只是显现的关闭一次）
			}
			else
			{
				//当前mb信号是有效的，设置再次检测MB的时间
				SetTimer(MT_MB_IN_CHECK,200,0);
			}
		}
		break;
	}
}
////////////////////////////////////////////////////////////////
void hardward_init(void)
{
	//硬件初始化程序,设置固定的端口方向,串口属性,AD属性等
	wdt_disable();
	//I/O设置
	DDRB=0;
	PORTB=0;//先设置所有的口为高阻
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
	//定时器0
	TCCR0 = 0x03;//4MHz时钟,64分频
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
    //外中断
    MCUCR &= 0xf0;
    MCUCR |= 0x08;//外中断1下降沿触发
    GICR &= 0x3f;
    GICR |= _BV(INT1);//外中断1使能
	
	
	
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
	wdt_enable(WDTO_2S);//启动看门狗定时器
}
////////////////////////////////////////////////////////////////
void turn_on_bp_out(void)
{
	pin_bp_out = 0;//输出bp信号
	//pd_bp_out = 1;
	pin_bp_out_bak = 0;
	//pd_bp_out_bak = 1;
}
////////////////////////////////////////////////////////////////
void turn_off_bp_out(void)
{
	pin_bp_out = 1;//关闭bp信号
	//pd_bp_out = 0;
	pin_bp_out_bak = 1;
	//pd_bp_out_bak = 0;
}
////////////////////////////////////////////////////////////////
void turn_on_hz_out(void)
{
	//pin_hz_out = 0;//输出hz信号
	//pd_hz_out = 1;
	//pin_hz_out_bak = 0;
	//pd_hz_out_bak = 1;
}
////////////////////////////////////////////////////////////////
void turn_off_hz_out(void)
{
	//pin_hz_out = 1;//关闭hz信号
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
	//脉冲
	eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,PULSE_TOP);
	eeprom_write_word_user((uint16_t *)&m_para[0].e_ml,PULSE_BOTTOM);
	//
	//参数设置标志
	eeprom_write_word_user((uint16_t *)&m_para[0].e_para_seted_flag,PARA_SETED_FLAG);
	//
	_delay_ms(100);
	wdt_reset_mcu();//重启
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
	//
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

	if (command_gaoliaowei == 1)
	{
		command_gaoliaowei =0;
		cli();

		m_mh_set = m_mcur;
		eeprom_write_word_user((uint16_t *)&m_para[0].e_mh,m_mh_set);
		//
		SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//灯延时

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
		SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//灯延时

		m_pulse_counter = 0;
		m_pulse_value.ml = 0;
		sei();

	}

	

    if(0 == bkmh )
    {
		command_gaoliaowei =0;
        //检测到高料位按键操作
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
	            SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//灯延时
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
				SetTimer(MT_LED_KEY_TURN_OFF,1000,0);//灯延时
	        }
	        m_pulse_counter = 0;
	        m_pulse_value.ml = 0;
	        sei();
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
                OCR1A--;
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
                OCR1A++;
        }
    }
#endif
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
		m_mcur = m_pulse_value.ms[1];//丢弃低16位,相当于除以2^16
		//
		m_mcur = pulse_value_handling(m_mcur);//平滑处理
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
		//计算pwm电压输出
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
			m_offset = m_mcur-m_ml_set;//当前测得的值减去低位设置值
			m_temp.ml = m_offset;
			m_temp.ml *= (unsigned short)((PWM_TOP/5)*4);//等距的平分1v到5v
			m_offset = m_mh_set-m_ml_set;//高位设置值减去低位设置值
			m_temp.ml += m_offset>>1;//四舍五入
			m_temp.ml /= m_offset;
			m_temp.ms[0] += (PWM_TOP/5);//加入最低的1v
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
						//uart_send_enable = 1;//串口发送使能
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
