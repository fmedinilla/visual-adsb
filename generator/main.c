#include <stdlib.h>
#include <stdio.h>
#include "generator.h"

const int NUM_FLIGHTS = 2;

int main(void)
{
    flight_t flights[] = {
        { .ICAO=0xABC1001, .callsign="KLM00101", .lat=51.1234, .lon=0.7777, .speed=400, .dir=0 },
        { .ICAO=0xABC1002, .callsign="KLM00102", .lat=47.2323, .lon=-1.4532, .speed=300, .dir=90 }
    };

    for (int i = 0; i < NUM_FLIGHTS; i++) create_flight(&flights[i]);
    for (int i = 0; i < NUM_FLIGHTS; i++) wait_for_flight(&flights[i]);

    return EXIT_SUCCESS;
}