#include "index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// forward declaration
int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out);

int index_load(Index *index)
{
    index->count = 0;

    FILE *f = fopen(INDEX_FILE, "r");
    if (!f)
    {
        // no index file yet → empty index
        return 0;
    }

    char hash_hex[HASH_HEX_SIZE + 1];
    char path[512];

    while (index->count < MAX_INDEX_ENTRIES)
    {
        IndexEntry *entry = &index->entries[index->count];

        int result = fscanf(f, "%o %64s %lu %u %511s\n",
                            &entry->mode,
                            hash_hex,
                            (unsigned long *)&entry->mtime_sec,
                            &entry->size,
                            path);

        if (result == EOF)
            break;

        if (result != 5)
        {
            fclose(f);
            return -1;
        }

        // convert hash string → ObjectID
        if (hex_to_hash(hash_hex, &entry->hash) != 0)
        {
            fclose(f);
            return -1;
        }

        strncpy(entry->path, path, sizeof(entry->path) - 1);
        entry->path[sizeof(entry->path) - 1] = '\0';

        index->count++;
    }

    fclose(f);
    return 0;
}