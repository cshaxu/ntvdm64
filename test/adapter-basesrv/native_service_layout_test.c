#include <base_server.h>
#include "basesrv.h"
#include "common/base_srv/base_values.h"
#include <stdio.h>
#include <stdint.h>

/* Local CSR layouts follow the consumer ABI; copied values do not. Numeric
 * identities in HANDLE-shaped fields are not actual kernel HANDLEs.
 * CONSOLERECORD includes the existing ReenterCompletionPending BOOL.
 * XP basemsg.h includes SxS process data: BASE_API_MSG is 288/192 bytes.
 * XP ntcsrsrv.h removes ShutDownStatus, ServerId and per-thread DLL data. */
int main(void)
{
    static const uint32_t ids[]={2u,0x80000002u,0xfffffffeu};
    unsigned i;
    size_t native_bits=sizeof(void *)*8;
    if(sizeof(ULONG)!=4 || sizeof(DWORD)!=4 ||
       sizeof(broker_vdm_check_values)!=36 ||
       sizeof(broker_vdm_update_values)!=16 ||
       sizeof(broker_vdm_get_values)!=32)return 1;
    if(sizeof(CLIENT_ID)!=2*sizeof(void *) ||
       sizeof(PORT_MESSAGE)!=(native_bits==64?40u:24u) ||
       sizeof(CSR_THREAD)!=(native_bits==64?96u:56u) ||
       sizeof(CSR_PROCESS)!=(native_bits==64?184u:112u) ||
       sizeof(BASE_API_MSG)!=(native_bits==64?288u:192u) ||
       sizeof(DOSRECORD)!=(native_bits==64?40u:24u) ||
       sizeof(CONSOLERECORD)!=(native_bits==64?80u:52u) ||
       sizeof(WOWRECORD)!=(native_bits==64?40u:24u)){
        fprintf(stderr,"CSR layout mismatch: bits=%zu client=%zu port=%zu thread=%zu process=%zu message=%zu dos=%zu console=%zu wow=%zu\n",
            native_bits,sizeof(CLIENT_ID),sizeof(PORT_MESSAGE),sizeof(CSR_THREAD),
            sizeof(CSR_PROCESS),sizeof(BASE_API_MSG),sizeof(DOSRECORD),
            sizeof(CONSOLERECORD),sizeof(WOWRECORD));
        return 2;
    }
    for(i=0;i<sizeof(ids)/sizeof(ids[0]);++i){
        HANDLE receipt=(HANDLE)(ULONG_PTR)ids[i];
        HANDLE tagged=(HANDLE)((ULONG_PTR)receipt|1u);
        HANDLE restored=(HANDLE)((ULONG_PTR)tagged&~(ULONG_PTR)1u);
        CLIENT_ID client={(HANDLE)(ULONG_PTR)ids[i],(HANDLE)(ULONG_PTR)ids[i]};
        if(restored!=receipt || (DWORD)(ULONG_PTR)restored!=ids[i] ||
           (DWORD)(ULONG_PTR)client.UniqueProcess!=ids[i] ||
           (DWORD)(ULONG_PTR)client.UniqueThread!=ids[i])return 3;
    }
    printf("PASS %zu-bit native CSR layouts; fixed wire=36/16/32; high numeric PID/TID/receipt and parent tag identities\n",native_bits);
    return 0;
}
