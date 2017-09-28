#!/usr/bin/env bash
#esptool.py --port /dev/tty.wchusbserial14270 -b 115200 write_flash --flash_mode dio --flash_size 32m 0x00000 ../nodemcu-master-18-modules-2017-01-05-07-21-29-integer.bin 0x3fc000 ../esp_init_data_default.bin
esptool.py --port $1 -b 115200 write_flash --flash_mode dio --flash_size 32m 0x00000 $2 