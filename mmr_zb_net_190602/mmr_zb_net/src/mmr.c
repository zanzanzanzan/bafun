/*///////////////////////////////////////////////////////////////
*																*
*     四辊电控主板控制程序										*
*																*
*     MCU:ATMega644pa											*
*     CRY:外部晶振4MHz											*
*																*
*     改单电控电磁阀、单继电器、增加磁性开关输入				*
*     修改离轧键为磨辊轧距调节锁定键							*
*     修改手动切换远程时不取消手动状态							*
*     修改键盘自动低位停机后再启动时取消自动状态				*
*																*
*     更换MCU：ATMega8(L) --> ATMega32(a)						*
*     增加两路输入，一路输出									*
*	  增加一路与上位机通信的rs485接口，波特率38400Bd			*
*     与上位机通信的软协议使用Modbus							*
*     MB检测方式由电平检测改为脉冲计数							*
*     其它一些显示和操作上的小改动								*
*     修改时间:20150610											*
*																*
*     更换MCU：ATMega32(a) --> ATMega644pa						*
*     增加网络支持（w5500）										*
*     bear														*
*     20160514													*
*																*
*	  复位向量要改到引导区										*
*     需要bootloader的软件，注意要把引导区设置为2K字（4K字节）  *
*	  bear														*
*	  20160718													*
*																*
*	  增加网络支持												*
*	  bear														*
*	  20161112													*
*																*
////////////////////////////////////////////////////////////////*/
/*
熔丝位配置注意事项(基本的):
1,    不要'使能JTAG接口'
2,    '使能ISP编程'
3,    '使能掉电检测'
4,    '掉电检测电平为4.0v'
5,    '外部晶振(3.0- MHz),启动时间16CK+4ms(或64ms)'
6,    加密选项，根据需要选择（调试时不要设置加密）
7,    其它的，使用默认值即可，修改也应该不会出现致命错误
8,    复位向量改到引导区
9,    引导区设置为2K字（4K字节）
*/
////////////////////////////////////////////////////////////
#include "mmr.h"
////////////////////////////////////////////////////////////
extern unsigned int zan_count ;
extern unsigned int bD2_weiyi ;
extern unsigned int bD3_weiyi ;


extern unsigned int li_he_zha;
extern unsigned int zuo_gao;
extern unsigned int zuo_di;


extern unsigned int you_gao;
extern unsigned int you_di;
extern unsigned char temp_data[80] ;//温度分配公司，0-1 代表一个温度
extern unsigned char tempe_flag;
unsigned int temp_count_data[11] ={0};

unsigned char temp1_flag_count;


////////////////////////////////////////////////////////////
int main()
{
	int i,j;
	reset_cause = (MCUSR&0x0f);
	is_wdt_rst = 0; //必须清除，否则可能无法关闭wdt
	//
    cli();//main
	wdt_reset();
	wdt_disable();
	//
	if(RST_CAUSE_WDT != reset_cause)
	{
		delay_ms(300);
	}
	//
	TRACE_STRING("init");
    read_para(0);//在初始化过程中有些需要用到部分参数
    hardward_init();
    softward_init();
    TRACE_STRING("init2");
    read_para(0);//初始化完后，可能参数被覆盖，重新读取
    sei();//启动全局中断
    //
    #if USED_W5500 == TRUE
    {
	    //初始化网络芯片，指定延时时间(ms)		
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
    //
	//if(MB_PULSE == m_rPara.para.mb_level_pulse)
	{
		//检测MB脉冲
		SetTimer(MT_MB_COUNTER,MB_DETECTION_INTERVAL*1000,-1);
	}
	//if(DW_PULSE == m_rPara.para.dw_level_pulse)
	{
		//检测DW脉冲
		SetTimer(MT_DW_COUNTER,DW_DETECTION_INTERVAL*1000,-1);
	}
	//
	//如果检测不到bootloader，程序不再往下执行
	check_bootloader();
	//
	if(RST_CAUSE_WDT != reset_cause)
	{
		delay_ms(200);
	}	
	//
	//启动看门狗定时器
	wdt_enable(WDTO_2S);
	//
	if(RST_CAUSE_BOR == reset_cause)
	{
		TRACE_STRING("bor reset completed\r\n\0");
	}
	else if(RST_CAUSE_POR == reset_cause)
	{
		TRACE_STRING("por reset completed\r\n\0");
	}
	else if(RST_CAUSE_EXT == reset_cause)
	{
		TRACE_STRING("extern reset completed\r\n\0");
	}
	else if(RST_CAUSE_WDT == reset_cause)
	{
		TRACE_STRING("wdt reset completed\r\n\0");
	}
	//
	//SetTimer(MT_TRY,1000,-1);//测试用
	SetTimer(MT_BASETASK,10,-1);//10ms执行一次基本任务
	SetTimer(MT_POLL_TASK,5,-1);//轮询任务
	SetTimer(MT_NET_TASK,10,-1);//网络任务，收发网络数据
	SetTimer(MT_LOW_SPEED_TASK,1000,-1);
	SetTimer(MT_READ_PARA, 60*60*1000UL, -1); //60分钟读一次参数
	//
    for(;;) //主循环
    {
        wdt_reset();//程序主循环中喂狗
        //
        for (i =0; i < 2; i++)
        {
        	if (temp_count_data[i] > (1000 * 10))
        	{
				for (j =0; j < 10; j ++)
				{
					temp_data[i*20 +j*2] = 0;
					temp_data[i*20+ j*2+1] = 0;
				}

			}

		}
			

		brollin = li_he_zha;
		//依赖串口数据的接收，没有接收到数据也就没有必要处理
        if(1==uart_frame_ok)
        {
			//按键处理
            deal_key(key_value);
            uart_frame_ok=0;
        }
        //
        if(1==material_2_vel_enable)
        {
            material_2_vel_enable=0;
            material_2_vel(m_material_value);
			//
			//料位
			modbus_status.status.m_lw = m_material_value;
        }
		//
		/////////////////////////////////////////
		//
		//硬件串口接收字节处理(128个接收缓冲)
		if(hard_uart_byte_count > 0)
		{
			hard_uart_byte_count--;
			hardware_uart_receive(hard_uart_rx_buf[hard_uart_load_addr]);
			hard_uart_load_addr++;
			hard_uart_load_addr &= UART0_RX_BUF_MASK;
		}
		//
		timer_run();
/*
		if (tempe_flag == 1)
		{
			tempe_flag = 0;


		    uart_tx_buf[0] = 0x55;
			uart_tx_buf[1] = 'A';
			uart_tx_buf[2] = 'A';
			uart_tx_buf[3] = 'A';
			uart_tx_buf[4] = 'A';




			//开始发送数据
			b485en = 1;//发送使能
			delay_us(100);
			UDR0 = uart_tx_buf[0];
			uart_tx_addr = 1;

		}
		*/

		
		//
        /////////////////////////////////////////
        //
		{
			//这段程序实际没有多大的意义了，在离轧过程中做了处理，理论上不会再出现
			//喂料电机标志和离轧标志不一致的情况。但代码空间和处理能力足够，保留为
			//异常处理。 bear,20160102
			if(((1==bit_auto)||(1==bit_manual)) && (0==bftstart))
			{
				//手动或者自动模式下，如果没有离轧，就执行离轧。
				//bear，20160101
				if(1 == brollin)
				{
					//不管什么原因，没有离轧必须执行离轧
					//roll_out();
				}
			}			
		}
		//
		//定时器溢出操作
		if(timer0_overflow_count > 0)
		{
			//1ms
			timer0_overflow_count--;
			//
			timer0_overflow_handling();
			ad_value_handling();
		}
    }
	//
    return 0;
}
////////////////////////////////////////////////////////////
void check_bootloader(void)
{
	/*语法:        操作数:       程序计数器:       堆栈:
	(i)JMP k    0 ≤ k < 4M       PC <- k         不改变	

	32位的操作码：
	1001 010k kkkk 110k	
	kkkk kkkk kkkk kkkk
	
	atmage644pa只有64Kflash，k的高五位总是为0，所以高16位的
	操作码总是0x940c，低16位是要跳转的地址。
	*/
	//
	unsigned char tmp[4];
	//
	//这句话没有实际的意义，只是让编译器不要优化掉了apli_id数组，为其分配空间。
	hard_uart_rx_addr = pgm_read_byte(&apli_id[2]);
	//
	tmp[0] = pgm_read_byte(0xf001);
	tmp[1] = pgm_read_byte(0xf005);
	tmp[2] = pgm_read_byte(0xf008);
	tmp[3] = pgm_read_byte(0xf00c);
	if((0x94!=tmp[0]) || (0x94!=tmp[1]) || (0x0c!=tmp[2]) || (0x0c!=tmp[3]))
	{
		//bootloader missed
		//
		is_wdt_rst = 0;
		wdt_reset(); //检测到没有引导软件，应用软件不执行
		wdt_disable();
		//
		for(;;) //没有bootloader
		{
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_enable();
			sleep_cpu();
			sleep_disable();
		}
	}
	//
	asm volatile("nop");
}
////////////////////////////////////////////////////////////
unsigned char usart_flag =0;
unsigned int time_flag = 0;


void base_task(void)
{
	//10ms执行一次任务
	//
	//rs485通信

	time_flag++;






    //if (time_flag >20)
	if (time_flag >20)
    {
	    uart_tx();
		time_flag= 0;
		temp1_flag_count ++;
    }



    //数码管显示处理
	led_display();
	//
	//自动手动运行处理
	auto_run();
	manual_run();
	//
	//变频器控制
    //控制B路
	fmotor_vel();
	fmotor_vel_a();
	//
	//远控模式下运行处理
	run_in_remote_ctrl();
	//
	//检测输入信号并处理
	mb_signal_handling();
	dw_signal_handling();
	//
	lw_signal_handling();
	//
	//设备运行时间记录
	runed_time();
	//
	//uart1_task();
}
////////////////////////////////////////////////////////////
void low_speed_task(void)
{	
	//1s
	//两个电流显示交替时间
	display_current_value();
	//
	if(bit_mm_control)//电流过载停机需要连锁控制
	{	
	//主电机电流溢出停机检测
	if(cur_overflow_delay > 20)//秒钟 //5
	{
		cur_overflow_delay=0;
		//
		all_stop();//主电机电流溢出情况下
		//bit_remote = 0;//退出远控模式
		modbus_status.status.m_yk = STATUS_OFF;
		//
		m_rPara.para.all_stop_id = 2;
		//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);
	  }
    }
	
	//
	//for debug
	//DEBUG_STRING("debug\r\n\0");
}
////////////////////////////////////////////////////////////
void poll_task(void)
{
	//5ms
	//
	//MODBUS UART
	#if USED_MODBUS_UART == TRUE
	{
		modbus_task();
	}
	#endif
}
////////////////////////////////////////////////////////////
void net_task(void)
{
	//大于等于10毫秒
	//网络数据量并不大，响应速度可以慢一点，当广播消息下发后，
	//尽量避免同时应答。
	//
	//w5500网络
	#if USED_W5500 == TRUE
	{
		w5500_task();
	}
	#endif
}
////////////////////////////////////////////////////////////
void hardward_init(void)
{
	//硬件初始化程序,设置固定的端口方向,串口属性,AD属性等
	//
	//IO
	if(RST_CAUSE_WDT != reset_cause)
	{
		pin_init();
	}
	//
	if(m_rPara.para.baud_modbus < MODBUS_BAUD_9600)
	{
		m_rPara.para.baud_modbus = MODBUS_BAUD_9600;
	}
	else if(m_rPara.para.baud_modbus > MODBUS_BAUD_38400)
	{
		m_rPara.para.baud_modbus = MODBUS_BAUD_38400;
	}
	//
	set_modbus_baud(m_rPara.para.baud_modbus);
	TRACE_STRING("uart1 set ok\r\n\0");
	//
	//定时器0
	TCCR0A = 0x00;//普通模式，不输出
	TCCR0B = 0x03;//4MHz时钟,64分频
	TCNT0 = 256-60;//计数62次,约1毫秒
	TIMSK0 |= _BV(TOIE0);//定时器0中断使能
	TRACE_STRING("timer0 set ok\r\n\0");
	//
	//定时器1
	TCCR1A = 0xa2;//oc1a,oc1b匹配时清零,达到TOP时置1
	TCCR1B = 0x19;//时钟不分频
	TCCR1C = 0x00;//没有强制输出
	ICR1 = 1001;//TOP
    bpwma = 500;//占空比50%
    bpwmb = 500;
    TRACE_STRING("timer1 set ok\r\n\0");
	//
	//外中断
	EIMSK = 0;//全部禁用外部中断
	EICRA = 0b00101010;//int2下降沿触发//int1下降沿触发//int0下降沿触发
	EIMSK &= ~_BV(INT2);//w5500_int中断禁用
	TRACE_STRING("int2 disable(w5500 int)\r\n\0");
	//
	PCICR = 0;
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;
	PCMSK3 = 0;
	//if(MB_PULSE == m_rPara.para.mb_level_pulse)
	{
		PCICR |= _BV(PCIE2);//PCINT23使能，检测MB脉冲
		PCMSK2 |= _BV(PCINT23);
	}
	//if(DW_PULSE == m_rPara.para.dw_level_pulse)
	{
		PCICR |= _BV(PCIE3);//PCINT31使能，检测DW脉冲
		PCMSK3 |= _BV(PCINT31);
	}
	//
	//SPI，控制网络芯片w5500
	//中断禁用，spi使能，高位先出,主机,mode0(sck空闲时为低，在上升沿的时候采样数据)
	SPCR = _BV(SPE)|_BV(MSTR);//fosc/4   //（1M）
	//SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR0);//fosc/16   //（256k）
	//SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1);//fosc/64   //（64k）
	//SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);//fosc/128   //（32k）
	SPSR |= _BV(SPI2X); //设置该位，SPI速度是上面设置速度的两倍
	//
	SPSR=0;
	ddr_w5500_mosi = 1;
	ddr_w5500_miso = 0;
	ddr_w5500_sck = 1;
	TRACE_STRING("spi set ok\r\n\0");
	//
	//ADC
	ADMUX=bD1;//0000 0000//AREF参考
	ADCSRA=0xa5;//1010 0101//允许ad转换,自动触发使能,不允许中断,32分频(125K)
	ADCSRB=0;//自由连续转换
	adsc=1;//启动AD转换
	TRACE_STRING("adc set ok\r\n\0");
	//
	//USART
	//38461.5 Bd //0.2%
	UBRR0H=0;
	UBRR0L=12;

	//19200
	//UBRR0L=24;
	UCSR0A=(1<<U2X0);//波特率分频因子从16降到8
	UCSR0B=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);//接收中断使能
	UCSR0C=(3<<UCSZ00);//8N1
	//
    b485en=0;//接收使能,发送禁止
    TRACE_STRING("uart0 set ok\r\n\0");
}
////////////////////////////////////////////////////////////////
void pin_init(void)
{
	//全部复位，然后在设置
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	DDRA = 0;
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	//
	//七路继电器输出
	ddr_by = 1;
	ddr_zk = 1;
	ddr_zt = 1;
	ddr_wg = 1;
	ddr_hz = 1;
	ddr_lz = 1;
	ddr_bj = 1;
	ddr_mbbj = 1; //mb信号故障报警
	//
	//四路ADC
	port_d1 = 0;
	port_d2 = 0;
	port_l = 0;
	port_d3 = 0;
	//
	//两路pwm出
	ddr_pwma = 1;
	ddr_pwmb = 1;
	//
	//八路数字入(光电隔离)
	port_lw = 1;
	port_yq = 1;
	port_yz = 1;
	port_mb = 1;
	port_qr = 1;
	port_ql = 1;
	port_dw = 1;
	//
	//485发送使能(高电平有效)
	ddr_485en = 1;
	port_485ro = 1;//RXD上拉
	ddr_485di = 1;//TXD
	//
	//spi模拟uart
	ddr_pc_485en = 1;
	port_pc_485ro = 1;//上拉
	ddr_pc_485di = 1;//输出
	//
	ddr_w5500_rst = 1;
	ddr_w5500_cs = 1;
	ddr_w5500_int = 0;
}
////////////////////////////////////////////////////////////////
void softward_init(void)
{
	timer0_overflow_count = 0;
	//
    ad_counter[bD1]=0;//ad采样个数清零
    ad_counter[bD2]=0;//ad采样个数清零
    ad_counter[bD3]=0;//ad采样个数清零
    ad_counter[bL]=0;//ad采样个数清零
	ad_add_value[bD1].ml=0;
    ad_add_value[bD2].ml=0;
    ad_add_value[bD3].ml=0;
    ad_add_value[bL].ml=0;
        
	//
	uart_rx_counter=0;
    uart_frame_head_ok=0;
    uart_frame_ok=0;
	//
    key_value=0xff;
    m_cgq_cmd=XUYAOSHUJU;
    m_material_counter=0;
    //
	part_stop_bmb = 0;
	bmbbj = 0;
    bit_lock=0;
	bit_roll_in=0;
	//
	mb_invalid_delay = 1;
	mb_is_invalid = 0;
	//
	hard_uart_rx_addr=0;
	hard_uart_load_addr=0;
	hard_uart_byte_count=0;
	//
	modbus_init();
	TRACE_STRING("modbus init ok\r\n\0");
	//
	init_soft_timer();
	TRACE_STRING("soft_timer init ok\r\n\0");
	//
	bmmstop = 0;
}
////////////////////////////////////////////////////////////////
void OnTimer(unsigned short id)
{
	//软件定时器处理程序
	switch(id)
	{
		case MT_TRY://测试代码
		{
			//三个灯同时闪亮
			//bmmstart = !bmmstart;
			//bmmstop = !bmmstop;
			//bftstart = !bftstart;
			//
			//send_soft_uart_to_computer(0,0x55);
			//
			//send_soft_uart_to_computer((unsigned char *)"soft uart\r\n\0",11);
		}
		break;
		case MT_READ_PARA:
		{
			//定时读取参数，刷新简单错误
			read_para(1);//不允许初始化上电参数
		}
		break;
		case MT_BASETASK:
		{
			base_task();//10ms任务集中处理
		}
		break;
		case MT_MB_COUNTER:
		{
			mb_60s_sample_buf[mb_60s_sample_addr] = (mb_counter_tmp+1)/2;//电平变化
			mb_3s_sample_buf[mb_3s_sample_addr] = (mb_counter_tmp+1)/2;
			mb_counter_tmp = 0;
			//
			mb_60s_sample_addr++;
			mb_60s_sample_addr %= MB_60S_SAMPLE_SIZE;
			mb_3s_sample_addr++;
			mb_3s_sample_addr %= MB_3S_SAMPLE_SIZE;
			//
			//更新一分钟MB计数值
			mb_counter_60s = get_mb_counter_60s();
			mb_counter_3s = get_mb_counter_3s();
		}
		break;
		case MT_DW_COUNTER:
		{
			dw_60s_sample_buf[dw_60s_sample_addr] = (dw_counter_tmp+1)/2;
			dw_3s_sample_buf[dw_3s_sample_addr] = (dw_counter_tmp+1)/2;
			dw_counter_tmp = 0;
			//
			dw_60s_sample_addr++;
			dw_60s_sample_addr %= DW_60S_SAMPLE_SIZE;
			dw_3s_sample_addr++;
			dw_3s_sample_addr %= DW_3S_SAMPLE_SIZE;
			//
			//更新一分钟MB计数值
			dw_counter_60s = get_dw_counter_60s();
			dw_counter_3s = get_dw_counter_3s();
		}
		break;
		case MT_START_QR_QL_CHECK:
		{
			SetTimer(MT_CHECK_QR_QL,200,-1);
		}
		break;
		case MT_CHECK_QR_QL:
		{
			qr_ql_signal_handling();
		}
		break;
		case MT_LOW_SPEED_TASK:
		{
			low_speed_task();
		}
		break;
		case MT_POLL_TASK:
		{
			poll_task();
		}
		break;
		case MT_NET_TASK:
		{
			net_task();
		}
		break;
		case MT_CLEAN_MODBUS_RX_FLAG:
		{
			extern void modbus_clean_rx_flag(void);
			modbus_clean_rx_flag();
		}
		break;
		case MT_AUTO_RUN_DELAY:
		{
			//延时启动自动运行
			key_auto_run();
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////
extern unsigned short m_cur_cal2;
void display_current_value(void)
{
	#if __D2_USED__  == TRUE
	{
		//启用了备用的电流采样
		if(1 == bit_current_1st)
		{
			//当前显示为第一屏，改为第二屏显示
			bit_current_1st = 0;
			bit_current_2nd = 1;
			m_cur_cal2 = m_cur_d2;
			m_cur_round = cur_d2_round;
			//
			//////////////////////电流过载判断
			//注意,这里的m_cur_cal有乘10的效果
			if(m_cur_cal2 > mmotor_cur2_cmp)
			{
				bit_over_i = 1;//过载等亮
				modbus_status.status.m_i_over = STATUS_ON;
				bcomerr = 1;//主板错误灯亮
			}
			else
			{
				bit_over_i = 0;//电流过载灯灭
				modbus_status.status.m_i_over = STATUS_OFF;
				if((0==bit_comm_error) && (0==bit_qr_ql_error))
				{
					bcomerr = 0;
				}
				else
				{
					bcomerr = 1;
				}
			}
			//
			///////////////////////
			//修正主电机电流显示值
			//因为主电机电流值不可能超过400,而修正值最大是130,所以直接乘即可
			m_cur_cal2*=m_rPara.para.current2_modify;
			m_cur_cal2/=100;
		}
		else
		{
			//当前显示为第二屏，改为第一屏显示
			bit_current_1st = 1;
			bit_current_2nd = 0;
			m_cur_cal = m_cur_d1;
			m_cur_round = cur_d1_round;
			//
			//////////////////////电流过载判断
			//注意,这里的m_cur_cal有乘10的效果
			if(m_cur_cal > mmotor_cur1_cmp)
			{
				bit_over_i = 1;//过载等亮
				modbus_status.status.m_i_over = STATUS_ON;
				bcomerr = 1;//主板错误灯亮
			}
			else
			{
				bit_over_i = 0;//电流过载灯灭
				modbus_status.status.m_i_over = STATUS_OFF;
				if((0==bit_comm_error) && (0==bit_qr_ql_error))
				{
					bcomerr = 0;
				}
				else
				{
					bcomerr = 1;
				}
			}
			//
			///////////////////////
			//修正主电机电流显示值
			//因为主电机电流值不可能超过400,而修正值最大是130,所以直接乘即可
			m_cur_cal*=m_rPara.para.current1_modify;
			m_cur_cal/=100;
		}
	}
	#else //__D2_USED__
	{
		//没有启用备用的电流采样
		bit_current_1st = 0;
		bit_current_2nd = 0;
		m_cur_cal = m_cur_d1;
		m_cur_round = cur_d1_round;
		//////////////////////电流过载判断
		//注意,这里的m_cur_cal有乘10的效果
		if(m_cur_cal > mmotor_cur1_cmp)
		{
			bit_over_i = 1;//过载等亮
			modbus_status.status.m_i_over = STATUS_ON;
			bcomerr = 1;//主板错误灯亮
		}
		else
		{
			bit_over_i = 0;//电流过载灯灭
			modbus_status.status.m_i_over = STATUS_OFF;
			if((0==bit_comm_error) && (0==bit_qr_ql_error))
			{
				bcomerr = 0;
			}
			else
			{
				bcomerr = 1;
			}
		}
		//
		///////////////////////
		//修正主电机电流显示值
		//因为主电机电流值不可能超过400,而修正值最大是130,所以直接乘即可
		m_cur_cal*=m_rPara.para.current1_modify;
		m_cur_cal/=100;
	}
	#endif //__D2_USED__
}
/////////////////////////////////////////////////////////////////
unsigned int AD_CHANGE =0;
    unsigned int i = 0,j=0;
//unsigned int	ad_counter = 0;

static unsigned long AD_time_mogun_delay=0;

void ad_value_handling(void)
{
	UCHAR4 m_temp;
	unsigned char ad_channel,ad_temp;
    //unsigned char yuzhi =m_rPara.para.set_yuzhi;



	//
	m_temp.mc[0] = ADCL;//累加ad值
	m_temp.mc[1] = ADCH;
	m_temp.ms[0] &= 0x03ff;

	
#if __ZAH_JU_TIANZHEN__ == TRUE        

    ad_temp = ADMUX&0x07;
    if (AD_CHANGE != 0)
    {
	    ad_add_value[ad_temp].ml += m_temp.ms[0];
    }

    //
	//采样个数计数
	ad_counter[ad_temp]++;
    AD_CHANGE ++;
	//采样1024个值,耗时1024毫秒,也就是1024毫秒得到一个有效的采样值8 

    //  1024  16
    //  128   2
	if(AD_CHANGE >= 17)//约一秒得到一个数据
	{
        AD_CHANGE = 0;
		//
		ad_channel = ADMUX&0xf0;	

        switch(ad_temp)
		{
			case bD1://电流1
			{

     			ad_channel |= bL;

				//
				//if(ad_counter[ad_temp] >= 1024)
				if(ad_counter[ad_temp] >= 512)
                {            

                    ad_counter[ad_temp] = 0;
                   // ad_add_value[ad_temp].ml >>= 4;
                    ad_add_value[ad_temp].ml >>= 3;
                    m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//得到采样值
                    m_temp.ms[1] = 0;
                    ad_add_value[ad_temp].ml = 0;



                    if(m_temp.ms[0] > 0x4175)// (0x4175//16757//1.3v)  (0x325a//12890//1v)
    				{
    					m_temp.ms[0] -= 0x325a;//0-4v
    					m_temp.ml *= m_rPara.para.cur1_full;
    					m_temp.ml *= 51;
    					m_temp.ml >>= 2;
    					if(m_temp.ms[0] > 0x7fff)
    					{
    						cur_d1_round=5;
    					}
    					else
    					{
    						cur_d1_round=0;
    					}
    					m_cur_d1=m_temp.ms[1];
    				}
    				else
    				{
    					if(m_temp.ms[0] >0x325a)
    					{
	    					m_temp.ms[0] -= 0x325a;//0-4v
    					}
    					else
    					{
	    					m_temp.ms[0] =0	;
    					}

    					m_temp.ml *= m_rPara.para.cur1_full;
    					m_temp.ml *= 51;
    					m_temp.ml >>= 2;
    					
    					m_cur_d1=m_temp.ms[1];
						//m_cur_d1=0;
    					cur_d1_round=0;
    					if(((0==m_rPara.para.mm_control) && (1==bit_mmotor)) && ((1==bit_remote) || (1==bit_auto) || (1==bit_manual)))
    					{
    						//在连锁控制模式下
    						//bit_remote=0;//退出远控模式
    						modbus_status.status.m_yk = STATUS_OFF;
							m_rPara.para.all_stop_id = 7;
							//eeprom_write_byte_user(&m_ePara[0].para.all_stop_id, m_rPara.para.all_stop_id);

							
							if (boot_time - AD_time_mogun_delay  > (NO_CURRENT_DELAY_TIME_S*1000))
							{
								all_stop();
							}

						}
						else
						{
							AD_time_mogun_delay = boot_time;
						}
    				}
                }
			}
			break;
			case bL://料位
			{
				ad_channel|=bD2;

                ///if (ad_counter[ad_temp] >= 1024)
                if (ad_counter[ad_temp] >= 512)
                {
                    ad_counter[ad_temp] = 0;
                    //ad_add_value[ad_temp].ml >>= 4;
                    ad_add_value[ad_temp].ml >>= 3;
                    m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//得到采样值
                    m_temp.ms[1] = 0;
                    ad_add_value[ad_temp].ml = 0;


                    //////////////////////////////////从AD采样料位数据
    				if((MATERIAL_PORT_ONLY_AD==m_rPara.para.material_port)	|| (MATERIAL_PORT_RS485_AD==m_rPara.para.material_port))
    				{
    					m_material_value=m_temp.ms[0];
    					material_2_vel_enable=1;
    				}
                }
			}
			break;

            case bD2://电流2
			{
				ad_channel|=bD3;
				//////////////////////////////////从AD采样料位数据
				
                if (ad_counter[ad_temp] >= 32)
                {
                   ad_counter[ad_temp] = 0;
                   ad_add_value[ad_temp].ml >>= 5;
                   m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//得到采样值
                   bD2_weiyi = ad_add_value[ad_temp].ms[0];
                   ad_add_value[ad_temp].ml = 0;
                   #if 1
                   brollin =0;   
                   brolllock = 0;

                   if (li_he_zha == 0)
                   {
                        if (bD2_weiyi > zuo_di)
                        {
                
                            if ((bD2_weiyi - zuo_di) > yuzhi)
                            {
                                brollin =1;  
                            }

                        }
                        else
                        {
                            if ((zuo_di - bD2_weiyi) > yuzhi)
                            {
                                 
                                 
								 brolllock =1;   
                            }

                        }
                        
                   }
                   else
                   {
                        if (bD2_weiyi > zuo_gao)
                        {
                
                            if ((bD2_weiyi - zuo_gao) > yuzhi)
                            {
                                
								brollin =1;

                            }

                        }
                        else
                        {
                            if ((zuo_gao - bD2_weiyi) > yuzhi)
                            {
                                   
                                 brolllock =1;    
                            }

                        }

                   }
                   ddr_zt =1;
                   ddr_lz =1;
                   #endif
                   //处理
                   //TRACE_STRING("BD2 ZHI123 =");
                   //send_ulong_ascii(m_temp.ms[0] ,TRUE);

                }

			}
			break;

            case bD3://料位
			{
				ad_channel|= bD1;
                if (ad_counter[ad_temp] >= 32)
                {
                    ad_counter[ad_temp] = 0;
                    ad_add_value[ad_temp].ml >>= 5;
                    m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//得到采样值
                    bD3_weiyi = ad_add_value[ad_temp].ms[0];
                    ad_add_value[ad_temp].ml = 0;


                   
                   #if 1
                   bmbby =0;
                   bmmstop =0;
                   if (li_he_zha == 0)
                   {
                        if (bD3_weiyi > you_di)
                        {
                
                            if ((bD3_weiyi - you_di) >yuzhi)
                            {
                                   bmbby =1; 
                            }

                        }
                        else
                        {
                            if ((you_di - bD3_weiyi) > yuzhi)
                            {
                                  
                                 
								 bmmstop =1;
                            }

                        }
                        
                   }
                   else
                   {
                        if (bD3_weiyi > you_gao)
                        {
                
                            if ((bD3_weiyi - you_gao) >yuzhi)
                            {
                                bmbby =1;
                            }

                        }
                        else
                        {
                            if ((you_gao - bD3_weiyi) > yuzhi)
                            {
                                   
                               
								 
								 bmmstop =1; 
                            }

                        }

                   }
                   ddr_by =1;
                   ddr_zt =1;
                    #endif
                
                    //TRACE_STRING("BD3 ZHI123 =");
                    //send_ulong_ascii(m_temp.ms[0] ,TRUE);

                    //处理

                }

			}
			break;
			default:
			{
				ad_channel |= bD1;
			}
			break;
		}//end switch
		ADMUX=ad_channel;
		}
#else

ad_add_value[0].ml += m_temp.ms[0];
//
//采样个数计数
ad_counter[0]++;
//采样1024个值,耗时1024毫秒,也就是1024毫秒得到一个有效的采样值
if(ad_counter[0] >= 1024)//约一秒得到一个数据
{
	ad_counter[0] = 0;
	ad_add_value[0].ml >>= 4;
	m_temp.ms[0] = ad_add_value[0].ms[0];//得到采样值
	m_temp.ms[1] = 0;
	ad_add_value[0].ml = 0;
	//
	ad_channel = ADMUX&0xf0;
	ad_temp = ADMUX&0x07;
	switch(ad_temp)
	{
		case bD1://电流1
		{
			#if __D2_USED__ == TRUE
			{
				ad_channel |= bD2;
			}
			#else
			{
				ad_channel |= bL;
			}
			#endif
			//
			if(m_temp.ms[0] > 0x4175)// (0x4175//16757//1.3v)  (0x325a//12890//1v)
			{
				m_temp.ms[0] -= 0x325a;//0-4v
				m_temp.ml *= m_rPara.para.cur1_full;
				m_temp.ml *= 51;
				m_temp.ml >>= 2;
				if(m_temp.ms[0] > 0x7fff)//25780//2v
				{
					cur_d1_round=5;
				}
				else
				{
					cur_d1_round=0;
				}
				m_cur_d1=m_temp.ms[1];
			}
			else
			{
				//m_cur_d1=0;
				if(m_temp.ms[0] >0x325a)
				{
					m_temp.ms[0] -= 0x325a;//0-4v
				}
				else
				{
					m_temp.ms[0] =0	;			
				}

				m_temp.ml *= m_rPara.para.cur1_full;
				m_temp.ml *= 51;
				m_temp.ml >>= 2;
								
				m_cur_d1=m_temp.ms[1];

				cur_d1_round=0;
				if(((0==m_rPara.para.mm_control) && (1==bit_mmotor)) && ((1==bit_remote) || (1==bit_auto) || (1==bit_manual)))
				{
					//在连锁控制模式下
					//bit_remote=0;//退出远控模式
					modbus_status.status.m_yk = STATUS_OFF;
					m_rPara.para.all_stop_id = 7;
					//ZANXIAOFEIQUXIAOQONG
					if (boot_time - AD_time_mogun_delay  > (NO_CURRENT_DELAY_TIME_S*1000))
					{
						all_stop();
					}

				}
				else
				{
					AD_time_mogun_delay = boot_time;
				}
			}
			
		}
		break;
		#if __D2_USED__ == TRUE
		{
			case bD2://电流2
			{
				ad_channel |= bL;
				//
				if(m_temp.ms[0] > 0x4175)// (0x4175//16757//1.3v)  (0x325a//12890//1v)
				{
					m_temp.ms[0]-=0x325a;//0-4v  0x325a
					m_temp.ml*=m_rPara.para.cur2_full;
					m_temp.ml*=51;
					m_temp.ml>>=2;
					if(m_temp.ms[0] > 0x7fff)
					{
						cur_d2_round=5;
					}
					else
					{
						cur_d2_round=0;
					}
					m_cur_d2=m_temp.ms[1];
				}
				else
				{
					//m_cur_d2=0;
					if(m_temp.ms[0] >0x325a)
					{
						m_temp.ms[0] -= 0x325a;//0-4v
					}
					else
					{
						m_temp.ms[0] =0	;
					}

					m_temp.ml *= m_rPara.para.cur1_full;
					m_temp.ml *= 51;
					m_temp.ml >>= 2;
					
					m_cur_d1=m_temp.ms[1];
					cur_d2_round=0;
					if(((0==m_rPara.para.mm_control) && (1==bit_mmotor)) && ((1==bit_remote) || (1==bit_auto) || (1==bit_manual)))
					{
						//bit_remote=0;//退出远控模式
						modbus_status.status.m_yk = STATUS_OFF;
						m_rPara.para.all_stop_id = 8;
									
					
						if (boot_time - AD_time_mogun_delay  > (NO_CURRENT_DELAY_TIME_S*1000))
						{
							all_stop();
						}

					}
					else
					{
						AD_time_mogun_delay = boot_time;
					}
				}
				}
			}
			break;
		}
		#endif //__D2_USED__
		case bL://料位
		{
			ad_channel|=bD1;
			//////////////////////////////////从AD采样料位数据
			if((MATERIAL_PORT_ONLY_AD==m_rPara.para.material_port)	|| (MATERIAL_PORT_RS485_AD==m_rPara.para.material_port))
			{
				m_material_value=m_temp.ms[0];
				material_2_vel_enable=1;
			}
		}
		break;
		default:
		{
			ad_channel |= bD1;
		}
		break;
	}//end switch
	ADMUX=ad_channel;
}//end if	

#endif		
		

}
/////////////////////////////////////////////////////////////////
void timer0_overflow_handling(void)
{
	//1ms中断一次
	//
	ms1_counter++;
	if(ms1_counter > 9)
	{
		ms1_counter=0;
		//10ms
		//
		//主电机启动,停止脉冲信号,合轧,离轧脉冲信号
		mmotor_delay++;
		roll_delay++;
		key_delay++;
		//
		comm_error_delay++;
		if(comm_error_delay > 99)
		{
			comm_error_delay=0;
			bcomerr=1;
			bit_comm_error=1;
			modbus_status.status.m_comm_error = STATUS_ON;
		}
		//
		mb_invalid_delay++;//10ms，判断mb信号是否有效
		//
		ms10_counter++;
		if(ms10_counter > 9)
		{
			ms10_counter=0;
			//
			//100ms
			m_cgq_ack_delay++;//传感器应答后显示时间(100ms)
			wait_rollin++;
			key_delay_set_in++;//内部参数设定进入等待
			password_delay++;
			//
			enable_mb_delay++; //100ms
			if(enable_mb_delay > 200)//20s
			{
				enable_mb_delay=200;
			}
			//
			ms100_counter++;
			if(ms100_counter > 9)
			{
				ms100_counter=0;
				//1s
				cur_overflow_delay++;
				#if LIMITEDTIME_ENABLE == 1
				{
					m_used_1s++;
				}
				#endif
				//
				set_idle_wait++;
				if((1==bit_setmode) || (1==bit_alarm_mode) || (1==bit_monitor_mode))
				{
					if(set_idle_wait > 30)//30s
					{
						TRACE_STRING("jingruguozuomoshi5");
						reset_set_status();
						//
						bit_monitor_mode = 0;
						bit_alarm_mode = 0;
					}
				}
			}
		}
	}	
}
/////////////////////////////////////////////////////////////////
void hardware_uart_receive(unsigned char val)
{
	//unsigned char sum;
	//UCHAR2 m_temp;
	//const unsigned char uart_value = val;
	//
	comm_error_delay=0;//通信延时清零
	bit_comm_error=0;
	modbus_status.status.m_comm_error = STATUS_OFF;
	if((0==bit_over_i) && (0==bit_qr_ql_error))
	{
		bcomerr = 0;
	}
	else
	{
		bcomerr = 1;
	}
	//
	/*
	if(1==uart_frame_head_ok)
	{
		uart_rx_buf[uart_rx_counter++]=uart_value;
		if(uart_rx_counter > 6)//接收3个字节有效参数和一个字节校验和
		{
			uart_rx_counter=0;
			uart_frame_head_ok=0;
			//
			sum=0xaa;
			for(i=0;i<6;i++)
			{
				sum+=uart_rx_buf[i];
			}
			if(sum == uart_rx_buf[6])
			{
                usart_flag =0;
                if(ADDR_XSB==uart_rx_buf[0])
				{
					key_value=uart_rx_buf[1];//保存键值
					// key_value_secondary=uart_rx_buf[2];//辅助键值，主要针对总停机等
					uart_frame_ok=1;
				}
				else if(ADDR_CGQ==uart_rx_buf[0])
				{
					if(GAOLIAOWEI==uart_rx_buf[5])
					{
						m_cgq_ack_delay=0;
						m_cgq_ack=GAOLIAOWEI;
					}
					else if(DILIAOWEI==uart_rx_buf[5])
					{
						m_cgq_ack_delay=0;
						m_cgq_ack=DILIAOWEI;
					}
					//
					m_cgq_cmd=XUYAOSHUJU;//这里是为远程设置料位高低之后,复位命令用的
					m_temp.mc[0]=uart_rx_buf[1];//低位值先发
					m_temp.mc[1]=uart_rx_buf[2];
					m_material_add_value.ms+=m_temp.ms;
					m_material_counter++;
					if(m_material_counter > 62)// >= 1260ms
					{
						//if((MATERIAL_PORT_ONLY_RS485==m_rPara.para.material_port)|| (MATERIAL_PORT_RS485_AD==material_port))
						{
							//从485口接收料位数据
							m_temp.ms=m_material_add_value.mc[1];
							m_temp.ms*=6;//1000->1024=>1024=(1+6/250)*1000=>1024=(1+6/256)*1000
							m_material_value=m_material_add_value.ms+m_temp.ms+3;
							material_2_vel_enable=1;
						}
						//
						m_material_counter=0;
						m_material_add_value.ms=0;
					}
				}
			}
			else
			{
				uart_frame_ok=0;
			}
		}
	}
	else
	{
		if(0xaa==uart_value)
		{
			uart_rx_counter=0;
			uart_frame_head_ok=1;
		}
	}
	*/
} 
/////////////////////////////////////////////////////////////////
///////////////       中断服务程序       ////////////////////////
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
//
//定时器0作为全局定时器,用来管理所有任务的调度
//
//unsigned int temp_count_data[10] = {0};
ISR(TIMER0_OVF_vect/*,ISR_NOBLOCK*/)//加入ISR_NOBLOCK属性,在中断服务程序中允许中断套嵌
{
	TCNT0=256-62;//4MHz时钟,64分频,计数62次,定时约1毫秒
    //1ms
    boot_time++;//软件定时器时间基准，应该放到中断服务函数里，便于主程序等待延时
    zan_count ++;
    //
	timer0_overflow_count++;
	temp_count_data[0]++;
	temp_count_data[1]++;
	temp_count_data[2]++;
	temp_count_data[3]++;
	temp_count_data[4]++;
	temp_count_data[5]++;
	temp_count_data[6]++;
	temp_count_data[7]++;
	temp_count_data[8]++;
	temp_count_data[9]++;
	temp_count_data[10]++;	
}
/////////////////////////////////////////////////////////////////
//
//串口发送中断
//
extern  char canshu_peizhi;
ISR(USART0_TX_vect)
{

    if(uart_tx_addr < 10)
    {
        UDR0 = uart_tx_buf[uart_tx_addr++];
    }
    else
    {
	    delay_us(250);
        b485en=0;
    }
		
}


/////////////////////////////////////////////////////////////////
//
//串口接收中断
//
ISR(USART0_RX_vect)
{
	/*
	hard_uart_rx_buf[hard_uart_rx_addr] = UDR0;
	hard_uart_rx_addr++;
	hard_uart_rx_addr &= UART0_RX_BUF_MASK;
	hard_uart_byte_count++;
	*/
	//unsigned char data[40];
	unsigned int UART_Timeout = 0;
	unsigned char i = 0;
	//unsigned char j = 0;
    //unsigned char temp;
	unsigned char sum;
	
    uart_rx_buf[i] = UDR0;
    while(uart_rx_buf[i]!='E')
	{
	
		i++;
		
		while((0==(UCSR0A&0x80))&&(UART_Timeout<5000))
		{UART_Timeout++;}
		if(UART_Timeout>=5000)
		{
			uart_rx_buf[i]='E'; // 如果接收超时，退出中断
		}
		else
		{
			uart_rx_buf[i] = UDR0;
		}
		
		
		if(i>38)
		{
			return;
		}
		UART_Timeout = 0;
		
	}
	
	//if(i >= 6)
	{
		sum=0;
		for(i=0;i<7;i++)
		{
			sum+=uart_rx_buf[i];
		}
		if(sum == uart_rx_buf[7])
		{
			if(ADDR_XSB==uart_rx_buf[1])
			{
				key_value=uart_rx_buf[2];//保存键值
				// key_value_secondary=uart_rx_buf[2];//辅助键值，主要针对总停机等
				uart_frame_ok=1;
			}
		}
	}
	//else	
	/*if(uart_rx_buf[0] == 0x55 &&  uart_rx_buf[4]== 0x45 && uart_rx_buf[3] < 10)
	{
		temp_data[uart_rx_buf[4]*2] = uart_rx_buf[1];
		temp_data[uart_rx_buf[4]*2 + 1] = uart_rx_buf[2];
	}*/
}
/////////////////////////////////////////////////////////////////
ISR(PCINT2_vect)
{
	//MB脉冲检测，下降沿中断
	mb_counter_tmp++;
}
/////////////////////////////////////////////////////////////////
ISR(PCINT3_vect)
{
	//DW脉冲检测，下降沿中断
	dw_counter_tmp++;
}
/////////////////////////////////////////////////////////////////
ISR(WDT_vect)
{
	//看门狗中断
	DEBUG_STRING("wdt interrupt");
}
/////////////////////////////////////////////////////////////////
