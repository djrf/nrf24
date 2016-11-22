#!/bin/python2

import spi as spidev
import RPi.GPIO as gpio


channel = 40

CE = 15
gpio.setmode(gpio.BOARD)
gpio.setup(CE, gpio.OUT, initial=gpio.LOW)

spi = spidev.SPI("/dev/spidev0.0")
spi.mode = spidev.SPI.MODE_0
spi.bits_per_word = 8
spi.speed = 80000


def write_register(address, value):
    spi.transfer([0x20 | (0x1f & address), value])
    print(read_register(address))


def read_register(address):
    result = spi.transfer([0x1f & address, 0x00])
    return result[1]


def read_status():
    status = spi.transfer([0xff])
    return status[0]


def send_packet(packet):
    spi.write([0xa0] + packet)
    print(hex(read_register(0x17)))

    gpio.output(CE, gpio.HIGH)
    while (read_register(0x17) & 0x10) is 0:       # tx fifo empty?
        if (read_status() & 0x10) is not 0:        # max rt?
            print("Flush TX FIFO")
            spi.transfer([0xe1])                   # flush fifo
            write_register(0x07, 0x10)
    gpio.output(CE, gpio.LOW)


print("Read register 0")
print(hex(read_register(0)))

# Power on
print("Power on")
write_register(0x00, 0x0a)
print(hex(read_register(0)))

# Set channel
print("Set channel")
write_register(0x05, channel)


print("Status:")
print(hex(read_status()))

send_packet([0xde, 0xad, 0xbe, 0xef])
