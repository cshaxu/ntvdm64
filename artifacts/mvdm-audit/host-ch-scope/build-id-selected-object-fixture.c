#include <stdio.h>
#include <string.h>

static unsigned char al;
static unsigned short ax = 0x1234, bx = 0x5678;
static unsigned int address, length, calls;
static char name[6];

unsigned char c_getAL(void) { return al; }
unsigned short c_getCX(void) { return 0x2468; }
unsigned short c_getDS(void) { return 0x1234; }
unsigned int c_effective_addr(unsigned int segment, unsigned int offset)
{
    return segment * 16 + offset;
}
void c_sas_stores(unsigned int dest, const char *src, unsigned int count)
{
    address = dest;
    length = count;
    ++calls;
    if (count <= 5) memcpy(name, src, count);
}
void c_setAX(unsigned short value) { ax = value; }
void c_setBX(unsigned short value) { bx = value; }
void Get_build_id(void);

int main(void)
{
    al = 1;
    Get_build_id();
    if (calls != 1 || address != 0x147a8 || length != 5 ||
        memcmp(name, "Base$", 5) || ax != 0 || bx != 0x0af0) return 1;
    al = 2;
    ax = 0x1234;
    bx = 0x5678;
    Get_build_id();
    if (calls != 1 || ax != 0x1234 || bx != 0x5678) return 2;
    printf("base name=%s address=%x bx=%04x; unknown unchanged\n", name, address, 0x0af0);
    return 0;
}
