#include "staging.h"
#include <string.h>
#include <stdint.h>
#define STAGE_MAGIC0 UINT64_C(0x534e544953363157)
#define STAGE_MAGIC1 UINT64_C(0x3130474154534550)
#define IMPORT_LIMIT 1024u
#define STAGE_LIMIT (1024u*1024u)
typedef struct stage_header {
    uint64_t magic0,magic1;
    uint32_t version,allocation_bytes,packet_bytes,descriptor_offset;
    uint64_t allocation_base,image_base;
    IMAGE_DATA_DIRECTORY imports,bound,iat;
    DWORD checksum,reserved;
} stage_header;
_Static_assert(sizeof(stage_header)==80,"stage header must remain width independent");
typedef struct image_info {BYTE *base;IMAGE_DOS_HEADER dos;IMAGE_NT_HEADERS nt;} image_info;
static SIZE_T align_size(SIZE_T n,SIZE_T alignment){return (n+alignment-1)&~(alignment-1);}
static DWORD read_memory(HANDLE process,const void *address,void *out,SIZE_T bytes)
{
    SIZE_T read=0;
    if(!ReadProcessMemory(process,address,out,bytes,&read))return GetLastError();
    return read==bytes?ERROR_SUCCESS:ERROR_PARTIAL_COPY;
}
static DWORD read_image(HANDLE process,BYTE *base,image_info *image)
{
    DWORD error;ZeroMemory(image,sizeof(*image));image->base=base;
    error=read_memory(process,base,&image->dos,sizeof(image->dos));if(error)return error;
    if(image->dos.e_magic!=IMAGE_DOS_SIGNATURE || image->dos.e_lfanew<64 || image->dos.e_lfanew>1024*1024)
        return ERROR_BAD_EXE_FORMAT;
    error=read_memory(process,base+image->dos.e_lfanew,&image->nt,sizeof(image->nt));if(error)return error;
    if(image->nt.Signature!=IMAGE_NT_SIGNATURE ||
       image->nt.FileHeader.Machine!=(sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386) ||
       image->nt.OptionalHeader.Magic!=(sizeof(void *)==8?IMAGE_NT_OPTIONAL_HDR64_MAGIC:IMAGE_NT_OPTIONAL_HDR32_MAGIC) ||
       image->nt.FileHeader.SizeOfOptionalHeader<sizeof(IMAGE_OPTIONAL_HEADER) ||
       image->nt.OptionalHeader.NumberOfRvaAndSizes<=IMAGE_DIRECTORY_ENTRY_IAT ||
       image->nt.OptionalHeader.SizeOfImage<(DWORD)image->dos.e_lfanew+sizeof(image->nt))return ERROR_BAD_EXE_FORMAT;
    return ERROR_SUCCESS;
}
static uintptr_t address_limit(void)
{
#ifdef _WIN64
    SYSTEM_INFO info;GetSystemInfo(&info);return (uintptr_t)info.lpMaximumApplicationAddress;
#else
    /* WOW64 large-address-aware targets may map images above 2 GiB. */
    return UINTPTR_MAX-0x10000u;
#endif
}
static DWORD find_image(HANDLE process,image_info *image)
{
    uintptr_t cursor=0,limit=address_limit();MEMORY_BASIC_INFORMATION region;
    for(unsigned visited=0;visited<65536 && cursor<limit;++visited){
        if(!VirtualQueryEx(process,(void *)cursor,&region,sizeof(region)))return GetLastError();
        if(region.State==MEM_COMMIT && region.Type==MEM_IMAGE && region.BaseAddress==region.AllocationBase){
            DWORD error=read_image(process,region.AllocationBase,image);
            if(!error && !(image->nt.FileHeader.Characteristics&IMAGE_FILE_DLL))return ERROR_SUCCESS;
            /* A query-only handle can enumerate regions but cannot read their
             * PE headers. Preserve its access error rather than reporting an
             * unsupported executable after exhausting the address space. */
            if(error && error!=ERROR_BAD_EXE_FORMAT)return error;
        }
        uintptr_t next=(uintptr_t)region.BaseAddress+region.RegionSize;
        if(next<=cursor)break;
        cursor=next;
    }
    return ERROR_BAD_EXE_FORMAT;
}
static DWORD current_stage(HANDLE process,const image_info *image,stage_header *stage,BOOL *found)
{
    DWORD rva=image->nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    *found=FALSE;ZeroMemory(stage,sizeof(*stage));
    if(rva<sizeof(*stage))return ERROR_SUCCESS;
    if(read_memory(process,image->base+rva-sizeof(*stage),stage,sizeof(*stage)))return ERROR_SUCCESS;
    if(stage->magic0!=STAGE_MAGIC0 || stage->magic1!=STAGE_MAGIC1)return ERROR_SUCCESS;
    *found=TRUE;
    if(stage->version!=1 || stage->reserved>1 || stage->image_base!=(uint64_t)(uintptr_t)image->base ||
       stage->allocation_base>(uint64_t)UINTPTR_MAX || stage->allocation_bytes>STAGE_LIMIT ||
       stage->descriptor_offset<sizeof(*stage) || stage->descriptor_offset>stage->allocation_bytes ||
       !stage->packet_bytes || stage->packet_bytes>65536 ||
       stage->packet_bytes>stage->descriptor_offset-sizeof(*stage) ||
       stage->allocation_base+(uint64_t)stage->descriptor_offset!=(uint64_t)(uintptr_t)image->base+rva ||
       stage->allocation_base+stage->allocation_bytes<stage->allocation_base)return ERROR_INVALID_DATA;
    return ERROR_SUCCESS;
}
static BYTE *allocate_near(HANDLE process,const image_info *image,SIZE_T bytes)
{
    uintptr_t base=(uintptr_t)image->base,limit=address_limit(),cursor;
    MEMORY_BASIC_INFORMATION region;SYSTEM_INFO info;GetSystemInfo(&info);
    if(image->nt.OptionalHeader.SizeOfImage>limit || base>limit-image->nt.OptionalHeader.SizeOfImage){
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;
    }
    cursor=align_size(base+image->nt.OptionalHeader.SizeOfImage,info.dwAllocationGranularity);
    if(limit-base>UINT32_MAX)limit=base+UINT32_MAX;
    for(unsigned visited=0;visited<65536 && cursor<limit && bytes<=limit-cursor;++visited){
        if(!VirtualQueryEx(process,(void *)cursor,&region,sizeof(region)))return NULL;
        uintptr_t end=(uintptr_t)region.BaseAddress+region.RegionSize;
        if(end<=cursor)break;
        if(region.State==MEM_FREE && bytes<=end-cursor){
            BYTE *allocation=VirtualAllocEx(process,(void *)cursor,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
            if(allocation)return allocation;
        }
        if(end>limit)break;
        cursor=align_size(end,info.dwAllocationGranularity);
    }
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;
}
static DWORD write_header(HANDLE process,const image_info *image,const IMAGE_NT_HEADERS *nt)
{
    DWORD protect,unused,error=ERROR_SUCCESS;SIZE_T written=0;
    BYTE *address=image->base+image->dos.e_lfanew;
    if(!VirtualProtectEx(process,address,sizeof(*nt),PAGE_READWRITE,&protect))return GetLastError();
    if(!WriteProcessMemory(process,address,nt,sizeof(*nt),&written))error=GetLastError();
    else if(written!=sizeof(*nt))error=ERROR_PARTIAL_COPY;
    if(!VirtualProtectEx(process,address,sizeof(*nt),protect,&unused) && !error)error=GetLastError();
    return error;
}
/* An old image may omit DIRECTORY_ENTRY_IAT. Its original import section then
 * supplies the loader's fallback range. Moving descriptors outside that image
 * removes this fallback and can make Windows skip ALL imports. Describe the
 * original thunk destinations explicitly before moving the descriptors; never
 * relocate or rewrite the target's lookup arrays. Restore the original (empty)
 * directory along with the other metadata in stage_restore. */
static DWORD complete_iat(HANDLE child,const image_info *image,const IMAGE_IMPORT_DESCRIPTOR *imports,
                          unsigned count,IMAGE_DATA_DIRECTORY *iat)
{
    DWORD first=MAXDWORD,last=0;
    if(iat->VirtualAddress && iat->Size)return ERROR_SUCCESS;
    if(iat->VirtualAddress || iat->Size)return ERROR_BAD_EXE_FORMAT;
    for(unsigned i=0;i<count;++i){
        DWORD rva=imports[i].FirstThunk;BOOL terminated=FALSE;
        if(!rva)return ERROR_BAD_EXE_FORMAT;
        for(unsigned j=0;j<IMPORT_LIMIT;++j){
            uint64_t at=(uint64_t)rva+(uint64_t)j*sizeof(uintptr_t);uintptr_t thunk;
            if(at>image->nt.OptionalHeader.SizeOfImage || sizeof(thunk)>image->nt.OptionalHeader.SizeOfImage-at)
                return ERROR_BAD_EXE_FORMAT;
            DWORD error=read_memory(child,image->base+(SIZE_T)at,&thunk,sizeof(thunk));if(error)return error;
            if(!thunk){
                if(rva<first)first=rva;
                if(at+sizeof(thunk)>last)last=(DWORD)(at+sizeof(thunk));
                terminated=TRUE;break;
            }
        }
        if(!terminated)return ERROR_BAD_EXE_FORMAT;
    }
    if(count){iat->VirtualAddress=first;iat->Size=last-first;}
    return ERROR_SUCCESS;
}
static DWORD stage_process(HANDLE child,const char *dll,const void *packet,DWORD bytes)
{
    image_info image;stage_header previous,*stage;BOOL staged;DWORD error;unsigned old_count=0;
    IMAGE_IMPORT_DESCRIPTOR *old,*imports;BYTE *scratch=NULL,*local=NULL,*remote=NULL;
    IMAGE_NT_HEADERS next;SIZE_T header_offset,descriptor_offset,thunk_offset,name_offset,total,written=0;
    uintptr_t *thunks;SIZE_T dll_bytes;
    if(!child || !packet || !bytes || bytes>65536)return ERROR_INVALID_PARAMETER;
    dll_bytes=dll?strnlen(dll,4096)+1:0;if(dll_bytes>4096)return ERROR_FILENAME_EXCED_RANGE;
    error=find_image(child,&image);if(error)return error;
    error=current_stage(child,&image,&previous,&staged);if(error)return error;
    scratch=HeapAlloc(GetProcessHeap(),0,65536);if(!scratch)return ERROR_NOT_ENOUGH_MEMORY;
    old=(IMAGE_IMPORT_DESCRIPTOR *)scratch;next=image.nt;
    if(staged){
        if(previous.reserved==(dll?0u:1u) && previous.packet_bytes==bytes && !read_memory(child,(void *)(uintptr_t)previous.allocation_base,scratch,bytes) &&
           !memcmp(scratch,packet,bytes)){
            if(!dll)goto done;
            IMAGE_IMPORT_DESCRIPTOR descriptor;
            uintptr_t base=(uintptr_t)image.base;
            DWORD rva=image.nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
            error=read_memory(child,(void *)(base+rva),&descriptor,sizeof(descriptor));
            if(error)goto done;
            if(base>UINTPTR_MAX-descriptor.Name || dll_bytes-1>UINTPTR_MAX-base-descriptor.Name){error=ERROR_BAD_EXE_FORMAT;goto done;}
            error=read_memory(child,(void *)(base+descriptor.Name),scratch,dll_bytes);
            if(error)goto done;
            if(!memcmp(scratch,dll,dll_bytes))goto done;
        }
        next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]=previous.imports;
        next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]=previous.bound;
        next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT]=previous.iat;
        next.OptionalHeader.CheckSum=previous.checksum;
    }
    DWORD import_rva=next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(import_rva){
        for(;old_count<IMPORT_LIMIT;++old_count){
            /* Another startup installer may already have moved the imports
             * into a private allocation beyond SizeOfImage. Preserve that
             * valid table; RVA arithmetic and the bounded remote read, rather
             * than the original image extent, determine readability. */
            uint64_t at=(uint64_t)import_rva+(uint64_t)old_count*sizeof(*old);
            uintptr_t base=(uintptr_t)image.base;
            if(at>UINT32_MAX-(sizeof(*old)-1) || at>UINTPTR_MAX-base ||
               sizeof(*old)-1>UINTPTR_MAX-base-(uintptr_t)at){error=ERROR_BAD_EXE_FORMAT;goto done;}
            error=read_memory(child,(void *)(base+(uintptr_t)at),&old[old_count],sizeof(*old));if(error)goto done;
            if(!old[old_count].Name && !old[old_count].FirstThunk && !old[old_count].OriginalFirstThunk)break;
        }
        if(old_count==IMPORT_LIMIT){error=ERROR_BAD_EXE_FORMAT;goto done;}
    }
    header_offset=align_size(bytes,16);descriptor_offset=header_offset+sizeof(*stage);
    thunk_offset=align_size(descriptor_offset+(old_count+1+(dll?1:0))*sizeof(*imports),sizeof(uintptr_t));
    name_offset=thunk_offset+(dll?4*sizeof(uintptr_t):0);total=name_offset+dll_bytes;
    if(total>STAGE_LIMIT){error=ERROR_INSUFFICIENT_BUFFER;goto done;}
    local=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,total);if(!local){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    remote=allocate_near(child,&image,total);if(!remote){error=GetLastError();goto done;}
    memcpy(local,packet,bytes);stage=(stage_header *)(local+header_offset);
    stage->magic0=STAGE_MAGIC0;stage->magic1=STAGE_MAGIC1;stage->version=1;
    stage->reserved=dll?0u:1u;
    stage->allocation_bytes=(DWORD)total;stage->packet_bytes=bytes;stage->descriptor_offset=(DWORD)descriptor_offset;
    stage->allocation_base=(uint64_t)(uintptr_t)remote;stage->image_base=(uint64_t)(uintptr_t)image.base;
    stage->imports=next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    stage->bound=next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT];
    stage->iat=next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];stage->checksum=next.OptionalHeader.CheckSum;
    error=complete_iat(child,&image,old,old_count,&next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT]);
    if(error)goto done;
    DWORD rva=(DWORD)((uintptr_t)remote-(uintptr_t)image.base);
    imports=(IMAGE_IMPORT_DESCRIPTOR *)(local+descriptor_offset);
    memcpy(imports+(dll?1:0),old,old_count*sizeof(*old));
    if(dll){
        imports->OriginalFirstThunk=rva+(DWORD)thunk_offset;imports->FirstThunk=imports->OriginalFirstThunk+2*sizeof(uintptr_t);
        imports->Name=rva+(DWORD)name_offset;
        thunks=(uintptr_t *)(local+thunk_offset);thunks[0]=thunks[2]=((uintptr_t)1<<(sizeof(uintptr_t)*8-1))|1;
        memcpy(local+name_offset,dll,dll_bytes);
    }
    if(!WriteProcessMemory(child,remote,local,total,&written)){error=GetLastError();goto done;}
    if(written!=total){error=ERROR_PARTIAL_COPY;goto done;}
    next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress=rva+(DWORD)descriptor_offset;
    next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size=(old_count+1+(dll?1:0))*sizeof(*imports);
    next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress=0;
    next.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size=0;next.OptionalHeader.CheckSum=0;
    error=write_header(child,&image,&next);
    if(error){
        /* Creator still owns this suspended target and must roll it back if
         * any write/protection step failed. Never release a possibly live table. */
        remote=NULL;goto done;
    }
    remote=NULL;
    /* The creator still holds the target suspended. A superseded pure
     * payload table is no longer referenced after the new header is written. */
    if(staged && previous.reserved==1 &&
       !VirtualFreeEx(child,(void *)(uintptr_t)previous.allocation_base,0,MEM_RELEASE))error=GetLastError();
done:
    if(remote)VirtualFreeEx(child,remote,0,MEM_RELEASE);
    if(local)HeapFree(GetProcessHeap(),0,local);
    if(scratch)HeapFree(GetProcessHeap(),0,scratch);
    return error;
}
DWORD process_hook_stage_process(HANDLE child,const char *dll,const void *packet,DWORD bytes)
{return dll?stage_process(child,dll,packet,bytes):ERROR_INVALID_PARAMETER;}
DWORD process_hook_stage_payload(HANDLE child,const void *packet,DWORD bytes)
{return stage_process(child,NULL,packet,bytes);}
DWORD process_hook_stage_read(process_hook_payload_consumer consume,void *context,BOOL *found)
{
    image_info image;stage_header stage;BYTE *packet;DWORD error;
    if(!consume || !context || !found)return ERROR_INVALID_PARAMETER;
    *found=FALSE;
    error=read_image(GetCurrentProcess(),(BYTE *)GetModuleHandleW(NULL),&image);if(error)return error;
    error=current_stage(GetCurrentProcess(),&image,&stage,found);if(error || !*found)return error;
    packet=HeapAlloc(GetProcessHeap(),0,stage.packet_bytes);if(!packet)return ERROR_NOT_ENOUGH_MEMORY;
    error=read_memory(GetCurrentProcess(),(void *)(uintptr_t)stage.allocation_base,packet,stage.packet_bytes);
    if(!error)error=consume(packet,stage.packet_bytes,image.nt.FileHeader.Machine,context);
    HeapFree(GetProcessHeap(),0,packet);return error;
}
DWORD process_hook_stage_restore(void)
{
    image_info image;stage_header stage;BOOL found;DWORD error;
    error=read_image(GetCurrentProcess(),(BYTE *)GetModuleHandleW(NULL),&image);if(error)return error;
    error=current_stage(GetCurrentProcess(),&image,&stage,&found);if(error || !found)return error?error:ERROR_INVALID_DATA;
    image.nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]=stage.imports;
    image.nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]=stage.bound;
    image.nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT]=stage.iat;
    image.nt.OptionalHeader.CheckSum=stage.checksum;
    error=write_header(GetCurrentProcess(),&image,&image.nt);
    /* A payload-only receiver calls this after its own loader has finished;
     * unlike an imported DLL's DllMain, the descriptor table is no longer in
     * use by the loader. Keep the original DLL path's lifetime unchanged. */
    if(!error && stage.reserved==1 && !VirtualFree((void *)(uintptr_t)stage.allocation_base,0,MEM_RELEASE))return GetLastError();
    return error;
}
