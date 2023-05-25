#include "crypt.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

    printf("[BEGIN] Now encrypting data -> %s\n", argv[1]);

    decodeCipher(argv[1]);

    printf("Finished encryption routine, here is your data -> %s", argv[1]);

    return 0;
}