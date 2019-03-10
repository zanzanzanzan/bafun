CC = iccavr
LIB = ilibw
CFLAGS =  -IE:\WORK\北京康杰斯\16程序\步进电机 -e -D__ICC_VERSION=722 -D_EE_EXTIO -DATMega644  -l -g -MLongJump -MHasMul -MEnhanced -Wf-use_elpm 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -g -e:0x10000 -ucrtatmega.o -bfunc_lit:0x70.0x10000 -dram_end:0x10ff -bdata:0x100.0x10ff -dhwstk_size:30 -beeprom:0.2048 -fihx_coff -S2
FILES = mian.o time.o uart.o delay.o 

STEP:	$(FILES)
	$(CC) -o STEP $(LFLAGS) @STEP.lk   -lcatmega
mian.o: D:\iccv7avr\include\iom16v.h D:\iccv7avr\include\macros.h D:\iccv7avr\include\AVRdef.h E:\WORK\北京康杰斯\16程序\步进电机\delay.h E:\WORK\北京康杰斯\16程序\步进电机\time.h E:\WORK\北京康杰斯\16程序\步进电机\uart.h
mian.o:	mian.c
	$(CC) -c $(CFLAGS) mian.c
time.o: D:\iccv7avr\include\iom16v.h D:\iccv7avr\include\macros.h D:\iccv7avr\include\AVRdef.h
time.o:	time.c
	$(CC) -c $(CFLAGS) time.c
uart.o: D:\iccv7avr\include\iom16v.h D:\iccv7avr\include\macros.h D:\iccv7avr\include\AVRdef.h
uart.o:	uart.c
	$(CC) -c $(CFLAGS) uart.c
delay.o: E:\WORK\北京康杰斯\16程序\步进电机\delay.h
delay.o:	delay.c
	$(CC) -c $(CFLAGS) delay.c
