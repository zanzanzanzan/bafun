/*
 * uart_to_pc.c
 *
 * Created: 2016/5/4 19:55:31
 *  Author: xiong
 */ 
/////////////////////////////////////////////////////////////////
//
#include "typedef_user.h"
#include "modbus_rtu.h"
//
/////////////////////////////////////////////////////////////////
//
extern NVM_DATA m_rPara;
//
/////////////////////////////////////////////////////////////////
void uart1_task(void);
void uart1_init(void);
void set_modbus_baud(unsigned char baud);
void send_uart1_to_computer(unsigned char *p,unsigned char len);
void send_ulong_ascii(unsigned long n,unsigned char newline);
void bootloader_key_word(unsigned char val);
void bootloader_handling(void);
//
volatile UCHAR4 bootloader_flag;
volatile unsigned bootloader_flay_is_valid;
/////////////////////////////////////////////////////////////////
void uart1_task(void)
{	
	if(TRUE == bootloader_flay_is_valid)
	{
		//???bootloader
		bootloader_flay_is_valid = FALSE;
		bootloader_handling();
	}
}
/////////////////////////////////////////////////////////////////
void uart1_init(void)
{
	set_modbus_baud(m_rPara.para.baud_modbus);
	//
	ddr_pc_485di = 1;
	ddr_pc_485ro = 0;
	ddr_pc_485en = 1;
	bpc_485di=1;
	port_pc_485ro=1;//??
	bpc_485en = 0;	
	//
	bootloader_flay_is_valid = FALSE;
	bootloader_flag.ml = 0;
}
/////////////////////////////////////////////////////////////////
void set_modbus_baud(unsigned char baud)
{
	//?????????
	UCSR1B &= ~(_BV(RXEN1)|_BV(TXEN1));
	delay_ms(5);
	//

	baud = MODBUS_BAUD_38400;
	switch(baud)
	{
		case MODBUS_BAUD_9600://9600
		{
			//9615.38 Bd //0.16%
			UBRR1H = 0;
			UBRR1L = 25;
			UCSR1A &= ~_BV(U2X1);//????????16,???
		}
		break;
		case MODBUS_BAUD_19200://19200
		{
			//19230.76 Bd //0.16%
			UBRR1H = 0;
			UBRR1L = 12;
			UCSR1A &= ~_BV(U2X1);//????????16,???
		}
		break;
		case MODBUS_BAUD_38400://38400
		{
			//38461.5 Bd //0.2%
			UBRR1H = 0;
			UBRR1L = 12;
			UCSR1A |= _BV(U2X1);//????????8,??
		}
		break;
		default:
		{
			//9615.38 Bd //0.16%
			UBRR1H = 0;
			UBRR1L = 25;
			UCSR1A &= ~_BV(U2X1);//????????16,???
		}
		break;
	}
	//
	UCSR1B = _BV(RXEN1)|_BV(TXEN1)|_BV(RXCIE1);//??????
	UCSR1C = (3<<UCSZ10);//8N1
	bpc_485en = 0;//????,????
	delay_ms(10);
}
////////////////////////////////////////////////////////////////
void send_uart1_to_computer_1(unsigned char *p,unsigned char len)
{


	ddr_pc_485di=1;
	ddr_pc_485en=1;
	bpc_485di=1;
	bpc_485en=1;//发送使能
	delay_us(800);
	//
	if(NULL == p)
	{
		//等待发送缓冲器为空
		while (!(UCSR1A & (1<<UDRE1)));
		//将数据放入缓冲器，发送数据
		UDR1 = (unsigned char)len;
	}
	else
	{
		unsigned char i;
		for(i=0; i<len; i++)
		{
			while (!(UCSR1A & (1<<UDRE1)));
			UDR1 = p[i];
		}
	}
	//
	while (!(UCSR1A & (1<<UDRE1)));
	//
	delay_us(400);
	bpc_485en=0;
}

void send_uart1_to_computer(unsigned char *p,unsigned char len)
{
}

////////////////////////////////////////////////////////////////
void send_ulong_ascii(unsigned long n,unsigned char newline)
{
	char tmp[13];
	int i = sizeof(tmp) - 1;

	// Convert the given number to an ASCII decimal representation.
	tmp[i] = '\0';
	if(TRUE == newline)
	{
		tmp[--i]= '\n';
		tmp[--i]= '\r';
	}
	else
	{
		tmp[--i]= ' ';
	}
	do
	{
		tmp[--i] = '0' + n % 10;
		n /= 10;
	} while (n);

	// Transmit the resulting string with the given USART.
	send_uart1_to_computer((unsigned char*)(tmp+i),(U8)strlen(tmp+i));
}
////////////////////////////////////////////////////////////////
void bootloader_key_word(unsigned char val)
{
	bootloader_flag.ml <<= 8;
	bootloader_flag.mc[0] = val;
	if(('B'==bootloader_flag.mc[3]) && ('E'==bootloader_flag.mc[2]) &&
	   ('A'==bootloader_flag.mc[1]) && ('R'==bootloader_flag.mc[0]))
	{
		bootloader_flay_is_valid = TRUE;
		//
		bootloader_handling();
		//
		//for test
		//DEBUG_STRING("enter bootloader\r\n\0");
	}
}
/////////////////////////////////////////////////////////////////
void bootloader_handling(void)
{
	//设置复位后进入bootloader标志，并执行复位操作
	//
	#define UPDATE_KEY_WORD  0x1324
	//
	extern void wdt_reset_mcu(void);
	//
	//在链接过程中已经隔离出一部分EEPROM空间给bootloader用
	eeprom_write_word((uint16_t *)0x10,UPDATE_KEY_WORD);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)0x30,UPDATE_KEY_WORD);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)0x50,UPDATE_KEY_WORD);
	eeprom_busy_wait();
	//
	delay_ms(50);
	wdt_reset_mcu();
}
/////////////////////////////////////////////////////////////////
/////////////////////    中断服务函数   ////////////////////////
////////////////////////////////////////////////////////////////
//
//串口接收中断
//
unsigned char usart1_receive_data[24];
unsigned char usart1_receive_count =0;
extern unsigned char temp_data[40];
void temp_rtu_rx_frame(unsigned char val)
{

/*
	unsigned char i,sum ;
	if (val ==0x55)
	{
		usart1_receive_count = 0;
	}
	usart1_receive_data[usart1_receive_count++]=val;

	if (usart1_receive_count==6)
	{
		usart1_receive_count = 0;
		sum =0x55;
		for (i = 1;i < 5; i++)
		{
			sum += usart1_receive_data[i];

		}
		//if (sum == usart1_receive_data[5])
		{
			if(usart1_receive_data[0] == 0x55 &&  usart1_receive_data[4]== 0x45 && usart1_receive_data[3] < 10)
			{
				temp_data[usart1_receive_data[4]*2] = usart1_receive_data[1];
				temp_data[usart1_receive_data[4]*2 + 1] = usart1_receive_data[2];
			}
		}
	}
*/
}

extern unsigned int temp_count_data[10];
//10通道寄存器
//AA 55 01 E5 01 A7 01 AB 01 AD 01 A9 01 B0 01 E5 01 A0 01 BC 01 DC 55 AA 
ISR(USART1_RX_vect)
{

	unsigned char sum =0,i;
	//接收中断处理函数
	//
	unsigned char val = UDR1;
	//
	bootloader_key_word(val);
	modbus_rtu_rx_frame(val);
	//temp_rtu_rx_frame(val);
	
	if (val == 0xaa)
	{
		usart1_receive_count = 0;
	}
	usart1_receive_data[usart1_receive_count++] = val;

	if (usart1_receive_count == 24)
	{	

		sum = 0xaa;
		for (i = 1; i< 23; i++)
		{
			sum += usart1_receive_data[i];
		}

		usart1_receive_count = 0;
		{
			if(sum == usart1_receive_data[23]&&usart1_receive_data[0] == 0xaa &&  usart1_receive_data[1]  <2)
			{
				for (i =1; i < 11; i ++)
				{
					temp_count_data[usart1_receive_data[1]] =0;
					temp_data[ usart1_receive_data[1] * 20 + i * 2]      =    usart1_receive_data[i * 2];					
					temp_data[ usart1_receive_data[1] * 20 + i * 2 + 1]  =    usart1_receive_data[i * 2 + 1];
				}
			}
		}
		
	}
	
	
}
/////////////////////////////////////////////////////////////////




