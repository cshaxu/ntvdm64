#include "mvdm_softpc_termination.h"

#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ntvdm-exe/session/session.h"

/* Original host close is idempotent: it rejects both pre-start and repeated
 * calls. Direct standalone termination otherwise longjmps around the normal
 * original host_main -> host_applClose tail. */
extern void host_applClose(void);

/* These are the original DPMI provider's selected in-process table carriers.
 * CPU40 intentionally reads its own guest-linear shadow instead of the
 * source table.  They are consulted only by the explicit trace below. */
extern void *Ldt;
extern unsigned long IntelBase;
extern unsigned long Cpu40GdtShadowAddress;
extern unsigned long Cpu40LdtShadowAddress;
extern unsigned long FlatAddress[];
extern int xtrn2phy(unsigned long linear, unsigned char access,
    unsigned long *physical);
extern void c_sas_loads(unsigned long source, unsigned char *destination,
    unsigned long byte_count);
/* Observation only. Remember adjacent decode states, never alter guest FLAGS.
 * The path is fixed per thread on first execution, and only transitions log. */
void mvdm_softpc_report_nt_transition(unsigned short cs, unsigned long ip,
    unsigned long flags, unsigned short ss, unsigned long sp)
{
    static __declspec(thread) char path[MAX_PATH];
    static __declspec(thread) int initialized;
    static __declspec(thread) unsigned count;
    static __declspec(thread) unsigned short previous_cs;
    static __declspec(thread) unsigned long previous_ip, previous_flags;
    DWORD saved = GetLastError(), written, size;
    HANDLE file;
    char line[256];
    if (!initialized) {
        DWORD length = GetEnvironmentVariableA("MVDM_WOW_NT_TRACE_PATH", path, sizeof(path));
        if (!length || length >= sizeof(path)) path[0] = 0;
        initialized = 1;
    }
    if (path[0] && ((previous_flags ^ flags) & 0x4000u) && count++ < 64) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            size = (DWORD)sprintf_s(line, sizeof(line),
                "tid=%lu NT-transition previous=%04X:%08lX flags=%08lX current=%04X:%08lX flags=%08lX stack=%04X:%08lX\r\n",
                GetCurrentThreadId(), previous_cs, previous_ip, previous_flags,
                cs, ip, flags, ss, sp);
            if (size) (void)WriteFile(file, line, size, &written, NULL);
            CloseHandle(file);
        }
    }
    previous_cs = cs; previous_ip = ip; previous_flags = flags;
    SetLastError(saved);
}

void mvdm_softpc_report_termination(char const *stage,
    void const *return_address)
{
    char path[MAX_PATH];
    DWORD bytes;
    HANDLE file;
    char line[64];
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_TERMINATE_VDM_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "%s return=%p\r\n", stage, return_address);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_virtual_memory_failure(unsigned long status)
{
    char path[MAX_PATH];
    DWORD bytes;
    HANDLE file;
    char line[64];
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_TERMINATE_VDM_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "vdm-allocate status=%08lx\r\n", status);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_table_address(char const *stage,
    unsigned long source_address, unsigned long gdt_shadow_address,
    unsigned long ldt_shadow_address)
{
    char path[MAX_PATH];
    char line[224];
    unsigned long source[2] = {0, 0};
    unsigned long gdt[2] = {0, 0};
    unsigned long ldt[2] = {0, 0};
    DWORD bytes;
    DWORD written;
    HANDLE file;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    /* The callback selector is an ordinary LDT entry.  Sampling its two
     * words at the pre-existing PassTableAddress observation boundary
     * distinguishes an initial source-table publication from a later DOSX
     * relocation.  This observer never selects, copies, or writes a table. */
    if (Ldt != NULL)
        CopyMemory(source, (unsigned char *)Ldt + 0x83b0u, sizeof(source));
    if (IntelBase != 0u && gdt_shadow_address != 0u)
        CopyMemory(gdt, (unsigned char *)(uintptr_t)(IntelBase +
            gdt_shadow_address) + 0x83b0u, sizeof(gdt));
    if (IntelBase != 0u && ldt_shadow_address != 0u)
        CopyMemory(ldt, (unsigned char *)(uintptr_t)(IntelBase +
            ldt_shadow_address) + 0x83b0u, sizeof(ldt));
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-table-address stage=%s source=%08lx gdt=%08lx ldt=%08lx selector=83b7 source=%08lx:%08lx gdt=%08lx:%08lx ldt=%08lx:%08lx\\r\\n",
        stage, source_address, gdt_shadow_address, ldt_shadow_address,
        source[0], source[1], gdt[0], gdt[1], ldt[0], ldt[1]);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_bop_state(unsigned long cr0, unsigned long cr3,
    unsigned short cs, unsigned short ip, unsigned long ds_base,
    unsigned long ds_limit)
{
    char path[MAX_PATH];
    char line[144];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S41_WOW_BOP_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "wow-bop cr0=%08lx cr3=%08lx csip=%04x:%04x dsbase=%08lx dslimit=%08lx\r\n",
        cr0, cr3, (unsigned int)cs, (unsigned int)ip, ds_base, ds_limit);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_dispatch_return_state(unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip)
{
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S42_WOW_RETURN_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "wow-dispatch-return cr0=%08lx cr3=%08lx csip=%04x:%04x\r\n",
        cr0, cr3, (unsigned int)cs, (unsigned int)ip);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_stage(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip)
{
    char path[MAX_PATH];
    char line[144];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    if (stage == NULL) return;
    bytes = GetEnvironmentVariableA("MVDM_S42_WOW_RETURN_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "wow-stage stage=%s cr0=%08lx cr3=%08lx csip=%04x:%04x\\r\\n",
        stage, cr0, cr3, (unsigned int)cs, (unsigned int)ip);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_fast_callback_binding(char const *stage,
    unsigned long address)
{
    char path[MAX_PATH];
    char line[128];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    if (stage == NULL) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "wow-fast-binding stage=%s address=%08lx\\r\\n",
        stage, address);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit)
{
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S41_DPMI_MODE_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (stage == NULL || bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "%s cr0=%08lx cr3=%08lx csip=%04x:%04x dsbase=%08lx dslimit=%08lx\r\n",
        stage, cr0, cr3, (unsigned int)cs, (unsigned int)ip, ds_base, ds_limit);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_descriptor(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high)
{
    char path[MAX_PATH];
    char line[208];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_DESCRIPTOR_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "selector=%04x source=%08lx:%08lx gdt=%08lx:%08lx ldt=%08lx:%08lx\r\n",
        (unsigned int)selector, source_low, source_high, gdt_low, gdt_high,
        ldt_low, ldt_high);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_cpu40_descriptor_publish(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high)
{
    char path[MAX_PATH], filter[32], line[224];
    DWORD bytes, written;
    unsigned long selected;
    HANDLE file;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) return;
    selected = strtoul(filter, NULL, 16);
    if ((unsigned short)selected != selector) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "descriptor-publish selector=%04x source=%08lx:%08lx gdt=%08lx:%08lx ldt=%08lx:%08lx\r\n",
        (unsigned int)selector, source_low, source_high, gdt_low, gdt_high,
        ldt_low, ldt_high);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_set_descriptor(unsigned short es,
    unsigned short bx, unsigned short ax, unsigned short cx,
    unsigned long descriptor_pointer, unsigned long protected_mode)
{
    char path[MAX_PATH];
    char line[176];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes=GetEnvironmentVariableA("MVDM_S42_DPMI_SET_DESCRIPTOR_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file=CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-setdesc esbx=%04x:%04x ax=%04x cx=%04x ptr=%08lx pm=%lu\r\n",
        (unsigned int)es, (unsigned int)bx, (unsigned int)ax,
        (unsigned int)cx, descriptor_pointer, protected_mode);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_set_descriptor_entry(unsigned short ax,
    unsigned short bx, unsigned short cx)
{
    char path[MAX_PATH];
    char line[96];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes=GetEnvironmentVariableA("MVDM_S42_DPMI_SET_DESCRIPTOR_ENTRY_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file=CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-setdesc-entry ax=%04x bx=%04x cx=%04x aligned=%u\r\n",
        (unsigned int)ax, (unsigned int)bx, (unsigned int)cx,
        (unsigned int)((ax % 8u) == 0u));
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_bop(unsigned long index, unsigned short ax,
    unsigned short bx, unsigned short cx)
{
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes=GetEnvironmentVariableA("MVDM_S42_DPMI_BOP_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file=CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "dpmi-bop index=%02lx ax=%04x bx=%04x cx=%04x\r\n",
        index, (unsigned int)ax, (unsigned int)bx, (unsigned int)cx);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_fast_bop_result(unsigned short bx,
    unsigned short dx, unsigned short es)
{
    char path[MAX_PATH];
    char line[96];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_BOP_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "dpmi-fast-bop-result bx=%04x dx=%04x es=%04x\\r\\n",
        (unsigned int)bx, (unsigned int)dx, (unsigned int)es);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_virtual_device_control(unsigned long eax,
    unsigned short bx, unsigned short cx, unsigned short dx)
{
    char path[MAX_PATH];
    char line[128];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S42_VXD_BOP_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "vxd-control eax=%08lx bx=%04x cx=%04x dx=%04x\r\n", eax,
        (unsigned int)bx, (unsigned int)cx, (unsigned int)dx);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_swint(unsigned long interrupt_number,
    unsigned short ax, unsigned short bx, unsigned short cx,
    unsigned short caller_cs, unsigned long caller_eip)
{
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes=GetEnvironmentVariableA("MVDM_S42_DPMI_SWINT_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file=CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-swint int=%02lx ax=%04x bx=%04x cx=%04x caller=%04x:%08lx\r\n",
        interrupt_number, (unsigned int)ax, (unsigned int)bx, (unsigned int)cx,
        (unsigned int)caller_cs, caller_eip);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_swint_target(unsigned long interrupt_number,
    unsigned short handler_cs, unsigned long handler_eip,
    unsigned short handler_flags, unsigned char const *code,
    unsigned long code_size)
{
    char path[MAX_PATH];
    char line[192];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    if (interrupt_number != 0x31u) return;
    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_SWINT_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-swint-target int=%02lx handler=%04x:%08lx flags=%04x code=%02x%02x%02x%02x%02x%02x%02x%02x\\r\\n",
        interrupt_number, (unsigned int)handler_cs, handler_eip,
        (unsigned int)handler_flags, (unsigned int)(code_size > 0u ? code[0] : 0u),
        (unsigned int)(code_size > 1u ? code[1] : 0u),
        (unsigned int)(code_size > 2u ? code[2] : 0u),
        (unsigned int)(code_size > 3u ? code[3] : 0u),
        (unsigned int)(code_size > 4u ? code[4] : 0u),
        (unsigned int)(code_size > 5u ? code[5] : 0u),
        (unsigned int)(code_size > 6u ? code[6] : 0u),
        (unsigned int)(code_size > 7u ? code[7] : 0u));
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_allocsel_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short bx,
    unsigned short cx, unsigned short dx, unsigned short bp,
    unsigned long eflags)
{
    char path[MAX_PATH];
    char line[160];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    /* DOSX's source map assigns i31_WOW_AllocSel to 0A8D:4F6A.  The
     * selector relocates at runtime, but this routine's 30-byte code range
     * is fixed inside the original DOSX image. */
    if (eip < 0x4f6au || eip >= 0x4f88u) return;
    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_ALLOCSEL_TRACE_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-allocsel csip=%04x:%08lx ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x cf=%u\\r\\n",
        (unsigned int)cs, eip, (unsigned int)ax, (unsigned int)bx,
        (unsigned int)cx, (unsigned int)dx, (unsigned int)bp,
        (unsigned int)(eflags & 1u));
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_setdescriptor_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short bx,
    unsigned short cx, unsigned short dx, unsigned short bp,
    unsigned long eflags)
{
    char path[MAX_PATH];
    char line[168];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    /* DOSX's source map assigns i31_WOW_SetDescriptor to 0A8D:4F88.
     * KRNL386's retail WOW path instead enters DPMIProc/WowSetSelector at
     * 0001:3947..3A5F before it asks the NT kernel to publish a selector.
     * Both ranges are observation-only; neither is an emulation route. */
    if (!((eip >= 0x4f88u && eip < 0x4fa4u) ||
          (eip >= 0x3947u && eip < 0x3a60u))) return;
    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_SETDESCRIPTOR_TRACE_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-setdescriptor csip=%04x:%08lx ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x cf=%u\\r\\n",
        (unsigned int)cs, eip, (unsigned int)ax, (unsigned int)bx,
        (unsigned int)cx, (unsigned int)dx, (unsigned int)bp,
        (unsigned int)(eflags & 1u));
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_getsel_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short cx,
    unsigned short si, unsigned short ds, unsigned long eflags)
{
    char path[MAX_PATH];
    char line[152];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    /* KRNL386's retail map places get_sel at 0001:2C26 and the following
     * routine at 2D3E.  Only record entry plus its final instruction. */
    if (eip != 0x2c26u && eip != 0x2d3du) return;
    bytes = GetEnvironmentVariableA("MVDM_S42_KRNL_GETSEL_TRACE_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "krnl-getsel stage=%s csip=%04x:%08lx ax=%04x cx=%04x si=%04x ds=%04x cf=%u\\r\\n",
        eip == 0x2c26u ? "entry" : "return", (unsigned int)cs, eip,
        (unsigned int)ax, (unsigned int)cx, (unsigned int)si,
        (unsigned int)ds, (unsigned int)(eflags & 1u));
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_longptradd_instruction(unsigned short cs,
    unsigned long eip, unsigned short ax, unsigned short bx,
    unsigned short cx, unsigned short dx, unsigned short ss,
    unsigned long esp)
{
    char path[MAX_PATH];
    char line[176];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    if (eip < 0x3482u || eip >= 0x3492u) return;
    bytes = GetEnvironmentVariableA("MVDM_S42_WOW_LONGPTRADD_TRACE_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "wow-longptradd csip=%04x:%08lx ax=%04x bx=%04x cx=%04x dx=%04x ss=%04x esp=%08lx\\r\\n",
        (unsigned int)cs, eip, (unsigned int)ax, (unsigned int)bx,
        (unsigned int)cx, (unsigned int)dx, (unsigned int)ss, esp);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_iret16(unsigned short frame_bits,
    unsigned short service_ax, unsigned short result_ax,
    unsigned short result_bx, unsigned short continuation_cs,
    unsigned long continuation_eip, unsigned long eflags)
{
    char path[MAX_PATH];
    char line[128];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    /* 0000h is normally too noisy.  A high selector result is the exact
     * original Allocate-LDT-Descriptors event that can become a WOW 16:16
     * procedure; retain it only for this default-off publication witness. */
    if (service_ax != 0x04f1u && service_ax != 0x04f2u &&
            service_ax != 0x04f3u &&
            !(service_ax == 0x0000u && (result_ax & 0x8000u)))
        return;
    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_IRET16_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-iret frame=%u service=%04x ax=%04x bx=%04x next=%04x:%08lx cf=%u\\r\\n",
        (unsigned int)frame_bits, (unsigned int)service_ax,
        (unsigned int)result_ax, (unsigned int)result_bx,
        (unsigned int)continuation_cs, continuation_eip,
        (unsigned int)(eflags & 1u));
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_iret16_state(unsigned short frame_bits,
    unsigned short ax, unsigned short bx, unsigned short cx,
    unsigned short dx, unsigned short si, unsigned short di,
    unsigned short bp, unsigned short ds, unsigned short es,
    unsigned short ss, unsigned long sp, unsigned short return_ip,
    unsigned short return_cs, unsigned short return_flags)
{
    char path[MAX_PATH];
    char line[256];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    if (ax != 0x04f1u && ax != 0x04f2u && ax != 0x04f3u)
        return;
    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_IRET16_STATE_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-iret-state frame=%u ax=%04x bx=%04x cx=%04x dx=%04x si=%04x di=%04x bp=%04x ds=%04x es=%04x ss=%04x sp=%08lx return=%04x:%04x flags=%04x\\r\\n",
        (unsigned int)frame_bits, (unsigned int)ax, (unsigned int)bx,
        (unsigned int)cx, (unsigned int)dx, (unsigned int)si,
        (unsigned int)di, (unsigned int)bp, (unsigned int)ds,
        (unsigned int)es, (unsigned int)ss, sp, (unsigned int)return_cs,
        (unsigned int)return_ip, (unsigned int)return_flags);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_code_segment_load(unsigned short selector,
    unsigned long base, unsigned long limit, unsigned long access_rights)
{
    char path[MAX_PATH];
    char filter[16];
    char line[256];
    DWORD bytes;
    HANDLE file;
    DWORD written;
    unsigned long selected;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) return;
    selected = strtoul(filter, NULL, 16);
    if (selector != (unsigned short)selected) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "cs-load selector=%04x base=%08lx limit=%08lx ar=%08lx\r\n",
        (unsigned int)selector, base, limit, access_rights);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_far_call_attempt(unsigned short selector,
    unsigned long offset, unsigned short caller_cs, unsigned long caller_eip,
    unsigned short ldt_selector, unsigned long ldt_base,
    unsigned long ldt_limit)
{
    char path[MAX_PATH];
    char filter[16];
    char line[256];
    DWORD bytes;
    HANDLE file;
    DWORD written;
    unsigned long selected;
    unsigned long caller_source[2] = {0, 0};
    unsigned long caller_shadow[2] = {0, 0};
    unsigned long caller_offset;
    unsigned long caller_base;
    unsigned long caller_linear;
    unsigned long caller_physical = 0;
    int caller_mapped;
    unsigned char caller_bytes[16] = {0};

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) return;
    selected = strtoul(filter, NULL, 16);
    if (selector != (unsigned short)selected) return;
    if (Ldt != NULL) {
        caller_offset = (unsigned long)(caller_cs & 0xfff8u);
        CopyMemory(caller_source, (unsigned char *)Ldt + caller_offset,
            sizeof(caller_source));
        if (Cpu40LdtShadowAddress != 0u && IntelBase != 0u) {
            CopyMemory(caller_shadow, (unsigned char *)(uintptr_t)(IntelBase +
                Cpu40LdtShadowAddress) + caller_offset, sizeof(caller_shadow));
        }
        caller_base = ((caller_source[0] >> 16) & 0xffffu) |
            ((caller_source[1] & 0xffu) << 16) |
            (caller_source[1] & 0xff000000u);
        /* CALLF has already consumed its operand when this witness runs;
         * retain the eight preceding bytes as well as the post-call bytes. */
        caller_linear = caller_base + caller_eip - 8u;
        caller_mapped = xtrn2phy(caller_linear, 0u, &caller_physical);
        c_sas_loads(caller_mapped ? caller_physical : caller_linear,
            caller_bytes, sizeof(caller_bytes));
    }
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "far-call selector=%04x offset=%08lx caller=%04x:%08lx ldt=%04x:%08lx/%08lx source=%08lx:%08lx shadow=%08lx:%08lx code=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
        (unsigned int)selector, offset, (unsigned int)caller_cs, caller_eip,
        (unsigned int)ldt_selector, ldt_base, ldt_limit,
        caller_source[0], caller_source[1], caller_shadow[0], caller_shadow[1],
        caller_bytes[0], caller_bytes[1], caller_bytes[2], caller_bytes[3],
        caller_bytes[4], caller_bytes[5], caller_bytes[6], caller_bytes[7],
        caller_bytes[8], caller_bytes[9], caller_bytes[10], caller_bytes[11],
        caller_bytes[12], caller_bytes[13], caller_bytes[14], caller_bytes[15]);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_far_call_validation(unsigned short selector,
    unsigned long descriptor_address, unsigned long access_rights,
    unsigned long current_privilege)
{
    char path[MAX_PATH];
    char filter[16];
    char line[144];
    DWORD bytes;
    HANDLE file;
    DWORD written;
    unsigned long selected;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) return;
    selected = strtoul(filter, NULL, 16);
    if (selector != (unsigned short)selected) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "far-validate selector=%04x descriptor=%08lx ar=%02lx cpl=%lu\r\n",
        (unsigned int)selector, descriptor_address, access_rights,
        current_privilege);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_descriptor_domains(unsigned short selector,
    unsigned long descriptor_address, unsigned long ldt_base,
    unsigned long ldt_limit, unsigned short ldt_selector,
    unsigned long descriptor_low, unsigned long descriptor_high)
{
    char path[MAX_PATH];
    char filter[16];
    char line[192];
    DWORD bytes;
    HANDLE file;
    DWORD written;
    unsigned long selected;
    unsigned long source[2] = {0, 0};
    unsigned long shadow[2] = {0, 0};
    unsigned long offset;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) return;
    selected = strtoul(filter, NULL, 16);
    if (selector != (unsigned short)selected || Ldt == NULL ||
            Cpu40LdtShadowAddress == 0u) return;
    offset = (unsigned long)(selector & 0xfff8u);
    CopyMemory(source, (unsigned char *)Ldt + offset, sizeof(source));
    CopyMemory(shadow, (unsigned char *)(uintptr_t)(IntelBase +
        Cpu40LdtShadowAddress) + offset, sizeof(shadow));
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "far-domains selector=%04x observed=%08lx ldt=%04x:%08lx/%08lx actual=%08lx:%08lx source=%08lx:%08lx shadow=%08lx:%08lx flat=%08lx\\r\\n",
        (unsigned int)selector, descriptor_address, (unsigned int)ldt_selector,
        ldt_base, ldt_limit, descriptor_low, descriptor_high, source[0],
        source[1], shadow[0], shadow[1], FlatAddress[(selector & 0xfff8u) >> 3]);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_fetch_translation(unsigned short selector,
    unsigned long offset, unsigned long linear, unsigned long physical,
    unsigned long mapped)
{
    char path[MAX_PATH];
    char filter[16];
    char line[256];
    DWORD bytes;
    HANDLE file;
    DWORD written;
    unsigned long selected;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) return;
    selected = strtoul(filter, NULL, 16);
    if (selector != (unsigned short)selected) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "far-fetch selector=%04x offset=%08lx linear=%08lx mapped=%lu physical=%08lx\\r\\n",
        (unsigned int)selector, offset, linear, mapped, physical);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_int2a_ldt(unsigned short selector,
    unsigned long descriptor_low, unsigned long descriptor_high)
{
    char path[MAX_PATH];
    char line[128];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "int2a-setldt selector=%04x descriptor=%08lx:%08lx\\r\\n",
        (unsigned int)selector, descriptor_low, descriptor_high);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_int2a(unsigned long eax, unsigned long ebx,
    unsigned long ecx, unsigned long edx, unsigned long ebp)
{
    char path[MAX_PATH];
    char line[160];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "int2a eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx ebp=%08lx\\r\\n",
        eax, ebx, ecx, edx, ebp);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_exception_hook(unsigned long exception_number,
    unsigned long error_code, unsigned long handler_installed,
    unsigned long handled)
{
    char path[MAX_PATH];
    char line[128];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "exception-hook vector=%02lx error=%04lx installed=%lu handled=%lu\r\n",
        exception_number, error_code, handler_installed, handled);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_fault_dispatch(unsigned long exception_number,
    unsigned long error_code, unsigned short handler_cs, unsigned long handler_eip,
    unsigned short resulting_cs, unsigned long resulting_eip,
    unsigned short resulting_ss, unsigned long resulting_esp,
    unsigned long frame32, unsigned short source_cs, unsigned long source_eip)
{
    char path[MAX_PATH];
    char line[320];
    DWORD bytes;
    DWORD written;
    HANDLE file;
    unsigned long source_descriptor[2] = {0, 0};
    unsigned long source_offset;
    unsigned long source_base = 0;
    unsigned long source_linear = 0;
    unsigned long source_physical = 0;
    int source_mapped = 0;
    unsigned char source_bytes[8] = {0};

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    if (Ldt != NULL) {
        source_offset = (unsigned long)(source_cs & 0xfff8u);
        CopyMemory(source_descriptor, (unsigned char *)Ldt + source_offset,
            sizeof(source_descriptor));
        source_base = ((source_descriptor[0] >> 16) & 0xffffu) |
            ((source_descriptor[1] & 0xffu) << 16) |
            (source_descriptor[1] & 0xff000000u);
        source_linear = source_base + source_eip;
        source_mapped = xtrn2phy(source_linear, 0u, &source_physical);
        c_sas_loads(source_mapped ? source_physical : source_linear,
            source_bytes, sizeof(source_bytes));
    }
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-fault vector=%02lx error=%04lx source=%04x:%08lx base=%08lx linear=%08lx mapped=%d physical=%08lx code=%02x%02x%02x%02x%02x%02x%02x%02x handler=%04x:%08lx result=%04x:%08lx stack=%04x:%08lx frame32=%lu\r\n",
        exception_number, error_code, (unsigned int)source_cs, source_eip,
        source_base, source_linear, source_mapped, source_physical,
        source_bytes[0], source_bytes[1], source_bytes[2], source_bytes[3],
        source_bytes[4], source_bytes[5], source_bytes[6], source_bytes[7],
        (unsigned int)handler_cs, handler_eip, (unsigned int)resulting_cs,
        resulting_eip, (unsigned int)resulting_ss, resulting_esp, frame32);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_fault_stack(char const *stage,
    unsigned short saved_ss, unsigned long saved_esp,
    unsigned short active_ss, unsigned long active_esp,
    unsigned short const *words, unsigned long word_count)
{
    char path[MAX_PATH];
    char line[256];
    char word_text[128];
    DWORD bytes;
    DWORD written;
    HANDLE file;
    unsigned long index;
    size_t used;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    word_text[0] = '\0';
    for (index = 0u; index < word_count && index < 8u; ++index) {
        used = strlen(word_text);
        snprintf(word_text + used, sizeof(word_text) - used,
            "%s%04x", index == 0u ? "" : ",", (unsigned int)words[index]);
    }
    snprintf(line, sizeof(line),
        "dpmi-fault-stack stage=%s saved=%04x:%08lx active=%04x:%08lx words=%s\\r\\n",
        stage, (unsigned int)saved_ss, saved_esp, (unsigned int)active_ss,
        active_esp, word_text);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_source_descriptor_store(unsigned long address,
    unsigned long value, unsigned long width, unsigned short cs,
    unsigned long eip, unsigned short ss, unsigned long esp)
{
    char path[MAX_PATH];
    char filter[16];
    char line[288];
    DWORD bytes;
    HANDLE file;
    DWORD written;
    unsigned long selected;
    unsigned long source_address;
    unsigned long gdt_shadow_address;
    unsigned long ldt_shadow_address;
    unsigned long descriptor_address;
    char const *domain;
    unsigned long writer_source[2] = {0, 0};
    unsigned long writer_shadow[2] = {0, 0};
    unsigned long writer_offset;
    unsigned long writer_base;
    unsigned long writer_linear;
    unsigned long writer_physical = 0;
    int writer_mapped;
    unsigned char writer_bytes[8] = {0};

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter) || Ldt == NULL || IntelBase == 0u)
        return;
    selected = strtoul(filter, NULL, 16);
    source_address = (unsigned long)((unsigned char *)Ldt -
        (unsigned char *)(uintptr_t)IntelBase);
    descriptor_address = source_address + (selected & 0xfff8u);
    gdt_shadow_address = Cpu40GdtShadowAddress + (selected & 0xfff8u);
    ldt_shadow_address = Cpu40LdtShadowAddress + (selected & 0xfff8u);
    if (!(address + width > descriptor_address &&
            address < descriptor_address + sizeof(LDT_ENTRY)) &&
        !(address + width > gdt_shadow_address &&
            address < gdt_shadow_address + sizeof(LDT_ENTRY)) &&
        !(address + width > ldt_shadow_address &&
            address < ldt_shadow_address + sizeof(LDT_ENTRY))) return;
    domain = (address + width > ldt_shadow_address &&
        address < ldt_shadow_address + sizeof(LDT_ENTRY)) ? "ldt" :
        ((address + width > gdt_shadow_address &&
        address < gdt_shadow_address + sizeof(LDT_ENTRY)) ? "gdt" : "source");
    writer_offset = (unsigned long)(cs & 0xfff8u);
    CopyMemory(writer_source, (unsigned char *)Ldt + writer_offset,
        sizeof(writer_source));
    if (Cpu40LdtShadowAddress != 0u) {
        CopyMemory(writer_shadow, (unsigned char *)(uintptr_t)(IntelBase +
            Cpu40LdtShadowAddress) + writer_offset, sizeof(writer_shadow));
    }
    writer_base = ((writer_source[0] >> 16) & 0xffffu) |
        ((writer_source[1] & 0xffu) << 16) |
        (writer_source[1] & 0xff000000u);
    writer_linear = writer_base + eip;
    writer_mapped = xtrn2phy(writer_linear, 0u, &writer_physical);
    /* A pre-paging DOSX segment is physically addressed.  Capture that
     * original fetch location too; this is only a trace read and does not
     * select, publish, or alter the current CCPU translation. */
    c_sas_loads(writer_mapped ? writer_physical : writer_linear,
        writer_bytes, sizeof(writer_bytes));
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "%s-descriptor-store selector=%04lx address=%08lx width=%lu value=%08lx writer=%04x:%08lx stack=%04x:%08lx source=%08lx:%08lx shadow=%08lx:%08lx linear=%08lx mapped=%d physical=%08lx code=%02x%02x%02x%02x%02x%02x%02x%02x\\r\\n",
        domain, selected, address, width, value, (unsigned int)cs, eip,
        (unsigned int)ss, esp,
        writer_source[0], writer_source[1], writer_shadow[0], writer_shadow[1],
        writer_linear, writer_mapped, writer_physical, writer_bytes[0], writer_bytes[1],
        writer_bytes[2], writer_bytes[3], writer_bytes[4], writer_bytes[5],
        writer_bytes[6], writer_bytes[7]);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    (void)vdm_for_wow;
    /* Restore the original host close cohort before the standalone escape.
     * In NT4 this escape was ExitProcess; in this process it is longjmp, so
     * an outer application entry cannot be the cleanup owner. */
    host_applClose();
    return session_terminate_current(completion_code);
}
