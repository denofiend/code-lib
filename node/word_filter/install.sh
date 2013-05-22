#!/bin/bash

if [ "$#" -lt 1 ]; then
	echo "install.sh  <log path>"
	exit -1
fi

LOG_PATH=$1
LOG_PATH=${LOG_PATH%/}

if [ ! -d "${LOG_PATH}" ]; then
	echo "create directory $LOG_PATH"
	mkdir -p ${LOG_PATH}
fi


REP_LOG_PATH=${LOG_PATH//\//\\/}

echo "replace LOG_PATH"
sed -i "s/LOG_PATH/$REP_LOG_PATH/g" ./config.js
sed -i "s/LOG_PATH/$REP_LOG_PATH/g" ./log_turn.sh

npm install .
 

