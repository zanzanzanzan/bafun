/*
 * 该驱动是在w5200的驱动基础上修改而来
 * bear
 * 201607
*/
#include "typedef_user.h"

#if USED_W5500 == TRUE

#include "w5500.h"

/////////////////////////////////////////////////
void w5500_chipselect(Bool bSelect);
U8 w5500_spi_write(U8 data);
U8 w5500_spi_read(U8 *data);
U8 w5500_read(U32 addr);
void w5500_write(U32 addr,U8 value);
U16 w5500_write_buf(U32 addr,U8* buf,U16 len);
U16 w5500_read_buf(U32 addr, U8* buf,U16 len);
//
void w5500_sysinit(U8 * tx_size, U8 * rx_size); // setting tx/rx buf size
//
void set_w5500_MR(U8 val);
void set_w5500_IR( U8 val );
void set_w5500_SIR( U8 val );
void set_w5500_Sn_MR(SOCKET s,U8 val);
void set_w5500_Sn_CR(SOCKET s,U8 val);
void set_w5500_Sn_IR(SOCKET s,U8 val);
void set_w5500_Sn_IMR(SOCKET s,U8 val);
void set_w5500_Sn_PORT(SOCKET s,U16 port);
void set_w5500_Sn_DPORT(SOCKET s,U16 port);
void set_w5500_Sn_DIPR(SOCKET s,U8 *ip);
void set_w5500_RTR(U16 timeout); // set retry duration for data transmission, connection, closing ...
void set_w5500_RCR(U8 retry); // set retry count (above the value, assert timeout interrupt)
void set_w5500_IMR(U8 mask); // set interrupt mask.
void set_w5500_SIMR(U8 mask);
void set_w5500_Sn_MSSR(SOCKET s, U16 MSS); // set maximum segment size
void set_w5500_Sn_PROTO(SOCKET s, U8 proto); // set IP Protocol value using IP-Raw mode
void set_w5500_Sn_TTL(SOCKET s, U8 ttl);
void set_w5500_PHYSTATUS(U8 val);
void set_w5500_Sn_RX_RD(SOCKET s,U16 val);
//
U8 get_w5500_ISR(U8 s);
void put_w5500_ISR(U8 s, U8 val);
U16 get_w5500_RxMAX(U8 s);
U16 get_w5500_TxMAX(U8 s);
U16 get_w5500_RxMASK(U8 s);
U16 get_w5500_TxMASK(U8 s);
U8 get_w5500_IR( void );
U8 get_w5500_PHYSTATUS(void);
U8 get_w5500_SIR( void );
U8 get_w5500_Sn_IR(SOCKET s); // get socket interrupt status
U8 get_w5500_Sn_SR(SOCKET s); // get socket status
U8 get_w5500_Sn_CR(SOCKET s);
U16 get_w5500_Sn_TX_FSR(SOCKET s); // get socket TX free buf size
U16 get_w5500_Sn_RX_RSR(SOCKET s); // get socket RX recv buf size
U8 get_w5500_Sn_SR(SOCKET s);
U16 get_w5500_Sn_RX_RD(SOCKET s);
//
void set_w5500_GAR(U8 * addr); // set gateway address
void set_w5500_SUBR(U8 * addr); // set subnet mask address
void set_w5500_SHAR(U8 * addr); // set local MAC address
void set_w5500_SIPR(U8 * addr); // set local IP address
void get_w5500_GAR(U8 * addr);
void get_w5500_SUBR(U8 * addr);
void get_w5500_SHAR(U8 * addr);
void get_w5500_SIPR(U8 * addr);
//
U8 incr_windowfull_retry_cnt(U8 s);
void init_windowfull_retry_cnt(U8 s);
//
extern U16 swap_ushort(U16 val);
//
/////////////////////////////////////////////////
#define spi_interrupt_flag BIT(SPSR,SPIF)
//
U8 windowfull_retry_cnt[W5500_MAX_SOCK_NUM];
//
static U8  I_STATUS[W5500_MAX_SOCK_NUM];
static U16 SMASK[W5500_MAX_SOCK_NUM]; /**< Variable for Tx buffer MASK in each channel */
static U16 RMASK[W5500_MAX_SOCK_NUM]; /**< Variable for Rx buffer MASK in each channel */
static U16 SSIZE[W5500_MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static U16 RSIZE[W5500_MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */
//
/////////////////////////////////////////////////
U8 incr_windowfull_retry_cnt(U8 s)
{
	return windowfull_retry_cnt[s]++;
}
/////////////////////////////////////////////////
void init_windowfull_retry_cnt(U8 s)
{
	windowfull_retry_cnt[s] = 0;
}
/////////////////////////////////////////////////
U8 get_w5500_ISR(U8 s)
{
	return I_STATUS[s];
}
/////////////////////////////////////////////////
void put_w5500_ISR(U8 s, U8 val)
{
	I_STATUS[s] = val;
}
/////////////////////////////////////////////////
U16 get_w5500_RxMAX(U8 s)
{
	return RSIZE[s];
}
/////////////////////////////////////////////////
U16 get_w5500_TxMAX(U8 s)
{
	return SSIZE[s];
}
/////////////////////////////////////////////////
U16 get_w5500_RxMASK(U8 s)
{
	return RMASK[s];
}
/////////////////////////////////////////////////
U16 get_w5500_TxMASK(U8 s)
{
	return SMASK[s];
}
/////////////////////////////////////////////////
void w5500_sysinit( U8 *tx_size, U8 *rx_size	)
{
	U16 ssum,rsum;

	ssum = 0;
	rsum = 0;
	
	U8 i;
	for (i=0 ; i<W5500_MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		w5500_write((Sn_TXBUF_SIZE(i)),tx_size[i]);//单位是K（1024字节）
		w5500_write((Sn_RXBUF_SIZE(i)),rx_size[i]);


		SSIZE[i] = (U16)(0);
		RSIZE[i] = (U16)(0);

		if (ssum <= 16384)//0x4000
		{
			if(tx_size[i] == 0)
			{				
				SSIZE[i] = (U16)(0);
				SMASK[i] = (U16)(0);
			}
			else if(tx_size[i] <= 16)
			{
				SSIZE[i] = (U16)(1024)*(U16)tx_size[i];
				SMASK[i] = SSIZE[i]-1;
			}
		}

		if (rsum <= 16384)//0x4000
		{			
			if(rx_size[i] == 0)
			{
				RSIZE[i] = (U16)(0);
				RMASK[i] = (U16)(0);				
			}
			else if(rx_size[i] <= 16)
			{
				RSIZE[i] = (U16)(1024)*(U16)rx_size[i];
				RMASK[i] = SSIZE[i]-1;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];
	}
}
///////////////////////////////////////////////////
void set_w5500_GAR(U8 * addr)//a pointer to a 4 -byte array responsible to set the Gateway IP address.
{
	//This function sets up gateway IP address.
	w5500_write_buf(GAR0,addr,4);
}
///////////////////////////////////////////////////
void set_w5500_SUBR(U8 * addr)//a pointer to a 4 -byte array responsible to set the SubnetMask address
{
	//It sets up SubnetMask address
	w5500_write_buf(SUBR0,addr,4);
}
///////////////////////////////////////////////////
void set_w5500_SHAR(U8 * addr)//a pointer to a 6 -byte array responsible to set the MAC address.
{
	//This function sets up MAC address.
	w5500_write_buf(SHAR0,addr,6);
}
///////////////////////////////////////////////////
void set_w5500_SIPR(U8 * addr)//a pointer to a 4 -byte array responsible to set the Source IP address.
{
	//This function sets up Source IP address.
	w5500_write_buf(SIPR0,addr,4);
}
///////////////////////////////////////////////////
void get_w5500_GAR(U8 * addr)
{
	//This function sets up Source IP address.
	w5500_read_buf(GAR0,addr,4);
}
///////////////////////////////////////////////////
void get_w5500_SUBR(U8 * addr)
{
	w5500_read_buf(SUBR0,addr,4);
}
///////////////////////////////////////////////////
void get_w5500_SHAR(U8 * addr)
{
	w5500_read_buf(SHAR0,addr,6);
}
///////////////////////////////////////////////////
void get_w5500_SIPR(U8 * addr)
{
	w5500_read_buf(SIPR0,addr,4);
}
///////////////////////////////////////////////////
void set_w5500_MR(U8 val)
{
	w5500_write(MR,val);
}
///////////////////////////////////////////////////
void set_w5500_Sn_MR(SOCKET s,U8 val)
{
	w5500_write(Sn_MR(s),val);
}
///////////////////////////////////////////////////
void set_w5500_Sn_PORT(SOCKET s,U16 port)
{
	port = swap_ushort(port);
	w5500_write_buf(Sn_PORT0(s),(U8*)&port,2);
}
///////////////////////////////////////////////////
void set_w5500_Sn_DPORT(SOCKET s,U16 port)
{
	port = swap_ushort(port);
	w5500_write_buf(Sn_DPORT0(s),(U8*)&port,2);
}
///////////////////////////////////////////////////
void set_w5500_Sn_DIPR(SOCKET s,U8 *ip)
{	
	w5500_write_buf(Sn_DIPR0(s),ip,4);
}
///////////////////////////////////////////////////
void set_w5500_Sn_CR(SOCKET s,U8 val)
{
	w5500_write(Sn_CR(s),val);
}
///////////////////////////////////////////////////
U8 get_w5500_IR( void )
{
	//This function gets Interrupt register in common register.
	U8 val,val1;
	U8 i;
	for(i=0; i<5; i++)
	{
		val = w5500_read(IR);
		asm volatile("nop");
		asm volatile("nop");
		val1 = w5500_read(IR);
		if(val == val1)
		{
			break;
		}
	}
	//
	return val;
}
///////////////////////////////////////////////////
U8 get_w5500_PHYSTATUS(void)
{
	U8 val,val1;
	U8 i;
	for(i=0; i<5; i++)
	{
		val = w5500_read(PHYCFGR);
		asm volatile("nop");
		asm volatile("nop");
		val1 = w5500_read(PHYCFGR);
		if(val == val1)
		{
			break;
		}
	}
	//
	return val;
}	
///////////////////////////////////////////////////
void set_w5500_IR( U8 val )
{
    w5500_write(IR,val);
}
///////////////////////////////////////////////////
U8 get_w5500_SIR( void )
{
	//This function gets Interrupt register in common register.
	U8 val,val1;
	U8 i;
	for(i=0; i<5; i++)
	{
		val = w5500_read(SIR);
		asm volatile("nop");
		asm volatile("nop");
		val1 = w5500_read(SIR);
		if(val == val1)
		{
			break;
		}
	}
	//
	return val;
}
///////////////////////////////////////////////////
void set_w5500_PHYSTATUS(U8 val)
{
	w5500_write(PHYCFGR,val);
}
///////////////////////////////////////////////////
void set_w5500_SIR( U8 val )
{
    w5500_write(SIR,val);
}
///////////////////////////////////////////////////
void set_w5500_RTR(U16 timeout)
{
	//This function sets up Retransmission time.
	//
	//If there is no response from the peer or delay in response then retransmission 
	//will be there as per RTR (Retry Time-value Register)setting
	//
	timeout = swap_ushort(timeout);
	w5500_write_buf(RTR0,(U8*)&timeout,2);
}
///////////////////////////////////////////////////
void set_w5500_RCR(U8 retry)
{
	//This function set the number of Retransmission.
	//
	//If there is no response from the peer or delay in response then recorded time 
	//as per RTR & RCR register seeting then time out will occur.
	//
	w5500_write(RCR,retry);
}
///////////////////////////////////////////////////
void set_w5500_SIMR(U8 mask)
{
	//This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)
	//
	//If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
	//set in IR register.
	//
	w5500_write(SIMR,mask); 
}
///////////////////////////////////////////////////
void set_w5500_IMR(U8 mask)
{
	w5500_write(IMR,mask);
}
///////////////////////////////////////////////////
void set_w5500_Sn_IR(SOCKET s,U8 val)
{
	w5500_write(Sn_IR(0),val);
}
///////////////////////////////////////////////////
void set_w5500_Sn_IMR(SOCKET s,U8 val)
{
	w5500_write(Sn_IMR(0),val);
}
///////////////////////////////////////////////////
void set_w5500_Sn_MSSR(SOCKET s, U16 MSS)
{
	//This sets the maximum segment size of TCP in Active Mode), 
	//while in Passive Mode this is set by peer
	//
	MSS = swap_ushort(MSS);
	w5500_write_buf(Sn_MSSR0(s),(U8*)&MSS,2);
}
///////////////////////////////////////////////////
void set_w5500_Sn_TTL(SOCKET s, U8 ttl)
{
	w5500_write(Sn_TTL(s), ttl);
}
///////////////////////////////////////////////////
U8 get_w5500_Sn_IR(SOCKET s)
{
	//get socket interrupt status
	//
	//These below functions are used to read the Interrupt & Soket Status register
	//
	U8 val,val1;
	U8 i;
	for(i=0; i<5; i++)
	{
		val = w5500_read(Sn_IR(s));
		asm volatile("nop");
		asm volatile("nop");
		val1 = w5500_read(Sn_IR(s));
		if(val == val1)
		{
			break;
		}
	}
	//
	return val;
}
///////////////////////////////////////////////////
U8 get_w5500_Sn_SR(SOCKET s)
{
	//get socket status
	U8 val,val1;
	U8 i;
	for(i=0; i<5; i++)
	{
		val = w5500_read(Sn_SR(s));
		asm volatile("nop");
		asm volatile("nop");
		val1 =w5500_read(Sn_SR(s));
		if(val == val1)
		{
			break;
		}
	}
	//
	return val;
}
///////////////////////////////////////////////////
U8 get_w5500_Sn_CR(SOCKET s)
{
	//get CMD status
	U8 val,val1;
	U8 i;
	for(i=0; i<5; i++)
	{
		val = w5500_read(Sn_CR(s));
		asm volatile("nop");
		asm volatile("nop");
		val1 = w5500_read(Sn_CR(s));
		if(val == val1)
		{
			break;
		}
	}
	//
	return val;
}
///////////////////////////////////////////////////
U16 get_w5500_Sn_TX_FSR(SOCKET s)
{
	//get socket TX free buf size
	//
	//This gives free buffer size of transmit buffer. This is the data size that user can transmit.
	//User shuold check this value first and control the size of transmitting data
	//
	U16 val=0,val1=0;
	unsigned char retry=0;
	//
	do
	{
		if(retry++ > 5)
		{
			break;
		}
		//
		val1 = w5500_read(Sn_TX_FSR0(s));
		asm volatile("nop");
		val1 = (val1 << 8) + w5500_read(Sn_TX_FSR1(s));
		if (val1 != 0)
		{
			asm volatile("nop");
			asm volatile("nop");
   			val = w5500_read(Sn_TX_FSR0(s));
   			asm volatile("nop");
   			val = (val << 8) + w5500_read(Sn_TX_FSR1(s));
		}
	} while (val != val1);
	//
	return val;
}
///////////////////////////////////////////////////
U16 get_w5500_Sn_RX_RSR(SOCKET s)
{
	//get socket RX recv buf size
	//
	//This gives size of received data in receive buffer. 
	//
	U16 val=0,val1=0;
	unsigned char retry=0;
	//
	do
	{
		if(retry++ > 5)
		{
			break;
		}
		//
		val1 = w5500_read(Sn_RX_RSR0(s));
		asm volatile("nop");
		val1 = (val1 << 8) + w5500_read(Sn_RX_RSR1(s));
		if(val1 != 0)
		{
			asm volatile("nop");
			asm volatile("nop");
   			val = w5500_read(Sn_RX_RSR0(s));
   			asm volatile("nop");
   			val = (val << 8) + w5500_read(Sn_RX_RSR1(s));
		}
	} while (val != val1);
	//
	return val;
}
///////////////////////////////////////////////////
void set_w5500_Sn_RX_RD(SOCKET s,U16 val)
{
	val = swap_ushort(val);
	w5500_write_buf(Sn_RX_RD0(s),(U8*)&val,2);
}
///////////////////////////////////////////////////
U16 get_w5500_Sn_RX_RD(SOCKET s)
{
	U16 tmp=0,tmp1=0;
	unsigned char retry = 0;
	do 
	{
		if(retry++ > 5)
		{
			break;
		}
		//
		tmp1 = w5500_read(Sn_RX_RD0(s));
		asm volatile("nop");
		tmp1 = ((tmp1 & 0x00ffUL) << 8) + w5500_read(Sn_RX_RD1(s));
		asm volatile("nop");
		asm volatile("nop");
		tmp = w5500_read(Sn_RX_RD0(s));
		asm volatile("nop");
		tmp = ((tmp & 0x00ffUL) << 8) + w5500_read(Sn_RX_RD1(s));
	} while (tmp != tmp1);
	//
	return tmp;
}
///////////////////////////////////////////////////
void w5500_chipselect(Bool bSelect)
{
	//确保w5500_cs管腿为输出
	ddr_w5500_cs = 1;
	//
	if (bSelect)
	{
		// Select SPI chip.
		//
		bw5500_cs = 0;
		asm volatile("nop");
		asm volatile("nop");
	}
	else
	{
		// Unselect SPI chip.
		asm volatile("nop");
		asm volatile("nop");
		bw5500_cs = 1;
	}
}
/////////////////////////////////////////////////
U16 w5500_write_buf(U32 addr,U8* buf,U16 len)
{
	//This function writes into W5500 memory(Buffer)
	U16 idx;
	
	if(len == 0)
	{
		return 0;
	}
	
	//SPI MODE I/F
	w5500_chipselect(TRUE);                             // CS=0, SPI start
	ddr_w5500_mosi = 1;
	ddr_w5500_sck = 1;
	
	w5500_spi_write((addr & 0x00FF0000) >> 16);              // Address byte 1
	w5500_spi_write((addr & 0x0000FF00) >> 8);              // Address byte 2
	w5500_spi_write((U8)(addr & 0x000000F8)+0b0100);        // 控制字段
	//
	delay_us(5);
	//
	for(idx = 0; idx < len; idx++)                          // Write data in loop
	{
		w5500_spi_write(buf[idx]);
	}
	//
	w5500_chipselect(FALSE);                            // CS=1, SPI end
	//
	return len;
}
///////////////////////////////////////////////////
U16 w5500_read_buf(U32 addr, U8* buf,U16 len)
{
	//This function reads into W5500 memory(Buffer)
	//
	U16 idx;
	//
	w5500_chipselect(TRUE);
	ddr_w5500_mosi = 1;
	ddr_w5500_sck = 1;                             // CS=0, SPI start
	//
	w5500_spi_write((addr & 0x00FF0000) >> 16);              // Address byte 1
	w5500_spi_write((addr & 0x0000FF00) >> 8);              // Address byte 2
	w5500_spi_write(addr & 0x000000F8);                     // 控制字段
	//
	delay_us(5);
	//
	for(idx = 0; idx < len; idx++)                      // Read data in loop
	{
		w5500_spi_read(&buf[idx]);
	}
	//
	w5500_chipselect(FALSE);                            // CS=0, SPI end
	//
	return len;
}
///////////////////////////////////////////////////
U8 w5500_read(U32 addr)
{
	U8 val;
	//
	w5500_chipselect(TRUE);
	ddr_w5500_mosi = 1;
	ddr_w5500_sck = 1;
	//
	w5500_spi_write((addr&0x00ff0000)>>16);
	w5500_spi_write((addr&0x0000ff00)>>8);
	w5500_spi_write(addr&0x000000f8);
	//
	delay_us(5);
	//
	w5500_spi_read(&val);
	//
	w5500_chipselect(FALSE);
	//
	return val;
}
/////////////////////////////////////////////////
void w5500_write(U32 addr,U8 value)
{
	w5500_chipselect(TRUE);
	ddr_w5500_mosi = 1;
	ddr_w5500_sck = 1;
	//
	w5500_spi_write((addr&0x00ff0000)>>16);
	w5500_spi_write((addr&0x0000ff00)>>8);
	w5500_spi_write((U8)(addr&0x000000f8)+0b0100);
	//
	delay_us(5);
	//
	w5500_spi_write(value);
	//
	w5500_chipselect(FALSE);
}
/////////////////////////////////////////////////
//
//the SPIF bit is cleared by first reading the SPI Status Register with SPIF set, 
//then accessing the SPI Data Register (SPDR).
//
//该函数只是写数据,对读出的数据不关心
U8 w5500_spi_write(U8 data)
{
	U16 timeout = SPI_TIMEOUT;
	SPSR;//清除SPIF(不要改变)
	SPDR = data;//(不要改变)
	asm volatile("nop");
	asm volatile("nop");
	while(FALSE == spi_interrupt_flag)
	{
		timeout--;
		if(0 == timeout)
		{
			return FALSE;
		}
	}
	//
	return TRUE;
}
//////////////////////////////////////////////////
//该函数只为读数据,写入的数值无所谓
U8 w5500_spi_read(U8 *data)
{
	U16 timeout = SPI_TIMEOUT;
	//
	SPDR = SPSR;//清除SPIF(不要改变)
	asm volatile("nop");
	asm volatile("nop");
	while(FALSE == spi_interrupt_flag)
	{
		timeout--;
		if(0 == timeout)
		{
			return FALSE;
		}
	}
	//
	*data = SPDR;
	//
	return TRUE;
}
//////////////////////////////////////////////////




#endif //USED_W5500


