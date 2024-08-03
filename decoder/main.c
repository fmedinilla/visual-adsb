#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
ADS-B Short message 56 bits (7 bytes)
ADS-B Long message 112 bits (14 bytes)

Long message
    +----------+----------+-------------+------------------------+-----------+
    |  DF (5)  |  CA (3)  |  ICAO (24)  |         ME (56)        |  PI (24)  |
    +----------+----------+-------------+------------------------+-----------+

    icao_address = 0xABC123
    latitude = 37.7749
    longitude = -122.4194
    identification = 'N12345'

    DF: 17 = 10001
    CA: 0 = 000
    ICAO: 0xABC123 = 1010 1011 1100 0001 0010 0011
    ME: 00000000000000000000000000000000000000000000000000000000
    PI: 000000000000000000000000

    8 4 2 1

    ADS-B MESSAGE: 1000 1000 1010 1011 1100 0001 0010 0011 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
              HEX: 8    8    A    B    C    1    2    3    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0    0
              HEX: 88ABC12300000000000000000000


    lat_bin = decimal_to_binary(int((lat + 90) * 1e6), 32)
    lon_bin = decimal_to_binary(int((lon + 180) * 1e6), 32)
*/

#define LONG_MESSAGE_BYTES 14
#define LONG_MESSAGE_BITS 14 * 8

unsigned long get_block(unsigned char *source, int start, int bits);
void bytes_to_binary_string(unsigned char *bytes, int length, char *output);
void hexstr_to_bytes(char *hexstr, unsigned char *bytes);
char identification_code(unsigned char encoded);


int main(int argc, char *argv[])
{
    char *hexstr = "8D4840D6202CC371C32CE0576098";
    printf("RAW: %s\n", hexstr);

    unsigned char message[LONG_MESSAGE_BYTES] = {0};
    hexstr_to_bytes(hexstr, message);

    char bin_message[LONG_MESSAGE_BITS + 1] = { 0 };
    bytes_to_binary_string(message, LONG_MESSAGE_BYTES, bin_message);
    printf("BIN: %s\n", bin_message);

    /* BLOQUE DE BITS */
    unsigned long DF = get_block(bin_message, 0, 5);
    printf("DF: %d\n", DF);

    unsigned long CA = get_block(bin_message, 5, 3);
    printf("CA: %d\n", CA);

    unsigned long ICAO = get_block(bin_message, 8, 24);
    printf("ICAO: 0x%X\n", ICAO);

    unsigned long MESSAGE = get_block(bin_message, 32, 56);
    printf("MESSAGE: 0x%X\n", MESSAGE);
    
    unsigned long TC = get_block(bin_message, 32, 5);
    printf("TC: %d\n", TC);

    switch (TC)
    {
        case 1:
        case 2:
        case 3:
        case 4:
            CA = get_block(bin_message, 37, 3);
            printf("CA: %d\n", CA);
            
            int start = 40;
            int step = 6;
            for (int i = 0; i < 8; i++) {
                unsigned long C = get_block(bin_message, start, step);
                printf("C%d: %d (%c)\n", i+1, C, identification_code(C));
                start += step;
            }
            break;
    }

   

    unsigned long PARITY = get_block(bin_message, 88, 24);
    printf("PARITY: 0x%X\n", PARITY);
    /* --------------------------------------------------------------------- */


    return 0;
}


unsigned long get_block(unsigned char* source, int start, int bits)
{
    unsigned long block = 0;

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
