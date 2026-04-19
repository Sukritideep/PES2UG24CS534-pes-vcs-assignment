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

int hex_to_hash(const char *hex, ObjectID *id_out)
{
    if (strlen(hex) < HASH_HEX_SIZE)
        return -1;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        unsigned int byte_val;

        if (sscanf(hex + i * 2, "%2x", &byte_val) != 1)
            return -1;

        id_out->hash[i] = (uint8_t)byte_val;
    }

    return 0;
}