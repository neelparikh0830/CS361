#include <stdio.h>
#include "lib361.h"

int secretoperation(int x, int y) {
    int z = x*y;
    if (z == 2)
    {
        return 4;
    }
    return 10;
}