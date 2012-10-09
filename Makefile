BOARD_TAG = diecimila
TARGET = labnotifier

ARDUINO_LIBS = auduino cJSON
SKIP_SUFFIX_CHECK = 1

SERIAL_BAUDRATE = 115200

OSTYPE := $(shell uname)

ifndef ARDUINO_PORT
	ifeq ($(OSTYPE),Darwin)
		ARDUINO_PORT = /dev/tty.usbserial*
	else
		ARDUINO_PORT = /dev/ttyUSB*
	endif
endif

USER_LIB_PATH = ./libs
ARDUINO_MAKEFILE_HOME = ./arduino.mk

include $(ARDUINO_MAKEFILE_HOME)/Arduino.mk

flash: upload
