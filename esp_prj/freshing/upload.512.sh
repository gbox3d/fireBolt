#!/usr/bin/env bash

#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-dev-8-modules-2016-07-15-13-45-06-integer.bin 0x7c000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-master-10-modules-2016-08-09-02-29-19-integer.bin 
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-master-10-modules-2016-08-09-02-29-19-integer.bin 0x7c000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode dio --flash_size 32m 0x00000 nodemcu-master-10-modules-2016-08-09-02-29-19-integer.bin 0x3fc000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode dio --flash_size 32m 0x00000 nodemcu-master-8-modules-2016-08-10-00-45-42-integer.bin
#./esptool.py --port /dev/tty.usbserial-A9048IIZ -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-master-9-modules-2016-08-10-00-57-30-integer.bin 0x7c000 esp_init_data_default.bin

esptool.py --port /dev/tty.usbserial-A9048IIZ -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-master-18-modules-2016-11-23-11-05-35-integer.bin 0x7c000 esp_init_data_default.bin
