################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../message_api-1.1alpha2/Test/options.c \
../message_api-1.1alpha2/Test/test_options.c 

OBJS += \
./message_api-1.1alpha2/Test/options.o \
./message_api-1.1alpha2/Test/test_options.o 

C_DEPS += \
./message_api-1.1alpha2/Test/options.d \
./message_api-1.1alpha2/Test/test_options.d 


# Each subdirectory must supply rules for building sources it contributes
message_api-1.1alpha2/Test/%.o: ../message_api-1.1alpha2/Test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/local/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


