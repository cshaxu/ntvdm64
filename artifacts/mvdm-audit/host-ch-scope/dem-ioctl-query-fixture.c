#include <stdio.h>
#include <stdint.h>
static unsigned code, drive, exists, ax, cf, seen_drive, calls;
unsigned getCL(void) { return code; }
unsigned getBL(void) { return drive; }
void setAX(unsigned v) { ax=v&65535; }
void setCF(unsigned v) { cf=v&1; }
void *demGetBDS(unsigned char v) { seen_drive=v; calls++; return exists ? (void*)(uintptr_t)1 : 0; }
void demIoctlDiskQuery(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s code=%u\n",__LINE__,#x,code); return 1; } } while(0)
int main(void) {
  static const unsigned accepted[]={0x40,0x41,0x42,0x46,0x60,0x61,0x62,0x66};
  unsigned i,j,valid; drive=4;
  for(exists=0;exists<2;exists++) for(code=0;code<256;code++) {
    ax=0xffff;cf=99;calls=seen_drive=0;demIoctlDiskQuery(); valid=0;
    for(j=0;j<sizeof(accepted)/sizeof(accepted[0]);j++) valid|=(code==accepted[j]);
    CHECK(calls==1 && seen_drive==drive);
    if(!exists) CHECK(ax==2 && cf==1);
    else if(valid) CHECK(ax==0 && cf==0);
    else CHECK(ax==5 && cf==1);
  }
  puts("PASS all 256 IOCTL query codes with present/absent drive (mocked BDS)"); return 0;
}
