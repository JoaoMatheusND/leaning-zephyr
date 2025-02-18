import serial
import time

if __name__ == '__main__':

    with serial.Serial('/dev/ttyUSB0', 115200) as ser:
        while True:
            ser.write(b'Hello, world!\n')
            time.sleep(1)
            print(ser.readline())
