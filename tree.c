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