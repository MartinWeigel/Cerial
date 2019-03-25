# Cerial

Cerial is a C-module to communicate over a serial connection.
It supports binary transmission of arbitrary data (`struct`, `char[]`, ...).
Each data frame is transferred using start and end guards.
The data is escaped (if necessary) and validated against a basic checksum.

It works best if the serial communication always uses the same data structure.
For example, a microcontroller that sends the same sensor data repeatedly.
An example can be found in [example.c](https://github.com/MartinWeigel/Cerial/blob/master/example.c).

## Features

- Simple interface with three functions: Cerial_init() / Cerial_read() / Cerial_write()
- Binary format reduces data transmission compared to ASCII
- Checksum helps to validate the received data
- Start and end of data can be distinguished from the message
- Liberal license for free and commercial software

## Protocol Format

Each message over the serial starts and ends with `0x00` (guards).
An escape byte `0xAA` avoids confusion of the data bytes and the guards.
The last byte in each message is a checksum to validate the message.

Byte | Description
---- | -----------
0x00 | Guard at the start and end of each message
0xAA | Escape character (0xAA in data is escaped as 0xAA 0xAA)
0xFF | Escaped guard character (0x00 in data is escaped as 0xAA 0xFF)


**Example:**

If we want to send the following data message:

12 34 **AA** 56 **00** 78 FF 90

The following bytes will be sent over the serial connection:

**00** 12 34 **AA AA** 56 **AA FF** 78 FF 90 **00**
