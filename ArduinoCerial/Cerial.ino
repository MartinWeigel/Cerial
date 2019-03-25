/**
 * Cerial.ino
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
#define CERIAL_GUARD     (0x00)           // Guard at begin and end of frame
#define CERIAL_ESCGUARD  (0xFF)           // Replacement character of guard (if escaped)
#define CERIAL_ESCAPE    (0xAA)           // Escaping character

int Cerial_waitForByte()
{
    while(Serial.available() == 0)
        ; // Just wait
    return Serial.read();
}

/**
 * Reads the given data over the serial port.
 * @data:
 *      The data read over the serial.
 * @size:
 *      The size of the data.
 */
void Cerial_read(unsigned char* data, size_t size)
{
    // Wait for first serial guard
    int b1;
    do {
        b1 = Cerial_waitForByte();
    } while(b1 != CERIAL_GUARD);

    // Skip all following guards (in case we got an end guard)
    do {
        b1 = Cerial_waitForByte();
    } while(b1 == CERIAL_GUARD);

    // Write byte to data message
    size_t index = 0;
    unsigned char temp[size+1];
    do {
        if(b1 == CERIAL_ESCAPE) {
            int b2;
            // If we have an escape, unescape next character
            b2 = Cerial_waitForByte();
            switch(b2) {
                case CERIAL_ESCAPE:
                    temp[index] = CERIAL_ESCAPE;
                    break;
                case CERIAL_ESCGUARD:
                    temp[index] = CERIAL_GUARD;
                    break;
                default:
                    return;
            }
        } else {
            // Save read character
            temp[index] = b1;
        }

        index++;
        b1 = Cerial_waitForByte();
    } while(index < sizeof(temp));

    // Calculate the checksum of the message
    char checksum = 0;
    for(size_t i=0; i<size; i++) {
        checksum ^= temp[i];
    }

    // Only return new data if the checksum matches
    if(temp[size] == checksum) {
        memcpy(data, temp, size);
    }
}

/**
 * Sends the given data over the serial port. Escapes bytes if necessary.
 * @data:
 *      The data to send over serial.
 * @size:
 *      The size of the data.
 */
void Cerial_write(unsigned char* data, size_t size)
{
    char checksum = 0;

    Serial.write(CERIAL_GUARD);
    // Start transmission of escaped data
    for(size_t i=0; i<size; i++) {
        checksum ^= data[i];               // Calculate checksum
        Cerial_sendEscaped(data[i]); // Send byte
    }
    // Add a checksum to the end of transmission
    Cerial_sendEscaped(checksum);
    Serial.write(CERIAL_GUARD);
}

/**
 * Sends the given byte over the serial. Escapes data if necessary.
 * This function does not need to be called externally.
 */
void Cerial_sendEscaped(unsigned char data)
{
    switch(data) {
        case CERIAL_GUARD:
            Serial.write(CERIAL_ESCAPE);
            Serial.write(CERIAL_ESCGUARD);
            break;
        case CERIAL_ESCAPE:
            Serial.write(CERIAL_ESCAPE);
            Serial.write(CERIAL_ESCAPE);
            break;
        default:
            Serial.write(data);
    }
}