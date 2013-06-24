################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../message_api-1.1alpha2/Conf/test_my_config_h.c \
../message_api-1.1alpha2/Conf/test_pthread.c \
../message_api-1.1alpha2/Conf/test_rwlock.c \
../message_api-1.1alpha2/Conf/test_spread.c \
../message_api-1.1alpha2/Conf/test_spread_lib.c \
../message_api-1.1alpha2/Conf/test_spread_ver.c 

OBJS += \
./message_api-1.1alpha2/Conf/test_my_config_h.o \
./message_api-1.1alpha2/Conf/test_pthread.o \
./message_api-1.1alpha2/Conf/test_rwlock.o \
./message_api-1.1alpha2/Conf/test_spread.o \
./message_api-1.1alpha2/Conf/test_spread_lib.o \
./message_api-1.1alpha2/Conf/test_spread_ver.o 

C_DEPS += \
./message_api-1.1alpha2/Conf/test_my_config_h.d \
./message_api-1.1alpha2/Conf/test_pthread.d \
./message_api-1.1alpha2/Conf/test_rwlock.d \
./message_api-1.1alpha2/Conf/test_spread.d \
./message_api-1.1alpha2/Conf/test_spread_lib.d \
./message_api-1.1alpha2/Conf/test_spread_ver.d 


# Each subdirectory must supply rules for building sources it contributes
message_api-1.1alpha2/Conf/%.o: ../message_api-1.1alpha2/Conf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/local/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


