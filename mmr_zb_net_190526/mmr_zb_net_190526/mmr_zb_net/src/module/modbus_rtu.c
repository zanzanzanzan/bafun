/*
 * modbus_rtu.c
 *
 * Created: 2014/11/22 10:52:06
 *  Author: xiong
 */
 /////////////////////////////////////////////////////////////////
 #include "typedef_user.h"
 #include "core_func.h"
 #include "uart_to_pc.h"
 #include "soft_timer.h"
 #include "w5500_manager.h"
 #include "Scan_key.h"
 /////////////////////////////////////////////////////////////////
 #define CMD_READ_MAINTAIN_REG		0x03 //读保持寄存器
 #define CMD_WRITE_MULTI_REG        0x10 //写多个寄存器
 //
 #define SIZE_READ_MAINTAIN_REG     sizeof(READ_MAINTAIN_REG_FRAME)//2+6
 #define SIZE_WRITE_MULTI_REG       sizeof(WRITE_MULTI_REG_FRAME)
 //
 #define ERR_OPCODE					1//操作码错误
 #define ERR_DATA_ADDR				2//数据地址错误
 #define ERR_DATA_VALUE				3//数据值错误 
 //
 #define ADDR_START_INPUT_STATUS	0x0100 //输入端状态起始地址
 #define ADDR_START_OUTPUT_STATUS	0x0200 //输出端状态起始地址
 #define ADDR_START_RUNNING_STATUS	0x0300 //运行状态起始地址
 //
 #define ADDR_START_NVM_PARA        0x0400 //参数
 /////////////////////////////////////////////////////////////////
 void modbus_task(void);
 void modbus_init(void);
 void modbus_rtu_rx_frame(unsigned char val);
 void modbus_read_maintain_reg_frame_handling(U8 port_type);
 unsigned short modbus_cal_crc16(unsigned char *p,unsigned short len);
 void modbus_read_maintain_reg_frame_response(U8 port_type,READ_MAINTAIN_REG_FRAME *p);
 void modbus_write_multi_reg_frame_handling(U8 port_type);
 void modbus_write_multi_reg_frame_response(U8 port_type,WRITE_MULTI_REG_FRAME *p);
 void modbus_clean_rx_flag(void);
 void modbus_status_update(void);
 //
 extern U8 W5500_socket_send(U8 *data,U8 len);
 /////////////////////////////////////////////////////////////////
 //
 volatile SYSTEM_STATUS_MODBUS modbus_status;
 extern unsigned char key_status[20];
 extern volatile unsigned char reset_cause;
 //
 unsigned char is_read_maintain_reg_frame=FALSE,is_write_multi_reg_frame=FALSE;
 READ_MAINTAIN_REG_FRAME read_maintain_reg_frame[2];
 READ_MAINTAIN_REG_FRAME *read_maintain_reg_frame_ok;
 READ_MAINTAIN_REG_FRAME *read_maintain_reg_frame_tmp;
 WRITE_MULTI_REG_FRAME write_multi_reg_frame[2];
 WRITE_MULTI_REG_FRAME *write_multi_reg_frame_ok;
 WRITE_MULTI_REG_FRAME *write_multi_reg_frame_tmp;
 unsigned char read_maintain_reg_frame_id;
 unsigned char write_multi_reg_frame_id;
 unsigned char is_read_maintain_reg_frame_ok=FALSE,is_write_multi_reg_frame_ok=FALSE;
 unsigned char modbus_tx_buf[254];
 /////////////////////////////////////////////////////////////////
 void modbus_task(void)
 {
	 modbus_status_update();
	 //
	 if(TRUE == is_read_maintain_reg_frame_ok)
	 {
		 //读维护寄存器
		 is_read_maintain_reg_frame_ok = FALSE;
		 modbus_read_maintain_reg_frame_handling(PORT_USART);
	 }
	 //
	 if(TRUE == is_write_multi_reg_frame_ok)
	 {
		 //写多个寄存器
		 is_write_multi_reg_frame_ok = FALSE;
		 modbus_write_multi_reg_frame_handling(PORT_USART);
	 }
 }
 extern char bit_rolllock;
 /////////////////////////////////////////////////////////////////
 void modbus_status_update(void)
 {
	 //原有的一些状态就不管了，新增加的状态尽量集中在该处更新。
	 //5ms调用一次，对效率没有什么影响。
	 //bear, 20170514
	 modbus_status.status.m_rolllocked = bit_rolllock;
	 modbus_status.status.m_duliao = bit_duliao;
	 modbus_status.status.m_mb_validity = mb_is_invalid;
 }
 /////////////////////////////////////////////////////////////////
 void modbus_init(void)
 {
	 uart1_init();
	 //
	 is_read_maintain_reg_frame = FALSE;
	 is_write_multi_reg_frame = FALSE;
	 is_read_maintain_reg_frame_ok = FALSE;
	 is_write_multi_reg_frame_ok = FALSE;
	 //
	 read_maintain_reg_frame_ok = &read_maintain_reg_frame[1];
	 read_maintain_reg_frame_tmp = &read_maintain_reg_frame[0];
	 write_multi_reg_frame_ok = &write_multi_reg_frame[1];
	 write_multi_reg_frame_tmp = &write_multi_reg_frame[0];
	 //
	 memset((char*)modbus_status.buf,STATUS_OFF,sizeof(SYSTEM_STATUS_MODBUS));
	 //
	 modbus_status.status.m_wendu.gun[0] = 21;
	 modbus_status.status.m_wendu.gun[1] = 25;
	 modbus_status.status.m_wendu.zhou1[0] = 64; //>55
	 modbus_status.status.m_wendu.zhou1[1] = 33;
	 modbus_status.status.m_wendu.zhou2[0] = 48; //>55
	 modbus_status.status.m_wendu.zhou2[1] = 70;
 }
 /////////////////////////////////////////////////////////////////
 void modbus_write_multi_reg_frame_handling(U8 port_type)
 {
	 UCHAR2 modbus_crc16;
	 modbus_crc16.ms = modbus_cal_crc16(write_multi_reg_frame_ok->buf,SIZE_WRITE_MULTI_REG-2);
	 if((modbus_crc16.mc[0] == write_multi_reg_frame_ok->frame.crc_l) &&
	    (modbus_crc16.mc[1] == write_multi_reg_frame_ok->frame.crc_h))
	 {
		 //crc校验正确
		 modbus_write_multi_reg_frame_response(port_type,write_multi_reg_frame_ok);
	 }
	 else
	 {
		 //crc校验错误
		 write_multi_reg_frame_id = 0;
		 is_write_multi_reg_frame = FALSE;
		 //
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_WRITE_MULTI_REG+0x80;
		 modbus_tx_buf[2] = ERR_DATA_VALUE;//数据内容错误
		 modbus_crc16.ms = modbus_cal_crc16(modbus_tx_buf,3);
		 modbus_tx_buf[3] = modbus_crc16.mc[0];
		 modbus_tx_buf[4] = modbus_crc16.mc[1];
		 modbus_tx_buf[5] = 0xaa;
		 modbus_tx_buf[6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			send_uart1_to_computer(modbus_tx_buf,5/*5+2*/);
		 }
		 else
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,5/*5+2*/);
			 #endif
		 }
	 }
 }
 /////////////////////////////////////////////////////////////////
 void modbus_write_multi_reg_frame_response(U8 port_type,WRITE_MULTI_REG_FRAME *p)
 {
	 UCHAR2 write_addr_s,write_num,crc16;
	 //
	 write_addr_s.mc[0] = p->frame.addr_sl;
	 write_addr_s.mc[1] = p->frame.addr_sh;
	 write_num.mc[0] = p->frame.num_l;
	 write_num.mc[1] = p->frame.num_h;
	 //
	 if((ADDR_START_NVM_PARA <= write_addr_s.ms) &&
	    ((ADDR_START_NVM_PARA+sizeof(WRITE_MULTI_REG_FRAME)) >= (write_addr_s.ms+write_num.ms)))
	 {
		 //NVM_PARA 
		 {			 
			 //运行参数立即适用（注意：数据的有效性由上位机软件来保证，该处不做处理）
			 PARA_COMM_PKT *pPCP = &p->frame.data;
			 NVM_DATA *pND = &pPCP->para;
			 if(FALSE == pPCP->para_advanced_modify_enable)
			 {
				 //高级参数不允许修改	
				 //覆盖高级参数到原来的值。		
				 pND->para.material_start = m_rPara.para.material_start;
				 pND->para.material_stop = m_rPara.para.material_stop;
				 pND->para.current1_modify = m_rPara.para.current1_modify;
				 pND->para.current2_modify = m_rPara.para.current2_modify;
				 pND->para.addr_modbus = m_rPara.para.addr_modbus;
				 pND->para.baud_modbus = m_rPara.para.baud_modbus;
				 pND->para.dw_level_pulse = m_rPara.para.dw_level_pulse;
				 pND->para.mb_level_pulse = m_rPara.para.mb_level_pulse;
				 memcpy(&pND->para.w5500_net_para.ip_para,&m_rPara.para.w5500_net_para.ip_para,sizeof(IP_PARA));
			 }
			 //
			 memcpy(m_rPara.buf,pND->buf,sizeof(NVM_DATA));
		 }
		 //数据写入EEPROM（注意：数据的有效性由上位机软件来保证，该处不做处理）
		 extern void write_para_to_eeprom(NVM_DATA *p,unsigned char rst_used_hours);
		 write_para_to_eeprom(&m_rPara,FALSE); 
		 //
		 #if USED_W5500 == TRUE
		 {
			 //初始化网络芯片，指定延时时间(ms)
			 init_w5500(10);
		 }
		 #endif
		 //
		 // 应答
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_WRITE_MULTI_REG;
		 modbus_tx_buf[2] = write_addr_s.mc[0];
		 modbus_tx_buf[3] = write_addr_s.mc[1];
		 modbus_tx_buf[4] = write_num.mc[0];
		 modbus_tx_buf[5] = write_num.mc[1];
		 crc16.ms = modbus_cal_crc16(modbus_tx_buf,6);
		 modbus_tx_buf[6] = crc16.mc[0];
		 modbus_tx_buf[7] = crc16.mc[1];
		 modbus_tx_buf[8] = 0xaa;
		 modbus_tx_buf[9] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,8+2);
		 }
		 else
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,8);
			 #endif
		 }		 
	 }
	 else
	 {
		 //数据地址不正常
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_WRITE_MULTI_REG+0x80;
		 modbus_tx_buf[2] = ERR_DATA_ADDR;
		 crc16.ms = modbus_cal_crc16(modbus_tx_buf,3);
		 modbus_tx_buf[3] = crc16.mc[0];
		 modbus_tx_buf[4] = crc16.mc[1];
		 modbus_tx_buf[5] = 0xaa;
		 modbus_tx_buf[6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,5+2);
		 }
		 else if(PORT_ENET == port_type)
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,5);
			 #endif
		 }		 
	 }
 }
 /////////////////////////////////////////////////////////////////
 void modbus_read_maintain_reg_frame_handling(U8 port_type)
 {
	 UCHAR2 modbus_crc16;
	 modbus_crc16.ms = modbus_cal_crc16(read_maintain_reg_frame_ok->buf,SIZE_READ_MAINTAIN_REG-2);
	 if((modbus_crc16.mc[0] == read_maintain_reg_frame_ok->frame.crc_l) &&
		(modbus_crc16.mc[1] == read_maintain_reg_frame_ok->frame.crc_h))
	 {
		 //crc校验正确
		 modbus_read_maintain_reg_frame_response(port_type,read_maintain_reg_frame_ok);
	 }
	 else
	 {
		 //crc校验错误
		 //
		 DEBUG_STRING("crc16 error, modbus_crc16.ms=\0");
		 DEBUG_VALUE_TEXT(modbus_crc16.ms);
		 //
		 read_maintain_reg_frame_id = 0;
		 is_read_maintain_reg_frame = FALSE;
		 //
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_READ_MAINTAIN_REG+0x80;
		 modbus_tx_buf[2] = ERR_DATA_VALUE;//数据内容错误
		 modbus_crc16.ms = modbus_cal_crc16(modbus_tx_buf,3);
		 modbus_tx_buf[3] = modbus_crc16.mc[0];
		 modbus_tx_buf[4] = modbus_crc16.mc[1];
		 modbus_tx_buf[5] = 0xaa;
		 modbus_tx_buf[6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,5+2);
		 }
		 else if(PORT_ENET == port_type)
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,5);
			 #endif
		 }
	 }
 }
 extern unsigned char temp_data[80];
 /////////////////////////////////////////////////////////////////
 void modbus_read_maintain_reg_frame_response(U8 port_type,READ_MAINTAIN_REG_FRAME *p)
 {
	 UCHAR2 read_addr_s,read_num,crc16;
	 //
	 read_addr_s.mc[0] = p->frame.addr_sl;
	 read_addr_s.mc[1] = p->frame.addr_sh;
	 read_num.mc[0] = p->frame.num_l;
	 read_num.mc[1] = p->frame.num_h;
	 //
	 if((ADDR_START_INPUT_STATUS <= read_addr_s.ms) && 
	    ((ADDR_START_INPUT_STATUS+20) >= (read_addr_s.ms+read_num.ms)))//20个输入端
	 {
		 //根据具体硬件环境实现（注意具体的输入端口数量）
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_READ_MAINTAIN_REG;
		 modbus_tx_buf[2] = read_num.mc[0];
		 memcpy(&modbus_tx_buf[3],&key_status[read_addr_s.ms-ADDR_START_INPUT_STATUS],read_num.ms);
		 crc16.ms = modbus_cal_crc16(modbus_tx_buf,read_num.ms+3);
		 modbus_tx_buf[read_num.ms+3] = crc16.mc[0];
		 modbus_tx_buf[read_num.ms+4] = crc16.mc[1];
		 modbus_tx_buf[read_num.ms+5] = 0xaa;
		 modbus_tx_buf[read_num.ms+6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,read_num.ms+5+2);
		 }
		 else if(PORT_ENET == port_type)
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,read_num.ms+5);
			 #endif
		 }
	 }
	 else if((ADDR_START_RUNNING_STATUS <= read_addr_s.ms) && 
	         ((ADDR_START_RUNNING_STATUS+sizeof(SYSTEM_STATUS_MODBUS)+4) >= (read_addr_s.ms+read_num.ms)))//运行参数
	 {
		 //根据具体的软件检测要求实现（注意具体的运行参数数量，根据需要可以细化）
		 //
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_READ_MAINTAIN_REG;
		 modbus_tx_buf[2] = read_num.mc[0];
		 //
		 #if 1
		 {
			 //for test
			 //温度测试
			 /*unsigned long tmp = boot_time; //1ms
			 tmp /= 1000; //1s
			 modbus_status.status.m_wendu.zhou1[0] = tmp/(24*3600UL);
			 tmp %= 24*3600UL;
			 modbus_status.status.m_wendu.zhou1[1] = tmp/3600;
			 tmp %= 3600;
			 modbus_status.status.m_wendu.gun[0] = tmp/60;
			 modbus_status.status.m_wendu.zhou2[0] = tmp/60;
			 tmp %= 60;
			 modbus_status.status.m_wendu.gun[1] = tmp;
			 modbus_status.status.m_wendu.zhou2[1] = tmp;
	 */

			 unsigned short temp2;
			 
			 unsigned char temp;

			 temp2 = temp_data[2] *256 + temp_data[3];
			 temp = temp2/16;
			 			 
			 modbus_status.status.m_wendu.zhou1[0] =temp;

			  
			 temp2 = temp_data[4] *256 + temp_data[5];
			 temp = temp2/16;
			 modbus_status.status.m_wendu.zhou1[1] =temp;

			
			 temp2 = temp_data[6] *256 + temp_data[7];
			 temp = temp2/16;
			 modbus_status.status.m_wendu.gun[0] = temp;

			  
			 temp2 = temp_data[8] *256 + temp_data[9];
			 temp = temp2/16;
			 modbus_status.status.m_wendu.zhou2[0] = temp;

			 
			 temp2 = temp_data[14] *256 + temp_data[15];
			 temp = temp2/16;
			 modbus_status.status.m_wendu.gun[1] =temp;


			 
			 temp2 = temp_data[16] *256 + temp_data[17];
			 temp = temp2/16;
			 modbus_status.status.m_wendu.zhou2[1] = temp;
		

		 }
		 #endif
		 //
		 modbus_status.status.m_BootTime = boot_time; //系统引导时间
		 modbus_status.status.m_ResetCause = reset_cause; //复位原因
		 //
		 memcpy(&modbus_tx_buf[3],(char*)&modbus_status.buf[read_addr_s.ms-ADDR_START_RUNNING_STATUS],read_num.ms);
		 crc16.ms = modbus_cal_crc16(modbus_tx_buf,read_num.ms+3);
		 modbus_tx_buf[read_num.ms+3] = crc16.mc[0];
		 modbus_tx_buf[read_num.ms+4] = crc16.mc[1];
		 modbus_tx_buf[read_num.ms+5] = 0xaa;
		 modbus_tx_buf[read_num.ms+6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,read_num.ms+5+2);
		 }
		 else if(PORT_ENET == port_type)
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,read_num.ms+5);
			 #endif
		 }		 		 
	 }
	 else if((ADDR_START_NVM_PARA <= read_addr_s.ms) && 
	         ((ADDR_START_NVM_PARA+sizeof(NVM_DATA)) >= (read_addr_s.ms+read_num.ms)))//nvm_para
	 {
		 //配合上位机读系统参数
		 //
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_READ_MAINTAIN_REG;
		 modbus_tx_buf[2] = read_num.mc[0];
		 memcpy(&modbus_tx_buf[3],(char*)&m_rPara.buf[read_addr_s.ms-ADDR_START_NVM_PARA],read_num.ms);
		 crc16.ms = modbus_cal_crc16(modbus_tx_buf,read_num.ms+3);
		 modbus_tx_buf[read_num.ms+3] = crc16.mc[0];
		 modbus_tx_buf[read_num.ms+4] = crc16.mc[1];
		 modbus_tx_buf[read_num.ms+5] = 0xaa;
		 modbus_tx_buf[read_num.ms+6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,read_num.ms+5+2);
		 }
		 else if(PORT_ENET == port_type)
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,read_num.ms+5);
			 #endif
		 }		 		 
	 }
	 else if((ADDR_START_OUTPUT_STATUS <= read_addr_s.ms) && 
	         ((ADDR_START_OUTPUT_STATUS+6) >= (read_addr_s.ms+read_num.ms)))//6个输出端
	 {
		 //根据具体硬件环境实现（注意具体的输出端口数量）
		 
		 //demo中没有实现功能
		 
	 }
	 else
	 {
		 //数据地址不正常
		 modbus_tx_buf[0] = m_rPara.para.addr_modbus;
		 modbus_tx_buf[1] = CMD_READ_MAINTAIN_REG+0x80;
		 modbus_tx_buf[2] = ERR_DATA_ADDR;
		 crc16.ms = modbus_cal_crc16(modbus_tx_buf,3);
		 modbus_tx_buf[3] = crc16.mc[0];
		 modbus_tx_buf[4] = crc16.mc[1];
		 modbus_tx_buf[5] = 0xaa;
		 modbus_tx_buf[6] = 0xaa;
		 if(PORT_USART == port_type)
		 {
			 send_uart1_to_computer(modbus_tx_buf,5+2);
		 }
		 else if(PORT_ENET == port_type)
		 {
			 #if USED_W5500 == TRUE
			 W5500_socket_send(modbus_tx_buf,5);
			 #endif
		 }
	 }
 }
 /////////////////////////////////////////////////////////////////
 unsigned short modbus_cal_crc16(unsigned char *p,unsigned short len)
 {
	 /*
	 计算CRC码的步骤为：
	 1、预置16位寄存器为十六进制FFFF（即全为1）。称此寄存器为CRC寄存器；
	 2、把第一个8位数据与16位CRC寄存器的低位相异或，把结果放于CRC寄存器；
	 3、把寄存器的内容右移一位(朝低位)，用0填补最高位，检查最低位；
	 4、如果最低位为0：重复第3步(再次移位); 如果最低位为1：CRC寄存器与多项式A001进行异或；
	 5、重复步骤3和4，直到右移8次，这样整个8位数据全部进行了处理；
	 6、重复步骤2到步骤5，进行下一个8位数据的处理；
	 最后得到的CRC寄存器即为CRC码。
	 */
	 UCHAR2 crc16;
	 unsigned short i;
	 unsigned char j;
	 unsigned char tmp;
	 crc16.ms = 0xffff;
	 for(i=0; i<len; i++)
	 {
		 crc16.mc[0] ^= p[i];
		 for(j=0; j<8; j++)
		 {
			 tmp = crc16.mc[0] & 0x01;
			 crc16.ms >>= 1;
			 if(tmp)
			 {
				 crc16.ms ^= 0xa001;
			 }
		 }
	 }
	 //
	 return crc16.ms;
 }
 /////////////////////////////////////////////////////////////////
 void modbus_rtu_rx_frame(unsigned char val)
 {
	 static unsigned char uart_rx_tmp=0;
	 //
	 if(TRUE == is_read_maintain_reg_frame)
	 {
		 //读寄存器
		 read_maintain_reg_frame_tmp->buf[read_maintain_reg_frame_id++] = val;
		 if(read_maintain_reg_frame_id >= SIZE_READ_MAINTAIN_REG)
		 {
			 //帧接收完成
			 read_maintain_reg_frame_id = 0;
			 is_read_maintain_reg_frame = FALSE;
			 KillTimer(MT_CLEAN_MODBUS_RX_FLAG);
			 //
			 READ_MAINTAIN_REG_FRAME *pt = read_maintain_reg_frame_tmp;
			 read_maintain_reg_frame_tmp = read_maintain_reg_frame_ok;
			 read_maintain_reg_frame_ok = pt;
			 //
			 is_read_maintain_reg_frame_ok = TRUE;
		 }
	 }
	 else if(TRUE == is_write_multi_reg_frame)
	 {
		 //写多个寄存器
		 write_multi_reg_frame_tmp->buf[write_multi_reg_frame_id++] = val;
		 if(write_multi_reg_frame_id >= SIZE_WRITE_MULTI_REG)
		 {
			 //帧接收完成
			 write_multi_reg_frame_id = 0;
			 is_write_multi_reg_frame = FALSE;
			 KillTimer(MT_CLEAN_MODBUS_RX_FLAG);
			 //
			 WRITE_MULTI_REG_FRAME *pt = write_multi_reg_frame_tmp;
			 write_multi_reg_frame_tmp = write_multi_reg_frame_ok;
			 write_multi_reg_frame_ok = pt;
			 //
			 is_write_multi_reg_frame_ok = TRUE;
		 }
	 }
	 else //if((FALSE==is_read_maintain_reg_frame) && (FALSE==is_write_multi_reg_frame))
	 {
		 if(m_rPara.para.addr_modbus == uart_rx_tmp)
		 {
			 //地址匹配
			 if(CMD_READ_MAINTAIN_REG == val)
			 {
				 //功能码匹配（暂时只实现了读保持寄存器功能）
				 is_read_maintain_reg_frame = TRUE;
				 uart_rx_tmp = 0;
				 SetTimer(MT_CLEAN_MODBUS_RX_FLAG,50,0);
				 //
				 read_maintain_reg_frame_tmp->buf[0] = m_rPara.para.addr_modbus;//便于计算crc16
				 read_maintain_reg_frame_tmp->buf[1] = CMD_READ_MAINTAIN_REG;
				 read_maintain_reg_frame_id = 2;
			 }
			 else if(CMD_WRITE_MULTI_REG == val)
			 {
				 //写多个寄存器
				 is_write_multi_reg_frame = TRUE;
				 uart_rx_tmp = 0;
				 SetTimer(MT_CLEAN_MODBUS_RX_FLAG,150,0);
				 //
				 write_multi_reg_frame_tmp->buf[0] = m_rPara.para.addr_modbus;//便于计算crc16
				 write_multi_reg_frame_tmp->buf[1] = CMD_WRITE_MULTI_REG;
				 write_multi_reg_frame_id = 2;
			 }
		 }
		 else
		 {
			 uart_rx_tmp = val;
		 }
	 }
 }
 /////////////////////////////////////////////////////////////////
 void modbus_clean_rx_flag(void)
 {
	 read_maintain_reg_frame_id = 0;
	 is_read_maintain_reg_frame = FALSE;
	 //
	 write_multi_reg_frame_id = 0;
	 is_write_multi_reg_frame = FALSE;
 }
 /////////////////////////////////////////////////////////////////
 
 
