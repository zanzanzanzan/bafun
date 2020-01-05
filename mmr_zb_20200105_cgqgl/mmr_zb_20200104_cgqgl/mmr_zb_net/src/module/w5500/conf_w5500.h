
/*
 *  针对四辊电控主板使用w5200的情况做的一些配置
 *  bear
 *  20160529
*/
#ifndef __CONF_W5500_H__
#define __CONF_W5500_H__

typedef U8 SOCKET;
//
#define	W5500_MAX_SOCK_NUM		8	/* < Maxmium number of socket  */
//
/*! this MAC address is an Atmel Corporation example */
#define W5500_CONF_ETHADDR0			0x00
#define W5500_CONF_ETHADDR1			0x04
#define W5500_CONF_ETHADDR2			0x25
#define W5500_CONF_ETHADDR3			0x41
#define W5500_CONF_ETHADDR4			0x56
#define W5500_CONF_ETHADDR5			0x52
//
//ip address
#define W5500_CONF_IPADDR0          192
#define W5500_CONF_IPADDR1          168
#define W5500_CONF_IPADDR2            0
#define W5500_CONF_IPADDR3          253
//
//net gateway
#define W5500_CONF_GATEWAY_ADDR0    192
#define W5500_CONF_GATEWAY_ADDR1    168
#define W5500_CONF_GATEWAY_ADDR2      0
#define W5500_CONF_GATEWAY_ADDR3      1
//
//submask
#define W5500_CONF_NET_MASK0        255
#define W5500_CONF_NET_MASK1        255
#define W5500_CONF_NET_MASK2        255
#define W5500_CONF_NET_MASK3          0
//
#define WINDOWFULL_MAX_RETRY_NUM	3
#define WINDOWFULL_WAIT_TIME		10000//(x100us)
//

#define Sn_IMR_SENDOK                 0x10
#define Sn_IMR_TIMEOUT                0x08
#define Sn_IMR_RECV                   0x04
#define Sn_IMR_DISCON                 0x02
#define Sn_IMR_CON                    0x01
//
#define IMR_CONFLICT                  0x80
#define IMR_UNREACH                   0x40
#define IMR_PPPoE                     0x20
#define IMR_MP                        0x10
//
#define W5500_RX_BUFFER_FRAME_SIZE		128 //应用数据最大的长度，包括应用头，注意根据需要修改
#define W5500_RX_BUFFER_FRAME_NUM		4
//
#define W5500_VALUE_MAX_SIZE  120//协议中规定最大240字节，为了控制内存用量，接收数据包目前限制120字节
//
#define SPI_TIMEOUT       45000
//
#define ENET_FRAME_FLAG   0x5AA5
//
#define FRAME_COMMAND_RUNNING  210
#define FRAME_COMMAND_PARA     211
//
#define CMD_ACK_SPACK           100
#define CMD_NAK_SPACK           50
//维护类命令
#define CMD_READ_SYSTEM_STATUS            51
#define CMD_LINK_TO_STATION               52
#define CMD_READ_PARA                     53
#define CMD_WRITE_PARA                    54
//
#define NAK_READ_SYSTEM_STATUS            (CMD_READ_SYSTEM_STATUS+CMD_NAK_SPACK)
#define NAK_LINK_TO_STATION               (CMD_LINK_TO_STATION+CMD_NAK_SPACK)
#define NAK_READ_PARA                     (CMD_READ_PARA+CMD_NAK_SPACK)
#define NAK_WRITE_PARA                    (CMD_WRITE_PARA+CMD_NAK_SPACK)
//
#define ACK_READ_SYSTEM_STATUS            (CMD_READ_SYSTEM_STATUS+CMD_ACK_SPACK)
#define ACK_LINK_TO_STATION               (CMD_LINK_TO_STATION+CMD_ACK_SPACK)
#define ACK_READ_PARA                     (CMD_READ_PARA+CMD_ACK_SPACK)
#define ACK_WRITE_PARA                    (CMD_WRITE_PARA+CMD_ACK_SPACK)
////////////////////////////////////////////////////////////////////////
typedef struct
{
	unsigned char *data;
	unsigned int len;
} macb_packet_t;
//
typedef struct __W5500_UDP_HEAD__
{
	U8 sent_ip[4];//4
	U16 sent_port;//2
	U16 len;//2
}W5500_UDP_HEAD;
//
typedef union __UNION_PU8__
{
	//定义该类型的目的是为了消除一些警告，没有别的目的。
	//经过测试，该处的指针变量占四个字节。  bear,20120725
	U32 val;
	U8 *ptr;
}UNION_PU8;
//
////////////////////////////////////////////////////////////////////////



#endif

