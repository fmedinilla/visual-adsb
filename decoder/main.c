#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* CONSTANTS */
#define EMPTY_INT_FIELD -123456
/* ------------------------- */


/* USER TYPES */
typedef unsigned char byte;
typedef unsigned long long ull;

typedef struct {
    int DF;
    int CA;
    int ICAO;
    ull ME;
    int TC;
    int PI;
} ADSB_Frame;

typedef struct Aircraft {
    int ICAO;
    int CA;
    char callsign[9];

    int baro_altitude;

    double latitude;
    int lat_cpr_even;
    int lat_cpr_odd;

    struct Aircraft *next;
} Aircraft;

typedef struct {
    Aircraft *HEAD;
    int size;
} Aircraft_List;
/* ------------------------- */


/* PROTOTYPES */
void get_bytes(char *message, byte *bytes);
void create_adsb_frame(byte *bytes, ADSB_Frame *frame);
void print_adsb_frame(ADSB_Frame *frame);
void print_aircraft(Aircraft *aircraft);

int get_aircraft_CA(ull ME);
void get_aircraft_callsign(ull ME, char *callsign);
int get_aircraft_baro_alt(ull ME);
int get_cpr_format(ull ME);
void get_lat_cpr(ull ME, Aircraft *aircraft);
void decode_lat(Aircraft *aircraft);
/* ------------------------- */


/* ENTRY POINT */
int main()
{
    Aircraft_List alist;
    alist.HEAD = NULL;
    alist.size = 0;

    byte bytes[14];
    char buffer[29];

    while (1) {
        printf("INPUT: ");
        fgets(buffer, 29, stdin);
        fflush(stdin);

        printf("RAW: %s\n", buffer);
        get_bytes(buffer, bytes);
        
        ADSB_Frame frame;
        create_adsb_frame(bytes, &frame);
        print_adsb_frame(&frame);

        Aircraft *aux = alist.HEAD;
        while (aux) {
            if (aux->ICAO == frame.ICAO) break;
            aux = aux->next;
        }

        if (!aux) {
            Aircraft a;
            a.ICAO = frame.ICAO;
            a.CA = EMPTY_INT_FIELD;
            strcpy(a.callsign, "");
            a.next = alist.HEAD;
            a.baro_altitude = EMPTY_INT_FIELD;
            a.lat_cpr_even = EMPTY_INT_FIELD;
            a.lat_cpr_odd = EMPTY_INT_FIELD;
            a.latitude = EMPTY_INT_FIELD;
            
            alist.HEAD = &a;
            alist.size++;

            aux = &a;
        }

        // Aircraft identification
        if (frame.TC >= 1 && frame.TC <= 4) {
            aux->CA = get_aircraft_CA(frame.ME);
            get_aircraft_callsign(frame.ME, aux->callsign);
        }

        // Airborne position (w/Baro Altitude)
        if (frame.TC >= 9 && frame.TC <= 18) {
            aux->baro_altitude = get_aircraft_baro_alt(frame.ME);

            get_lat_cpr(frame.ME, aux);
            decode_lat(aux);
        }

        print_aircraft(aux);

    }

    return 0;
}
/* ------------------------- */


/* FUNCTIONS IMPLEMENTATION */
void get_bytes(char *message, byte *bytes)
{
    for (int i = 0; i < 28; i+=2) {
        char num[3];
        sprintf(num, "%c%c", message[i], message[i+1]);
        byte aux = strtol(num, NULL, 16);
        bytes[i/2] = aux;
    }
}
void create_adsb_frame(byte *bytes, ADSB_Frame *frame)
{
    // DF: 0B 1-5 bits
    frame->DF = bytes[0] >> 3;

    // CA: 0B 6-8 bits
    frame->CA = (bytes[0] & 0b00000111);
    
    // ICAO: 1B 2B 3B
    frame->ICAO = 0;
    int start = 16;
    for (int i = 1; i < 4; i++) {
        frame->ICAO |= bytes[i] << start;
        start -= 8;
    }
    
    // ME: 4B 5B 6B 7B 8B 9B 10B
    frame->ME = 0;
    start = 48;
    for (int i = 4; i < 11; i++) {
        frame->ME |= (ull)bytes[i] << start;
        start -= 8;
    }
    
    // TC: first 5 bits of ME
    frame->TC = frame->ME >> 51;

    // PI: 11B 12B 13B
    frame->PI = 0;
    start = 16;
    for (int i = 11; i < 14; i++) {
        frame->PI |= bytes[i] << start;
        start -= 8;
    }
}
void print_adsb_frame(ADSB_Frame *frame)
{
    printf("ADS-B Frame\n");
    printf("  DF: %d\n", frame->DF);
    printf("  CA: %d\n", frame->CA);
    printf("  ICAO: 0x%X\n", frame->ICAO);
    printf("  ME: 0x%X\n", frame->ME);
    printf("  TC: %d\n", frame->TC);
    printf("  PI: 0x%X\n", frame->PI);
}
void print_aircraft(Aircraft *aircraft)
{
    if (!aircraft) return;

    printf("Aircraft\n");
    printf("  ICAO: 0x%X\n", aircraft->ICAO);
    printf("  CA: %d\n", aircraft->CA);
    printf("  callsign: '%s'\n", aircraft->callsign);
    printf("  Barometric altitude: %d ft\n", aircraft->baro_altitude);
    printf("  Lat: %f\n", aircraft->latitude);
}

int get_aircraft_CA(ull ME)
{
    // ME: 56 bits
    // TC 5 - CA 3 - C1 6 - C2 6 - C3 6 - C4 6 - C5 6 - C6 6 - C7 6 - C8 6

    int aircraft_CA = (ME & ((ull)0x7 << 48)) >> 48;
    return aircraft_CA;
}
void get_aircraft_callsign(ull ME, char *callsign)
{
    // ME: 56 bits
    // TC 5 - CA 3 - C1 6 - C2 6 - C3 6 - C4 6 - C5 6 - C6 6 - C7 6 - C8 6

    char *character_map = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";

    int start = 42;
    for (int i = 0; i < 8; i++) {
        int c = (ME & ((ull)0x3f << start)) >> start;
        callsign[i] = character_map[c];
        start -= 6;
    }
    callsign[8] = '\0';
}
int get_aircraft_baro_alt(ull ME)
{
    // 56 bits
    // | TC, 5 | SS, 2 | SAF, 1 | ALT, 12 | T, 1 | F, 1 | LAT-CPR, 17 | LON-CPR, 17 |

    int ALT = (ME & ((ull)0b111111111111 << 36)) >> 36;
    
    // 1: 25ft, 0: 100ft (gray code)
    int q_bit = (ALT & (0b1 << 4)) >> 4; 
    int alt_left = (ALT &  (0b1111111 << 5)) >> 5;
    int alt_right = ALT & 0b1111;
    int alt = (alt_left << 4) | alt_right;
    int altitude = alt * 25 - 1000;
    return altitude;
}
int get_cpr_format(ull ME)
{
    // 56 bits
    // | TC, 5 | SS, 2 | SAF, 1 | ALT, 12 | T, 1 | F, 1 | LAT-CPR, 17 | LON-CPR, 17 |

    int CPR_F = (ME & ((ull)0b1 << 34)) >> 34;
    return CPR_F;
}
void get_lat_cpr(ull ME, Aircraft *aircraft)
{
    // 56 bits
    // | TC, 5 | SS, 2 | SAF, 1 | ALT, 12 | T, 1 | F, 1 | LAT-CPR, 17 | LON-CPR, 17 |

    int LAT_CPR = (ME & ((ull)0x1FFFF << 17)) >> 17;

    int is_odd = get_cpr_format(ME);

    if (is_odd) aircraft->lat_cpr_odd = LAT_CPR;
    else aircraft->lat_cpr_even = LAT_CPR;
}
void decode_lat(Aircraft *aircraft)
{
    if (aircraft->lat_cpr_even == EMPTY_INT_FIELD || aircraft->lat_cpr_odd == EMPTY_INT_FIELD) return;

    double dLat_even = (double) 360 / 60;
    double dLat_odd = (double) 360 / 59;

    double lat_cpr_even = (double) aircraft->lat_cpr_even / (1 << 17);
    double lat_cpr_odd = (double) aircraft->lat_cpr_odd / (1 << 17);

    int j = floor(59 * lat_cpr_even - 60 * lat_cpr_odd + 0.5);

    double lat_even = dLat_even * ((j % 60) + lat_cpr_even);
    double lat_odd = dLat_odd * ((j % 59) + lat_cpr_odd);

    if (lat_even >= 270) lat_even -= 360;
    if (lat_odd >= 270) lat_odd -= 360;

    // TODO: more recent of these two latitudes
    aircraft->latitude = (lat_even + lat_odd) / 2;
}
/* ------------------------- */
