#include <stdio.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>


/* CONSTANTS */
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

    double longitude;
    int lon_cpr_even;
    int lon_cpr_odd;

    struct Aircraft *next;
} Aircraft;
/* ------------------------- */


/* PROTOTYPES */
void print_bytes(byte *bytes);
void encode_frame(ADSB_Frame *frame, byte *bytes);
void set_preamble(byte *bytes, int DF, int CA);
void set_ICAO(byte *bytes, int ICAO);
void set_ME(byte *bytes, ull ME);
void set_PI(byte *bytes, int PI);
int NL(double latitude);
int max(int a, int b);

void encode_aircraft_identification(Aircraft *aircraft, ull *ME);
void encode_aircraft_position(Aircraft *aircraft, ull *ME, int is_odd);
/* ------------------------- */


/* ENTRY POINT */
int main()
{
    Aircraft aircraft;
    aircraft.CA = 7;
    aircraft.ICAO = 0xABC123;
    aircraft.baro_altitude = 28000;
    aircraft.latitude = 52.2572;
    aircraft.longitude = 3.91937;
    strcpy(aircraft.callsign, "KLM10110");

    ADSB_Frame frame;
    frame.DF = 17;
    frame.CA = 5;
    frame.ICAO = aircraft.ICAO;
    frame.ME = 0;
    frame.PI = 0;

    // Bytes
    byte bytes[14];

    // Aircraft identification
    encode_aircraft_identification(&aircraft, &frame.ME);
    encode_frame(&frame, bytes);
    print_bytes(bytes);

    // Airborne position (w/Baro Altitude)
    encode_aircraft_position(&aircraft, &frame.ME, 0); // even message
    encode_frame(&frame, bytes);
    print_bytes(bytes);

    encode_aircraft_position(&aircraft, &frame.ME, 1); // odd message
    encode_frame(&frame, bytes);
    print_bytes(bytes);

    return 0;
}
/* ------------------------- */



/* FUNCTION IMPLEMENTATION */
void print_bytes(byte *bytes)
{
    // Print bytes
    printf("ENCODED: ");
    for (int i = 0; i < 14; i++) {
        printf("%02X", bytes[i]);
    }
    printf("\n");
}
void encode_frame(ADSB_Frame *frame, byte *bytes)
{
    set_preamble(bytes, frame->DF, frame->CA);
    set_ICAO(bytes, frame->ICAO);
    set_ME(bytes, frame->ME);
    set_PI(bytes, frame->PI);
}
void set_preamble(byte *bytes, int DF, int CA)
{
    bytes[0] = (DF << 3) | CA;
}
void set_ICAO(byte *bytes, int ICAO)
{
    bytes[1] = (ICAO & 0xff0000) >> 16;
    bytes[2] = (ICAO & 0x00ff00) >> 8;
    bytes[3] = (ICAO & 0x0000ff) >> 0;
}
void set_ME(byte *bytes, ull ME)
{
    bytes[4]  = (ME & 0xff000000000000) >> 48;
    bytes[5]  = (ME & 0x00ff0000000000) >> 40;
    bytes[6]  = (ME & 0x0000ff00000000) >> 32;
    bytes[7]  = (ME & 0x000000ff000000) >> 24;
    bytes[8]  = (ME & 0x00000000ff0000) >> 16;
    bytes[9]  = (ME & 0x0000000000ff00) >> 8;
    bytes[10] = (ME & 0x000000000000ff) >> 0;
}
void set_PI(byte *bytes, int PI)
{
    bytes[11] = (PI & 0xff0000) >> 16;
    bytes[12] = (PI & 0x00ff00) >> 8;
    bytes[13] = (PI & 0x0000ff) >> 0;
}
int NL(double latitude)
{
    if (latitude == 0) return 59;
    else if (latitude == -87 || latitude == 87) return 2;
    else return floor((2*M_PI) / (acos(1 - ((1-cos(M_PI/30)) / (pow(cos(M_PI*latitude/180), 2))))));
}
int max(int a, int b)
{
    if (a >= b) return a;
    return b;
}

void encode_aircraft_identification(Aircraft *aircraft, ull *ME)
{
    // | TC,5 | CA,3 | C1,6 | C2,6 | C3,6 | C4,6 | C5,6 | C6,6 | C7,6 | C8,6 |
    int TC = 4;
    int CA = aircraft->CA;

    char *character_map = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";
    int cmap_len = strlen(character_map);
    int idx[8];

    for (int i = 0; i < 8; i++) {
        char c = aircraft->callsign[i];
        for (int j = 0; j < cmap_len; j++) {
            if (c == character_map[j]) idx[i] = j;
        }
    }

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
void encode_aircraft_position(Aircraft *aircraft, ull *ME, int is_odd)
{
    // | TC, 5 | SS, 2 | SAF, 1 | ALT, 12 | T, 1 | F, 1 | LAT-CPR, 17 | LON-CPR, 17 |

    int TC = 9;
    int SS = 0;
    int SAF = 0;
    int T = 0;
    int F = is_odd & 1;

    int e = 131072; // 2^17

    // Latitude
    double lat = aircraft->latitude;
    double dLat = (double) 360 / (60 - F);
    int yz = floor(e * fmod(lat, dLat) / dLat + 0.5);
    int LAT_CPR = (int)(yz) & (e-1);

    // Longitude
    double lon = aircraft->longitude;
    int nlcalc = max(NL(lat)-F, 1);
    double dLon = (double) 360 / (nlcalc);
    int xz = floor(e * ((fmod(lon, dLon))/dLon) + 0.5);
    int LON_CPR = (int)(xz) & (e-1);

    // Altitude
    int q_bit = 1;
    int N = (aircraft->baro_altitude + 1000) / 25;

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
/* ------------------------- */
