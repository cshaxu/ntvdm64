#include <stddef.h>

#include "dosdef.h"

int main(void)
{
    return offsetof(SRCHDTA, pFFindEntry) != 0u ||
        offsetof(SRCHDTA, FFindId) != 4u || sizeof(SRCHDTA) != 43u ||
        offsetof(DIRENT, pFFindEntry) != 12u ||
        offsetof(DIRENT, FFindId) != 16u || sizeof(DIRENT) != 32u;
}
