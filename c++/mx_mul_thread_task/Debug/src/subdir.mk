################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HttpsClient.cpp \
../src/SyncTask.cpp \
../src/TaskBean.cpp \
../src/TaskQueue.cpp \
../src/TaskThread.cpp \
../src/UpdateDbTask.cpp \
../src/main.cpp 

OBJS += \
./src/HttpsClient.o \
./src/SyncTask.o \
./src/TaskBean.o \
./src/TaskQueue.o \
./src/TaskThread.o \
./src/UpdateDbTask.o \
./src/main.o 

CPP_DEPS += \
./src/HttpsClient.d \
./src/SyncTask.d \
./src/TaskBean.d \
./src/TaskQueue.d \
./src/TaskThread.d \
./src/UpdateDbTask.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/local/include/mxlib -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


