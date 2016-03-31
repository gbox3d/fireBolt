#!/usr/bin/env bash
echo upload start

nodemcu-uploader --port /dev/tty.usbserial -B 115200 upload init.lua config.lua BeanconHttpServer.lua index.html favicon.ico tcp_safe_sender.lua

echo upload end
