/* Actual nt_nls.obj and the production resource object, using real USER
 * string lookup and system formatting. No dialog or exception stub. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "host_rrr.h"
#include "nt_uis.h"
#ifndef MVDM_NLS_EXPECT_CAPACITY
#define MVDM_NLS_EXPECT_CAPACITY MAX_PATH
#endif
typedef char nls_capacity_abi[(MVDM_NLS_EXPECT_CAPACITY==MAX_PATH && sizeof(void *)==4)?1:-1];
extern void nls_init(void),host_nls_get_msg(int,char *,int);
extern char szDoomMsg[MAX_PATH],szSysErrMsg[MAX_PATH];
extern char szHideMouseMenuStr[64],szDisplayMouseMenuStr[64];
static unsigned checks,failures;
#define CHECK(x) do {++checks;if(!(x)){++failures;fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
int main(void)
{
    char expected[MAX_PATH];unsigned capacity,i;
    static const int ids[]={EHS_SYSTEM_ERROR,SM_HIDE_MOUSE,SM_DISPLAY_MOUSE};
    nls_init();
    CHECK(strcmp(szSysErrMsg,"NTVDM has encountered a System Error")==0);
    CHECK(strcmp(szHideMouseMenuStr,"&Hide Mouse Pointer")==0);
    CHECK(strcmp(szDisplayMouseMenuStr,"&Display Mouse Pointer")==0);
    memset(expected,0,sizeof(expected));
    CHECK(FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,0,ERROR_NOT_ENOUGH_MEMORY,0,expected,sizeof(expected),0)>0);
    CHECK(strcmp(expected,szDoomMsg)==0);
    for(i=0;i<sizeof(ids)/sizeof(ids[0]);++i)for(capacity=1;capacity<=MAX_PATH;++capacity){
        unsigned char buffer[MAX_PATH+2];char oracle[MAX_PATH];
        memset(buffer,0xa5,sizeof(buffer));memset(oracle,0,sizeof(oracle));
        CHECK(LoadStringA(GetModuleHandleA(0),(UINT)ids[i],oracle,(int)capacity)>=0);
        host_nls_get_msg(ids[i],(char *)buffer+1,(int)capacity);
        CHECK(buffer[0]==0xa5 && buffer[capacity+1]==0xa5);
        CHECK(buffer[capacity]==0 || strlen((char *)buffer+1)<capacity);
        CHECK(strcmp((char *)buffer+1,capacity==1?"":oracle)==0);
    }
    for(capacity=1;capacity<=MAX_PATH;++capacity){
        unsigned char buffer[MAX_PATH+2];size_t n=strlen(szDoomMsg);
        if(n>=capacity)n=capacity-1;
        memset(buffer,0xa5,sizeof(buffer));host_nls_get_msg(0xfffe,(char *)buffer+1,(int)capacity);
        CHECK(buffer[0]==0xa5 && buffer[capacity+1]==0xa5);
        CHECK(buffer[n+1]==0 && memcmp(buffer+1,szDoomMsg,n)==0);
    }
    printf("NLS_OBJECT_CONTRACT checks=%u failures=%u callbacks=0\n",checks,failures);
    return failures?1:0;
}
