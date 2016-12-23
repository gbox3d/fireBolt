#!/usr/bin/env bash
echo upload start

../../nodemcu-uploader.py --baud 115200  --port /dev/tty.usbserial upload init.lua wifi_setup.lua app.lua gpio.lua u2u.lua

echo upload end

