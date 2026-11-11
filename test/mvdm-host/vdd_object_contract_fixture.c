/* Actual nt_vdd/IOS/in/out objects; no guest, real ports or DLL unload. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <ios.h>
#include <dma.h>
#include <nt_vdd.h>
#include <nt_vddp.h>
#ifndef MVDM_VDD_EXPECT_HANDLER_BYTES
#define MVDM_VDD_EXPECT_HANDLER_BYTES 32
#endif
typedef char handlers_abi[(sizeof(VDD_IO_HANDLERS)==MVDM_VDD_EXPECT_HANDLER_BYTES)?1:-1];
typedef char other_abi[(sizeof(VDD_IO_PORTRANGE)==4 && sizeof(MEM_HOOK_DATA)==20 && sizeof(void *)==4)?1:-1];
extern half_word get_ms_bop_index(void (*)(void));
extern void free_MS_bop_index(half_word),ms_bop(void);
extern WORD GetVddAdapter(HANDLE),FreeVddAdapter(HANDLE);
extern HANDLE VddAdapter[NUMBER_SPARE_ADAPTERS],IrqLines[16];
static unsigned checks,failures,callbacks,read_calls,write_calls,bop_calls,dma_access,lock_calls,lock_depth;
static BYTE ah;
static CRITICAL_SECTION ica;
static struct {DMA_ADAPT state;BYTE guard[sizeof(DMA_CNTRL)+32];} dma_owned;
#define CHECK(x) do{++checks;if(!(x)){if(failures<12)fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);++failures;}}while(0)
IU8 c_getAH(void){return ah;}
IBOOL c_IOVirtualised(io_addr port,IU32 *value,LIN_ADDR offset,IU8 width)
{(void)port;(void)value;(void)offset;(void)width;return 0;}
void host_ica_lock(void){EnterCriticalSection(&ica);++lock_calls;++lock_depth;}
void host_ica_unlock(void){CHECK(lock_depth==1);--lock_depth;LeaveCriticalSection(&ica);}
DMA_ADAPT *dmaGetAdaptor(void){++dma_access;return &dma_owned.state;}
int dma_request(half_word port,char *buffer,word length)
{(void)port;(void)buffer;(void)length;fputs("UNEXPECTED real DMA transfer\n",stderr);exit(90);}
static VOID read_byte(WORD port,BYTE *value){(void)port;++callbacks;++read_calls;*value=0x56;}
static VOID write_byte(WORD port,BYTE value){(void)port;(void)value;++callbacks;++write_calls;}
static void bop_callback(void){++callbacks;++bop_calls;}
static VDD_IO_HANDLERS handlers={read_byte,NULL,NULL,NULL,write_byte,NULL,NULL,NULL};
static void reset_io(void){io_init();memset(VddAdapter,0,sizeof(VddAdapter));}
static void controls(void)
{
    HANDLE owner=GetModuleHandle(NULL);unsigned i,round,before;WORD reserved[3];BYTE value;VDD_DMA_INFO info;
    VDD_IO_PORTRANGE range={0x2e0,0x2e1};VDD_IO_HANDLERS invalid;
    for(round=0;round<8;++round){
        for(i=0;i<10;++i)CHECK(get_ms_bop_index(bop_callback)==i);
        CHECK(get_ms_bop_index(bop_callback)==0xff);before=bop_calls;
        for(i=0;i<256;++i){ah=(BYTE)i;ms_bop();}
        CHECK(bop_calls==before+10);
        for(i=0;i<10;++i)free_MS_bop_index((half_word)i);
        before=bop_calls;for(i=0;i<256;++i){ah=(BYTE)i;ms_bop();}CHECK(bop_calls==before);
    }
    reset_io();
    for(i=0;i<NUMBER_SPARE_ADAPTERS;++i)CHECK(GetVddAdapter((HANDLE)(UINT_PTR)(0x1000+i))==SPARE_ADAPTER1+i);
    SetLastError(0);CHECK(GetVddAdapter((HANDLE)0x2000)==0 && GetLastError()==ERROR_OUTOFMEMORY);
    SetLastError(0);CHECK(GetVddAdapter((HANDLE)0x1000)==0 && GetLastError()==ERROR_ALREADY_EXISTS);
    for(i=0;i<NUMBER_SPARE_ADAPTERS;++i){(void)FreeVddAdapter((HANDLE)(UINT_PTR)(0x1000+i));CHECK(VddAdapter[i]==NULL);}
    CHECK(VDDInstallIOHook(owner,1,&range,&handlers));
    before=callbacks;inb(range.First,&value);outb(range.Last,0x37);
    CHECK(value==0x56 && callbacks==before+2);
    SetLastError(0);CHECK(!VDDInstallIOHook(owner,1,&range,&handlers) && GetLastError()==ERROR_ALREADY_EXISTS);
    invalid=handlers;invalid.inb_handler=NULL;SetLastError(0);
    CHECK(!VDDInstallIOHook((HANDLE)0x2000,1,&range,&invalid) && GetLastError()==ERROR_INVALID_ADDRESS);
    /* Public uninstall is tested separately against its full postcondition.
     * Explicit fixture reset here is not evidence that uninstall succeeded. */
    reset_io();
    for(i=0;i<16;++i){
        WORD got=VDDReserveIrqLine(owner,(WORD)i);
        if(i==11 || i==12 || i==15){CHECK(got==i);CHECK(VDDReleaseIrqLine(owner,(WORD)i));}
        else CHECK(got==0xffff);
    }
    for(i=0;i<3;++i)reserved[i]=VDDReserveIrqLine(owner,0xffff);
    CHECK(reserved[0]==15 && reserved[1]==12 && reserved[2]==11);
    SetLastError(0);CHECK(VDDReserveIrqLine(owner,0xffff)==0xffff && GetLastError()==ERROR_INVALID_PARAMETER);
    SetLastError(0);CHECK(!VDDReleaseIrqLine((HANDLE)0x2000,15) && GetLastError()==ERROR_INVALID_PARAMETER);
    for(i=0;i<3;++i)CHECK(VDDReleaseIrqLine(owner,reserved[i]));
    SetLastError(0);CHECK(VDDReserveIrqLine(NULL,11)==0xffff && GetLastError()==ERROR_INVALID_PARAMETER);
    CHECK(VDDReserveIrqLine(owner,16)==0xffff);CHECK(!VDDReleaseIrqLine(NULL,11));CHECK(!VDDReleaseIrqLine(owner,16));
    memset(&dma_owned,0,sizeof(dma_owned));memset(dma_owned.guard,0x5a,sizeof(dma_owned.guard));
    for(i=0;i<8;++i){
        VDD_DMA_INFO returned;memset(&info,0,sizeof(info));info.addr=(WORD)(0x1230+i);info.count=(WORD)(0x3400+i);info.page=(WORD)(0x50+i);info.status=0x6a;
        CHECK(VDDSetDMA(owner,(WORD)i,VDD_DMA_ALL,&info));CHECK(VDDQueryDMA(owner,(WORD)i,&returned));
        CHECK(returned.addr==info.addr && returned.count==info.count && returned.page==info.page && returned.status==info.status);
    }
    for(i=0;i<sizeof(dma_owned.guard);++i)CHECK(dma_owned.guard[i]==0x5a);
    before=dma_access;SetLastError(0);CHECK(!VDDQueryDMA(owner,9,&info) && GetLastError()==ERROR_INVALID_ADDRESS);
    CHECK(!VDDSetDMA(owner,0xffff,VDD_DMA_ALL,&info));CHECK(!VDDRequestDMA(owner,9,NULL,0));CHECK(dma_access==before);
}
static void deinstall_contract(void)
{
    HANDLE owner=GetModuleHandle(NULL);unsigned index,i,before_read,before_write;BYTE value;
    VDD_IO_PORTRANGE range={0x2e0,0x2e0};
    for(index=0;index<NUMBER_SPARE_ADAPTERS;++index){
        reset_io();
        for(i=0;i<index;++i)CHECK(GetVddAdapter((HANDLE)(UINT_PTR)(0x1000+i))==SPARE_ADAPTER1+i);
        CHECK(VDDInstallIOHook(owner,1,&range,&handlers));inb(range.First,&value);CHECK(value==0x56);
        before_read=read_calls;before_write=write_calls;VDDDeInstallIOHook(owner,1,&range);
        inb(range.First,&value);outb(range.First,0x39);
        printf("VDD_DEINSTALL index=%u adapter=%u value=%02x read_calls=%u write_calls=%u owner_released=%d\n",index,SPARE_ADAPTER1+index,value,read_calls-before_read,write_calls-before_write,VddAdapter[index]==NULL);
        CHECK(value==0xff && read_calls==before_read);CHECK(write_calls==before_write);CHECK(VddAdapter[index]==NULL);
    }
    reset_io();
}
static void dma_boundary(void)
{
    HANDLE owner=GetModuleHandle(NULL);VDD_DMA_INFO info;DWORD result;unsigned before;BYTE saved[sizeof(dma_owned.guard)];
    memset(&dma_owned,0,sizeof(dma_owned));memset(dma_owned.guard,0x5a,sizeof(dma_owned.guard));memcpy(saved,dma_owned.guard,sizeof(saved));
    memset(&info,0,sizeof(info));info.addr=0x1234;info.count=0x5678;info.page=0x9a;info.status=0xbc;
    before=dma_access;SetLastError(0);result=VDDRequestDMA(owner,8,NULL,0);
    printf("VDD_DMA_BOUNDARY operation=request result=%lu accesses=%u\n",result,dma_access-before);
    CHECK(result==0 && GetLastError()==ERROR_INVALID_ADDRESS);CHECK(dma_access==before);
    before=dma_access;SetLastError(0);result=VDDQueryDMA(owner,8,&info);
    printf("VDD_DMA_BOUNDARY operation=query result=%lu accesses=%u\n",result,dma_access-before);
    CHECK(result==FALSE && GetLastError()==ERROR_INVALID_ADDRESS);CHECK(dma_access==before);
    /* Query may copy guard bytes into info. Reset the write payload so that
     * an out-of-bounds Set cannot appear harmless by writing them back. */
    memset(&info,0,sizeof(info));info.addr=0x1234;info.count=0x5678;info.page=0x9a;info.status=0xbc;
    before=dma_access;SetLastError(0);result=VDDSetDMA(owner,8,VDD_DMA_ALL,&info);
    printf("VDD_DMA_BOUNDARY operation=set result=%lu accesses=%u guard_changed=%d\n",result,dma_access-before,memcmp(saved,dma_owned.guard,sizeof(saved))!=0);
    CHECK(result==FALSE && GetLastError()==ERROR_INVALID_ADDRESS);CHECK(dma_access==before);CHECK(memcmp(saved,dma_owned.guard,sizeof(saved))==0);
}
int main(int argc,char **argv)
{
    const char *mode=argc>1?argv[1]:"controls";InitializeCriticalSection(&ica);
    if(strcmp(mode,"--deinstall-contract")==0)deinstall_contract();else if(strcmp(mode,"--dma-boundary")==0)dma_boundary();else controls();
    CHECK(lock_depth==0);DeleteCriticalSection(&ica);
    printf("VDD_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u locks=%u dma_accesses=%u mode=%s\n",checks,failures,callbacks,lock_calls,dma_access,mode);
    return failures?1:0;
}
