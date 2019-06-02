////////////////////////////////////////////////
//
//   该驱动是基于w5500中文数据手册v1.3描述的信息编写
//
//   UDP模式
//
//   bear,201605
//
////////////////////////////////////////////////


#include "typedef_user.h"


#if USED_W5500 == TRUE


#include "pin_def.h"
#include "conf_w5500.h"
#include "core_func.h"
#include "crc32.h"
#include "crc8.h"
#include "w5500.h"
////////////////////////////////////////////////
//函数声明
//
Bool init_w5500(U32 reset_ms);
Bool init_w5500_sub(void);
void w5500_read_para(void);
//
void w5500_soft_reset(void); // reset w5500
//
U8 W5500_socket_send(U8 *data,const U8 len);
U16 W5500_socket_recv(U8 *data,const U16 max_len);
U16 w5500_socket_frame_handing(U8 *data,const U16 max_len);
void w5500_send_data_processing(SOCKET s, U8 *wizdata, const U16 len);
U16 w5500_recv_data(SOCKET s, U8 *data,const U16 max_len);
U16 w5200_read_data(SOCKET s, U8 *data, const U16 len);
//
void w5500_task(void);
//
void w5500_judge_and_read_write_data(void);
void w5500_judge_and_read_data(void);
//
void reset_w5500(U16);
void create_mac(const IP_PARA *pIP);
void enet_analysis_udp(APP_HEAD *pAH,U8 *pV,U8 len);
//
U8 w5500_udp_open(U8 socket,U16 port);
U8 w5500_udp_close(U8 socket);
void w5500_phy_status_check(void);
void w5500_socket_status_check(void);
void w5500_rx_ignore(U8 sn, U16 len);
//
U8 get_sum(U8 *p,U8 len);
U16 swap_ushort(U16 val);
////////////////////////////////////////////////
//变量定义
//
//收发通道分别最大缓存16k
//大数据量的分配在通道0和1（如大量的历史数据查询等）
//中等数据量的分配在通道2和3（如单路的语音实时传输等）
//小数据量的分配在通道4到7（如服务器状态，单项的指令等）
U8 w5500_txsize[W5500_MAX_SOCK_NUM] = {16,0,0,0,0,0,0,0};//2代表2k（2*1024）
U8 w5500_rxsize[W5500_MAX_SOCK_NUM] = {16,0,0,0,0,0,0,0};
//
U16 pre_sent_ptr, sent_ptr;
U8 w5500_link_on_count=0;
U16 w5500_link_off_count=0;
//
volatile Bool w5500_is_tx_ing;
volatile U8 w5500_rx_buf[W5500_RX_BUFFER_FRAME_NUM][W5500_RX_BUFFER_FRAME_SIZE];
volatile U8 w5500_rx_frame_count,w5500_rx_write_id,w5500_rx_read_id;
volatile DEC_ADDR_PACKET w5500_dap_tx;
//
volatile Bool w5500_phy_link=FALSE,w5500_udp_open_ok=FALSE;
volatile Bool w5500_phy_speed_100m,w5500_phy_speed_10m,w5500_phy_dpx_full,w5500_phy_dpx_half;
volatile U32 w5500_phystatus_read_space;
//
unsigned short local_udp_port=16529;//20160529
//
volatile U32 w5500_rx_count = 0;
////////////////////////////////////////////////
void w5500_task(void)
{
	//#define W5500_TASK TRUE
	#define W5500_TASK FALSE
	//
	wdt_reset();//网络任务中喂狗
	//
	//w5500物理层状态检测
	w5500_phy_status_check(); 
	//
	//w5500数据接收和发送（不主动发送数据，只在收到命令后才回数据）
	w5500_judge_and_read_write_data();
}
////////////////////////////////////////////////
void w5500_phy_status_check(void)
{
	U16 w5500_link_off_wait=150; //1500ms
	//
	U8 status = get_w5500_PHYSTATUS();
	//
	if(PHYCFGR_LNK_ON == (status&PHYCFGR_LNK_ON))//LINK
	{
		//link on
		if(w5500_link_on_count >= 2)
		{
			if(FALSE == w5500_phy_link)
			{
				w5500_phy_link = TRUE;
				w5500_link_off_wait=150; //1500ms
				//
				TRACE_STRING("w5500 phy link on\r\n\0");
				//
				if(FALSE == w5500_udp_open_ok)
				{
					w5500_udp_open_ok = w5500_udp_open(0,local_udp_port);
				}
			}
		}
		//
		w5500_link_off_count=0;
		w5500_link_on_count++;
	}
	else
	{
		//link off
		if(w5500_link_off_count >= 3)
		{
			if(TRUE == w5500_phy_link)
			{
				w5500_phy_link = FALSE;
				TRACE_STRING("w5500 phy link off\r\n\0");
				//
				w5500_udp_open_ok = FALSE;
			}
		}
		//
		w5500_link_off_count++;
		w5500_link_on_count=0;
	}
	//
	#if 1
	if(PHYCFGR_SPD_100 == (status&PHYCFGR_SPD_100))//SPEED
	{
		w5500_phy_speed_10m = FALSE;
		if(FALSE == w5500_phy_speed_100m)
		{
			w5500_phy_speed_100m = TRUE;
			//
			TRACE_STRING("w5500 phy speed 100m\r\n\0");
		}
	}
	else
	{
		w5500_phy_speed_100m = FALSE;
		if(FALSE == w5500_phy_speed_10m)
		{
			w5500_phy_speed_10m = TRUE;
			//
			TRACE_STRING("w5500 phy speed 10m\r\n\0");
		}
	}
	//
	if(PHYCFGR_DPX_FULL == (status&PHYCFGR_DPX_FULL))//DPX
	{
		w5500_phy_dpx_half = FALSE;
		if(FALSE == w5500_phy_dpx_full)
		{
			w5500_phy_dpx_full = TRUE;
			//
			TRACE_STRING("w5500 phy dpx full\r\n\0");
		}
	}
	else
	{
		w5500_phy_dpx_full = FALSE;
		if(FALSE == w5500_phy_dpx_half)
		{
			w5500_phy_dpx_half = TRUE;
			//
			TRACE_STRING("w5500 phy dpx half\r\n\0");
		}
	}
	#endif
	///////////////////////////////////////////////////////////////////
	#if 1
	//断开网络，尝试初始化W5500
	if(w5500_link_off_count >= w5500_link_off_wait) //n*10ms
	{
		w5500_link_off_count=0;
		//
		signed char retry;
		for(retry=3; retry>0; retry--)
		{
			if(TRUE == init_w5500(2)) //100
			{
				break;
			}
		}
		if(0 == retry)
		{
			w5500_link_off_wait=2000; //n*10ms //2s
		}
		//
		TRACE_STRING("w5500 link off overtime,init w5500\r\n\0");
	}
	#endif
	///////////////////////////////////////////////////////////////////	
}
////////////////////////////////////////////////
void w5500_judge_and_read_write_data(void)
{	
	//关键函数
	//
	if(FALSE == w5500_udp_open_ok)
	{
		return;
	}
	if(FALSE == w5500_phy_link)
	{
		return;
	}
	//读取数据
	w5500_judge_and_read_data();
	//
	//处理数据
	U8 read_count;
	for(read_count=0; read_count<W5500_RX_BUFFER_FRAME_NUM/2; read_count++)
	{
		if(w5500_rx_frame_count > 0)
		{
			//处理接收缓存数据
			w5500_socket_frame_handing((U8*)w5500_rx_buf[w5500_rx_read_id],W5500_RX_BUFFER_FRAME_SIZE);
			//
			w5500_rx_frame_count--;
			//
			w5500_rx_read_id++;
			w5500_rx_read_id %= W5500_RX_BUFFER_FRAME_NUM;
		}
		else
		{
			break;
		}
	}	
}
////////////////////////////////////////////////
void w5500_judge_and_read_data(void)
{
	//把w5500接收缓存的数据读取到本地缓存
	//
	//
	if(TRUE == w5500_phy_link)
	{
		//读取w5500接收的数据
		int i;
		for(i=0; i<W5500_RX_BUFFER_FRAME_NUM/2; i++)
		{
			if(0 == (get_w5500_Sn_RX_RSR(0) & get_w5500_RxMASK(0)))
			{
				break;
			}
			//
			//接收缓存有数据
			//
			//W5500_socket_recv是个重点函数
			U16 rx_write_len = W5500_socket_recv((U8*)w5500_rx_buf[w5500_rx_write_id],W5500_RX_BUFFER_FRAME_SIZE);
			if(rx_write_len > 0)
			{
				//读取了全部或者部分数据
				//更新缓存写入位置
				w5500_rx_write_id++;
				w5500_rx_write_id %= W5500_RX_BUFFER_FRAME_NUM;
				//
				//更新接收数据缓存区有效字节数
				w5500_rx_frame_count ++;
			}
		}
	}
	else
	{
		DEBUG_STRING("w5500_phy_link is FALSE\r\n\0");
	}	
}
////////////////////////////////////////////////
void enet_analysis_udp(APP_HEAD *pAH,U8 *pV,U8 len)
{
	extern void modbus_read_maintain_reg_frame_handling(U8 port_type);
	extern READ_MAINTAIN_REG_FRAME *read_maintain_reg_frame_ok;
	extern void modbus_write_multi_reg_frame_handling(U8 port_type);
	extern WRITE_MULTI_REG_FRAME *write_multi_reg_frame_ok;
	//
	switch(pAH->cmd_type)
	{
		//根据命令类型分析，为稍微复杂点的应用做准备
		case FRAME_COMMAND_RUNNING:
		{
			//确定命令类型后，分析具体命令
			switch(pAH->cmd)
			{
				//根据具体命令，分析帧内容
				case CMD_READ_SYSTEM_STATUS:
				{
					memcpy(read_maintain_reg_frame_ok->buf,pV,sizeof(READ_MAINTAIN_REG_FRAME));
					modbus_read_maintain_reg_frame_handling(PORT_ENET);
				}
				break;
			}
		}
		break;
		case FRAME_COMMAND_PARA:
		{
			//确定命令类型后，分析具体命令
			switch(pAH->cmd)
			{
				//根据具体命令，分析帧内容
				case CMD_READ_PARA:
				{
					memcpy(read_maintain_reg_frame_ok->buf,pV,sizeof(READ_MAINTAIN_REG_FRAME));
					modbus_read_maintain_reg_frame_handling(PORT_ENET);
				}
				break;
				case CMD_WRITE_PARA:
				{
					memcpy(write_multi_reg_frame_ok->buf,pV,sizeof(WRITE_MULTI_REG_FRAME));
					modbus_write_multi_reg_frame_handling(PORT_ENET);
				}
				break;
			}
		}
		break;
	}
}
//////////////////////////////////////////////////
void reset_w5500(U16 ms)
{
	ddr_w5500_rst = 1;
	bw5500_rst = 0;
	//
	U16 i;
	for(i=0; i<ms; i++)
	{
		delay_ms(1);
	}
	//
	bw5500_rst = 1;
}
////////////////////////////////////////////////
void create_mac(const IP_PARA *pIP)
{
	//需要计算mac地址
	unsigned char tmp[32];
	unsigned long crc32_value;
	tmp[0] = 'I';
	tmp[1] = 'P';
	tmp[2] = 'J';//74//注意：MAC地址的第一个字节必须是偶数，奇数是用来作为组播地址的。
	tmp[3] = 'X';
	tmp[4] = TCNT0;//随机数
	tmp[5] = TCNT2;
	tmp[6] = TCNT0;
	tmp[7] = TCNT2;
	tmp[8] = TCNT0;
	tmp[9] = TCNT2;
	memcpy(&tmp[10],(char*)pIP,12);
	tmp[22] = TCNT1L;//随机数
	tmp[23] = TCNT1H;
	tmp[24] = TCNT3L;
	tmp[25] = TCNT3H;
	crc32_reset();
	crc32_iterate_buffer(tmp,22+4);
	crc32_value=crc32_get_value();
	//
	memcpy((char*)&tmp[4],(char*)&crc32_value,4);
	memcpy((char*)&tmp[8],(char*)pIP,sizeof(IP_PARA));
	//将新的网络参数写入EEPROM
	unsigned char i;
	uint8_t *p = (uint8_t *)&m_ePara[0].para.w5500_net_para;//‘IP’也写入了FLASH了
	for(i=0; i<sizeof(NET_PARA); i++)
	{
		eeprom_write_byte_user(&p[i],tmp[i]);
	}
}
///////////////////////////////////////////////
U8 w5500_udp_open(U8 socket,U16 port)
{
	if(socket >= W5500_MAX_SOCK_NUM)
	{
		return FALSE;
	}
	//
	if (0 == port)
	{
		return FALSE;
	}
	//
	w5500_udp_close(socket);
	//
	set_w5500_Sn_MR(socket,Sn_MR_UDP);
	set_w5500_Sn_PORT(socket,port);
	//
	signed char retry;
	for(retry = 5;retry > 0;retry--)
	{
		//套接字socket设置为UDP模式并打开套接字
		set_w5500_Sn_CR(socket,Sn_CR_OPEN);
		//
		unsigned char i;
		unsigned char sn_sr;
		for(i=50;i>0;i--)
		{
			delay_ms(2);
			sn_sr = get_w5500_Sn_SR(socket);
			if(sn_sr == SOCK_UDP)
			{
				break;
			}
			else
			{
				DEBUG_STRING("w5500_Sn_SR = \0");
				DEBUG_VALUE_TEXT(sn_sr);
			}
		}
		if(0==i)
		{
			w5500_udp_close(socket);
			delay_ms(10);
		}
		else
		{
			break;
		}
	}
	//
	if(0 == retry)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}	
}
///////////////////////////////////////////////
U8 w5500_udp_close(U8 socket)
{
	U8 i;
	//
	if(socket >= W5500_MAX_SOCK_NUM)
	{
		return FALSE;
	}
	//
	set_w5500_Sn_CR(socket, Sn_CR_CLOSE);// 等待命令执行完成
	for(i=0; i<200; i++) //100ms
	{
		if(0 == get_w5500_Sn_CR(socket))
		{
			break;
		}
		delay_us(500);
	}
	//
	return TRUE;	
}
///////////////////////////////////////////////
Bool init_w5500_sub(void)
{
	w5500_is_tx_ing=FALSE;
	w5500_phy_link=FALSE;
	w5500_phystatus_read_space=boot_time;
	//
	w5500_rx_frame_count=0;
	w5500_rx_write_id=0;
	w5500_rx_read_id=0;
	//
	ddr_w5500_int = 0;
	bw5500_int = 1;//上拉
	//
	//Init. TX & RX Memory size
	w5500_sysinit(w5500_txsize, w5500_rxsize);
	//
	set_w5500_MR(0);//MR_PB//资料显示复位值为0，该处显性设置下
	//
    //Set PTR and RCR register	
    set_w5500_RTR(WINDOWFULL_WAIT_TIME);//重试时间寄存器，每一个单位值代表100us
    set_w5500_RCR(WINDOWFULL_MAX_RETRY_NUM);//重试计数寄存器，最多重传次数
	//
	set_w5500_SHAR(m_rPara.para.w5500_net_para.mac_para.mac);
    set_w5500_SUBR(m_rPara.para.w5500_net_para.ip_para.net_mask);
    set_w5500_GAR(m_rPara.para.w5500_net_para.ip_para.gateway);
    set_w5500_SIPR(m_rPara.para.w5500_net_para.ip_para.ip_addr);
	//
	//可以用硬件来设置w5500的PHY工作模式，如果硬件已经设置正确该处就不需要再设置了
	//设置PHY为10M全双工，并设置RST位为零，重启PHY
	set_w5500_PHYSTATUS(PHYCFGR_OPMD|PHYCFGR_OPMDC_10F);//PHYCFGR_OPMDC_10H
	delay_ms(1);
	//PHY重启后，设置RST为1，终止PHY的重启
	set_w5500_PHYSTATUS(get_w5500_PHYSTATUS()|PHYCFGR_RST);
	//
	set_w5500_Sn_MR(0,0);//资料显示复位值为0，该处显性设置下
	set_w5500_IMR(IMR_UNREACH|IMR_CONFLICT);
	set_w5500_Sn_IMR(0,Sn_IMR_TIMEOUT);//通道0允许时间溢出中断
	//
	set_w5500_SIMR(0b00000000);
	//
	w5500_udp_open_ok = w5500_udp_open(0,local_udp_port);//套接字0，端口号16529，打开
	//
	return w5500_udp_open_ok;
}
////////////////////////////////////////////////
Bool init_w5500(U32 reset_ms)
{
	bw5500_int = 1;//上拉
	ddr_w5500_int = 0;
	bw5500_cs = 1;//不选中
	ddr_w5500_cs = 1;
	bw5500_rst = 1;//不复位
	ddr_w5500_rst = 1;
	//
	reset_w5500(reset_ms);//带有延时
	//
	return init_w5500_sub();
}
////////////////////////////////////////////////
U8 W5500_socket_send(U8 *data,const U8 len)
{
	//#define W5500_SOCKET0_SEND TRUE
	#define W5500_SOCKET0_SEND FALSE
	//
	static U8 tx_tmp[sizeof(APP_HEAD)+260]; //按最大缓存配备，没管发送多少字节了
	U16 tx_len = sizeof(APP_HEAD)+len;
	//
	//外部程序调用
	U8 *w5500_frame_data;
	U16 w5500_frame_len;
	//
	#if W5500_SOCKET0_SEND == TRUE
	{
		TRACE_STRING("w5500 socket0 send\r\n\0");
	}
	#endif
	//
	{
		//发送已经完成
		//本来该检测是应该有的，但我们的程序结构不一样，要发送的数据不会多，而且
		//基本不会两个帧同时等待发送，而且发送缓存设置足够大（目前是8K），所以
		//可以不检测发送缓存不够的情况	
		U32 w5500_tx_wait;
		w5500_tx_wait = boot_time;
		for(;;) //等待发送缓存足够发送待发送数据
		{
			//等待发送缓存空出
			if(tx_len <= get_w5500_Sn_TX_FSR(0))
			{
				break;
			}
			if((boot_time-w5500_tx_wait) > (2000))//2000ms
			{
				DEBUG_STRING("tx_len <= get_w5500_Sn_TX_FSR(0)\r\n\0");
				return FALSE;
			}
		}
	}
	//#endif
	//
	#if W5500_SOCKET0_SEND == TRUE
	{
		TRACE_STRING("socket0 tx buf idle\r\n\0");
	}
	#endif
	//
	APP_HEAD tmp;
	tmp.frame_flag = ENET_FRAME_FLAG;
	tmp.cmd_type = FRAME_COMMAND_RUNNING;
	tmp.cmd = ACK_READ_SYSTEM_STATUS;
	tmp.frame_id = 240; //不关心的数值
	tmp.len = len;
	tmp.verify.crc8 = crc8_cal(data,len);
	tmp.verify.sum8 = 0;
	tmp.verify.sum8 = get_sum((U8*)&tmp,sizeof(APP_HEAD));
	tmp.verify.sum8 ^= 0xff;
	tmp.verify.sum8++;
	//
	memcpy(tx_tmp,(U8*)&tmp,sizeof(APP_HEAD));
	memcpy(&tx_tmp[sizeof(APP_HEAD)],data,len);
	//
	w5500_frame_len = tx_len;
	w5500_frame_data = tx_tmp;
	//
	set_w5500_Sn_DIPR(0,(U8*)w5500_dap_tx.ip);
	set_w5500_Sn_DPORT(0,w5500_dap_tx.port);
	//
	w5500_is_tx_ing=TRUE;
	w5500_send_data_processing(0,w5500_frame_data,w5500_frame_len);
	//
	return TRUE;
}
///////////////////////////////////////////////////
U8 get_sum(U8 *p,U8 len)
{
	U8 sum=0;
	U8 i;
	for(i=0; i<len; i++)
	{
		sum += p[i];
	}	
	//
	return sum;
}
///////////////////////////////////////////////////
U16 swap_ushort(U16 val)
{
	//ATMega系列单片机是小端模式存储
	//
	UCHAR2 tmp;
	U8 tmp_t;
	//
	tmp.ms = val;
	tmp_t = tmp.mc[0];
	tmp.mc[0] = tmp.mc[1];
	tmp.mc[1] = tmp_t;
	//
	return tmp.ms;
}
///////////////////////////////////////////////////
U16 w5500_socket_frame_handing(U8 *data,const U16 max_len)
{	
	APP_HEAD head;
	memcpy((char*)&head,data,sizeof(APP_HEAD));
	//
	if(ENET_FRAME_FLAG != head.frame_flag)
	{
		DEBUG_STRING("frame_flag is error\r\n\0");
		return 0;
	}
	//
	U8 sum = get_sum((U8*)&head,sizeof(APP_HEAD));
	if(0 != sum)
	{
		DEBUG_STRING("sum8 error, sum=\0");
		DEBUG_VALUE_TEXT(sum);
		return 0;
	}
	//
	if(FRAME_COMMAND_RUNNING == head.cmd_type)
	{	
		SGDK_UDP_READ_MAINTAIN_REG_FRAME tmp;
		memcpy(tmp.buf,data,sizeof(SGDK_UDP_READ_MAINTAIN_REG_FRAME));
		//
		U8 crc8 = crc8_cal(tmp.frame.val.buf,sizeof(READ_MAINTAIN_REG_FRAME));
		if(tmp.frame.head.verify.crc8 != crc8)
		{
			DEBUG_STRING("crc8 error(read maintain reg), crc8=\0");
			DEBUG_VALUE_TEXT(crc8);
			return 0;
		}
		//
		enet_analysis_udp(&tmp.frame.head,tmp.frame.val.buf,sizeof(READ_MAINTAIN_REG_FRAME));	
	}
	else if(FRAME_COMMAND_PARA == head.cmd_type)
	{
		if(CMD_READ_PARA == head.cmd)
		{
			//读参数
			SGDK_UDP_READ_PARA_FRAME tmp;
			memcpy(tmp.buf,data,sizeof(SGDK_UDP_READ_PARA_FRAME));
			//
			U8 crc8 = crc8_cal(tmp.frame.val.buf,sizeof(READ_MAINTAIN_REG_FRAME));
			if(tmp.frame.head.verify.crc8 != crc8)
			{
				DEBUG_STRING("crc8 error(read para), crc8=\0");
				DEBUG_VALUE_TEXT(crc8);
				return 0;
			}
			//
			enet_analysis_udp(&tmp.frame.head,tmp.frame.val.buf,sizeof(READ_MAINTAIN_REG_FRAME));
		}
		else if(CMD_WRITE_PARA == head.cmd)
		{
			//写参数
			SGDK_UDP_WRITE_PARA_FRAME tmp;
			memcpy(tmp.buf,data,sizeof(SGDK_UDP_WRITE_PARA_FRAME));
			//
			U8 crc8 = crc8_cal(tmp.frame.val.buf,sizeof(WRITE_MULTI_REG_FRAME));
			if(tmp.frame.head.verify.crc8 != crc8)
			{
				DEBUG_STRING("crc8 error(write para), crc8=\0");
				DEBUG_VALUE_TEXT(crc8);
				return 0;
			}
			//
			enet_analysis_udp(&tmp.frame.head,tmp.frame.val.buf,sizeof(WRITE_MULTI_REG_FRAME));
		}
	}
	//
	return head.len;
}
///////////////////////////////////////////////////
void w5500_read_para(void)
{
	//地址等信息
	unsigned char i;
	for(i=0; i<4; i++)
	{
		//ipaddr,netgate,netmask
		m_rPara.para.w5500_net_para.ip_para.ip_addr[i] = eeprom_read_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.ip_addr[i]);
		m_rPara.para.w5500_net_para.ip_para.gateway[i] = eeprom_read_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.gateway[i]);
		m_rPara.para.w5500_net_para.ip_para.net_mask[i] = eeprom_read_byte_user(&m_ePara[0].para.w5500_net_para.ip_para.net_mask[i]);
	}
	//
	//macaddr
	for(i=0; i<6; i++)
	{
		m_rPara.para.w5500_net_para.mac_para.mac[i] = eeprom_read_byte_user(&m_ePara[0].para.w5500_net_para.mac_para.mac[i]);
	}
}
/////////////////////////////////////////////////
void w5500_soft_reset(void)
{
	//This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode	
	set_w5500_MR(MR_RST);
}
///////////////////////////////////////////////////
void w5500_send_data_processing(SOCKET s, U8 *data, const U16 len)
{
	//#define W5500_SEND_DATA_PROCESSING TRUE
	#define W5500_SEND_DATA_PROCESSING FALSE

	//This function is being called by send() and sendto() function also. 
	//
	//This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
	//register. User should read upper byte first and lower byte later to get proper value.
	//
	UNION_PU8 tmp;
	tmp.val = w5500_read(Sn_TX_WR0(s));
	tmp.val = ((tmp.val & 0x000000ffUL) << 8) + w5500_read(Sn_TX_WR1(s));
	//
	#if W5500_SEND_DATA_PROCESSING == TRUE
	{
		TRACE_STRING("dst addr is: ");
		TRACE_VALUE_TEXT(tmp.val);
		TRACE_STRING("len = ");
		TRACE_VALUE_TEXT(len);
	}
	#endif
	//
	//w5500_write_data(s, data, tmp.ptr, len);
	U32 addrbsb = (tmp.val<<8) + (s<<5) + 0x10; //0x10实际上是选择套接字发送缓存0x02左移了三位
	w5500_write_buf(addrbsb, data, len);
	//
	tmp.val += len;
	w5500_write(Sn_TX_WR0(s),(U8)((tmp.val & 0x0000ff00UL) >> 8));
	w5500_write(Sn_TX_WR1(s),(U8)(tmp.val & 0x000000ffUL));	
	//
	set_w5500_Sn_CR(0,Sn_CR_SEND);
	//
	U8 i;
	for(i=0; i<200; i++) //等待命令执行完
	{
		if(0 == get_w5500_Sn_CR(s))
		{
			break;
		}
		delay_us(500);
	}	
	//
	w5500_is_tx_ing=FALSE; //中断处理中完成
}
///////////////////////////////////////////////////
U16 W5500_socket_recv(U8 *data,const U16 max_len)
{
	//注意比对帧长度和缓存长度，帧长度不应该超过缓存长度
	//重点函数
	//
	return w5500_recv_data(0,data,max_len);
}
///////////////////////////////////////////////////
U16 w5500_recv_data(SOCKET s, U8 *data,const U16 max_len)
{
	//This function read the Rx read pointer register
	//and after copy the data from receive buffer update the Rx write pointer register.
	//User should read upper byte first and lower byte later to get proper value.
	//
	U16 len,len_r;
	//
	len_r = get_w5500_Sn_RX_RSR(s); //读套接字已经接收的字节数
	len = (len_r&get_w5500_RxMASK(s));
	//
	//if(len < sizeof(W5500_UDP_HEAD))
	if(len <= sizeof(W5500_UDP_HEAD))
	{
		//udp包信息头的长度为8字节
		DEBUG_STRING("len <= sizeof(W5500_UDP_HEAD), len=\0");
		DEBUG_VALUE_TEXT(len);
		DEBUG_STRING("len_r=\0");
		DEBUG_VALUE_TEXT(len_r);
		//
		w5500_rx_ignore(s,len); //更新读指针
		return 0;
	}
	//
	if(len > W5500_RX_BUFFER_FRAME_SIZE)
	{
		//接收的数据太长也不正常
		DEBUG_STRING("len > W5500_RX_BUFFER_FRAME_SIZE, len=\0");
		DEBUG_VALUE_TEXT(len);
		//
		w5500_rx_ignore(s,len); //更新读指针
		return 0;		
	}
	//
	W5500_UDP_HEAD m_head;
	w5200_read_data(s,(U8*)&m_head,sizeof(W5500_UDP_HEAD));
	//
	//网络通信是以大端的方式传送的，需要转换为小端模式
	m_head.len = swap_ushort(m_head.len);
	m_head.sent_port = swap_ushort(m_head.sent_port);
	//
	//保存发送方ip地址和端口号，便于发送时使用
	//目前的程序结构是，不会主动往外发，必须是收到数据帧才会响应
	memcpy((void*)w5500_dap_tx.ip,(void*)m_head.sent_ip,4);
	w5500_dap_tx.port=m_head.sent_port;
	//
	if(0 == m_head.len)
	{
		w5500_udp_open_ok = w5500_udp_open(0,local_udp_port);
		DEBUG_STRING("m_head.len is zero\r\n\0");
		return 0;		
	}
	//
	if(m_head.len > (len-sizeof(W5500_UDP_HEAD)))
	{
		//判断接收缓存接收长度正常
		w5500_rx_ignore(s,len-sizeof(W5500_UDP_HEAD)); //更新读指针
		//
		DEBUG_STRING("m_head.len > (len-sizeof(W5500_UDP_HEAD))\r\n\0");
		DEBUG_STRING("m_head.len=\0");
		DEBUG_VALUE_TEXT(m_head.len);
		DEBUG_STRING("len=\0");
		DEBUG_VALUE_TEXT(len);
		return 0;
	}
	//
	if(m_head.len > max_len)
	{
		//限制读取长度，保证缓存不溢出
		w5500_rx_ignore(s,m_head.len); //更新读指针
		DEBUG_STRING("m_head.len > max_len\r\n\0");
		return 0;
	}
	//
	w5200_read_data(s,data,m_head.len);
	//
	//for debug
	w5500_rx_count++;
	//
	return m_head.len;
}
//////////////////////////////////////////////////
U16 w5200_read_data(SOCKET s, U8 *data, const U16 len)
{
	U16 len_r;
	UNION_PU8 tmp,tmp1;
	//
	tmp1.val = get_w5500_Sn_RX_RD(s); //读接收读指针寄存器
	tmp.val = tmp1.val&get_w5500_RxMASK(s);
	//
	U32 addr_t;
	U32 addrbsb;
	addr_t = tmp.val+len;
	if(addr_t >= get_w5500_RxMAX(s))
	{
		//已经循环存储，需要分两次读
		//
		//DEBUG_STRING("w5500 rx buf is full\r\n\0");
		//
		len_r = get_w5500_RxMAX(s)-tmp.val;
		addrbsb = (tmp.val<<8) + (s<<5) + 0x18; //0x18实际上是选择套接字接收缓存0x03左移了三位
		w5500_read_buf(addrbsb,data,len_r);
		//
		data += len_r;
		//
		len_r = len-len_r;
		tmp.val = 0; //前面已经读到缓存结尾处了，现在从头开始读
		addrbsb = (tmp.val<<8) + (s<<5) + 0x18; //0x18实际上是选择套接字接收缓存0x03左移了三位
		w5500_read_buf(addrbsb,data,len_r);
		//
		tmp1.val += len;
		set_w5500_Sn_RX_RD(s,(U16)tmp1.val);//更新读指针
	}
	else
	{
		//还没有循环存储，一次就可以读取完成
		len_r = len;
		addrbsb = (tmp.val<<8) + (s<<5) + 0x18; //0x18实际上是选择套接字接收缓存0x03左移了三位
		w5500_read_buf(addrbsb,data,len_r);
		//
		tmp1.val += len;
		set_w5500_Sn_RX_RD(s,(U16)tmp1.val);//更新读指针
	}
	//
	set_w5500_Sn_CR(0,Sn_CR_RECV);
	U8 i;
	for(i=0; i<200; i++) //等待命令执行完
	{
		if(0 == get_w5500_Sn_CR(s))
		{
			break;
		}
		delay_us(500);
	}
	//
	return len;	
}
//////////////////////////////////////////////////
void w5500_rx_ignore(U8 s, U16 len)
{
	w5500_udp_open_ok = w5500_udp_open(0,local_udp_port);
}
///////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////







#endif //USED_W5500




