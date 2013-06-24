################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../message_api-1.1alpha2/options.c \
../message_api-1.1alpha2/udf_spread.c 

OBJS += \
./message_api-1.1alpha2/options.o \
./message_api-1.1alpha2/udf_spread.o 

C_DEPS += \
./message_api-1.1alpha2/options.d \
./message_api-1.1alpha2/udf_spread.d 


# Each subdirectory must supply rules for building sources it contributes
message_api-1.1alpha2/%.o: ../message_api-1.1alpha2/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/local/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


