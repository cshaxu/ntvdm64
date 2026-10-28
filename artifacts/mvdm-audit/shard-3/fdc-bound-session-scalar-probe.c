#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/session/session.h"
int mvdm_command_guest_state_set_scs_scalars(uint16_t,uint16_t,uint32_t,uint32_t,uint16_t,uint16_t,uint16_t,uint16_t);
int mvdm_command_guest_state_add_fd_access(uint16_t);
typedef struct { unsigned char bytes[4096]; } mem;
static int rd(void *c,uint32_t a,uint8_t *p,uint32_t n){if(a>4096 || n>4096-a)return 0;memcpy(p,((mem*)c)->bytes+a,n);return 1;}
static int wr(void *c,uint32_t a,const uint8_t *p,uint32_t n){if(a>4096 || n>4096-a)return 0;memcpy(((mem*)c)->bytes+a,p,n);return 1;}
static DWORD WINAPI run(void *p){session *s=p; if(!session_thread_bind(s))return 1; int result=mvdm_command_guest_state_add_fd_access(1); session_thread_unbind(s);return result?2:0;}
int main(void){session s;mem m={0};session_initialize(&s,1); if(!session_activate(&s)||!session_guest_memory_begin(&s,&m,rd,wr)||!session_thread_bind(&s)||!mvdm_command_guest_state_set_scs_scalars(0,0x100,32,1,0,0x200,0,0x202))return 10; if(!mvdm_command_guest_state_add_fd_access(1) || m.bytes[0x202]!=1) return 12; HANDLE h=CreateThread(0,0,run,&s,0,0); if(!h)return 11; WaitForSingleObject(h,INFINITE);DWORD c=99;GetExitCodeThread(h,&c);CloseHandle(h); printf("bound child update=%lu parent_count=%u\n",(unsigned long)c,m.bytes[0x202]);session_thread_unbind(&s);session_guest_memory_end(&s);session_dispose(&s);return c || m.bytes[0x202]!=1;}
int IsFirstVDM; unsigned char cmdVDMEnvBlk[1024]; void cmdCheckForPIF(void *p){(void)p; ExitProcess(98);}
