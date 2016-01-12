#!/usr/bin/env bash
echo upload start

../../../nodemcu-uploader.py --baud 9600  --port /dev/tty.usbserial upload init.lua 1.html

echo upload end

