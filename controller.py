#!/usr/bin/env python
"""Send a test command to the Arduino synth."""

import random
from random import randint
import time
import serial
import json


def main():
    random.seed(42)
    device = serial.Serial("/dev/tty.usbserial-A20e1evS", 115200)
    time.sleep(2)

    while True:
        command = {"command": "play", "options": [randint(0, 300),
                randint(0, 300), randint(0, 300), randint(0, 100),
                randint(0, 100)]}
        print command
        device.write(json.dumps(command) + "\r")
        time.sleep(.01)

    command = {"command": "stop"}
    device.write(json.dumps(command) + "\r")
    time.sleep(2)


if __name__ == '__main__':
    main()
