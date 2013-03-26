#!/bin/bash

INSTALL_PATH=$1
LOG_PATH=$2

if [ "$#" -lt 2 ]; then
	echo "install.sh <install path> <log path>"
	exit -1
fi

if [ ! -d "${INSTALL_PATH}" ]; then
	echo "${INSTALL_PATH} dose not exists"
	exit -1
fi

if [ ! -d "${LOG_PATH}" ]; then
	echo "${LOG_PATH} dose not exists"
	exit -1
fi

LINUX_NAME="Linux"

OS_NAME="$(uname -s)"

SERVER_NAME=mx_user_sync

echo "${SERVER_NAME} will install"
echo "install path: ${INSTALL_PATH}"
echo "install log_path: ${LOG_PATH}"
echo "are you sure(y/n)?:"

read IS_SURE

if [ "${IS_SURE}" != "y" ]; then
	exit -3
fi 

INSTALL_PATH=${INSTALL_PATH%/}
LOG_PATH=${LOG_PATH%/}

echo "create dirs..."

INSTALL_PATH=${INSTALL_PATH}/${SERVER_NAME}

REP_INSTALL_PATH=${INSTALL_PATH//\//\\/}
REP_LOG_PATH=${LOG_PATH//\//\\/}

if [ ! -d "${INSTALL_PATH}" ]; then
	echo "sudo mkdir ${INSTALL_PATH}"
	sudo mkdir ${INSTALL_PATH}
fi
	
if [ ! -d "${LOG_PATH}" ]; then
	echo "sudo mkdir -p ${LOG_PATH}"
	sudo mkdir -p ${LOG_PATH}
fi


if [ ! -d "${INSTALL_PATH}/script" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/script"
	sudo mkdir ${INSTALL_PATH}/script
fi

if [ ! -d "${INSTALL_PATH}/conf" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/conf"
	sudo mkdir ${INSTALL_PATH}/conf
fi

if [ ! -d "${INSTALL_PATH}/bin" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/bin"
	sudo mkdir ${INSTALL_PATH}/bin
fi

#if [ ! -d "${INSTALL_PATH}/log" ]; then
#	echo "sudo mkdir ${INSTALL_PATH}/log"
#	sudo mkdir ${INSTALL_PATH}/log
#fi

if [ ! -d "${LOG_PATH}" ]; then
	echo "sudo mkdir ${LOG_PATH}"
	sudo mkdir ${LOG_PATH}
fi

if [ ! -d "${INSTALL_PATH}/html" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/html"
	sudo mkdir ${INSTALL_PATH}/html
fi

echo "copy files..."

sudo cp bin/${SERVER_NAME} ${INSTALL_PATH}/bin/${SERVER_NAME}

if [ ! -f "${INSTALL_PATH}/conf/conf.ini" ]; then
	echo "sudo cp conf/conf.ini ${INSTALL_PATH}/conf"
	sudo cp conf/conf.ini ${INSTALL_PATH}/conf
	
	if [ $(uname -s) == "Linux" ]; then
		echo "sudo sed -i "s/INSTALL_PATH/${REP_INSTALL_PATH}/g" ${INSTALL_PATH}/conf/conf.ini"
		sudo sed -i "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/conf/conf.ini
	else
		echo "sudo sed -i .bak "s/INSTALL_PATH/${REP_INSTALL_PATH}/g" ${INSTALL_PATH}/conf/conf.ini"
		sudo sed -i .bak "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/conf/conf.ini
		echo "sudo rm -f ${INSTALL_PATH}/conf/conf.ini"
		sudo rm -f ${INSTALL_PATH}/conf/conf.ini
	fi
	if [ $(uname -s) == "Linux" ]; then
		echo "sudo sed -i "s/LOG_PATH/${REP_LOG_PATH}/g" ${INSTALL_PATH}/conf/conf.ini"
		sudo sed -i "s/LOG_PATH/$REP_LOG_PATH/g" ${INSTALL_PATH}/conf/conf.ini
	else
		echo "not support os"
	fi
fi

echo "*.sh ${INSTALL_PATH}/script/"
sudo cp script/*.sh ${INSTALL_PATH}/script/

echo "sudo chmod 755 ${INSTALL_PATH}/script/*"
sudo chmod 755 ${INSTALL_PATH}/script/*

if [ $(uname -s) == "Linux" ]; then
	echo "sudo sed -i "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/script/*.sh"
	sudo sed -i "s/SNAME/$SERVER_NAME/g" ${INSTALL_PATH}/script/*.sh
	sudo sed -i "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/script/*.sh
else
	sudo sed -i .bak "s/SNAME/$SERVER_NAME/g" ${INSTALL_PATH}/script/*.sh
	echo "sudo sed -i .bak "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/script/*.sh"
	sudo sed -i .bak "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/script/*.sh
	echo "sudo rm -f ${INSTALL_PATH}/script/*.sh.bak"	
	sudo rm -f ${INSTALL_PATH}/script/*.sh.bak
fi

echo "success to install ${SERVER_NAME} to ${INSTALL_PATH}"
