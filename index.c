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

// helper for sorting entries
static int compare_entries(const void *a, const void *b)
{
    return strcmp(((const IndexEntry *)a)->path,
                  ((const IndexEntry *)b)->path);
}

int index_save(const Index *index)
{
    // sort entries (cast away const)
    Index *temp = (Index *)index;
    qsort(temp->entries, temp->count, sizeof(IndexEntry), compare_entries);

    char temp_path[520];
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", INDEX_FILE);

    FILE *f = fopen(temp_path, "w");
    if (!f)
        return -1;

    for (int i = 0; i < index->count; i++)
    {
        const IndexEntry *entry = &index->entries[i];

        char hash_hex[HASH_HEX_SIZE + 1];
        hash_to_hex(&entry->hash, hash_hex);

        fprintf(f, "%o %s %lu %u %s\n",
                entry->mode,
                hash_hex,
                (unsigned long)entry->mtime_sec,
                entry->size,
                entry->path);
    }

    fflush(f);
    fclose(f);

    if (rename(temp_path, INDEX_FILE) != 0)
    {
        remove(temp_path);
        return -1;
    }

    return 0;
}