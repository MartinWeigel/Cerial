/**
 * Serial.h
 *
 * Simple serial connection with implementations for Windows and UNIX.
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

typedef struct Serial
{
    uint32_t valid;
    char* serialPort;
    uint32_t baudrate;
    void* hComm;            // Windows Serial
    int fd;                 // UNIX Serial
} Serial;

Serial* Serial_new();
/**
 * Opens a socket connection.
 * @serialPort:
 *      The serial port to open (e.g. COM5).
 * @baudrate:
 *      The baudrate to communicate over the socket.
 * @return:
 *      True of serial connection was initialized, else false.
 */
bool Serial_init(Serial* self, const char* serialPort, uint32_t baudrate);
void Serial_clear(Serial* self);
void Serial_delete(Serial* self);

/**
 * Checks if the given serial is valid.
 */
bool Serial_isValid(Serial* self);

/**
 * Reads a single byte over the serial connection.
 * @byte:
 *      Location where the byte should be saved.
 * @return:
 *      True of serial connection was initialized, else false.
 */
bool Serial_readByte(Serial* self, unsigned char* byte);

/**
 * Reads a single byte over the serial connection.
 * @byte:
 *      Byte that shall be send.
 * @return:
 *      True of serial connection was initialized, else false.
 */
bool Serial_writeByte(Serial* self, const unsigned char byte);
