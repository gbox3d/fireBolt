#!/usr/bin/env bash
echo upload start

../../nodemcu-uploader.py --baud 9600  --port /dev/tty.usbserial upload init.lua wifi_setup.lua esutil.lua udpstub.lua

echo upload end

