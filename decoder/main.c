#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
            a.CA = -1;
            strcpy(a.callsign, "");
            a.next = alist.HEAD;
            
            alist.HEAD = &a;
            alist.size++;

            aux = &a;
        }

        // Aircraft identification
        if (frame.TC >= 1 && frame.TC <= 4) {
            aux->CA = get_aircraft_CA(frame.ME);
            get_aircraft_callsign(frame.ME, aux->callsign);
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
/* ------------------------- */
