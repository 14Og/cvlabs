#!/bin/bash

set -e

if [ "$#" -ne 4 ]; then
	echo "cut_video.sh [input_file] [output_file] [start_time] [end_time]"
	echo "start_time and end_time: hh:mm:ss.ms"
	exit 1  
fi

INPUT_FILE=$1
OUT_FILE=$2
START_TIME=$3
END_TIME=$4
COMMAND="ffmpeg -ss $START_TIME -to $END_TIME -i $INPUT_FILE -c copy $OUT_FILE"

echo $COMMAND
eval $COMMAND


