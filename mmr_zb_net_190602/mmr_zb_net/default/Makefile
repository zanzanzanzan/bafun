################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../src/ \
../src/global \
../src/module \
../src/module/w5500/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../src/mmr.c \
../src/module/core_func.c \
../src/module/crc32.c \
../src/module/crc8.c \
../src/module/display.c \
../src/module/modbus_rtu.c \
../src/module/scan_key.c \
../src/module/soft_timer.c \
../src/module/uart_to_pc.c \
../src/module/w5500/w5500.c \
../src/module/w5500/w5500_manager.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
src/mmr.o \
src/module/core_func.o \
src/module/crc32.o \
src/module/crc8.o \
src/module/display.o \
src/module/modbus_rtu.o \
src/module/scan_key.o \
src/module/soft_timer.o \
src/module/uart_to_pc.o \
src/module/w5500/w5500.o \
src/module/w5500/w5500_manager.o

OBJS_AS_ARGS +=  \
src/mmr.o \
src/module/core_func.o \
src/module/crc32.o \
src/module/crc8.o \
src/module/display.o \
src/module/modbus_rtu.o \
src/module/scan_key.o \
src/module/soft_timer.o \
src/module/uart_to_pc.o \
src/module/w5500/w5500.o \
src/module/w5500/w5500_manager.o

C_DEPS +=  \
src/mmr.d \
src/module/core_func.d \
src/module/crc32.d \
src/module/crc8.d \
src/module/display.d \
src/module/modbus_rtu.d \
src/module/scan_key.d \
src/module/soft_timer.d \
src/module/uart_to_pc.d \
src/module/w5500/w5500.d \
src/module/w5500/w5500_manager.d

C_DEPS_AS_ARGS +=  \
src/mmr.d \
src/module/core_func.d \
src/module/crc32.d \
src/module/crc8.d \
src/module/display.d \
src/module/modbus_rtu.d \
src/module/scan_key.d \
src/module/soft_timer.d \
src/module/uart_to_pc.d \
src/module/w5500/w5500.d \
src/module/w5500/w5500_manager.d

OUTPUT_FILE_PATH +=mmr_zb_net.elf

OUTPUT_FILE_PATH_AS_ARGS +=mmr_zb_net.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler























src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DF_CPU=4000000UL  -I"../src" -I"../src/global" -I"../src/module" -I"../src/module/w5500"  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -mmcu=atmega644pa -c -gdwarf-2 -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/module/%.o: ../src/module/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DF_CPU=4000000UL  -I"../src" -I"../src/global" -I"../src/module" -I"../src/module/w5500"  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -mmcu=atmega644pa -c -gdwarf-2 -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

src/module/w5500/%.o: ../src/module/w5500/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DF_CPU=4000000UL  -I"../src" -I"../src/global" -I"../src/module" -I"../src/module/w5500"  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -mmcu=atmega644pa -c -gdwarf-2 -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="mmr_zb_net.map" -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections -Wl,-section-start=.apflag=0xeff8  -Wl,-section-start=.eeprom=0x810100  -mmcu=atmega644pa  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "mmr_zb_net.elf" "mmr_zb_net.hex"  -R .eeprom
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom   -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --change-section-lma .eeprom=0x100 --no-change-warnings -O ihex "mmr_zb_net.elf" "mmr_zb_net.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "mmr_zb_net.elf" > "mmr_zb_net.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "mmr_zb_net.elf" "mmr_zb_net.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "mmr_zb_net.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "mmr_zb_net.elf" "mmr_zb_net.a" "mmr_zb_net.hex" "mmr_zb_net.lss" "mmr_zb_net.eep" "mmr_zb_net.map" "mmr_zb_net.srec" "mmr_zb_net.usersignatures"
	