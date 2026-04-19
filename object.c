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



void compute_hash(const void *data, size_t len, ObjectID *id_out)
{
    const unsigned char *bytes = (const unsigned char *)data;

    for (int i = 0; i < HASH_SIZE; i++)
        id_out->hash[i] = 0;

    for (size_t i = 0; i < len; i++)
    {
        id_out->hash[i % HASH_SIZE] ^= bytes[i];
    }
}