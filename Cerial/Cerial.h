/**
 * Cerial.h
 *
 * Connects to a serial port and reads/writes binary data (e.g., structs/arrays).
 * Guards the messages, escapes the content, and validates a checksum to ensure
 * the data is received correctly. Supports Windows and UNIX.
 * Requires both sides of the serial connections to use Cerial.
 *
 * Copyright (C) 2019 Martin Weigel <mail@MartinWeigel.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "Serial.h"

typedef struct Cerial
{
    uint32_t valid;
    Serial serial;
} Cerial;

Cerial* Cerial_new();
/**
 * Opens a socket connection.
 * @serialPort:
 *      The serial port to open (e.g. COM5).
 * @baudrate:
 *      The baudrate to communicate over the socket.
 */
bool Cerial_init(Cerial* self, const char* serialPort, uint32_t baudrate);
void Cerial_clear(Cerial* self);
void Cerial_delete(Cerial* self);

/**
 * Checks if the given serial is valid.
 */
bool Cerial_isValid(Cerial* self);

/**
 * Reads data from a serial port. This call is blocking and returns
 * either on successfull receiving data or an error in the serial format.
 * @data:
 *      Pointer to the data where to write the received message (should be >= dataSize).
 * @dataSize:
 *      Size of the data to receive.
 * @return:
 *      Success or failure of receiving the data. On success, the received data is stored
 *      at the data pointer. On error, memory of the data pointer is unmodified.
 */
bool Cerial_read(Cerial* self, unsigned char* data, size_t dataSize);

/**
 * Sends data over a serial port. This call is blocking and returns
 * either on successfull sending data or an error while sending.
 * @data:
 *      Pointer to the data to send (should be >= dataSize).
 * @dataSize:
 *      Size of the data to send.
 * @return:
 *      Success or failure of sending the data.
 */
bool Cerial_write(Cerial* self, const unsigned char* data, size_t dataSize);
