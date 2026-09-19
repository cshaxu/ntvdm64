#include "mvdm/dos/command/cmd.h"
#include "mvdm_command_guest_state.h"
#include "ntvdm-exe/session/session.h"

/* Unrelated scalar bindings in the same TU must never execute here. */
session *session_thread_current(void) { ExitProcess(99); return NULL; }
int session_valid(session const *value) { (void)value; ExitProcess(99); return 0; }
int mvdm_guest_location_read_u16(mvdm_guest_location const *value,uint16_t *out)
{ (void)value;(void)out;ExitProcess(99);return 0; }

VDMENVBLK cmdVDMEnvBlk;
BOOL IsFirstVDM=TRUE;
static char guest[]="S37_CONFIGROOT=O:\\winnt\\tests\\P37\0";
static int active,mode,calls,failed,allocations;
void *pif_test_malloc(size_t bytes)
{ void *p; if(mode==3) return NULL; p=malloc(bytes);if(p)++allocations;return p; }
void pif_test_free(void *p) { if(p)--allocations;free(p); }
int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment,uint16_t offset)
{ (void)location; return segment==0x1234 && offset==0; }
int mvdm_guest_location_acquire(mvdm_guest_location const *location,
    uint32_t bytes,uint32_t access,mvdm_guest_location_lease *lease)
{
    (void)location;
    if (mode==1) return 0;
    if (active || access!=GUEST_MEMORY_ACCESS_READ || bytes!=sizeof(guest)) failed=1;
    active=1;lease->bytes=(uint8_t *)guest;return 1;
}
int mvdm_guest_location_release(mvdm_guest_location_lease *lease,int commit)
{ (void)lease; if(!active || commit) failed=1;active=0;return mode!=2; }
VOID cmdCheckForPIF(PVDMINFO info)
{
    (void)info;
    ++calls;
    if (active || !cmdVDMEnvBlk.lpszzEnv ||
        strcmp(cmdVDMEnvBlk.lpszzEnv,guest)) failed=1;
    if (IsFirstVDM && cmdVDMEnvBlk.lpszzEnv==guest) failed=1;
}
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL %d\n",__LINE__);return 1; } } while(0)
int main(void)
{
    VDMINFO info={0};
    int row;
    for(row=0;row<4;++row) {
        mode=row;calls=0;info.ErrorCode=0;
        mvdm_command_check_pif(&info,0x1234,sizeof(guest));
        CHECK(!active && !failed && !allocations && !cmdVDMEnvBlk.lpszzEnv);
        CHECK(row ? calls==0 && info.ErrorCode==(row==3 ? ERROR_NOT_ENOUGH_MEMORY : ERROR_INVALID_ADDRESS) : calls==1 && !info.ErrorCode);
    }
    mode=0;calls=0;info.ErrorCode=0;
    guest[sizeof(guest)-1]='X';
    mvdm_command_check_pif(&info,0x1234,sizeof(guest));
    CHECK(!calls && !active && !allocations && !cmdVDMEnvBlk.lpszzEnv && info.ErrorCode==ERROR_INVALID_ADDRESS);
    guest[sizeof(guest)-1]=0;
    info.ErrorCode=0;
    mvdm_command_check_pif(&info,0x1234,1);
    CHECK(!calls && info.ErrorCode==ERROR_INVALID_ADDRESS);
    IsFirstVDM=FALSE;cmdVDMEnvBlk.lpszzEnv=guest;info.ErrorCode=0;
    mvdm_command_check_pif(&info,0,0);
    CHECK(calls==1 && !failed && !active && cmdVDMEnvBlk.lpszzEnv==guest);
    puts("S37_PIF_ENV_COPY_RELEASE_FAILURE_RESTORE_OK");
    return 0;
}
