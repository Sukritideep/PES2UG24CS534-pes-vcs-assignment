#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "index.h"

// forward declaration
int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out);

// mode constants
#define MODE_FILE 0100644
#define MODE_EXEC 0100755
#define MODE_DIR 0040000


// determine file mode
uint32_t get_file_mode(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return 0;

    if (S_ISDIR(st.st_mode))
        return MODE_DIR;

    if (st.st_mode & S_IXUSR)
        return MODE_EXEC;

    return MODE_FILE;
}

// helper for sorting entries
static int compare_entries(const void *a, const void *b)
{
    return strcmp(((const TreeEntry *)a)->name,
                  ((const TreeEntry *)b)->name);
}

// serialize tree into binary format
int tree_serialize(const Tree *tree, void **data_out, size_t *len_out)
{
    size_t max_size = tree->count * 300;
    unsigned char *buffer = malloc(max_size);
    if (!buffer) return -1;

    // copy and sort entries
    Tree temp = *tree;
    qsort(temp.entries, temp.count, sizeof(TreeEntry), compare_entries);

    size_t offset = 0;

    for (int i = 0; i < temp.count; i++)
    {
        const TreeEntry *entry = &temp.entries[i];

        int written = sprintf((char *)buffer + offset,
                              "%o %s", entry->mode, entry->name);

        offset += written + 1; // include null byte

        memcpy(buffer + offset, entry->hash.hash, HASH_SIZE);
        offset += HASH_SIZE;
    }

    *data_out = buffer;
    *len_out = offset;

    return 0;
}

int tree_from_index(ObjectID *id_out)
{
    // Step 1: load index
    Index index;
    if (index_load(&index) != 0)
        return -1;

    // Step 2: initialize tree
    Tree tree;
    memset(&tree, 0, sizeof(Tree));

    // Step 3: convert index entries → tree entries
    for (int i = 0; i < index.count; i++)
    {
        const char *path = index.entries[i].path;

        // skip nested paths for simplicity
        if (!path || strchr(path, '/'))
            continue;

        if (tree.count >= MAX_TREE_ENTRIES)
            return -1;

        TreeEntry *entry = &tree.entries[tree.count];

        entry->mode = index.entries[i].mode;

        strncpy(entry->name, path, sizeof(entry->name) - 1);
        entry->name[sizeof(entry->name) - 1] = '\0';

        entry->hash = index.entries[i].hash;

        tree.count++;
    }

    // Step 4: serialize tree
    void *data = NULL;
    size_t len = 0;

    if (tree_serialize(&tree, &data, &len) != 0)
        return -1;

    // Step 5: write object
    if (object_write(OBJ_TREE, data, len, id_out) != 0)
    {
        free(data);
        return -1;
    }

    free(data);
    return 0;
}