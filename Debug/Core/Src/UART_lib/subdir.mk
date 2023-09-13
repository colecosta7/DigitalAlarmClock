################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/UART_lib/UART.c 

OBJS += \
./Core/Src/UART_lib/UART.o 

C_DEPS += \
./Core/Src/UART_lib/UART.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/UART_lib/%.o Core/Src/UART_lib/%.su Core/Src/UART_lib/%.cyclo: ../Core/Src/UART_lib/%.c Core/Src/UART_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-UART_lib

clean-Core-2f-Src-2f-UART_lib:
	-$(RM) ./Core/Src/UART_lib/UART.cyclo ./Core/Src/UART_lib/UART.d ./Core/Src/UART_lib/UART.o ./Core/Src/UART_lib/UART.su

.PHONY: clean-Core-2f-Src-2f-UART_lib

