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
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../soft_timer.c \
../sgdkcgq_485_cry4m16m_pulse_ad.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
soft_timer.o \
sgdkcgq_485_cry4m16m_pulse_ad.o

OBJS_AS_ARGS +=  \
soft_timer.o \
sgdkcgq_485_cry4m16m_pulse_ad.o

C_DEPS +=  \
soft_timer.d \
sgdkcgq_485_cry4m16m_pulse_ad.d

C_DEPS_AS_ARGS +=  \
soft_timer.d \
sgdkcgq_485_cry4m16m_pulse_ad.d

OUTPUT_FILE_PATH +=sgdkcgq_485_cry4m16m_pulse_ad.elf

OUTPUT_FILE_PATH_AS_ARGS +=sgdkcgq_485_cry4m16m_pulse_ad.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler





./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -mmcu=atmega8a -c -gdwarf-2 -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
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
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="sgdkcgq_485_cry4m16m_pulse_ad.map" -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections -Wl,-section-start=.apflag=0x17f8  -Wl,-section-start=.eeprom=0x810080  -mmcu=atmega8a  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "sgdkcgq_485_cry4m16m_pulse_ad.elf" "sgdkcgq_485_cry4m16m_pulse_ad.hex"  -R .eeprom
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom   -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --change-section-lma .eeprom=0x80 --no-change-warnings -O ihex "sgdkcgq_485_cry4m16m_pulse_ad.elf" "sgdkcgq_485_cry4m16m_pulse_ad.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "sgdkcgq_485_cry4m16m_pulse_ad.elf" > "sgdkcgq_485_cry4m16m_pulse_ad.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "sgdkcgq_485_cry4m16m_pulse_ad.elf" "sgdkcgq_485_cry4m16m_pulse_ad.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "sgdkcgq_485_cry4m16m_pulse_ad.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "sgdkcgq_485_cry4m16m_pulse_ad.elf" "sgdkcgq_485_cry4m16m_pulse_ad.a" "sgdkcgq_485_cry4m16m_pulse_ad.hex" "sgdkcgq_485_cry4m16m_pulse_ad.lss" "sgdkcgq_485_cry4m16m_pulse_ad.eep" "sgdkcgq_485_cry4m16m_pulse_ad.map" "sgdkcgq_485_cry4m16m_pulse_ad.srec" "sgdkcgq_485_cry4m16m_pulse_ad.usersignatures"
	