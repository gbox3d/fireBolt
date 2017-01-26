#!/usr/bin/env bash

#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 ../../nodemcu-firmware/bin/0x00000.bin 0x10000 ../../nodemcu-firmware/bin/0x10000.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-dev096-10-modules-2015-08-18-03-08-51-integer.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-dev-14-modules-2015-11-12-06-05-49-integer.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-dev-10-modules-2015-11-12-07-11-04-integer.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-dev-8-modules-2016-03-01-13-38-21-integer.bin

#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-dev-14-modules-2016-03-30-10-37-19-integer.bin

#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-dev-8-modules-2016-07-15-13-45-06-integer.bin 0x7c000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-master-10-modules-2016-08-09-02-29-19-integer.bin 
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash 0x00000 nodemcu-master-10-modules-2016-08-09-02-29-19-integer.bin 0x7c000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode dio --flash_size 32m 0x00000 nodemcu-master-9-modules-2016-08-10-00-57-30-integer.bin 0x3fc000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode qio --flash_size 4m 0x00000 nodemcu-master-9-modules-2016-08-10-00-57-30-integer.bin 0x7c000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 write_flash --flash_mode dio --flash_size 32m 0x00000 nodemcu-master-17-modules-2016-08-11-08-24-59-integer.bin 0x3fc000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 115200 write_flash --flash_mode dio --flash_size 32m 0x00000 nodemcu-master-15-modules-2016-08-11-12-00-38-integer.bin 0x3fc000 esp_init_data_default.bin
#./esptool.py --port /dev/tty.usbserial -b 230400 read_flash 0x00000
#./esptool.py --port /dev/tty.usbserial -b 230400 erase_flash --flash_mode dio --flash_size 32m 
#./esptool.py --port /dev/tty.wchusbserial14170 -b 115200 write_flash --flash_mode dio --flash_size 32m 0x00000 nodemcu-master-17-modules-2016-08-11-08-24-59-integer.bin 0x3fc000 esp_init_data_default.bin


esptool.py --port $1 -b 230400 write_flash --flash_mode dio --flash_size 32m 0x00000 $2 0x3fc000 esp_init_data_default.bin