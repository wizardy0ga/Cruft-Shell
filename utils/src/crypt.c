#include "crypt.h"

int encodeCipher (char* pStringToEncrypt) {

    // Declare loop counter
    int counter;

    // Get length of string to encrypt
    int intStringSize = strlen(pStringToEncrypt);

    for (counter = 0; (counter < (intStringSize)); counter++) {
        pStringToEncrypt[counter] = pStringToEncrypt[counter] + 3;
    }
    
    return 0;

}

int decodeCipher (char* pStringToEncrypt) {

    // Declare loop counter
    int counter;

    // Get length of string to encrypt
    int intStringSize = strlen(pStringToEncrypt);

    for (counter = 0; (counter < (intStringSize)); counter++) {
        pStringToEncrypt[counter] = pStringToEncrypt[counter] - 3;
    }
    
    return 0;

}