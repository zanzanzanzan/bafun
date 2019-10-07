/*///////////////////////////////////////////////////////////////
*																*
*     �Ĺ����������Ƴ���										*
*																*
*     MCU:ATMega644pa											*
*     CRY:�ⲿ����4MHz											*
*																*
*     �ĵ���ص�ŷ������̵��������Ӵ��Կ�������				*
*     �޸�������Ϊĥ���������������							*
*     �޸��ֶ��л�Զ��ʱ��ȡ���ֶ�״̬							*
*     �޸ļ����Զ���λͣ����������ʱȡ���Զ�״̬				*
*																*
*     ����MCU��ATMega8(L) --> ATMega32(a)						*
*     ������·���룬һ·���									*
*	  ����һ·����λ��ͨ�ŵ�rs485�ӿڣ�������38400Bd			*
*     ����λ��ͨ�ŵ���Э��ʹ��Modbus							*
*     MB��ⷽʽ�ɵ�ƽ����Ϊ�������							*
*     ����һЩ��ʾ�Ͳ����ϵ�С�Ķ�								*
*     �޸�ʱ��:20150610											*
*																*
*     ����MCU��ATMega32(a) --> ATMega644pa						*
*     ��������֧�֣�w5500��										*
*     bear														*
*     20160514													*
*																*
*	  ��λ����Ҫ�ĵ�������										*
*     ��Ҫbootloader�������ע��Ҫ������������Ϊ2K�֣�4K�ֽڣ�  *
*	  bear														*
*	  20160718													*
*																*
*	  ��������֧��												*
*	  bear														*
*	  20161112													*
*																*
////////////////////////////////////////////////////////////////*/
/*
��˿λ����ע������(������):
1,    ��Ҫ'ʹ��JTAG�ӿ�'
2,    'ʹ��ISP���'
3,    'ʹ�ܵ�����'
4,    '�������ƽΪ4.0v'
5,    '�ⲿ����(3.0- MHz),����ʱ��16CK+4ms(��64ms)'
6,    ����ѡ�������Ҫѡ�񣨵���ʱ��Ҫ���ü��ܣ�
7,    �����ģ�ʹ��Ĭ��ֵ���ɣ��޸�ҲӦ�ò��������������
8,    ��λ�����ĵ�������
9,    ����������Ϊ2K�֣�4K�ֽڣ�
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
extern unsigned char temp_data[80] ;//�¶ȷ��乫˾��0-1 ����һ���¶�
extern unsigned char tempe_flag;
unsigned int temp_count_data[11] ={0};

unsigned char temp1_flag_count;


////////////////////////////////////////////////////////////
int main()
{
	int i,j;
	reset_cause = (MCUSR&0x0f);
	is_wdt_rst = 0; //�����������������޷��ر�wdt
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
    read_para(0);//�ڳ�ʼ����������Щ��Ҫ�õ����ֲ���
    hardward_init();
    softward_init();
    TRACE_STRING("init2");
    read_para(0);//��ʼ����󣬿��ܲ��������ǣ����¶�ȡ
    sei();//����ȫ���ж�
    //
    #if USED_W5500 == TRUE
    {
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
    //
	//if(MB_PULSE == m_rPara.para.mb_level_pulse)
	{
		//���MB����
		SetTimer(MT_MB_COUNTER,MB_DETECTION_INTERVAL*1000,-1);
	}
	//if(DW_PULSE == m_rPara.para.dw_level_pulse)
	{
		//���DW����
		SetTimer(MT_DW_COUNTER,DW_DETECTION_INTERVAL*1000,-1);
	}
	//
	//�����ⲻ��bootloader������������ִ��
	check_bootloader();
	//
	if(RST_CAUSE_WDT != reset_cause)
	{
		delay_ms(200);
	}	
	//
	//�������Ź���ʱ��
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
	//SetTimer(MT_TRY,1000,-1);//������
	SetTimer(MT_BASETASK,10,-1);//10msִ��һ�λ�������
	SetTimer(MT_POLL_TASK,5,-1);//��ѯ����
	SetTimer(MT_NET_TASK,10,-1);//���������շ���������
	SetTimer(MT_LOW_SPEED_TASK,1000,-1);
	SetTimer(MT_READ_PARA, 60*60*1000UL, -1); //60���Ӷ�һ�β���
	//
    for(;;) //��ѭ��
    {
        wdt_reset();//������ѭ����ι��
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
		//�����������ݵĽ��գ�û�н��յ�����Ҳ��û�б�Ҫ����
        if(1==uart_frame_ok)
        {
			//��������
            deal_key(key_value);
            uart_frame_ok=0;
        }
        //
        if(1==material_2_vel_enable)
        {
            material_2_vel_enable=0;
            material_2_vel(m_material_value);
			//
			//��λ
			modbus_status.status.m_lw = m_material_value;
        }
		//
		/////////////////////////////////////////
		//
		//Ӳ�����ڽ����ֽڴ���(128�����ջ���)
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




			//��ʼ��������
			b485en = 1;//����ʹ��
			delay_us(100);
			UDR0 = uart_tx_buf[0];
			uart_tx_addr = 1;

		}
		*/

		
		//
        /////////////////////////////////////////
        //
		{
			//��γ���ʵ��û�ж��������ˣ����������������˴��������ϲ����ٳ���
			//ι�ϵ����־��������־��һ�µ������������ռ�ʹ��������㹻������Ϊ
			//�쳣���� bear,20160102
			if(((1==bit_auto)||(1==bit_manual)) && (0==bftstart))
			{
				//�ֶ������Զ�ģʽ�£����û����������ִ��������
				//bear��20160101
				if(1 == brollin)
				{
					//����ʲôԭ��û����������ִ������
					//roll_out();
				}
			}			
		}
		//
		//��ʱ���������
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
	/*�﷨:        ������:       ���������:       ��ջ:
	(i)JMP k    0 �� k < 4M       PC <- k         ���ı�	

	32λ�Ĳ����룺
	1001 010k kkkk 110k	
	kkkk kkkk kkkk kkkk
	
	atmage644paֻ��64Kflash��k�ĸ���λ����Ϊ0�����Ը�16λ��
	����������0x940c����16λ��Ҫ��ת�ĵ�ַ��
	*/
	//
	unsigned char tmp[4];
	//
	//��仰û��ʵ�ʵ����壬ֻ���ñ�������Ҫ�Ż�����apli_id���飬Ϊ�����ռ䡣
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
		wdt_reset(); //��⵽û�����������Ӧ�������ִ��
		wdt_disable();
		//
		for(;;) //û��bootloader
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
	//10msִ��һ������
	//
	//rs485ͨ��

	time_flag++;






    //if (time_flag >20)
	if (time_flag >20)
    {
	    uart_tx();
		time_flag= 0;
		temp1_flag_count ++;
    }



    //�������ʾ����
	led_display();
	//
	//�Զ��ֶ����д���
	auto_run();
	manual_run();
	//
	//��Ƶ������
    //����B·
	fmotor_vel();
	fmotor_vel_a();
	//
	//Զ��ģʽ�����д���
	run_in_remote_ctrl();
	//
	//��������źŲ�����
	mb_signal_handling();
	dw_signal_handling();
	//
	lw_signal_handling();
	//
	//�豸����ʱ���¼
	runed_time();
	//
	//uart1_task();
}
////////////////////////////////////////////////////////////
void low_speed_task(void)
{	
	//1s
	//����������ʾ����ʱ��
	display_current_value();
	//
	if(bit_mm_control)//��������ͣ����Ҫ��������
	{	
	//������������ͣ�����
	if(cur_overflow_delay > 20)//���� //5
	{
		cur_overflow_delay=0;
		//
		all_stop();//�����������������
		//bit_remote = 0;//�˳�Զ��ģʽ
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
	//���ڵ���10����
	//������������������Ӧ�ٶȿ�����һ�㣬���㲥��Ϣ�·���
	//��������ͬʱӦ��
	//
	//w5500����
	#if USED_W5500 == TRUE
	{
		w5500_task();
	}
	#endif
}
////////////////////////////////////////////////////////////
void hardward_init(void)
{
	//Ӳ����ʼ������,���ù̶��Ķ˿ڷ���,��������,AD���Ե�
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
	//��ʱ��0
	TCCR0A = 0x00;//��ͨģʽ�������
	TCCR0B = 0x03;//4MHzʱ��,64��Ƶ
	TCNT0 = 256-60;//����62��,Լ1����
	TIMSK0 |= _BV(TOIE0);//��ʱ��0�ж�ʹ��
	TRACE_STRING("timer0 set ok\r\n\0");
	//
	//��ʱ��1
	TCCR1A = 0xa2;//oc1a,oc1bƥ��ʱ����,�ﵽTOPʱ��1
	TCCR1B = 0x19;//ʱ�Ӳ���Ƶ
	TCCR1C = 0x00;//û��ǿ�����
	ICR1 = 1001;//TOP
    bpwma = 500;//ռ�ձ�50%
    bpwmb = 500;
    TRACE_STRING("timer1 set ok\r\n\0");
	//
	//���ж�
	EIMSK = 0;//ȫ�������ⲿ�ж�
	EICRA = 0b00101010;//int2�½��ش���//int1�½��ش���//int0�½��ش���
	EIMSK &= ~_BV(INT2);//w5500_int�жϽ���
	TRACE_STRING("int2 disable(w5500 int)\r\n\0");
	//
	PCICR = 0;
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;
	PCMSK3 = 0;
	//if(MB_PULSE == m_rPara.para.mb_level_pulse)
	{
		PCICR |= _BV(PCIE2);//PCINT23ʹ�ܣ����MB����
		PCMSK2 |= _BV(PCINT23);
	}
	//if(DW_PULSE == m_rPara.para.dw_level_pulse)
	{
		PCICR |= _BV(PCIE3);//PCINT31ʹ�ܣ����DW����
		PCMSK3 |= _BV(PCINT31);
	}
	//
	//SPI����������оƬw5500
	//�жϽ��ã�spiʹ�ܣ���λ�ȳ�,����,mode0(sck����ʱΪ�ͣ��������ص�ʱ���������)
	SPCR = _BV(SPE)|_BV(MSTR);//fosc/4   //��1M��
	//SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR0);//fosc/16   //��256k��
	//SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1);//fosc/64   //��64k��
	//SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);//fosc/128   //��32k��
	SPSR |= _BV(SPI2X); //���ø�λ��SPI�ٶ������������ٶȵ�����
	//
	SPSR=0;
	ddr_w5500_mosi = 1;
	ddr_w5500_miso = 0;
	ddr_w5500_sck = 1;
	TRACE_STRING("spi set ok\r\n\0");
	//
	//ADC
	ADMUX=bD1;//0000 0000//AREF�ο�
	ADCSRA=0xa5;//1010 0101//����adת��,�Զ�����ʹ��,�������ж�,32��Ƶ(125K)
	ADCSRB=0;//��������ת��
	adsc=1;//����ADת��
	TRACE_STRING("adc set ok\r\n\0");
	//
	//USART
	//38461.5 Bd //0.2%
	UBRR0H=0;
	UBRR0L=12;

	//19200
	//UBRR0L=24;
	UCSR0A=(1<<U2X0);//�����ʷ�Ƶ���Ӵ�16����8
	UCSR0B=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);//�����ж�ʹ��
	UCSR0C=(3<<UCSZ00);//8N1
	//
    b485en=0;//����ʹ��,���ͽ�ֹ
    TRACE_STRING("uart0 set ok\r\n\0");
}
////////////////////////////////////////////////////////////////
void pin_init(void)
{
	//ȫ����λ��Ȼ��������
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	DDRA = 0;
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	//
	//��·�̵������
	ddr_by = 1;
	ddr_zk = 1;
	ddr_zt = 1;
	ddr_wg = 1;
	ddr_hz = 1;
	ddr_lz = 1;
	ddr_bj = 1;
	ddr_mbbj = 1; //mb�źŹ��ϱ���
	//
	//��·ADC
	port_d1 = 0;
	port_d2 = 0;
	port_l = 0;
	port_d3 = 0;
	//
	//��·pwm��
	ddr_pwma = 1;
	ddr_pwmb = 1;
	//
	//��·������(������)
	port_lw = 1;
	port_yq = 1;
	port_yz = 1;
	port_mb = 1;
	port_qr = 1;
	port_ql = 1;
	port_dw = 1;
	//
	//485����ʹ��(�ߵ�ƽ��Ч)
	ddr_485en = 1;
	port_485ro = 1;//RXD����
	ddr_485di = 1;//TXD
	//
	//spiģ��uart
	ddr_pc_485en = 1;
	port_pc_485ro = 1;//����
	ddr_pc_485di = 1;//���
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
    ad_counter[bD1]=0;//ad������������
    ad_counter[bD2]=0;//ad������������
    ad_counter[bD3]=0;//ad������������
    ad_counter[bL]=0;//ad������������
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
	//�����ʱ���������
	switch(id)
	{
		case MT_TRY://���Դ���
		{
			//������ͬʱ����
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
			//��ʱ��ȡ������ˢ�¼򵥴���
			read_para(1);//�������ʼ���ϵ����
		}
		break;
		case MT_BASETASK:
		{
			base_task();//10ms�����д���
		}
		break;
		case MT_MB_COUNTER:
		{
			mb_60s_sample_buf[mb_60s_sample_addr] = (mb_counter_tmp+1)/2;//��ƽ�仯
			mb_3s_sample_buf[mb_3s_sample_addr] = (mb_counter_tmp+1)/2;
			mb_counter_tmp = 0;
			//
			mb_60s_sample_addr++;
			mb_60s_sample_addr %= MB_60S_SAMPLE_SIZE;
			mb_3s_sample_addr++;
			mb_3s_sample_addr %= MB_3S_SAMPLE_SIZE;
			//
			//����һ����MB����ֵ
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
			//����һ����MB����ֵ
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
			//��ʱ�����Զ�����
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
		//�����˱��õĵ�������
		if(1 == bit_current_1st)
		{
			//��ǰ��ʾΪ��һ������Ϊ�ڶ�����ʾ
			bit_current_1st = 0;
			bit_current_2nd = 1;
			m_cur_cal2 = m_cur_d2;
			m_cur_round = cur_d2_round;
			//
			//////////////////////���������ж�
			//ע��,�����m_cur_cal�г�10��Ч��
			if(m_cur_cal2 > mmotor_cur2_cmp)
			{
				bit_over_i = 1;//���ص���
				modbus_status.status.m_i_over = STATUS_ON;
				bcomerr = 1;//����������
			}
			else
			{
				bit_over_i = 0;//�������ص���
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
			//���������������ʾֵ
			//��Ϊ���������ֵ�����ܳ���400,������ֵ�����130,����ֱ�ӳ˼���
			m_cur_cal2*=m_rPara.para.current2_modify;
			m_cur_cal2/=100;
		}
		else
		{
			//��ǰ��ʾΪ�ڶ�������Ϊ��һ����ʾ
			bit_current_1st = 1;
			bit_current_2nd = 0;
			m_cur_cal = m_cur_d1;
			m_cur_round = cur_d1_round;
			//
			//////////////////////���������ж�
			//ע��,�����m_cur_cal�г�10��Ч��
			if(m_cur_cal > mmotor_cur1_cmp)
			{
				bit_over_i = 1;//���ص���
				modbus_status.status.m_i_over = STATUS_ON;
				bcomerr = 1;//����������
			}
			else
			{
				bit_over_i = 0;//�������ص���
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
			//���������������ʾֵ
			//��Ϊ���������ֵ�����ܳ���400,������ֵ�����130,����ֱ�ӳ˼���
			m_cur_cal*=m_rPara.para.current1_modify;
			m_cur_cal/=100;
		}
	}
	#else //__D2_USED__
	{
		//û�����ñ��õĵ�������
		bit_current_1st = 0;
		bit_current_2nd = 0;
		m_cur_cal = m_cur_d1;
		m_cur_round = cur_d1_round;
		//////////////////////���������ж�
		//ע��,�����m_cur_cal�г�10��Ч��
		if(m_cur_cal > mmotor_cur1_cmp)
		{
			bit_over_i = 1;//���ص���
			modbus_status.status.m_i_over = STATUS_ON;
			bcomerr = 1;//����������
		}
		else
		{
			bit_over_i = 0;//�������ص���
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
		//���������������ʾֵ
		//��Ϊ���������ֵ�����ܳ���400,������ֵ�����130,����ֱ�ӳ˼���
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
	m_temp.mc[0] = ADCL;//�ۼ�adֵ
	m_temp.mc[1] = ADCH;
	m_temp.ms[0] &= 0x03ff;

	
#if __ZAH_JU_TIANZHEN__ == TRUE        

    ad_temp = ADMUX&0x07;
    if (AD_CHANGE != 0)
    {
	    ad_add_value[ad_temp].ml += m_temp.ms[0];
    }

    //
	//������������
	ad_counter[ad_temp]++;
    AD_CHANGE ++;
	//����1024��ֵ,��ʱ1024����,Ҳ����1024����õ�һ����Ч�Ĳ���ֵ8 

    //  1024  16
    //  128   2
	if(AD_CHANGE >= 17)//Լһ��õ�һ������
	{
        AD_CHANGE = 0;
		//
		ad_channel = ADMUX&0xf0;	

        switch(ad_temp)
		{
			case bD1://����1
			{

     			ad_channel |= bL;

				//
				//if(ad_counter[ad_temp] >= 1024)
				if(ad_counter[ad_temp] >= 512)
                {            

                    ad_counter[ad_temp] = 0;
                   // ad_add_value[ad_temp].ml >>= 4;
                    ad_add_value[ad_temp].ml >>= 3;
                    m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//�õ�����ֵ
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
    						//����������ģʽ��
    						//bit_remote=0;//�˳�Զ��ģʽ
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
			case bL://��λ
			{
				ad_channel|=bD2;

                ///if (ad_counter[ad_temp] >= 1024)
                if (ad_counter[ad_temp] >= 512)
                {
                    ad_counter[ad_temp] = 0;
                    //ad_add_value[ad_temp].ml >>= 4;
                    ad_add_value[ad_temp].ml >>= 3;
                    m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//�õ�����ֵ
                    m_temp.ms[1] = 0;
                    ad_add_value[ad_temp].ml = 0;


                    //////////////////////////////////��AD������λ����
    				if((MATERIAL_PORT_ONLY_AD==m_rPara.para.material_port)	|| (MATERIAL_PORT_RS485_AD==m_rPara.para.material_port))
    				{
    					m_material_value=m_temp.ms[0];
    					material_2_vel_enable=1;
    				}
                }
			}
			break;

            case bD2://����2
			{
				ad_channel|=bD3;
				//////////////////////////////////��AD������λ����
				
                if (ad_counter[ad_temp] >= 32)
                {
                   ad_counter[ad_temp] = 0;
                   ad_add_value[ad_temp].ml >>= 5;
                   m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//�õ�����ֵ
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
                   //����
                   //TRACE_STRING("BD2 ZHI123 =");
                   //send_ulong_ascii(m_temp.ms[0] ,TRUE);

                }

			}
			break;

            case bD3://��λ
			{
				ad_channel|= bD1;
                if (ad_counter[ad_temp] >= 32)
                {
                    ad_counter[ad_temp] = 0;
                    ad_add_value[ad_temp].ml >>= 5;
                    m_temp.ms[0] = ad_add_value[ad_temp].ms[0];//�õ�����ֵ
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

                    //����

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
//������������
ad_counter[0]++;
//����1024��ֵ,��ʱ1024����,Ҳ����1024����õ�һ����Ч�Ĳ���ֵ
if(ad_counter[0] >= 1024)//Լһ��õ�һ������
{
	ad_counter[0] = 0;
	ad_add_value[0].ml >>= 4;
	m_temp.ms[0] = ad_add_value[0].ms[0];//�õ�����ֵ
	m_temp.ms[1] = 0;
	ad_add_value[0].ml = 0;
	//
	ad_channel = ADMUX&0xf0;
	ad_temp = ADMUX&0x07;
	switch(ad_temp)
	{
		case bD1://����1
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
					//����������ģʽ��
					//bit_remote=0;//�˳�Զ��ģʽ
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
			case bD2://����2
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
						//bit_remote=0;//�˳�Զ��ģʽ
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
		case bL://��λ
		{
			ad_channel|=bD1;
			//////////////////////////////////��AD������λ����
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
	//1ms�ж�һ��
	//
	ms1_counter++;
	if(ms1_counter > 9)
	{
		ms1_counter=0;
		//10ms
		//
		//���������,ֹͣ�����ź�,����,���������ź�
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
		mb_invalid_delay++;//10ms���ж�mb�ź��Ƿ���Ч
		//
		ms10_counter++;
		if(ms10_counter > 9)
		{
			ms10_counter=0;
			//
			//100ms
			m_cgq_ack_delay++;//������Ӧ�����ʾʱ��(100ms)
			wait_rollin++;
			key_delay_set_in++;//�ڲ������趨����ȴ�
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
	comm_error_delay=0;//ͨ����ʱ����
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
		if(uart_rx_counter > 6)//����3���ֽ���Ч������һ���ֽ�У���
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
					key_value=uart_rx_buf[1];//�����ֵ
					// key_value_secondary=uart_rx_buf[2];//������ֵ����Ҫ�����ͣ����
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
					m_cgq_cmd=XUYAOSHUJU;//������ΪԶ��������λ�ߵ�֮��,��λ�����õ�
					m_temp.mc[0]=uart_rx_buf[1];//��λֵ�ȷ�
					m_temp.mc[1]=uart_rx_buf[2];
					m_material_add_value.ms+=m_temp.ms;
					m_material_counter++;
					if(m_material_counter > 62)// >= 1260ms
					{
						//if((MATERIAL_PORT_ONLY_RS485==m_rPara.para.material_port)|| (MATERIAL_PORT_RS485_AD==material_port))
						{
							//��485�ڽ�����λ����
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
///////////////       �жϷ������       ////////////////////////
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
//
//��ʱ��0��Ϊȫ�ֶ�ʱ��,����������������ĵ���
//
//unsigned int temp_count_data[10] = {0};
ISR(TIMER0_OVF_vect/*,ISR_NOBLOCK*/)//����ISR_NOBLOCK����,���жϷ�������������ж���Ƕ
{
	TCNT0=256-62;//4MHzʱ��,64��Ƶ,����62��,��ʱԼ1����
    //1ms
    boot_time++;//�����ʱ��ʱ���׼��Ӧ�÷ŵ��жϷ����������������ȴ���ʱ
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
//���ڷ����ж�
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
//���ڽ����ж�
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
			uart_rx_buf[i]='E'; // ������ճ�ʱ���˳��ж�
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
				key_value=uart_rx_buf[2];//�����ֵ
				// key_value_secondary=uart_rx_buf[2];//������ֵ����Ҫ�����ͣ����
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
	//MB�����⣬�½����ж�
	mb_counter_tmp++;
}
/////////////////////////////////////////////////////////////////
ISR(PCINT3_vect)
{
	//DW�����⣬�½����ж�
	dw_counter_tmp++;
}
/////////////////////////////////////////////////////////////////
ISR(WDT_vect)
{
	//���Ź��ж�
	DEBUG_STRING("wdt interrupt");
}
/////////////////////////////////////////////////////////////////
