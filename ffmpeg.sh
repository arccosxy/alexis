#!/bin/bash

echo "hello!!!!!!"
echo "$0"
echo "$1"
echo "$2"

#ffmpeg -f x11grab -s $(xwininfo -id $1| awk '/geometry/ {print $2}' | awk 'BEGIN {FS="-"} {print $1}') -r 25 -i :0.0+$(xwininfo -id $1 | awk '/Absolute upper-left X/ {print $4}'),$(xwininfo -id $1 | awk '/Absolute upper-left Y/ {print $4}') -vcodec libx264 -tune zerolatency -f rtp rtp://127.0.0.1:5060 > /home/xuyang/live.sdp

ffmpeg -f x11grab -s $(xwininfo -id $1| awk '/geometry/ {print $2}' | awk -F '[+,-]' '{print $1}') -r 25 -i :0.0+$(xwininfo -id $1 | awk '/Absolute upper-left X/ {print $4}'),$(xwininfo -id $1 | awk '/Absolute upper-left Y/ {print $4}') -vcodec libx264 -tune zerolatency -f rtp rtp://127.0.0.1:5060

exit 0
