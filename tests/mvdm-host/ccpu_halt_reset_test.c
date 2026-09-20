#include <stdint.h>
#include <windows.h>
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"

/* Formal original CCPU/SAS providers, not replacement decode/reset bodies. */
extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern void sas_init(uint32_t);
extern void sas_term(void);
extern void c_cpu_init(void);
extern void load_sw_cpu_access_functions(void);
extern void c_cpu_simulate(void);
extern void c_cpu_interrupt(int, unsigned short);
extern void c_setIP(unsigned short);
extern unsigned short c_getAX(void);
extern void c_sas_stores(uint32_t, unsigned char *, uint32_t);
extern unsigned char *c_GetPhyAdd(uint32_t);
extern int debug_exception_pending;
extern uint32_t CCPU_DR[8];
extern void MOV_DR(uint32_t, uint32_t);
extern void c_setGDT_BASE_LIMIT(uint32_t, uint16_t);
extern void c_setIDT_BASE_LIMIT(uint32_t, uint16_t);
extern void c_setTR_SELECTOR(uint16_t);
extern void c_setTR_BASE_LIMIT_AR(uint32_t, uint32_t, uint16_t);
extern int c_setCS(uint16_t);
extern int c_setSS(uint16_t);
extern int c_setDS(uint16_t);
extern void c_setSP(uint16_t);
extern void c_setCR3(uint32_t);
extern void c_setPG(int);
extern void c_setPE(int);
extern BOOL mvdm_debugger_read_debug_registers(USHORT, USHORT, PULONG);

static int debug_task_switch(int trap)
{
    /* Synthetic protected-mode machine: the original far JMP loads a 386
     * TSS with its T bit set. The original IDT path must deliver #DB once
     * before the first new-task instruction, despite sticky DR6.BT. */
    unsigned char gdt[] = {
        0,0,0,0,0,0,0,0,
        0xff,0xff,0,0,0x0f,0x9b,0,0,
        0xff,0xff,0,0,0,0x93,0,0,
        0x67,0,0,0x60,0,0x8b,0,0,
        0x67,0,0,0x70,0,0x89,0,0};
    unsigned char gate[] = {0x00,0x11,0x08,0,0,0x8e,0,0};
    unsigned char jump[] = {0xea,0,0,0x20,0};
    unsigned char handler[] = {
        0xff,0x06,0x06,0x03,             /* inc word [0306] */
        0x83,0x3e,0x06,0x03,0x01,       /* cmp word [0306],1 */
        0x75,0x09,                       /* jne failure */
        0x83,0x3e,0x08,0x03,0x00,       /* cmp word [0308],0 */
        0x75,0x02,0x66,0xcf,             /* jne failure; iretd */
        0xb8,0xad,0xde,0xd6,0xfe};
    unsigned char task[] = {0xff,0x06,0x08,0x03,
        0x90,0x90,0xb8,0xef,0xbe,0xd6,0xfe};
    unsigned char *tss;
    c_cpu_init();
    CopyMemory(c_GetPhyAdd(0x1000), gdt, sizeof(gdt));
    ZeroMemory(c_GetPhyAdd(0x6000), 0x2000);
    ZeroMemory(c_GetPhyAdd(0x9000), 16);
    CopyMemory(c_GetPhyAdd(0x9008), gate, sizeof(gate));
    CopyMemory(c_GetPhyAdd(0xf1000), jump, sizeof(jump));
    CopyMemory(c_GetPhyAdd(0xf1100), handler, sizeof(handler));
    CopyMemory(c_GetPhyAdd(0xf1300), task, sizeof(task));
    ZeroMemory(c_GetPhyAdd(0x306), 4);
    tss = c_GetPhyAdd(0x7000);
    *(ULONG *)(tss + 0x20) = 0x1300;
    *(ULONG *)(tss + 0x24) = 2;
    *(ULONG *)(tss + 0x38) = 0x800;
    *(USHORT *)(tss + 0x48) = 0x10;
    *(USHORT *)(tss + 0x4c) = 8;
    *(USHORT *)(tss + 0x50) = 0x10;
    *(USHORT *)(tss + 0x54) = 0x10;
    tss[0x64] = (unsigned char)trap;
    c_setGDT_BASE_LIMIT(0x1000, sizeof(gdt) - 1);
    c_setIDT_BASE_LIMIT(0x9000, 15);
    c_setPE(1);
    c_setCS(8);
    c_setSS(0x10);
    c_setDS(0x10);
    c_setSP(0x800);
    c_setTR_SELECTOR(0x18);
    c_setTR_BASE_LIMIT_AR(0x6000, 0x67, 0x8b);
    c_setIP(0x1000);
    c_cpu_simulate();
    return c_getAX() == 0xbeef && *c_GetPhyAdd(0x306) == trap &&
        *c_GetPhyAdd(0x308) == 1 && !!(CCPU_DR[6] & 0x8000) == trap &&
        !debug_exception_pending;
}

static int debug_descriptor_preflight(void)
{
    ULONG output[6];
    unsigned char descriptor[] = {0xff,0xff,0x00,0x20,0x00,0x92,0x00,0x00};
    c_cpu_init();
    c_setGDT_BASE_LIMIT(0x200000, 15);
    if (mvdm_debugger_read_debug_registers(8, 0, output)) return 0;
    c_setGDT_BASE_LIMIT(0x1000, 15);
    CopyMemory(c_GetPhyAdd(0x1008), descriptor, sizeof(descriptor));
    FillMemory(c_GetPhyAdd(0x2000), sizeof(output), 0x5a);
    if (!mvdm_debugger_read_debug_registers(8, 0, output) ||
        output[0] != 0x5a5a5a5a || output[5] != 0x5a5a5a5a) return 0;
    ZeroMemory(c_GetPhyAdd(0x4000), 0x2000);
    c_setCR3(0x4000);
    c_setPE(1);
    c_setPG(1);
    /* Missing PDE must return normally, not longjmp through a CPU #PF. */
    if (mvdm_debugger_read_debug_registers(8, 0, output)) return 0;
    *(ULONG *)c_GetPhyAdd(0x4000) = 0x5007;
    /* Present directory, missing descriptor PTE. */
    if (mvdm_debugger_read_debug_registers(8, 0, output)) return 0;
    *(ULONG *)c_GetPhyAdd(0x5004) = 0x1007;
    /* Descriptor exists, but the payload page is absent. */
    FillMemory(output, sizeof(output), 0xa5);
    if (mvdm_debugger_read_debug_registers(8, 0, output) ||
        output[0] != 0xa5a5a5a5 || output[5] != 0xa5a5a5a5) return 0;
    *(ULONG *)c_GetPhyAdd(0x5008) = 0x2007;
    if (!mvdm_debugger_read_debug_registers(8, 0, output) ||
        output[0] != 0x5a5a5a5a || output[5] != 0x5a5a5a5a) return 0;
    /* First payload page exists; the second half must not be zero-filled. */
    FillMemory(output, sizeof(output), 0xa5);
    if (mvdm_debugger_read_debug_registers(8, 0xff8, output) ||
        output[0] != 0xa5a5a5a5 || output[5] != 0xa5a5a5a5) return 0;
    *(ULONG *)c_GetPhyAdd(0x500c) = 0x3007;
    FillMemory(c_GetPhyAdd(0x2ff8), sizeof(output), 0x6b);
    if (!mvdm_debugger_read_debug_registers(8, 0xff8, output) ||
        output[0] != 0x6b6b6b6b || output[5] != 0x6b6b6b6b) return 0;
    c_setPG(0);
    c_setPE(0);
    return 1;
}

static int debug_resume_flag(void)
{
    /* IRETD resumes at F000:1014 with RF. The first INC must run once;
     * jumping back must fault before a second INC. A stuck RF reaches DEAD. */
    unsigned char program[] = {
        0x66,0x68,0x02,0x00,0x01,0x00,
        0x66,0x68,0x00,0xf0,0x00,0x00,
        0x66,0x68,0x14,0x10,0x00,0x00,0x66,0xcf,
        0xff,0x06,0x04,0x03,0x83,0x3e,0x04,0x03,0x02,
        0x73,0x02,0xeb,0xf3,0xb8,0xad,0xde,0xd6,0xfe};
    unsigned char handler[] = {0xb8,0xef,0xbe,0xd6,0xfe};
    unsigned char vector[] = {0x00,0x12,0x00,0xf0};
    c_cpu_init();
    CopyMemory(c_GetPhyAdd(0xf1000), program, sizeof(program));
    CopyMemory(c_GetPhyAdd(0xf1200), handler, sizeof(handler));
    CopyMemory(c_GetPhyAdd(4), vector, sizeof(vector));
    ZeroMemory(c_GetPhyAdd(0x304), 2);
    MOV_DR(0, 0xf1014);
    MOV_DR(7, 1);
    c_setIP(0x1000);
    c_cpu_simulate();
    return c_getAX() == 0xbeef && *c_GetPhyAdd(0x304) == 1 &&
        (CCPU_DR[6] & 1) && !debug_exception_pending;
}

static int debug_fault_cancellation(void)
{
    /* DIV reads a watched zero divisor, then raises #DE before retirement.
     * The pending data breakpoint must be cancelled, not delivered in #DE. */
    unsigned char program[] = {0xf6,0x36,0x00,0x03,
        0x90,0xb8,0xef,0xbe,0xd6,0xfe};
    unsigned char divide_handler[] = {0xff,0x06,0x02,0x03,
        0x55,0x89,0xe5,0x83,0x46,0x02,0x04,0x5d,0xcf};
    unsigned char debug_handler[] = {0xb8,0xad,0xde,0xd6,0xfe};
    unsigned char vectors[] = {0x00,0x11,0x00,0xf0,0x00,0x12,0x00,0xf0};
    c_cpu_init();
    CopyMemory(c_GetPhyAdd(0xf1000), program, sizeof(program));
    CopyMemory(c_GetPhyAdd(0xf1100), divide_handler, sizeof(divide_handler));
    CopyMemory(c_GetPhyAdd(0xf1200), debug_handler, sizeof(debug_handler));
    CopyMemory(c_GetPhyAdd(0), vectors, sizeof(vectors));
    ZeroMemory(c_GetPhyAdd(0x300), 4);
    MOV_DR(0, 0x300);
    MOV_DR(7, 0x00030001); /* one-byte read/write */
    c_setIP(0x1000);
    c_cpu_simulate();
    return c_getAX() == 0xbeef && *c_GetPhyAdd(0x302) == 1 &&
        (CCPU_DR[6] & 1) && !debug_exception_pending;
}

static void report(const char *text)
{
    DWORD written;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), text, lstrlenA(text), &written, NULL);
}

static DWORD WINAPI raise_reset(void *context)
{
    volatile unsigned char *entered = context;
    DWORD start = GetTickCount();
    while (*entered != 1) {
        if (GetTickCount() - start > 5000) return 1;
        Sleep(1);
    }
    /* Guest writes marker immediately before HLT; allow it to enter wait. */
    Sleep(50);
    c_cpu_interrupt(0, 0); /* original CPU_HW_RESET enum */
    return 0;
}

static LONG WINAPI report_exception(EXCEPTION_POINTERS *e)
{
    char message[128];
    wsprintfA(message, "exception=%08lx rva=%lx\n",
        e->ExceptionRecord->ExceptionCode,
        (unsigned long)((uintptr_t)e->ExceptionRecord->ExceptionAddress -
            (uintptr_t)GetModuleHandle(NULL)));
    report(message);
    wsprintfA(message, "caller-rva=%lx\n",
        *(DWORD *)(uintptr_t)e->ContextRecord->Esp - (DWORD)(uintptr_t)GetModuleHandle(NULL));
    report(message);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(void)
{
    session owner;
    HANDLE producer;
    DWORD producer_result = 1;
    unsigned short ax;
    char message[128];
    /* Padding permits either retained-IP or post-HLT-IP continuation in the
     * deliberately broken baseline to reach the DEAD sentinel safely. */
    unsigned char program[] = {0xc6,0x06,0x00,0x02,0x01,0xf4,0x90,
        0xb8,0xad,0xde,0xd6,0xfe};
    unsigned char reset_vector[] = {0xb8,0xef,0xbe,0xd6,0xfe};
    SetUnhandledExceptionFilter(report_exception);
    report("ntvdm-exe/session/gdp\n");
    session_initialize(&owner, 412);
    if (!session_activate(&owner) || !session_thread_bind(&owner) ||
        setup_global_data_ptr() == NULL) return 2;
    report("sas-init\n");
    sas_init(0x200000);
    if (!mvdm_softpc_guest_memory_begin(&owner)) return 3;
    report("vga/cpu-init\n");
    setup_vga_globals();
    load_sw_cpu_access_functions();
    debug_exception_pending = 1;
    CCPU_DR[6] = 0x4001;
    c_cpu_init();
    if (debug_exception_pending || CCPU_DR[6]) return 6;
    report("guest-program\n");
    c_sas_stores(0xf1000, program, sizeof(program));
    c_sas_stores(0xffff0, reset_vector, sizeof(reset_vector));
    /* The formal SAS initialization installs ROM protection. This test owns
     * its synthetic image: fill its backing explicitly, without asking guest
     * writes to overwrite ROM or changing the product ROM policy. */
    CopyMemory(c_GetPhyAdd(0xf1000), program, sizeof(program));
    CopyMemory(c_GetPhyAdd(0xffff0), reset_vector, sizeof(reset_vector));
    *c_GetPhyAdd(0x200) = 0;
    producer = CreateThread(NULL, 0, raise_reset, c_GetPhyAdd(0x200), 0, NULL);
    if (!producer) return 4;
    c_setIP(0x1000);
    report("simulate\n");
    c_cpu_simulate();
    ax = c_getAX();
    if (WaitForSingleObject(producer, 6000) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(producer, &producer_result)) return 5;
    CloseHandle(producer);
    wsprintfA(message, "original-CCPU HALT RESET: AX=%04x producer=%lu\n", ax, producer_result);
    report(message);
    /* BEEF proves execution restarted at the actual c_cpu_reset vector;
     * DEAD means it merely left HLT and continued after the instruction. */
    if (ax != 0xbeef || producer_result != 0) return 1;
    if (!debug_fault_cancellation()) return 7;
    report("S38_CCPU_DIV_FAULT_CANCELS_PENDING_DEBUG_OK\n");
    if (!debug_resume_flag()) return 8;
    report("S38_CCPU_RF_SUPPRESS_ONCE_THEN_BREAK_OK\n");
    if (!debug_descriptor_preflight()) return 9;
    report("S38_CCPU_REAL_DESCRIPTOR_PAGING_REFUSAL_OK\n");
    if (!debug_task_switch(1) || !debug_task_switch(0)) return 10;
    report("S38_CCPU_TSS_TBIT_SINGLE_DELIVERY_OK\n");
    return 0;
}
