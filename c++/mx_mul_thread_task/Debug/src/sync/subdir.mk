################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sync/HttpsClient.cpp \
../src/sync/SyncTask.cpp \
../src/sync/TaskBean.cpp \
../src/sync/UpdateDbTask.cpp \
../src/sync/main.cpp 

OBJS += \
./src/sync/HttpsClient.o \
./src/sync/SyncTask.o \
./src/sync/TaskBean.o \
./src/sync/UpdateDbTask.o \
./src/sync/main.o 

CPP_DEPS += \
./src/sync/HttpsClient.d \
./src/sync/SyncTask.d \
./src/sync/TaskBean.d \
./src/sync/UpdateDbTask.d \
./src/sync/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/sync/%.o: ../src/sync/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/local/include/mxlib -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


