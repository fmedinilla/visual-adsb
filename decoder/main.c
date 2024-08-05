#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES 1
#include <math.h>


#define LONG_MESSAGE_BYTES 14
#define LONG_MESSAGE_BITS 14 * 8


typedef struct {
    char DF;
    char CA;
    int ICAO;
    unsigned long ME;
    char TC;
    int PI;
} ADSB_Frame;

typedef struct {
    char CA;
    char id[9];
} Aircraft_Identification;

typedef struct {
    char SS;
    char SAF;
    int ALT;
    char T;
    char F;
    int LAT_CPR;
    int LON_CPR;

    int baro_height;
    int gps_height;
    int latitude;
    int longitude;
} Airbone_Position;

typedef struct {
    char raw_message[LONG_MESSAGE_BYTES * 2];
    unsigned char bytes[LONG_MESSAGE_BYTES];
    char bin_message[LONG_MESSAGE_BITS + 1];

    ADSB_Frame frame;
} ADSB_Message;


unsigned long get_block(unsigned char *source, int start, int bits);
void bytes_to_binary_string(unsigned char *bytes, int length, char *output);
void hexstr_to_bytes(char *hexstr, unsigned char *bytes);
char identification_code(unsigned char encoded);
void get_data(ADSB_Message *message, char *raw_message);
void get_aircraft_id(ADSB_Message *message, Aircraft_Identification *aircraft_id);


int main(int argc, char *argv[])
{
    // Identification: 8D40621D202CC371C32CE0576098
    // Airborne position: 8D40621D58C382D690C8AC2863A7 - 8D40621D58C386435CC412692AD6 (10 sec) - (lat: 52.2572, lon: 3.91937)

    ADSB_Message message;
    get_data(&message, "8D40621D58C382D690C8AC2863A7");
    printf("ICAO: 0x%02X\n", message.frame.ICAO);

    switch (message.frame.TC) {
        case 0:
            // None
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        {
            // Aircraft identification - bit 33
            Aircraft_Identification aircraft_id;
            get_aircraft_id(&message, &aircraft_id);
            printf("Aircraft ID: '%s'\n", aircraft_id.id);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
            // Surface position
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        {
            // Airborne position (w/Baro Altitude) - bit 33
            //  | 33    | 38    | 40     | 41      | 53   | 54   | 55          | 72          |
            //  | TC, 5 | SS, 2 | SAF, 1 | ALT, 12 | T, 1 | F, 1 | LAT-CPR, 17 | LON-CPR, 17 |

            unsigned long SS = get_block(message.bin_message, 38, 2);
            printf("SS: %d\n", SS);

            unsigned long SAF = get_block(message.bin_message, 40, 1);
            printf("SAF: %d\n", SAF);

            unsigned long ALT = get_block(message.bin_message, 41, 12);
            printf("ALT: %d\n", ALT);

            int q_bit = get_block(message.bin_message, 48, 1);
            printf("Q-Bit: %d\n", q_bit);
            
            int N_a = get_block(message.bin_message, 41, 7);
            int N_b = get_block(message.bin_message, 49, 4);
            int N = (N_a << 4) | N_b;
            int h = 25 * N - 1000; // ft
            printf("Altitude: %d ft\n", h);

            unsigned long T = get_block(message.bin_message, 53, 1);
            printf("T: %d\n", T);

            unsigned long F = get_block(message.bin_message, 54, 1);
            printf("F: %d\n", F);

            unsigned long LAT_CPR = get_block(message.bin_message, 55, 17);
            printf("LAT_CPR: %d\n", LAT_CPR);

            unsigned long LON_CPR = get_block(message.bin_message, 72, 17);
            printf("LON_CPR: %d\n", LON_CPR);

            break;
        }
        case 19:
            // Airborne velocities
            break;
        case 20:
        case 21:
        case 22:
            // Airborne position (w/GNSS Height)
            break;
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            // 	Reserved
            break;
        case 28:
            // 	Aircraft status
            break;
        case 29:
            // 	Target state and status information
            break;
        case 30:
            // None
            break;
        case 31:
            // Aircraft operation status
            break;
    }
    
    return 0;
}


unsigned long get_block(unsigned char* source, int start, int bits)
{
    unsigned long block = 0;

    start--;

    for (int i = 0; i < bits; i++) {
        // Desplazar el bit correspondiente a su posición en el bloque
        block |= (source[start + i] & 1) << (bits - 1 - i);
    }

    return block;
}

void bytes_to_binary_string(unsigned char *bytes, int length, char *output)
{
    for (int i = 0; i < length; i++) {
        for (int j = 7; j >= 0; j--) {
            output[i * 8 + (7 - j)] = (bytes[i] & (1 << j)) ? '1' : '0';
        }
    }
    output[length * 8] = '\0'; // Null-terminator for the string
}

void hexstr_to_bytes(char *hexstr, unsigned char *bytes)
{
    const int LEN = strlen(hexstr) / 2;
    for (int i = 0; i < LEN; i++) {
        sscanf(hexstr + 2 * i, "%2hhx", &bytes[i]);
    }
}

char identification_code(unsigned char encoded)
{
    char decoded[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";
    return decoded[encoded];
}

void get_data(ADSB_Message *message, char *raw_message)
{
    strcpy(message->raw_message, raw_message);

    hexstr_to_bytes(message->raw_message, message->bytes);
    bytes_to_binary_string(message->bytes, LONG_MESSAGE_BYTES, message->bin_message);

    message->frame.DF = get_block(message->bin_message, 1, 5);
    message->frame.CA = get_block(message->bin_message, 6, 3);
    message->frame.ICAO = get_block(message->bin_message, 9, 24);
    message->frame.ME = get_block(message->bin_message, 33, 56);
    message->frame.PI = get_block(message->bin_message, 89, 24);
    message->frame.TC = get_block(message->bin_message, 33, 5);
}

void get_aircraft_id(ADSB_Message *message, Aircraft_Identification *aircraft_id)
{
    // Aircraft identification - bit 33
    // | 33   | 38   | 41   | 47   | 53   | 59   | 65   | 71   | 77   | 83   | 
    // | TC,5 | CA,3 | C1,6 | C2,6 | C3,6 | C4,6 | C5,6 | C6,6 | C7,6 | C8,6 |

    unsigned long CA = get_block(message->bin_message, 38, 3);
    char id[9] = {0};

    int start = 41;
    int step = 6;
    for (int i = 0; i < 8; i++) {
        unsigned long C = get_block(message->bin_message, start, step);
        id[i] = identification_code(C);
        start += step;
    }
    id[8] = '\0';

    aircraft_id->CA = CA;
    strcpy(aircraft_id->id, id);
}
