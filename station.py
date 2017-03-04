from networktables import NetworkTables
import serial.tools.list_ports
from serial import Serial, SerialException
import networktables
import sys
import logging
from time import sleep

OPERATION_SWITCH_MODE   = 0
OPERATION_SET_COLOR     = 1

MODE_SOLID      = 0
MODE_RAINBOW    = 1

COLOR_ORANGE    = 0
COLOR_BLUE      = 1
COLOR_GREEN     = 2
COLOR_RED       = 3

def setup_serial():
    try:
        portList = [port[0] for port in serial.tools.list_ports.comports() if port[2] != 'n/a']
        print("Connecting to serial port at", portList[0])
        arduino = Serial(portList[0], 9600)
    except SerialException:
        return None

    return arduino


def setup_network_tables():
    if len(sys.argv) != 2:
        print("Error: Please specify the IP with a command-line argument")
        sys.exit(1)  # Exit with error

    ip = sys.argv[1]
    NetworkTables.initialize(server=ip)

    NetworkTables.addConnectionListener(connectionListener, immediateNotify=True)

    sd = NetworkTables.getTable("DashControl")
    sd.addTableListener(valueChanged)

def valueChanged(table, key, value, isNew):
    if key == 'station_led_color':
        arduino.write(bytearray([OPERATION_SET_COLOR, value]))
    if key == 'station_led_mode':
        arduino.write(bytearray([OPERATION_SWITCH_MODE, value]))

    print("valueChanged: key: '%s'; value: %s; isNew: %s" % (key, value, isNew))

def connectionListener(connected, info):
    print(info, '; Connected=%s' % connected)


logging.basicConfig(level=logging.DEBUG)

arduino = setup_serial()
setup_network_tables()

if arduino is None:
    print("Error connecting to serial port")
    sys.exit(1)  # Exit with error

# Sleep to give the arduino some time
sleep(2)

# Write initial mode and color
arduino.write(bytearray([OPERATION_SWITCH_MODE, MODE_SOLID]))
arduino.write(bytearray([OPERATION_SET_COLOR, COLOR_BLUE]))

while True:  # Keep running so NetworkTables' event loop can run
    sleep(1)
    print(arduino.readline())

# Cleanup
arduino.close()
