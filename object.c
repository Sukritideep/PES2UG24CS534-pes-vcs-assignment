#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


// check if object already exists
int object_exists(const ObjectID *id)
{
    char path[512];
    object_path(id, path, sizeof(path));

    return access(path, F_OK) == 0;
}

// start object_write (partial)
int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out)
{
    const char *type_str;

    switch (type)
    {
        case OBJ_BLOB: type_str = "blob"; break;
        case OBJ_TREE: type_str = "tree"; break;
        case OBJ_COMMIT: type_str = "commit"; break;
        default: return -1;
    }

    char header[64];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type_str, len) + 1;

    size_t total_len = header_len + len;
    unsigned char *buffer = malloc(total_len);
    if (!buffer) return -1;

    memcpy(buffer, header, header_len);
    memcpy(buffer + header_len, data, len);

    compute_hash(buffer, total_len, id_out);

    if (object_exists(id_out))
    {
        free(buffer);
        return 0;
    }

    // remaining steps in next commit

    free(buffer);
    return -1;
}