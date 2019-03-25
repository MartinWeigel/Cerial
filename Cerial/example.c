/**
 * example.c
 *
 * Cerial example receiving and printing data from an Arduino.
 * Sends every 0.5s a new message over the Serial port.
 * Usable together with ../ArduinoCerial/ArduinoCerial.ino
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "Cerial.h"

// Tested only with 32/64 bit datatypes
typedef struct Data
{
    uint32_t frame;
    float example1;
    float example2;
    float example3;
} Data;

int main(int argc, const char **argv)
{
    // Disable buffering (esp. for cygwin)
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    if(argc != 2) {
        fprintf(stderr, "Error: No socket port is given.\n");
        fprintf(stderr, "Usage: ./example.exe COM3\n");
        return EXIT_FAILURE;
    }

    // Connect to serial with baud rate 115200
    Cerial cerial;
    if(Cerial_init(&cerial, argv[1], 115200)) {
        // Prepare data
        Data d1, d2;
        memset(&d1, 0, sizeof(d1));
        memset(&d2, 0, sizeof(d2));

        while(true) {
            // Read a data frame, this call is blocking
            bool status = Cerial_read(&cerial, (unsigned char*) &d1, sizeof(Data));
            if(status) {
                printf("Data received! Frame: %d\n", d1.frame);
            }

            // Write a data frame, this call is blocking
            status = Cerial_write(&cerial, (unsigned char*) &d2, sizeof(Data));
            if(status) {
                printf("Data sent! Frame: %d\n", d2.frame);
            }

            usleep(0.5*1000000);
            d2.frame++;
        }
        Cerial_clear(&cerial);
    }

    return EXIT_SUCCESS;
}
