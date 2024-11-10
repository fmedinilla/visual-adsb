#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "generator.h"

void _generator_test()
{
    puts("files\n  - generator.h\n  - generator.c");
}

void *flight_control(void *arg)
{
    flight_t *flight = (flight_t *)arg;

    while (1) {
        // TODO: send_message
        sleep(1);
    }

    return NULL;
}

void create_flight(flight_t *flight)
{
    pthread_create(&flight->control, NULL, flight_control, (void*)flight);
    printf("created flight 0x%06X\n", flight->ICAO);
}

void wait_for_flight(flight_t *flight)
{
    pthread_join(flight->control, NULL);
}