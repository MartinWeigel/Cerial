/**
 * Serial.c
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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Serial.h"

#define SERIAL_VALID     (0x20190309)

Serial* Serial_new()
{
    Serial* self = malloc(sizeof(Serial));
    return self;
}

bool Serial_isValid(Serial* self)
{
    return (self != NULL) && (self->valid == SERIAL_VALID);
}

void Serial_delete(Serial* self)
{
    free(self);
}


// The following code contains platform specific function calls
#ifdef _WIN32
#include <windows.h>

bool Serial_init(Serial* self, const char* serialPort, uint32_t baudrate)
{
    self->valid = SERIAL_VALID;
    self->serialPort = strdup(serialPort);
    self->baudrate = baudrate;

    // Open the port
    HANDLE hComm = CreateFile(self->serialPort,
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL);

    if(hComm == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "[Serial_WIN] Cannot open serial port %s\n", self->serialPort);
        return false;
    }

    // Set parameters for communication
    DCB params = { 0 };
    params.DCBlength = sizeof(params);
    params.BaudRate = baudrate;
    params.ByteSize = 8;
    params.StopBits = ONESTOPBIT;
    params.Parity   = NOPARITY;

    if(!SetCommState(hComm, &params)) {
        fprintf(stderr, "[Serial_WIN] Cannot set comm state for %s\n", self->serialPort);
        return false;
    }

    self->hComm = hComm;
    return true;
}

void Serial_clear(Serial* self)
{
    CloseHandle((HANDLE)self->hComm);
    free(self->serialPort);
    memset(self, 0, sizeof(Serial));
}

bool Serial_readByte(Serial* self, unsigned char* byte)
{
    DWORD bytesRead = 0;
    bool status = ReadFile((HANDLE)self->hComm, byte, 1, &bytesRead, NULL);
    // printf("%x ", *byte);
    return status && bytesRead == 1;
}

bool Serial_writeByte(Serial* self, const unsigned char byte)
{
    DWORD bytesWritten = 0;
    bool status = WriteFile((HANDLE)self->hComm, &byte, 1, &bytesWritten, NULL);
    return status && bytesWritten == 1;
}

#else
// POSIX, modified from https://www.cmrr.umn.edu/~strupp/serial.html
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#define CRTSCTS  (020000000000)

char* strdup(const char* s)
{
  size_t len = 1+strlen(s);
  char* p = malloc(len);
  return p ? memcpy(p, s, len) : NULL;
}

bool Serial_init(Serial* self, const char* serialPort, uint32_t baudrate)
{
    self->valid = SERIAL_VALID;
    self->serialPort = strdup(serialPort);
    self->baudrate = baudrate;

    // Open the serial port
    self->fd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
    if(self->fd == -1) {
        fprintf(stderr, "[Serial_POSIX] Cannot open serial port %s\n", self->serialPort);
        return false;
    }
    fcntl(self->fd, F_SETFL, 0); // Do not block read command

    // Set parameters for serial port
    struct termios options;
    if(tcgetattr(self->fd, &options) != 0) {
        fprintf(stderr, "[Serial_POSIX] Cannot get attributes from %s\n", self->serialPort);
        return false;
    }
    cfmakeraw(&options);
    cfsetospeed(&options, (speed_t)baudrate);
    cfsetispeed(&options, (speed_t)baudrate);
    options.c_cflag &= ~PARENB;   // 8N1
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    if(tcsetattr(self->fd, TCSANOW, &options) != 0) {
        fprintf(stderr, "[Serial_POSIX] Cannot set attributes (error: %s)\n", strerror(errno));
        return false;
    }
    return true;
}

void Serial_clear(Serial* self)
{
    free(self->serialPort);
    memset(self, 0, sizeof(Serial));
}

bool Serial_readByte(Serial* self, unsigned char* byte)
{
    size_t readSize = read(self->fd, byte, 1);
    // printf("%2x ", *byte);
    return readSize == 1;
}

bool Serial_writeByte(Serial* self, const unsigned char byte)
{
    size_t wroteSize = write(self->fd, &byte, 1);
    // printf("%2x ", byte);
    return wroteSize == 1;
}
#endif
