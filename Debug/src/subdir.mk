################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/keypad.c \
../src/lc75710_graphics.c \
../src/ma_audio.c \
../src/ma_gui.c \
../src/ma_strings.c \
../src/ma_util.c \
../src/manage_audio.c \
../src/printf.c \
../src/system.c \
../src/time.c \
../src/uart.c 

S_UPPER_SRCS += \
../src/ffft.S 

C_DEPS += \
./src/keypad.d \
./src/lc75710_graphics.d \
./src/ma_audio.d \
./src/ma_gui.d \
./src/ma_strings.d \
./src/ma_util.d \
./src/manage_audio.d \
./src/printf.d \
./src/system.d \
./src/time.d \
./src/uart.d 

OBJS += \
./src/ffft.o \
./src/keypad.o \
./src/lc75710_graphics.o \
./src/ma_audio.o \
./src/ma_gui.o \
./src/ma_strings.o \
./src/ma_util.o \
./src/manage_audio.o \
./src/printf.o \
./src/system.o \
./src/time.o \
./src/uart.o 

S_UPPER_DEPS += \
./src/ffft.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -g2 -gstabs -mmcu=atmega8 -DF_CPU=12000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -mmcu=atmega8 -DF_CPU=12000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


