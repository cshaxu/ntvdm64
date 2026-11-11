/* Actual original nt_umb.obj; owned lists and recording SAS/XMS boundaries. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <umb.h>

#ifndef MVDM_UMB_EXPECT_NODE_BYTES
#define MVDM_UMB_EXPECT_NODE_BYTES 20
#endif
typedef char node_abi[(sizeof(UMBNODE)==MVDM_UMB_EXPECT_NODE_BYTES)?1:-1];
typedef char address_abi[(sizeof(sys_addr)==4 && sizeof(void *)==4)?1:-1];
extern PUMBNODE UMBList;
extern PUMBNODE SpliceUMB(PUMBNODE,DWORD,DWORD,WORD);
extern VOID UMBNotify(unsigned char);
static unsigned checks,failures,callbacks,notifies;
static sys_addr last_low,last_high;
static half_word last_type;
static PVOID notified_address;
static DWORD notified_size;
#define CHECK(x) do{++checks;if(!(x)){if(failures<8)fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);++failures;}}while(0)

/* These seams record requests, not their successful mapping or guest effect. */
void c_sas_connect_memory(sys_addr low,sys_addr high,half_word type)
{++callbacks;last_low=low;last_high=high;last_type=type;}
VOID xmsReleaseUMBNotify(PVOID address,DWORD size)
{++notifies;notified_address=address;notified_size=size;}
static void dispose_list(void)
{PUMBNODE p=UMBList;while(p){PUMBNODE next=p->Next;free(p);p=next;}UMBList=NULL;}
static PUMBNODE node(DWORD base,DWORD size,WORD owner)
{
    PUMBNODE p=(PUMBNODE)calloc(1,sizeof(*p));if(!p)exit(90);
    p->Base=base;p->Size=size;p->Owner=owner;return p;
}
static void single(WORD owner)
{dispose_list();UMBList=node(0xc8000,0x10000,owner);}
static void split_cases(void)
{
    DWORD offset,size;WORD owners[]={UMB_OWNER_EMM,UMB_OWNER_VDD,UMB_OWNER_ROM};unsigned o;
    for(o=0;o<3;++o)for(offset=0;offset<0x10000;offset+=0x1000)
        for(size=0x1000;size<=0x10000-offset;size+=0x1000){
            PUMBNODE selected,p;DWORD cursor=0xc8000,total=0;unsigned owned=0,count=0;
            single(UMB_OWNER_NONE);selected=SpliceUMB(UMBList,0xc8000+offset,size,owners[o]);
            CHECK(selected!=NULL);CHECK(selected->Base==0xc8000+offset && selected->Size==size && selected->Owner==owners[o]);
            for(p=UMBList;p;p=p->Next){
                CHECK(++count<=3);CHECK(p->Base==cursor && p->Size>0);cursor+=p->Size;total+=p->Size;
                if(p==selected)++owned;else CHECK(p->Owner==UMB_OWNER_NONE);
            }
            CHECK(total==0x10000 && cursor==0xd8000 && owned==1);
        }
}
static void reservation_cases(void)
{
    PVOID address;DWORD size;unsigned before;
    single(UMB_OWNER_NONE);address=NULL;size=0x2000;
    CHECK(ReserveUMB(UMB_OWNER_EMM,&address,&size));CHECK(address==(PVOID)0xc8000 && size==0x2000);
    CHECK(UMBList->Owner==UMB_OWNER_EMM && UMBList->Size==size);
    before=callbacks;CHECK(ReleaseUMB(UMB_OWNER_EMM,address,size));
    CHECK(callbacks==before+1 && last_low==0xc8000 && last_high==0xc9fff && last_type==SAS_RAM);
    CHECK(notifies==1 && notified_address==address && notified_size==size && UMBList->Owner==UMB_OWNER_RAM);
    address=NULL;size=0;CHECK(ReserveUMB(UMB_OWNER_XMS,&address,&size));
    CHECK(address==(PVOID)0xc8000 && size==0x2000 && UMBList->Owner==UMB_OWNER_XMS);
    SetLastError(0);CHECK(!ReleaseUMB(UMB_OWNER_EMM,address,size) && GetLastError()==ERROR_INVALID_ADDRESS);
    single(UMB_OWNER_NONE);address=(PVOID)0xca000;size=0x3000;before=callbacks;
    CHECK(ReserveUMB(UMB_OWNER_VDD,&address,&size));
    CHECK(callbacks==before+1 && last_low==0xca000 && last_high==0xccfff && last_type==SAS_VDD);
    before=callbacks;CHECK(VDDCommitUMB(address,0x1000));CHECK(last_low==0xca000 && last_high==0xcafff && last_type==SAS_RAM);
    CHECK(VDDDeCommitUMB((PVOID)0xcb000,0x2000));CHECK(last_low==0xcb000 && last_high==0xccfff && last_type==SAS_VDD);
    CHECK(callbacks==before+2);CHECK(ReleaseUMB(UMB_OWNER_VDD,address,size));
    CHECK(notifies==2 && notified_address==address && notified_size==size);
    single(UMB_OWNER_NONE);before=callbacks;address=NULL;size=0;
    CHECK(ReserveUMB(UMB_OWNER_RAM,&address,&size));CHECK(UMBList->Owner==UMB_OWNER_RAM && callbacks==before+1);
    CHECK(last_low==0xc8000 && last_high==0xd7fff && last_type==SAS_RAM);
    single(UMB_OWNER_ROM);address=(PVOID)0xc8000;size=0x1000;before=callbacks;
    CHECK(!ReserveUMB(UMB_OWNER_NONE,&address,&size));CHECK(callbacks==before && UMBList->Owner==UMB_OWNER_ROM);
    SetLastError(0);CHECK(!ReserveUMB(UMB_OWNER_EMM,&address,&size) && GetLastError()==ERROR_OUTOFMEMORY);
    SetLastError(0);CHECK(!ReserveUMB(0,&address,&size) && GetLastError()==ERROR_INVALID_ADDRESS);
    single(UMB_OWNER_VDD);before=callbacks;SetLastError(0);
    CHECK(!VDDCommitUMB((PVOID)0xd8000,0x1000) && GetLastError()==ERROR_INVALID_ADDRESS);
    SetLastError(0);CHECK(!VDDDeCommitUMB((PVOID)0xc0000,0x1000) && GetLastError()==ERROR_INVALID_ADDRESS);
    CHECK(callbacks==before);
}
static void initialized_list(void)
{
    PUMBNODE p;DWORD cursor=UMB_BASE_ADDRESS,total=0;unsigned count=0;
    dispose_list();(void)InitUMBList(); /* original non-MONITOR success has no return statement */
    for(p=UMBList;p;p=p->Next){
        CHECK(++count<=4);CHECK(p->Base==cursor && p->Size>0);cursor+=p->Size;total+=p->Size;
        CHECK(p->Owner==UMB_OWNER_NONE || p->Owner==UMB_OWNER_ROM);
    }
    CHECK(count==3 && total==UMB_MAX_OFFSET && cursor==0x100000);
}
static void strict_bounds(void)
{
    DWORD addresses[]={0xc7000,0xc6000,0xffff0000};DWORD sizes[]={0x2000,0x2000,0xe0000};unsigned i,kind;
    for(kind=0;kind<2;++kind)for(i=0;i<3;++i){
        unsigned before=callbacks;BOOL accepted;
        single(UMB_OWNER_VDD);SetLastError(0);
        accepted=kind?VDDDeCommitUMB((PVOID)addresses[i],sizes[i]):VDDCommitUMB((PVOID)addresses[i],sizes[i]);
        printf("UMB_BOUNDARY kind=%s address=%08lx size=%08lx accepted=%d calls=%u low=%08lx high=%08lx\n",
            kind?"decommit":"commit",addresses[i],sizes[i],accepted,callbacks-before,(unsigned long)last_low,(unsigned long)last_high);
        CHECK(!accepted);CHECK(callbacks==before);
    }
}
int main(int argc,char **argv)
{
    unsigned i,before;(void)argv;
    if(argc>1)strict_bounds();else{
        initialized_list();split_cases();reservation_cases();before=callbacks;
        for(i=0;i<256;++i)UMBNotify((unsigned char)i);
        CHECK(callbacks==before);CHECK(GetUMBForEMM());
    }
    dispose_list();
    printf("UMB_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u notifies=%u boundary=%d\n",checks,failures,callbacks,notifies,argc>1);
    return failures?1:0;
}
