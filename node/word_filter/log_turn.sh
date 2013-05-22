#!/bin/sh


FILENAME_DATE=$(date -d"-0 day" "+%Y%m%d")
DELETEDLOGDAY=$(date -d"-10 day" "+%Y%m%d")
LOG_ROOT_PATH=/data/logs/mx_word_filter

echo ${FILENAME_DATE}
echo ${DELETEDLOGDAY}
echo ${LOG_ROOT_PATH}

cd ${LOG_ROOT_PATH}

filelist=`ls *.log 2>> /dev/null`

for logfile in ${filelist}
do
                echo "cp ${logfile} ${logfile%%.log}.${FILENAME_DATE}.txt"
                cp ${logfile} "${logfile%%.log}.${FILENAME_DATE}.txt"
                :>${logfile}

                DELETE_FILE="${logfile%%.log}.${DELETEDLOGDAY}.txt"
                echo "delete:${DELETE_FILE}"
                if [ -f "${DELETE_FILE}" ]; then
                        echo "rm -rf ${DELETE_FILE}"
                        rm -rf ${DELETE_FILE}
                fi
done
