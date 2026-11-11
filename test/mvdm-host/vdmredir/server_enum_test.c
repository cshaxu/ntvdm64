/* Original handler/converters, controlled public NetAPI response. */
#include "xactsrv_wksta_minimal.h"
#include <stdio.h>
#include <string.h>
static unsigned mode,calls,frees,failures,checks;
static SERVER_INFO_100 info0={500,L"ALPHA"};
static SERVER_INFO_101 info1={500,L"BETA",6,1,SV_TYPE_SERVER,L"COMMENT"};
NET_API_STATUS WINAPI fixture_server_enum(LPCWSTR server,DWORD level,LPBYTE *out,DWORD bytes,
    LPDWORD read,LPDWORD total,DWORD type,LPCWSTR domain,LPDWORD resume)
{
    (void)bytes;(void)type;(void)domain;
    ++calls;if(server || resume || (level!=100 && level!=101))++failures;
    *out=level==100?(LPBYTE)&info0:(LPBYTE)&info1;
    *read=mode==4?0:1;*total=mode==3?3:*read;
    return mode==1?ERROR_ACCESS_DENIED:mode==3?ERROR_MORE_DATA:0;
}
NET_API_STATUS WINAPI fixture_net_free(LPVOID p)
{
    if(p!=&info0 && p!=&info1)++failures;
    ++frees;return 0;
}
static void check(int good,const char *name)
{++checks;if(!good)++failures;printf("%s %s\n",good?"PASS":"FAIL",name);}
static void run(unsigned m,WORD level,WORD capacity,BOOL private_transport)
{
    XS_PARAMETER_HEADER h={0};XS_NET_SERVER_ENUM_2 p={0};
    BYTE buffer[1026];memset(buffer,0xa5,sizeof(buffer));
    mode=m;calls=frees=0;
    h.Converter=0xbeef;
    h.ClientTransportName=private_transport?L"private":NULL;
    SmbPutUshort(&p.Level,level);SmbPutUshort(&p.BufLen,capacity);
    SmbPutUlong(&p.Buffer,(ULONG)(ULONG_PTR)(buffer+1));
    SmbPutUlong(&p.ServerType,SV_TYPE_ALL);
    NTSTATUS status=XsNetServerEnum2(&h,&p,NULL,NULL);
    check(status==0,"original handler NTSTATUS completion");
    if(m==1 || level>1 || private_transport)check(h.Converter==0,"early-failure response converter initialized");
    check(buffer[0]==0xa5 && buffer[capacity+1]==0xa5,"output capacity canaries");
    if(private_transport)check(h.Status==ERROR_CALL_NOT_IMPLEMENTED && !calls && !frees && !p.BufLen,"private browser remains unavailable");
    else if(level>1)check(h.Status==ERROR_INVALID_LEVEL && !calls && !frees && !p.BufLen,"invalid level skips provider");
    else {
        check(calls==1 && frees==1,"public provider and exactly-once buffer retirement");
        if(m==1)check(h.Status==ERROR_ACCESS_DENIED && !p.BufLen && buffer[1]==0xa5,"native error with allocated buffer cleaned");
        else if(capacity==1)check(h.Status==ERROR_MORE_DATA && !p.EntriesRead,"short buffer returns zero entries");
        else if(m==4)check(!h.Status && !p.EntriesRead && !p.TotalAvail && !p.BufLen,"empty enumeration");
        else {
            check(h.Status==(m==3?ERROR_MORE_DATA:0) && p.EntriesRead==1 && p.TotalAvail==(m==3?3:1),"converted count and continuation status");
            check(!strcmp((char*)buffer+1,level?"BETA":"ALPHA"),"original native-to-RAP name conversion");
            if(level)check(memcmp(buffer+1+16,"\x06\x01",2)==0,"original version fields");
        }
    }
}
int main(void)
{
    run(0,0,1024,FALSE);run(0,1,1024,FALSE);run(1,0,1024,FALSE);
    run(0,2,1024,FALSE);run(0,0,1,FALSE);run(3,0,1024,FALSE);
    run(4,0,1024,FALSE);run(0,0,1024,TRUE);
    printf("SERVER_ENUM checks=%u failures=%u\n",checks,failures);
    return failures?1:0;
}
