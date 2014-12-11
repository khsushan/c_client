################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../token/Token.c \
../token/writetoken.c 

OBJS += \
./token/Token.o \
./token/writetoken.o 

C_DEPS += \
./token/Token.d \
./token/writetoken.d 


# Each subdirectory must supply rules for building sources it contributes
token/%.o: ../token/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


