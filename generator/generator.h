#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <pthread.h>

// ============================================== TYPES ==============================================
typedef unsigned char byte;
typedef unsigned long long ull;

typedef struct flight_t {
    int ICAO;
    char callsign[9];
    double lat;
    double lon;
    int speed;
    int dir;
    int baro_altitude;

    pthread_t control;
} flight_t;

typedef struct {
    int DF;
    int CA;
    int ICAO;
    ull ME;
    int TC;
    int PI;
} adsb_frame_t;

// ============================================== TEST ==============================================
void _generator_test();

// ============================================== FLIGHTS ==============================================
void create_flight(flight_t *flight);
void wait_for_flight(flight_t *flight);

#endif