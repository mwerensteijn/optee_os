#!/bin/bash

pid="$(pidof -s $1)"
if test -z "$pid"
then
	echo "Process $1 is not running"
	exit -1
else
	echo "Going to checkpoint $1 with PID $pid"
fi
criu dump -t `pidof $1` -D checkpoint --shell-job
./optee_app_migrator $pid $1
criu restore -D checkpoint --shell-job