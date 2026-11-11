#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
extern uint16_t c_getAX(void);
extern unsigned char *c_GetPhyAdd(uint32_t address);
extern HANDLE WINAPI host_CreateThread(LPSECURITY_ATTRIBUTES attributes,
    DWORD stack_size, LPTHREAD_START_ROUTINE start, LPVOID parameter,
    DWORD flags, LPDWORD thread_id);

/* A test-only real-mode instruction stream.  It runs ten million iterations
 * of INC AX / DEC ECX / JNZ, then returns through the original CCPU direct
 * unsimulate BOP.  It neither supplies a decoder nor changes CCPU state. */
static const unsigned char throughput_guest_template[] = {
    0x66, 0xb9, 0x80, 0x96, 0x98, 0x00, /* mov ecx,10000000 */
    0x40,                               /* inc ax */
    0x66, 0x49,                         /* dec ecx */
    0x75, 0xfb,                         /* jnz inc ax */
    0xd6, 0xfe                          /* original direct unsimulate */
};

typedef struct throughput_result {
    ULONGLONG elapsed_ms;
    uint16_t ax;
} throughput_result;

static DWORD WINAPI run_throughput(LPVOID parameter)
{
    throughput_result *result = (throughput_result *)parameter;
    ULONGLONG start = GetTickCount64();

    c_setIP(UINT16_C(0xfff0));
    c_cpu_simulate();
    result->elapsed_ms = GetTickCount64() - start;
    result->ax = c_getAX();
    return 0u;
}

int main(void)
{
    session owner;
    throughput_result result = { 0u, 0u };
    HANDLE thread;
    DWORD exit_code;
    const char *requested_iterations = getenv("CCPU_THROUGHPUT_ITERATIONS");
    uint32_t iterations = UINT32_C(10000000);
    unsigned char throughput_guest[sizeof(throughput_guest_template)];

    if (requested_iterations != NULL && requested_iterations[0] != '\0') {
        char *end = NULL;
        unsigned long parsed = strtoul(requested_iterations, &end, 10);

        if (end == requested_iterations || *end != '\0' || parsed == 0u ||
            parsed > UINT32_C(200000000))
            return 7;
        iterations = (uint32_t)parsed;
    }

    session_initialize(&owner, 441u);
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
    CopyMemory(throughput_guest, throughput_guest_template,
        sizeof(throughput_guest));
    CopyMemory(throughput_guest + 2u, &iterations, sizeof(iterations));
    CopyMemory(c_GetPhyAdd(UINT32_C(0x000ffff0)), throughput_guest,
        sizeof(throughput_guest));

    thread = host_CreateThread(NULL, 0u, run_throughput, &result, 0u, NULL);
    if (thread == NULL)
        return 3;
    if (WaitForSingleObject(thread, 30000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread, &exit_code)) {
        CloseHandle(thread);
        return 4;
    }
    CloseHandle(thread);
    mvdm_softpc_guest_memory_end(&owner);
    if (!session_thread_unbind(&owner) || !session_dispose(&owner)) {
        sas_term();
        return 5;
    }
    sas_term();
    if (exit_code != 0u || result.ax != (uint16_t)iterations ||
        result.elapsed_ms == 0u)
        return 6;
    fprintf(stderr, "CCPU_THROUGHPUT iterations=%lu guest_instructions=%llu elapsed_ms=%llu ips=%llu\n",
        (unsigned long)iterations,
        (unsigned long long)((uint64_t)iterations * 3u + 2u),
        (unsigned long long)result.elapsed_ms,
        (unsigned long long)(((uint64_t)iterations * 3u + 2u) * 1000u / result.elapsed_ms));
    return 0;
}
