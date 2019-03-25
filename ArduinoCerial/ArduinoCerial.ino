/**
 * ArduinoCerial.ino
 *
 * Example of an Arduino program sending data using Cerial and receiving
 * data of the same datastructure. Toggles LED with every received message.
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
#include <stdint.h>

typedef struct Data
{
    uint32_t frame;
    float example1;
    float example2;
    float example3;
} Data;

Data d1, d2;
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    // Set some example data
    d1.frame = 0;
    d1.example1 = 1.1;
    d1.example2 = 2.2;
    d1.example3 = 3.3;
}

void loop()
{
    d1.frame++;
    Cerial_write((unsigned char*)&d1, sizeof(d1));

    // Incoming communication, read the data
    if(Serial.available() > 0) {
        Cerial_read((unsigned char*)&d2, sizeof(d2));
        
        digitalWrite(LED_BUILTIN, d2.frame % 2 == 0);
    }
}
