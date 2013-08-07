################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mul_task/TaskQueue.cpp \
../src/mul_task/TaskThread.cpp 

OBJS += \
./src/mul_task/TaskQueue.o \
./src/mul_task/TaskThread.o 

CPP_DEPS += \
./src/mul_task/TaskQueue.d \
./src/mul_task/TaskThread.d 


# Each subdirectory must supply rules for building sources it contributes
src/mul_task/%.o: ../src/mul_task/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/local/include/mxlib -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


