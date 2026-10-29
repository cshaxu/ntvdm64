#include <stdio.h>
#include <string.h>
static char events[8]; static unsigned pos, value, success;
static void event(char c) { events[pos++]=c; events[pos]=0; }
void HostFloppyReset(void) { event('F'); }
void HostFdiskReset(void) { event('D'); }
int mvdm_command_guest_state_write_fd_access(unsigned short v) { value=v; event('W'); return success; }
void TerminateVDM(void) { event('T'); }
void demDiskReset(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
  pos=0; events[0]=0; value=99; success=1; demDiskReset(); CHECK(!strcmp(events,"FDW") && value==0);
  pos=0; events[0]=0; value=99; success=0; demDiskReset(); CHECK(!strcmp(events,"FDWT") && value==0);
  puts("PASS reset order, zero write and failure termination dispatch (mocked host/guest boundaries)"); return 0;
}
