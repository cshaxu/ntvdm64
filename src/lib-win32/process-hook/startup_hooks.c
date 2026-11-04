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

/* Adapted from MinHook v1.3.4 hook.c. Only the initial loader thread calls this
 * private transaction. The process is not handed to its caller until staging
 * succeeds. There is no late attach, thread enumeration, freeze or spin lock.
 * Buffer and relocated instruction generation are byte-exact upstream C.
 */
#include "startup_hooks.h"
#include "minhook/src/buffer.h"
#include "minhook/src/trampoline.h"
#include <string.h>
#define HOOK_LIMIT 88
typedef struct hook_entry {
    void *target,*replacement,*trampoline;
    void *slot;
    BYTE backup[sizeof(JMP_REL)+sizeof(JMP_REL_SHORT)];
    BOOL above,enabled;
} hook_entry;
static hook_entry hooks[HOOK_LIMIT];
static unsigned process_hook_startup_count;
static BOOL transaction,process_hook_startup_committed;

DWORD process_hook_startup_begin(void)
{
    if(transaction || process_hook_startup_committed)return ERROR_INVALID_STATE;
    InitializeBuffer();process_hook_startup_count=0;transaction=TRUE;return ERROR_SUCCESS;
}
DWORD process_hook_startup_attach(void *original_slot,void *replacement)
{
    TRAMPOLINE ct={0};hook_entry *hook;void *target;
    if(!transaction || !original_slot || !replacement)return ERROR_INVALID_PARAMETER;
    memcpy(&target,original_slot,sizeof(target));
    if(!target)return ERROR_INVALID_PARAMETER;
    if(process_hook_startup_count==HOOK_LIMIT)return ERROR_INSUFFICIENT_BUFFER;
    if(!IsExecutableAddress(target) || !IsExecutableAddress(replacement))return ERROR_INVALID_ADDRESS;
    for(unsigned i=0;i<process_hook_startup_count;++i)if(hooks[i].target==target)return ERROR_ALREADY_EXISTS;
    ct.pTarget=target;ct.pDetour=replacement;ct.pTrampoline=AllocateBuffer(target);
    if(!ct.pTrampoline)return ERROR_NOT_ENOUGH_MEMORY;
    if(!CreateTrampolineFunction(&ct)){FreeBuffer(ct.pTrampoline);return ERROR_NOT_SUPPORTED;}
    hook=&hooks[process_hook_startup_count++];ZeroMemory(hook,sizeof(*hook));
    hook->target=ct.pTarget;hook->trampoline=ct.pTrampoline;hook->above=ct.patchAbove;hook->slot=original_slot;
#ifdef _WIN64
    hook->replacement=ct.pRelay;
#else
    hook->replacement=ct.pDetour;
#endif
    memcpy(hook->backup,(BYTE *)hook->target-(hook->above?sizeof(JMP_REL):0),
        sizeof(JMP_REL)+(hook->above?sizeof(JMP_REL_SHORT):0));
    /* Publish every trampoline before modifying any export. */
    memcpy(original_slot,&hook->trampoline,sizeof(hook->trampoline));return ERROR_SUCCESS;
}
static DWORD patch(hook_entry *hook,BOOL enable)
{
    DWORD protect,unused,error=ERROR_SUCCESS;
    BYTE *target=(BYTE *)hook->target-(hook->above?sizeof(JMP_REL):0);
    SIZE_T size=sizeof(JMP_REL)+(hook->above?sizeof(JMP_REL_SHORT):0);
    if(!VirtualProtect(target,size,PAGE_EXECUTE_READWRITE,&protect))return GetLastError();
    if(enable){
        PJMP_REL jump=(PJMP_REL)target;jump->opcode=0xe9;
        jump->operand=(UINT32)((BYTE *)hook->replacement-(target+sizeof(JMP_REL)));
        if(hook->above){PJMP_REL_SHORT short_jump=(PJMP_REL_SHORT)hook->target;
            short_jump->opcode=0xeb;short_jump->operand=(UINT8)(0-sizeof(JMP_REL)-sizeof(JMP_REL_SHORT));}
    }else memcpy(target,hook->backup,size);
    hook->enabled=enable;
    if(!VirtualProtect(target,size,protect,&unused))error=GetLastError();
    if(!FlushInstructionCache(GetCurrentProcess(),target,size) && !error)error=GetLastError();
    return error;
}
void process_hook_startup_abort(void)
{
    if(!transaction)return;
    for(unsigned i=process_hook_startup_count;i;i--){hook_entry *hook=&hooks[i-1];
        if(hook->enabled && patch(hook,FALSE)){
            /* A partially patched process cannot safely return to application
             * entry. This is solely this freshly staged, uncommitted process. */
            TerminateProcess(GetCurrentProcess(),ERROR_DLL_INIT_FAILED);
            __builtin_unreachable();
        }
        memcpy(hook->slot,&hook->target,sizeof(hook->target));
    }
    UninitializeBuffer();process_hook_startup_count=0;transaction=FALSE;
}
DWORD process_hook_startup_commit(void)
{
    DWORD error;
    if(!transaction)return ERROR_INVALID_STATE;
    for(unsigned i=0;i<process_hook_startup_count;++i)if((error=patch(&hooks[i],TRUE))){process_hook_startup_abort();return error;}
    transaction=FALSE;process_hook_startup_committed=TRUE;return ERROR_SUCCESS;
}
