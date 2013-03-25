#!/bin/sh

SHORT_NAME=SNAME
echo SNAME
#NAME=INSTALL_PATH/bin/SNAME
NAME=./bin/mx_sync_serv

check_running()
{
	prog_name=$1
	
	counter=$(ps -ef|grep $prog_name |grep -v grep | wc -l)
	
	if [ "$counter" -eq "0" ]; then
		return 0
	else
		return 1
	fi
}

start()
{
	check_running ${NAME}
	
	if [ "$?" -eq "1" ]; then
		echo "${NAME} is running"
		return 0
	fi
	
	echo "starting ${NAME}... "
	
	if (${NAME}); then
		echo "[OK]"
	else
		echo "[FAIL]"
	fi
}

status()
{
	check_running ${NAME}
	
	if [ "$?" -eq "0" ]; then
		echo "${NAME} is not running"
	else
		pid=$(ps -ef | grep ${NAME} | grep -v grep | awk '{print $2}')
		echo mxhttpserv pid: $pid
	fi
}

stop()
{
	check_running ${NAME}

	if [ "$?" -eq "1" ]; then
		killall -9 ${SHORT_NAME}
	else
		echo "${NAME} is not running"
	fi
}


case "$1" in
        start)
                start
                ;;
        stop)
                stop
                ;;
        restart)
                stop
                start
                ;;
        status)
                status
                ;;
        *)
                echo $"Usage: $0 {start|stop|restart|status}"
esac
