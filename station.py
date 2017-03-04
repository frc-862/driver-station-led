from networktables import NetworkTables
import serial.tools.list_ports
from serial import Serial, SerialException
import networktables
import sys
import logging
from time import sleep

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

    sd = NetworkTables.getTable("SmartDashboard")
    sd.addTableListener(valueChanged)

def valueChanged(table, key, value, isNew):
    arduino.write("valueChanged: key: '%s'; value: %s; isNew: %s\n" % (key, value, isNew))
    print("valueChanged: key: '%s'; value: %s; isNew: %s" % (key, value, isNew))

def connectionListener(connected, info):
    print(info, '; Connected=%s' % connected)


logging.basicConfig(level=logging.DEBUG)

arduino = setup_serial()
setup_network_tables()

if arduino is None:
    print("Error connecting to serial port")
    sys.exit(1)  # Exit with error

while True:  # Keep running so NetworkTables' event loop can run
    sleep(1)

# Cleanup
arduino.close()