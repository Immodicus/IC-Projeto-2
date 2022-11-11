#include<iostream>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

// example: 
// printf("aux: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(aux));
// being aux a char

static void printAsBinary(char byte[]){
    printf("binary: ");
    for (int i = 0; i < 8; i++){
        printf("%d", byte[i]);
    }
    printf("\n");
}

static void printAsBinaryMsbtoLsb(char byte[], int numbits){
    printf("Encoded Value: ");
    for (int i = 0; i <numbits ; i++){
        printf("%d", byte[i]);
    }
    printf("\n");
}

static void printAsChar(char byte[]){
    unsigned char buff = 0;
    unsigned char bit;
    for(int i = 0; i < 8; i++){
        bit = (byte[i] & 0x01) << (7 - i);
        buff |= bit;
    }
    printf("char: %c\n", buff);
}
