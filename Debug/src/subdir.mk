################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lc75710.c \
../src/lc75710_graphics.c \
../src/ma_audio.c \
../src/ma_gui.c \
../src/ma_util.c \
../src/manage_audio.c \
../src/string_table.c \
../src/system.c \
../src/time.c \
../src/uart.c 

S_UPPER_SRCS += \
../src/ffft.S 

OBJS += \
./src/ffft.o \
./src/lc75710.o \
./src/lc75710_graphics.o \
./src/ma_audio.o \
./src/ma_gui.o \
./src/ma_util.o \
./src/manage_audio.o \
./src/string_table.o \
./src/system.o \
./src/time.o \
./src/uart.o 

C_DEPS += \
./src/lc75710.d \
./src/lc75710_graphics.d \
./src/ma_audio.d \
./src/ma_gui.d \
./src/ma_util.d \
./src/manage_audio.d \
./src/string_table.d \
./src/system.d \
./src/time.d \
./src/uart.d 

S_UPPER_DEPS += \
./src/ffft.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -g2 -gstabs -mmcu=atmega8 -DF_CPU=12000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -ffunction-sections -fdata-sections -std=gnu99 -mmcu=atmega8 -DF_CPU=12000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


