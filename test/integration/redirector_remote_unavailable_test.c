/* Exercise the actual product provider object, not a substitute transport. */
#include <windows.h>
#include <stdio.h>
#include <lm.h>
#include "vdmredir-dll/include/remote_browser.h"
#include <string.h>
DWORD __cdecl RxpTransactSmb(void *,void *,void *,DWORD,void *,DWORD,void *,DWORD,void *,DWORD *,BOOL);
DWORD __cdecl RxNetUserPasswordSet(void *,void *,void *,void *);
DWORD __cdecl GetLanmanSessionKey(void *,void *);

int main(void)
{
    BYTE send[64],parameters[64],data[64],key[16],header[128],args[128];
    memset(send,0x5a,sizeof(send));memset(parameters,0xa5,sizeof(parameters));
    memset(data,0xa5,sizeof(data));memset(key,0xa5,sizeof(key));
    memset(header,0xa5,sizeof(header));memset(args,0xa5,sizeof(args));
    for(unsigned i=0;i<1000;++i) {
        DWORD length=sizeof(data);
        if(RxpTransactSmb(L"\\\\NO-RAP-SERVER",NULL,send,sizeof(send),NULL,0,
            parameters,sizeof(parameters),data,&length,i&1)!=ERROR_CALL_NOT_IMPLEMENTED ||
            length!=sizeof(data))return 1;
        if(RxNetUserPasswordSet(NULL,NULL,NULL,NULL)!=ERROR_CALL_NOT_IMPLEMENTED)return 2;
        if(GetLanmanSessionKey(NULL,key)!=ERROR_CALL_NOT_IMPLEMENTED)return 3;
        if(I_BrowserServerEnumForXactsrv(NULL,NULL,100,0,header,sizeof(header),0,
            (LPDWORD)args,(LPDWORD)(args+4),0,NULL,NULL,(PWORD)(args+8))!=ERROR_CALL_NOT_IMPLEMENTED)return 4;
    }
    for(unsigned i=0;i<64;++i)if(send[i]!=0x5a || parameters[i]!=0xa5 || data[i]!=0xa5)return 5;
    for(unsigned i=0;i<16;++i)if(key[i]!=0xa5)return 6;
    for(unsigned i=0;i<128;++i)if(header[i]!=0xa5 || args[i]!=0xa5)return 7;
    puts("PASS actual RAP provider object: 4000 failures, exact status domains, outputs unchanged");
    LPBYTE native=NULL;DWORD read=0,total=0;
    DWORD status=NetServerEnum(NULL,100,&native,1024,&read,&total,SV_TYPE_ALL,NULL,NULL);
    if(native)NetApiBufferFree(native);
    printf("LOCAL_ENUM_STATUS=%lu\n",status);
    return 0;
}
