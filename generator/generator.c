#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "generator.h"

// ============================================== TEST ==============================================
void _generator_test()
{
    puts("files\n  - generator.h\n  - generator.c");
}

// ============================================== ENCODER ==============================================
int max(int a, int b)
{
    if (a >= b) return a;
    return b;
}

int NL(double latitude)
{
    if (latitude == 0) return 59;
    else if (latitude == -87 || latitude == 87) return 2;
    else return floor((2*M_PI) / (acos(1 - ((1-cos(M_PI/30)) / (pow(cos(M_PI*latitude/180), 2))))));
}

void encode_flight_identification(flight_t *flight, ull *ME)
{
    // | TC,5 | CA,3 | C1,6 | C2,6 | C3,6 | C4,6 | C5,6 | C6,6 | C7,6 | C8,6 |
    int TC = 4;
    int CA = 7;

    char *character_map = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";
    int cmap_len = strlen(character_map);
    int idx[8];

    for (int i = 0; i < 8; i++) {
        char c = flight->callsign[i];
        for (int j = 0; j < cmap_len; j++) {
            if (c == character_map[j]) idx[i] = j;
        }
    }

    // encode
    *ME = ((ull)TC << 51) |
            ((ull)CA << 48) |
            ((ull)idx[0] << 42) |
            ((ull)idx[1] << 36) |
            ((ull)idx[2] << 30) |
            ((ull)idx[3] << 24) |
            ((ull)idx[4] << 18) |
            ((ull)idx[5] << 12) |
            ((ull)idx[6] << 6) |
            (idx[7]);
}

void encode_flight_position(flight_t *flight, ull *ME, int is_odd)
{
    // | TC, 5 | SS, 2 | SAF, 1 | ALT, 12 | T, 1 | F, 1 | LAT-CPR, 17 | LON-CPR, 17 |

    int TC = 9;
    int SS = 0;
    int SAF = 0;
    int T = 0;
    int F = is_odd & 1;

    int e = 131072; // 2^17

    // Latitude
    double lat = flight->lat;
    double dLat = (double) 360 / (60 - F);
    int yz = floor(e * fmod(lat, dLat) / dLat + 0.5);
    int LAT_CPR = (int)(yz) & (e-1);

    // Longitude
    double lon = flight->lon;
    int nlcalc = max(NL(lat)-F, 1);
    double dLon = (double) 360 / (nlcalc);
    int xz = floor(e * ((fmod(lon, dLon))/dLon) + 0.5);
    int LON_CPR = (int)(xz) & (e-1);

    // Altitude
    int q_bit = 1;
    int N = (flight->baro_altitude + 1000) / 25;

    int ALT;
    ALT = ((N & 0b11111110000) >> 4);
    ALT = ALT << 1 | q_bit;
    ALT = ALT << 4 | (N & 0b1111);

    // encode
    *ME = ((ull)TC << 51) |
            ((ull)SS << 49) |
            ((ull)SAF << 48) |
            ((ull)ALT << 36) |
            ((ull)T << 35) |
            ((ull)F << 34) |
            ((ull)LAT_CPR << 17) |
            LON_CPR;
}

void encode_frame(adsb_frame_t *frame, byte *bytes)
{
    int DF = frame->DF;
    int CA = frame->CA;
    int ICAO = frame->ICAO;
    ull ME = frame->ME;
    int PI = frame->PI;

    // preamble
    bytes[0] = (DF << 3) | CA;

    // ICAO
    bytes[1] = (ICAO & 0xff0000) >> 16;
    bytes[2] = (ICAO & 0x00ff00) >> 8;
    bytes[3] = (ICAO & 0x0000ff) >> 0;

    // ME
    bytes[4]  = (ME & 0xff000000000000) >> 48;
    bytes[5]  = (ME & 0x00ff0000000000) >> 40;
    bytes[6]  = (ME & 0x0000ff00000000) >> 32;
    bytes[7]  = (ME & 0x000000ff000000) >> 24;
    bytes[8]  = (ME & 0x00000000ff0000) >> 16;
    bytes[9]  = (ME & 0x0000000000ff00) >> 8;
    bytes[10] = (ME & 0x000000000000ff) >> 0;

    // PI
    bytes[11] = (PI & 0xff0000) >> 16;
    bytes[12] = (PI & 0x00ff00) >> 8;
    bytes[13] = (PI & 0x0000ff) >> 0;
}

// ============================================== FLIGHTS ==============================================
void *flight_control(void *arg)
{
    flight_t *flight = (flight_t *)arg;

    int message_type = 0;   // 0. id
                            // 1. pos_o
                            // 2. pos_e
                            // 3. pos_o
                            // 4. pos_e
                            // 5. pos_o
                            // 6. pos_e

    while (1) {
        // TODO: update pos

        // TODO: send message
        adsb_frame_t adsb_frame;
        adsb_frame.DF = 17;
        adsb_frame.CA = 5;
        adsb_frame.ICAO = flight->ICAO;
        adsb_frame.ME = 0;
        adsb_frame.PI = 0;

        byte bytes[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        switch (message_type) {
            case 0:
                encode_flight_identification(flight, &adsb_frame.ME);
                break;
            case 1:
            case 3:
            case 5:
                encode_flight_position(flight, &adsb_frame.ME, 1);
                break;
            case 2:
            case 4:
            case 6:
                encode_flight_position(flight, &adsb_frame.ME, 0);
                break;
            default:
                printf("[0x%06X] send unknown message: ", flight->ICAO);
        }

        encode_frame(&adsb_frame, bytes);

        for (int i = 0; i < 14; i++) {
            printf("%02X", bytes[i]);
        }
        printf("\n");

        // update message
        message_type = (message_type + 1) % 7;
        
        // wait 1 second
        sleep(1);
    }

    return NULL;
}

void create_flight(flight_t *flight)
{
    flight->baro_altitude = 28000;
    pthread_create(&flight->control, NULL, flight_control, (void*)flight);
    printf("created flight 0x%06X\n", flight->ICAO);
}

void wait_for_flight(flight_t *flight)
{
    pthread_join(flight->control, NULL);
}
