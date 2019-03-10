//ICC-AVR application builder : 2016/5/14 10:46:21
// Target : M16
// Crystal: 4.0000Mhz

#include <iom16v.h>
#include <macros.h>

#include <delay.h>
#include <time.h>
#include <uart.h>
//delayms
//PA2 CLK
//PA3 SDA


#define  uint  unsigned int 
#define  uchar unsigned char 
#define  Nack_counter  10 


//************ 数据定义**************** 
unsigned char bit_out; 
unsigned char bit_in; 




//

#define SCKOUT DDRA|= 0X4; 
#define SCL1 PORTA |= 0X4;
#define SCL0 PORTA &= ~0X4;


#define SDAOUT DDRA|= 0X8;
#define SDA1 PORTA |= 0X8;
#define SDA0 PORTA &= ~0X8;

#define PIN  DDRA &= ~0X8;
#define READ  PINA&0X8







//************ 函数声明***************************************** 
void   start_bit(void);                 //MLX90614 发起始位子程序 
void   stop_bit(void);                 //MLX90614发结束位子程序 
uchar  rx_byte(void);              //MLX90614 接收字节子程序 
void   send_bit(void);             //MLX90614发送位子程序 
void   tx_byte(uchar dat_byte);     //MLX90614 接收字节子程序 
void   receive_bit(void);           //MLX90614接收位子程序 
uint   memread(unsigned char addr, unsigned char cmd);             // 读温度数据 

uchar DataH,DataL,Pecreg; 

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 
 PORTC = 0x03; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
}



unsigned int P_bao=100;
unsigned int Bao_sum=0;
unsigned int Bao_time=0;
//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts
 port_init();
 timer0_init();
 uart0_init();

 MCUCR = 0x00;
 GICR  = 0x00;
 TIMSK = 0x01; //timer interrupt sources
 SEI(); //re-enable interrupts
 //all peripherals are now initialized
}
unsigned int count=0;
void main(void)
{
 uint Tem; 
 unsigned char addr;
 init_devices();
 
SDAOUT;
SCKOUT; 
 //进入总线模式
 SCL1;SDA1;
 delayms(2);
 SCL0;
 delayms(5000);
 SCL1;

 
 
 //进入测试模式
 /*
 SCL0;SDA1;
 delayms(10);
 SCL1;
 delayms(10);
 SDA0;
 delayms(30000);
 SDA1;
*/
 

while(0)
{


PIN;
if (READ)
{
 SCL1;

 delayms(100);
 }
 else
 {

  SCL0;

   delayms(200);
}

}
 
 while(1)
 {


 	    
 if((count>=20))
  {
   count=0;
   //USART_send(0x55)*0.02-273.15;        
   
   Tem = memread(0,0x2e);
   Tem = memread(Tem&0xff,0x07);
   
   Tem = Tem / 50 - 273;
   	USART_send(Tem>>8);
	USART_send(Tem&0xff);

    
  }
 

}
}





//*********输入转换并显示********* 
void display(uint Tem) 
{ 
 uint T,a,b; 
 T=Tem*2; 
 if(T>=27315) 
 { 
   T=T-27315; 
   a=T/100; 
   b=T-a*100; 
 } 
 else 
 { 
  T=27315-T; 
    a=T/100; 
    b=T-a*100; 
 } 
} 

//************************************ 
void   start_bit(void) 
{ 
   SDAOUT;
   SDA1; 
   delayms(3);
   SCL1; 
   delayms(3);
   SDA0; 
   delayms(3);
   SCL0; 
   delayms(3);
 
} 
//------------------------------ 
void   stop_bit(void) 
{ 
   SDAOUT;
   SCL0; 
   delayms(3);
   SDA0; 
   delayms(3);
   SCL1; 
   delayms(3);
   SDA1; 
} 

/*
void  tx_byte(uchar dat_byte) 
{ 
   char i,n,dat; 
   n=Nack_counter; 
   dat=dat_byte; 
   for(i=0;i<8;i++) 
   { 
     if(dat&0x80) 
      bit_out=1; 
     else 
      bit_out=0; 
     send_bit(); 
     dat=dat<<1; 
   } 
   
   receive_bit(); 

} */



//--------- 发送一个字节--------- 

void  tx_byte(uchar dat_byte) 
{ 
   char i,n,dat; 
   n=Nack_counter; 
TX_again: 
   dat=dat_byte; 
   for(i=0;i<8;i++) 
   { 
     if(dat&0x80) 
	 {
      bit_out=1; 
	  }
     else 
	 {
      bit_out=0;
	  } 
     send_bit(); 
     dat=dat<<1; 
   } 
   
   receive_bit(); 
   if(bit_in==1) 
   { 
    stop_bit(); 
    if(n!=0) 
    {n--;goto Repeat;} 
    else 
     goto exit; 
   } 
   else 
    goto exit; 
Repeat: 
    start_bit(); 
    goto TX_again; 
exit: 
;
  // USART_send(n)  ; 
} 

//300 720us
//低电平  27 ms  高电平时  45 us
//-----------发送一个位--------- 
void  send_bit(void) 
{ 
   SDAOUT;
  if(bit_out==0) 
  {
     SDA0; 
  }
  else 
  {
     SDA1; 
  }
  delayms(1);
  SCL1; 
  delayms(3);
  SCL0; 
  delayms(2);

} 
//---------- 接收一个字节-------- 
uchar rx_byte(void) 
{ 
  uchar i,dat; 
  dat=0; 
  for(i=0;i<8;i++) 
  { 
    dat=dat<<1; 
    receive_bit(); 
    if(bit_in==0x8) 
     dat=dat+1; 
  } 
  send_bit(); 
  return dat; 
} 

//---------- 接收一个位---------- 
void receive_bit(void) 
{ 
  //SDA1;
  PIN;
  delayms(1);
  SCL1; 

  delayms(3);
  bit_in=READ; 
  //delayms(2);
  SCL0; 
  delayms(2);
} 
//------------ 延时-------------- 
/*void   delay(uint N) 
{ 
  uint i; 
  for(i=0;i<N;i++) 
  
     _nop_(); 
} */
//------------------------------ 
uint memread(unsigned char slave_addr, unsigned char cmd) 
{ 
  start_bit(); 
  tx_byte(slave_addr << 1);  //Send SlaveAddress ==============================
  //tx_byte(0x00); 
  tx_byte(cmd);  //Send Command 
  //------------ 
  start_bit(); 
  //tx_byte(0x01); 
  tx_byte((slave_addr << 1)+1);  //Send SlaveAddress ==============================
  bit_out=0; 
  DataL=rx_byte(); 
  bit_out=0; 
  DataH=rx_byte(); 
  bit_out=1; 
  Pecreg=rx_byte(); 
  stop_bit(); 
  return(DataH*256+DataL); 
} 
 
