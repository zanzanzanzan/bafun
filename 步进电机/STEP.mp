NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   0070   0008 =      8. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0070  __idata_start
       0078  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   0078   05F2 =   1522. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       003C  __text_start
       003C  __start
       005E  _exit
       005F  _port_init
       006A  _init_devices
       0077  _main
       00BC  _display
       00EC  _start_bit
       0101  _stop_bit
       0113  _tx_byte
       0135  _send_bit
       014B  _rx_byte
       015F  _receive_bit
       0172  _memread
       01A6  _timer0_init
       01AF  _timer0_ovf_isr
       01C3  _uart0_init
       01CE  _USART_send
       01D2  _uart0_rx_isr
       02C7  _delay
       02D8  _delayms
       02E9  mod16u
       02EB  div16u
       02EC  xdiv16u
       0305  empy16s
       0315  push_xgset303C
       031C  pop_xgset303C
       0323  push_xgsetF03C
       032C  pop_xgsetF03C
       0335  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0100   0008 =      8. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0100  _P_bao
       0100  __data_start
       0102  _Bao_sum
       0104  _Bao_time
       0106  _count
       0108  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   0108   0005 =      5. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0108  _Pecreg
       0108  __bss_start
       0109  _DataL
       010A  _DataH
       010B  _bit_in
       010C  _bit_out
       010D  __bss_end

Area                               Start  End    Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0030 =     48. bytes (abs,ovr,rom)

Files Linked      [ module(s) ]

D:\iccv7avr\lib\crtatmega.o	[ crtatmega.s ]
mian.o	[ mian.c ]
time.o	[ time.c ]
uart.o	[ uart.c ]
delay.o	[ delay.c ]
<library>	[ div16u.s, emul16s.s, gpusha.s, gpushb.s ]

User Global Definitions

ram_end = 0x10ff
hwstk_size = 0x1e

User Base Address Definitions

func_lit = 0x70
data = 0x100
eeprom:0.2048

