/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* Experimental late patch, adapted from the project startup hook backend. */
#include "late_patch.h"
#include "lib-win32/process-hook/minhook/src/buffer.h"
#include "lib-win32/process-hook/minhook/src/trampoline.h"
#include <stdint.h>
#include <string.h>
DWORD process_hook_late_patch_prepare(process_hook_late_patch *p,void *slot,void *replacement)
{
    void *target=NULL;TRAMPOLINE t={0};
    if(!p || p->target || !slot || !replacement)return ERROR_INVALID_PARAMETER;
    memcpy(&target,slot,sizeof(target));
    if(!target || !IsExecutableAddress(target) || !IsExecutableAddress(replacement))return ERROR_INVALID_ADDRESS;
    t.pTarget=target;t.pDetour=replacement;t.pTrampoline=AllocateBuffer(target);
    if(!t.pTrampoline)return ERROR_NOT_ENOUGH_MEMORY;
    if(!CreateTrampolineFunction(&t)){FreeBuffer(t.pTrampoline);return ERROR_NOT_SUPPORTED;}
    p->target=target;p->trampoline=t.pTrampoline;p->slot=slot;p->above=t.patchAbove;
#ifdef _WIN64
    p->replacement=t.pRelay;
#else
    p->replacement=t.pDetour;
#endif
    BYTE *start=(BYTE *)target-(p->above?sizeof(JMP_REL):0);
    memcpy(p->backup,start,sizeof(JMP_REL)+(p->above?sizeof(JMP_REL_SHORT):0));
    return ERROR_SUCCESS;
}
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
static DWORD fail_protect_after=MAXDWORD;
void process_hook_late_patch_test_fail_protect_after(DWORD n){fail_protect_after=n;}
#endif
#define PROCESS_HOOK_LATE_PATCH_LIMIT 88u
DWORD process_hook_late_patch_install_batch(process_hook_late_guard *guard,process_hook_late_patch **items,DWORD count)
{
    process_hook_late_region regions[PROCESS_HOOK_LATE_PATCH_LIMIT];DWORD protection[PROCESS_HOOK_LATE_PATCH_LIMIT];
    SYSTEM_INFO info;DWORD error=ERROR_SUCCESS,protected_count=0,ignored=0;GetSystemInfo(&info);
    if(!guard || guard->threads || guard->count || guard->suspended || guard->snapshots || !items || !count || count>PROCESS_HOOK_LATE_PATCH_LIMIT)return ERROR_INVALID_PARAMETER;
    for(DWORD i=0;i<count;++i){
        process_hook_late_patch *p=items[i];
        if(!p || !p->target || !p->trampoline || !p->slot || p->installed || p->guard.threads || p->guard.snapshots)return ERROR_INVALID_STATE;
        BYTE *start=(BYTE *)p->target-(p->above?sizeof(JMP_REL):0);
        SIZE_T length=sizeof(JMP_REL)+(p->above?sizeof(JMP_REL_SHORT):0);
        uintptr_t low=(uintptr_t)start;
        if(!info.dwPageSize || low>UINTPTR_MAX-(length-1) || low/info.dwPageSize!=(low+length-1)/info.dwPageSize)return ERROR_NOT_SUPPORTED;
        regions[i].address=start;regions[i].bytes=length;
        for(DWORD j=0;j<i;++j){uintptr_t prev=(uintptr_t)regions[j].address;
            if(items[j]==p || items[j]->slot==p->slot || (low<prev+regions[j].bytes && prev<low+length))return ERROR_INVALID_PARAMETER;
        }
    }
    error=process_hook_late_guard_begin(guard,regions,count);if(error)return error;
    for(DWORD i=0;i<count;++i)if(memcmp(regions[i].address,items[i]->backup,regions[i].bytes)){error=ERROR_REVISION_MISMATCH;goto done;}
    for(DWORD i=0;i<count;++i){
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
        if(i==fail_protect_after){error=ERROR_ACCESS_DENIED;goto restore;}
#endif
        if(!VirtualProtect((void *)regions[i].address,regions[i].bytes,PAGE_EXECUTE_READWRITE,&protection[i])){error=GetLastError();goto restore;}
        ++protected_count;
    }
    /* Publish every original-call slot before the first target changes. */
    for(DWORD i=0;i<count;++i)memcpy(items[i]->slot,&items[i]->trampoline,sizeof(items[i]->trampoline));
    for(DWORD i=0;i<count;++i){
        process_hook_late_patch *p=items[i];BYTE *start=(BYTE *)regions[i].address;
        JMP_REL jump={.opcode=0xe9,.operand=(UINT32)((uintptr_t)p->replacement-(uintptr_t)(start+sizeof(JMP_REL)))};
        memcpy(start,&jump,sizeof(jump));
        if(p->above){JMP_REL_SHORT short_jump={.opcode=0xeb,.operand=(UINT8)(0-sizeof(JMP_REL)-sizeof(JMP_REL_SHORT))};memcpy(p->target,&short_jump,sizeof(short_jump));}
        p->installed=TRUE;
    }
 restore:
    while(protected_count){--protected_count;if(!VirtualProtect((void *)regions[protected_count].address,regions[protected_count].bytes,protection[protected_count],&ignored) && !error)error=GetLastError();}
    for(DWORD i=0;i<count;++i)if(items[i]->installed && !FlushInstructionCache(GetCurrentProcess(),regions[i].address,regions[i].bytes) && !error)error=GetLastError();
 done:
    {DWORD release=process_hook_late_guard_end(guard);if(release && !error)error=release;}
    return error;
}
DWORD process_hook_late_patch_install(process_hook_late_patch *p)
{
    if(!p)return ERROR_INVALID_PARAMETER;
    return process_hook_late_patch_install_batch(&p->guard,&p,1);
}
DWORD process_hook_late_patch_discard(process_hook_late_patch *p)
{
    if(!p || p->installed || p->guard.threads || p->guard.snapshots)return ERROR_INVALID_STATE;
    if(p->trampoline)FreeBuffer(p->trampoline);
    ZeroMemory(p,sizeof(*p));return ERROR_SUCCESS;
}
