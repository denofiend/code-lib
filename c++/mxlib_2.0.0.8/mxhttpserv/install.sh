#!/bin/bash

INSTALL_PATH=$1
SERVER_NAME=$2

if [ "$#" -lt 2 ]; then
	echo "install.sh <install path> <server name>"
	exit -1
fi

if [ ! -d "${INSTALL_PATH}" ]; then
	echo "${INSTALL_PATH} dose not exists"
	exit -1
fi

LINUX_NAME="Linux"

OS_NAME="$(uname -s)"

SERVER_NAME=${SERVER_NAME%/}

if [ -z "${SERVER_NAME}" ]; then
	echo "server name must be not empty"
	exit -1
fi

echo "${SERVER_NAME} will install"
echo "install path: ${INSTALL_PATH}"
echo "are you sure(y/n)?:"

read IS_SURE

if [ "${IS_SURE}" != "y" ]; then
	exit -3
fi 

INSTALL_PATH=${INSTALL_PATH%/}

echo "create dirs..."

INSTALL_PATH=${INSTALL_PATH}/${SERVER_NAME}

REP_INSTALL_PATH=${INSTALL_PATH//\//\\/}

if [ ! -d "${INSTALL_PATH}" ]; then
	echo "sudo mkdir ${INSTALL_PATH}"
	sudo mkdir ${INSTALL_PATH}
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

if [ ! -d "${INSTALL_PATH}/log" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/log"
	sudo mkdir ${INSTALL_PATH}/log
fi

if [ ! -d "${INSTALL_PATH}/html" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/html"
	sudo mkdir ${INSTALL_PATH}/html
fi

if [ ! -d "${INSTALL_PATH}/cache" ]; then
	echo "sudo mkdir ${INSTALL_PATH}/cache"
	sudo mkdir ${INSTALL_PATH}/cache
fi

echo "copy files..."

sudo cp bin/mxhttpserv ${INSTALL_PATH}/bin/${SERVER_NAME}

if [ ! -f "${INSTALL_PATH}/conf/mxhttpserv.xml" ]; then
	echo "sudo cp conf/mxhttpserv.xml ${INSTALL_PATH}/conf"
	sudo cp conf/mxhttpserv.xml ${INSTALL_PATH}/conf
	
	if [ $(uname -s) == "Linux" ]; then
		echo "sudo sed -i "s/INSTALL_PATH/${REP_INSTALL_PATH}/g" ${INSTALL_PATH}/conf/mxhttpserv.xml"
		sudo sed -i "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/conf/mxhttpserv.xml
	else
		echo "sudo sed -i .bak "s/INSTALL_PATH/${REP_INSTALL_PATH}/g" ${INSTALL_PATH}/conf/mxhttpserv.xml"
		sudo sed -i .bak "s/INSTALL_PATH/$REP_INSTALL_PATH/g" ${INSTALL_PATH}/conf/mxhttpserv.xml
		echo "sudo rm -f ${INSTALL_PATH}/conf/mxhttpserv.xml.bak"
		sudo rm -f ${INSTALL_PATH}/conf/mxhttpserv.xml.bak
	fi
fi

echo "sudo cp script/mxhttpserv.sh ${INSTALL_PATH}/script/${SERVER_NAME}.sh"
sudo cp script/mxhttpserv.sh ${INSTALL_PATH}/script/${SERVER_NAME}.sh

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
