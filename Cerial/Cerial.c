/**
 * Cerial.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Cerial.h"

#define CERIAL_VALID     (0x20190309)
#define CERIAL_GUARD     (0x00)           // Guard at begin and end of frame
#define CERIAL_ESCGUARD  (0xFF)           // Replacement character of guard (if escaped)
#define CERIAL_ESCAPE    (0xAA)           // Escaping character


Cerial* Cerial_new()
{
    Cerial* self = malloc(sizeof(Cerial));
    return self;
}

bool Cerial_init(Cerial* self, const char* serialPort, uint32_t baudrate)
{
    self->valid = CERIAL_VALID;
    return Serial_init(&self->serial, serialPort, baudrate);
}

bool Cerial_isValid(Cerial* self)
{
    return (self != NULL) &&
           (self->valid == CERIAL_VALID) &&
           Serial_isValid(&self->serial);
}

void Cerial_clear(Cerial* self)
{
    Serial_clear(&self->serial);
    memset(self, 0, sizeof(Serial));
}

void Cerial_delete(Cerial* self)
{
    free(self);
}


bool Cerial_read(Cerial* self, unsigned char* data, size_t dataSize)
{
    assert(Cerial_isValid(self));
    assert(dataSize > 0);
    unsigned char b1;

    // Wait for first guard character
    do {
        bool status = Serial_readByte(&self->serial, &b1);
        if(!status) {
            fprintf(stderr, "[Cerial] Error in receiving start guard\n");
            return false;
        }
    } while(b1 != CERIAL_GUARD);

    // Skip following guards (in case we got an end guard)
    do {
        bool status = Serial_readByte(&self->serial, &b1);
        if(!status) {
            fprintf(stderr, "[Cerial] Error in receiving byte after start guard\n");
            return false;
        }
    } while(b1 == CERIAL_GUARD);

    // Read the message into temp (including checksum)
    unsigned char temp[dataSize+1];
    size_t index = 0;
    while(index < sizeof(temp)) {
        switch(b1) {
            case CERIAL_GUARD:
                fprintf(stderr, "[Cerial] Unexpected state guard (character %zu of %zu)\n", index, dataSize);
                return false;

            case CERIAL_ESCAPE: {
                unsigned char b2;

                // Read the byte following the escape
                bool status = Serial_readByte(&self->serial, &b2);
                if(!status) {
                    fprintf(stderr, "[Cerial] Error in receiving byte after escape\n");
                    return false;
                }
                // Unescape the read byte
                switch(b2) {
                    case CERIAL_ESCAPE:
                        temp[index] = CERIAL_ESCAPE;
                        break;
                    case CERIAL_ESCGUARD:
                        temp[index] = CERIAL_GUARD;
                        break;
                    default:
                        fprintf(stderr, "[Cerial] Invalid byte after escape 0x%x\n", b2);
                        return false;
                }
                break;
            }
            default:
                temp[index] = b1;
                break;
        }

        // Read next byte
        index++;
        bool status = Serial_readByte(&self->serial, &b1);
        if(!status) {
            fprintf(stderr, "[Cerial] Error in receiving byte %zu of %zu\n", index, dataSize);
            return false;
        }
    }

    // Check for end guard
    if(b1 != CERIAL_GUARD) {
        fprintf(stderr, "[Cerial] Received end guard (0x%x) does not match 0x%x\n", b1, CERIAL_GUARD);
        return false;
    }

    // Calculate checksum
    unsigned char checksum = 0;
    for(size_t i=0; i<dataSize; i++) {
        checksum ^= temp[i];
    }

    // Compare if message is valid by comparing checksums
    if(temp[dataSize] != checksum) {
        fprintf(stderr, "[Cerial] Received checksum (0x%x) does not match 0x%x\n", temp[dataSize], checksum);
        return false;
    } else {
        memcpy(data, temp, dataSize);
        return true;
    }
}


bool Cerial_writeEscapedByte(Cerial* self, const unsigned char byte)
{
    switch(byte) {
        case CERIAL_GUARD:
            return Serial_writeByte(&self->serial, CERIAL_ESCAPE) &&
                   Serial_writeByte(&self->serial, CERIAL_ESCGUARD);
        case CERIAL_ESCAPE:
            return Serial_writeByte(&self->serial, CERIAL_ESCAPE) &&
                   Serial_writeByte(&self->serial, CERIAL_ESCAPE);
        default:
            return Serial_writeByte(&self->serial, byte);
    }
}

bool Cerial_write(Cerial* self, const unsigned char* data, size_t dataSize)
{
    assert(Cerial_isValid(self));
    assert(dataSize > 0);

    unsigned char checksum = 0;

    // Send start guard
    if(!Serial_writeByte(&self->serial, CERIAL_GUARD)) {
        fprintf(stderr, "[Cerial] Error in sending start guard\n");
        return false;
    }

    // Send escaped data
    for(size_t i=0; i<dataSize; i++) {
        checksum ^= data[i];
        if(!Cerial_writeEscapedByte(self, data[i])) {
            fprintf(stderr, "[Cerial] Error in sending byte %zu of %zu\n", i, dataSize);
            return false;
        }
    }

    // Send escaped checksum
    if(!Cerial_writeEscapedByte(self, checksum)) {
        fprintf(stderr, "[Cerial] Error in sending checksum\n");
        return false;
    }

    // Send end guard
    if(!Serial_writeByte(&self->serial, CERIAL_GUARD)) {
        fprintf(stderr, "[Cerial] Error in sending end guard\n");
        return false;
    }
    return true;
}
