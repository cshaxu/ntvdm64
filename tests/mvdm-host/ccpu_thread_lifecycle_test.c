#include <stdint.h>
#include <windows.h>

#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"

extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern void c_cpu_init(void);
extern void c_cpu_simulate(void);
extern void c_setIP(uint16_t value);
extern unsigned char *c_GetPhyAdd(uint32_t address);
extern HANDLE WINAPI host_CreateThread(LPSECURITY_ATTRIBUTES attributes,
    DWORD stack_size, LPTHREAD_START_ROUTINE start, LPVOID parameter,
    DWORD flags, LPDWORD thread_id);
extern VOID WINAPI host_ExitThread(DWORD exit_code);

static void report(const char *text)
{
    DWORD written;

    (void)WriteFile(GetStdHandle(STD_ERROR_HANDLE), text,
        (DWORD)lstrlenA(text), &written, NULL);
}

typedef struct worker_exit_request {
    DWORD exit_code;
} worker_exit_request;

static DWORD WINAPI run_guest_and_exit(LPVOID parameter)
{
    const worker_exit_request *request = (const worker_exit_request *)parameter;

    /* This is the original direct CCPU unsimulate BOP.  It causes the
     * simulator to return through the TLS frame created for this host thread.
     * It is not a product BOP handler replacement. */
    c_setIP(UINT16_C(0xfff0));
    c_cpu_simulate();
    host_ExitThread(request->exit_code);
    return 1u;
}

int main(void)
{
    session owner;
    HANDLE thread;
    DWORD exit_code;
    worker_exit_request abnormal = { 53u };
    worker_exit_request normal = { 0u };
    static const unsigned char guest_exit[] = { 0xd6, 0xfe };

    session_initialize(&owner, 420u);
    if (!session_activate(&owner) || !session_thread_bind(&owner) ||
        setup_global_data_ptr() == NULL)
        return 1;
    sas_init(UINT32_C(0x00200000));
    if (!mvdm_softpc_guest_memory_begin(&owner)) {
        sas_term();
        return 2;
    }
    setup_vga_globals();
    c_cpu_init();
    CopyMemory(c_GetPhyAdd(UINT32_C(0x000ffff0)), guest_exit,
        sizeof(guest_exit));

    /* A nonzero worker end must run the original host_ExitThread cleanup
     * before the next worker obtains its own CCPU TLS/jmp frame. */
    thread = host_CreateThread(NULL, 0u, run_guest_and_exit, &abnormal, 0u, NULL);
    if (thread == NULL) return 3;
    if (WaitForSingleObject(thread, 5000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread, &exit_code)) {
        CloseHandle(thread);
        return 4;
    }
    CloseHandle(thread);
    if (exit_code != abnormal.exit_code) return 5;

    thread = host_CreateThread(NULL, 0u, run_guest_and_exit, &normal, 0u, NULL);
    if (thread == NULL) return 6;
    if (WaitForSingleObject(thread, 5000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread, &exit_code)) {
        CloseHandle(thread);
        return 7;
    }
    CloseHandle(thread);
    mvdm_softpc_guest_memory_end(&owner);
    if (!session_thread_unbind(&owner) || !session_dispose(&owner)) {
        sas_term();
        return 8;
    }
    sas_term();
    if (exit_code != normal.exit_code) return 9;
    report("CCPU thread lifecycle normal-and-abnormal OK\n");
    return 0;
}
