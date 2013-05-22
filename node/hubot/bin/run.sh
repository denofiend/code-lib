#########################################################################
# File Name: bin/run.sh
# Author: DenoFiend
# mail: denofiend@gmailcom
# Created Time: 2013年04月27日 星期六 16时38分25秒
#########################################################################

#!/bin/bash 
## 
## Wrapper for Hubot startup 
## 

HUBOT="/data/denofiend/workspace/git/code-lib/node/hubot/bin/hubot"
NAME="mxhubot"
ADAPTER="campfire" 
HUBOT_CAMPFIRE_TOKEN="28a7f2635415e6c8505b94e084802e49b60b9afc" 
HUBOT_CAMPFIRE_ACCOUNT="maxthon3" 
HUBOT_CAMPFIRE_ROOMS="560978" 
REDISTOGO_URL="10.100.15.7"

OPTS="--name ${NAME} --adapter ${ADAPTER}" 
export HUBOT_CAMPFIRE_TOKEN
export HUBOT_CAMPFIRE_ACCOUNT
export HUBOT_CAMPFIRE_ROOMS
export REDISTOGO_URL

until ${HUBOT} ${OPTS}; 
do echo "Hubot crashed with exit code $?. Restarting." >&2 
	sleep 5 
done
