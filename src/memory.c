#include "datastructs/memory.h"

#include <stdlib.h>

struct yu_allocator _yu_allocator = {
    .allocate = malloc,
    .calloc = calloc,
    .realloc = realloc,
    .free = free,
};
