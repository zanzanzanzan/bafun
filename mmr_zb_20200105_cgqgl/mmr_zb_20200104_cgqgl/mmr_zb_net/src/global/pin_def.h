/*
 * pin_def.h
 *
 * Created: 2014/10/4 12:29:19
 *  Author: xiong
 */ 


#ifndef PIN_DEF_H_
#define PIN_DEF_H_


/////////////////////////////////////////////////////////////////
//管脚定义
//七路继电器输出
#define bmmstart   BIT(PORTC,PC0) //out,19//ZK 是主电机启动  
#define ddr_zk     BIT(DDRC,PC0)
#define bmmstop    BIT(PORTC,PC1) //out,20//ZT是是主电机停    190915版  修改为传感器通讯控制
#define ddr_zt     BIT(DDRC,PC1)
#define bftstart   BIT(PORTC,PC2) //out,21//WG是启动变频器   
#define ddr_wg     BIT(DDRC,PC2)
#define brollin    BIT(PORTC,PC3) //out,22//HZ是离合闸
#define ddr_hz     BIT(DDRC,PC3)
#define brolllock  BIT(PORTC,PC4) //out,23//LZ磨辊锁紧
#define ddr_lz     BIT(DDRC,PC4)
#define bcomerr    BIT(PORTC,PC5) //out,24//BJ是通讯错误及过载输出以及QR和QL信号错误
#define ddr_bj     BIT(DDRC,PC5)
#define bmbbj      BIT(PORTC,PC6) //out,25//备用
#define ddr_mbbj   BIT(DDRC,PC6) //mb信号报警

#define bmbby      BIT(PORTC,PC6) //out,25//备用
#define ddr_by     BIT(DDRC,PC6) //mb信号报警

//四路ADC
#define bD1        6              //ADC6,31
#define port_d1    BIT(PORTA,PA6)
#define bD2        7              //ADC7,30
#define port_d2    BIT(PORTA,PA7)
#define bL         0              //ADC0,37
#define port_l     BIT(PORTA,PA0)
#define bD3        1              //ADC1,36
#define port_d3    BIT(PORTA,PA1)
//两路pwm出
#define bpwma      OCR1A           //pd5,out,14
#define ddr_pwma   BIT(DDRD,PD5)
#define bpwmb      OCR1B           //pd4,out,13
#define ddr_pwmb   BIT(DDRD,PD4)
//八路数字入(光电隔离)
#define blw        BIT(PINA,PA2) //in,35 //整个A口都设置为输入了
#define port_lw    BIT(PORTA,PA2)
#define byq        BIT(PINA,PA3) //in,34
#define port_yq    BIT(PORTA,PA3)
#define ddr_yq     BIT(DDRA,PA3)
#define byz        BIT(PINA,PA4) //in,33
#define port_yz    BIT(PORTA,PA4)
#define bmb        BIT(PINC,PC7) //in,11//int0
#define port_mb    BIT(PORTC,PC7) //PD2 -> PC7
#define bqr        BIT(PIND,PD6) //in,15
#define port_qr    BIT(PORTD,PD6)
#define bql        BIT(PINA,PA5) //in,32
#define port_ql    BIT(PORTA,PA5)
#define bdw        BIT(PIND,PD7) //in,16 //PB2 -> PD7
#define port_dw    BIT(PORTD,PD7)
//485发送使能(高电平有效)
#define b485en     BIT(PORTB,PB1) //out,41
#define ddr_485en  BIT(DDRB,PB1)
//硬件串口
#define b485ro     BIT(PIND,PD0)  //in,9,rxd
#define port_485ro BIT(PORTD,PD0)
#define b485di     BIT(PORTD,PD1) //out,10,txd
#define ddr_485di  BIT(DDRD,PD1)
//485发送使能
#define bpc_485en		BIT(PORTB,PB0) //out,40
#define ddr_pc_485en	BIT(DDRB,PB0)
//对上位机串口
#define bpc_485ro		BIT(PIND,PD2)  //in,11 //PD3 -> PD2
#define port_pc_485ro   BIT(PORTD,PD2)
#define ddr_pc_485ro    BIT(DDRD,PD2)
#define bpc_485di		BIT(PORTD,PD3) //out,22 // PC7 -> PD3
#define ddr_pc_485di	BIT(DDRD,PD3)
//
#define bw5500_int      BIT(PORTB,PB2)
#define ddr_w5500_int   BIT(DDRB,PB2)
#define bw5500_cs       BIT(PORTB,PB3)
#define ddr_w5500_cs    BIT(DDRB,PB3)
#define bw5500_rst      BIT(PORTB,PB4)
#define ddr_w5500_rst   BIT(DDRB,PB4)
#define bw5500_mosi     BIT(PORTB,PB5)
#define ddr_w5500_mosi  BIT(DDRB,PB5)
#define bw5500_miso     BIT(PORTB,PB6)
#define ddr_w5500_miso  BIT(DDRB,PB6)
#define bw5500_sck      BIT(PORTB,PB7)
#define ddr_w5500_sck   BIT(DDRB,PB7)
////////////////////////////////////////////////////////////////
#define adsc			BIT(ADCSRA,ADSC)
////////////////////////////////////////////////////////////////
#define is_wdt_rst      BIT(MCUSR,WDRF)
////////////////////////////////////////////////////////////////




#endif /* PIN_DEF_H_ */