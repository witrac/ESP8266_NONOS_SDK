# ESP8266_NONOS_SDK

All documentations @ http://espressif.com/en/support/download/documents?keys=&field_type_tid%5B%5D=14

IMPORTANT: To compile one example, move it to the root folder from the examples folder, access the directory and execute gen_misc.sh or to quick compile
for ESP8266 modules with 8Mbit EEPROM, 40MHz SPI speed, QIO SPI mode, boot v1.2+ and user1.bin output, launch this instead of gen_misc.sh:
make COMPILE=gcc BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=2

