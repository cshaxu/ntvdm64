/* Default-disabled ABI control: no SoftPC state or host behavior. */
#include <windows.h>

int main(void) {
    HLOCAL allocation = LocalAlloc(LMEM_FIXED, 0x408);
    if (allocation == 0)
        return 1;
    if (LocalFree(allocation) != 0)
        return 2;
    return 0;
}
