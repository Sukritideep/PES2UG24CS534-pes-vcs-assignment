#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// convert hash to hex string
void hash_to_hex(const ObjectID *id, char *hex_out)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        sprintf(hex_out + i * 2, "%02x", id->hash[i]);
    }
    hex_out[HASH_HEX_SIZE] = '\0';
}