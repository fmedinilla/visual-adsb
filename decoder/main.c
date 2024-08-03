#include <stdio.h>
#include <string.h>

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

void hex_to_bin(char hex, char *bin);
unsigned int get_block(unsigned char *source, int start, int bits);
void get_bin(unsigned char *message, char *bin_message);

int main(int argc, char *argv[])
{
    unsigned char message[LONG_MESSAGE_BYTES] = { 0x88, 0xAB, 0xC1, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    char bin_message[LONG_MESSAGE_BITS] = { 0 };

    get_bin(message, bin_message);

    printf("BIN: ");
    for (int i = 0; i < LONG_MESSAGE_BITS; i++) {
        printf("%c", bin_message[i]);
    }
    printf("\n");

    unsigned int DF = get_block(bin_message, 0, 5);
    printf("DF: %d\n", DF);

    unsigned int CA = get_block(bin_message, 5, 3);
    printf("CA: %d\n", CA);

    unsigned int ICAO = get_block(bin_message, 8, 24);
    printf("ICAO: 0x%X\n", ICAO);

    unsigned int MESSAGE = get_block(bin_message, 32, 56);
    printf("MESSAGE: 0x%X\n", MESSAGE);

    unsigned int PARITY = get_block(bin_message, 88, 24);
    printf("PARITY: 0x%X\n", PARITY);


    return 0;
}

void hex_to_bin(char hex, char *bin)
{
    switch (hex)
    {
    case '0':
        strcpy(bin, "0000");
        break;
    case '1':
        strcpy(bin, "0001");
        break;
    case '2':
        strcpy(bin, "0010");
        break;
    case '3':
        strcpy(bin, "0011");
        break;
    case '4':
        strcpy(bin, "0100");
        break;
    case '5':
        strcpy(bin, "0101");
        break;
    case '6':
        strcpy(bin, "0110");
        break;
    case '7':
        strcpy(bin, "0111");
        break;
    case '8':
        strcpy(bin, "1000");
        break;
    case '9':
        strcpy(bin, "1001");
        break;
    case 'a':
    case 'A':
        strcpy(bin, "1010");
        break;
    case 'b':
    case 'B':
        strcpy(bin, "1011");
        break;
    case 'c':
    case 'C':
        strcpy(bin, "1100");
        break;
    case 'd':
    case 'D':
        strcpy(bin, "1101");
        break;
    case 'e':
    case 'E':
        strcpy(bin, "1110");
        break;
    case 'f':
    case 'F':
        strcpy(bin, "1111");
        break;
    default:
        strcpy(bin, "0000");
    }
}

unsigned int get_block(unsigned char* source, int start, int bits)
{
    unsigned int block = 0;

    for (int i = 0; i < bits; i++) {
        // Desplazar el bit correspondiente a su posición en el bloque
        block |= (source[start + i] & 1) << (bits - 1 - i);
    }

    return block;
}

void get_bin(unsigned char *message, char *bin_message)
{
    char bin_1[5] = { 0 };
    char bin_2[5] = { 0 };

    int idx = 0;

    for (int i = 0; i < LONG_MESSAGE_BYTES; i++) {
        unsigned char byte = message[i];

        unsigned char top_nibble = (byte >> 4) & 0x0F;
        unsigned char bottom_nibble = byte & 0x0F;

        // Convertir cada nibble a su representación en caracteres '0'-'9' o 'A'-'F'
        char top_char = top_nibble < 10 ? '0' + top_nibble : 'A' + (top_nibble - 10);
        char bottom_char = bottom_nibble < 10 ? '0' + bottom_nibble : 'A' + (bottom_nibble - 10);

        hex_to_bin(top_char, bin_1);
        hex_to_bin(bottom_char, bin_2);

        for (int i = 0; i < 4; i++) {
            bin_message[idx + i] = bin_1[i];
            bin_message[idx + i + 4] = bin_2[i];
        }

        idx += 8;
    }
}
