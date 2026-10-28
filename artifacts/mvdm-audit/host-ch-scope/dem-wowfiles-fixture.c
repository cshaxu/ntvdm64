#include <stdio.h>
int VDMForWOW;
static unsigned al, calls;
void setAL(unsigned v) { calls++; al=v&255; }
void demWOWFiles(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
  VDMForWOW=0; al=17; calls=0; demWOWFiles(); CHECK(al==17 && calls==0);
  VDMForWOW=0; al=255; calls=0; demWOWFiles(); CHECK(al==255 && calls==0);
  VDMForWOW=1; al=17; calls=0; demWOWFiles(); CHECK(al==255 && calls==1);
  VDMForWOW=1; al=255; calls=0; demWOWFiles(); CHECK(al==255 && calls==1);
  puts("PASS DOS preserves FILES value; WOW sets maximum for both initial values"); return 0;
}
