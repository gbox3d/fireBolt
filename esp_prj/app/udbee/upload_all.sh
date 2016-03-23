#!/usr/bin/env bash
echo upload start

nodemcu-uploader --port /dev/tty.usbserial --baud 9600 upload init.lua config.lua BeanconHttpServer.lua index.html favicon.ico tcp_safe_sender.lua

echo upload end
