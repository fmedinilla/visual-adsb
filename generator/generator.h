#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <pthread.h>

typedef struct flight_t {
    int ICAO;
    char callsign[9];
    double lat;
    double lon;
    int speed;
    int dir;

    pthread_t control;
} flight_t;

void _generator_test();

void create_flight(flight_t *flight);

void wait_for_flight(flight_t *flight);

#endif