#
# make flash
# sudo screen /dev/tty.usbmodem1d121 9600
# (to quit from screen enter: ^a + ^\)
#
BOARD_TAG = uno
ARDUINO_LIBS = SPI Wire RTClib Time Timezone Adafruit-GFX-Library Adafruit-PCD8544-Nokia-5110-LCD-library
BOARD := uno
TARGET := aclock
USER_LIB_PATH = .
include Arduino-Makefile/Arduino.mk

flash: raw_upload
