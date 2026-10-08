/* Read-only, non-atomic diagnostic snapshots of a selected x86 test worker.
 * Layout: original ccpu386/c_reg.h, not the generated CVID GDP cache.
 * Symbol locations come from the exact deployed ntvdm.exe link map.
 * No remote calls, writes, injection, thread suspension or CPU patching. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct segment_register {
    uint16_t selector, padding;
    uint32_t dpl, expansion, readable, writable, conforming, big, base, limit;
} segment_register;

typedef struct virtual_ica {
    LONG count[8];
    LONG int_line;
    LONG cpu_int;
    WORD base, hipri, mode;
    BYTE master, irr, isr, imr, ssr;
} virtual_ica;

static DWORD map_address(FILE *map, const char *wanted)
{
    char line[1024], section[64], symbol[256];
    unsigned long address;
    rewind(map);
    while (fgets(line, sizeof(line), map)) {
        if (sscanf_s(line, "%63s %255s %lx", section, (unsigned)sizeof(section),
                symbol, (unsigned)sizeof(symbol), &address) == 3 &&
            !strcmp(symbol, wanted)) return address;
    }
    return 0;
}

static int read_exact(HANDLE process, DWORD address, void *buffer, SIZE_T count)
{
    SIZE_T read = 0;
    return ReadProcessMemory(process, (const void *)(uintptr_t)address,
        buffer, count, &read) && read == count;
}

static void print_bytes(HANDLE process, DWORD backing, DWORD length,
    DWORD linear, const char *name)
{
    BYTE bytes[32]; unsigned i;
    if (linear > length || sizeof(bytes) > length - linear ||
        !read_exact(process, backing + linear, bytes, sizeof(bytes))) {
        printf("%s unavailable linear=%08lX\n", name, linear);
        return;
    }
    printf("%s linear=%08lX", name, linear);
    for (i = 0; i < sizeof(bytes); ++i) printf(" %02X", bytes[i]);
    putchar('\n');
}

int main(int argc, char **argv)
{
    DWORD pid, actual = 0, preferred = 0, symbols[15], index, sample;
    DWORD suspended_tid = 0;
    HANDLE suspended_thread = NULL;
    HANDLE snapshot, process;
    MODULEENTRY32 module = {0};
    FILE *map;
    char line[1024];
    const char *names[] = {"_CCPU_IP", "_CCPU_GR", "_CCPU_SR",
        "_CCPU_CR", "_Start_of_M_area", "_Length_of_M_area", "_nt_init_called",
        "_CCPU_FLAGS", "_DelayIrqLine", "_cpu_heartbeat", "_quick_mode",
        "_PigSynchCount", "_DelayHandle"};
    const char *ica_name = "_VirtualIca";
    const char *interrupt_map_name = "_cpu_interrupt_map";
    const char *pig_sync_name = "_PigSynchCount";
    const char *delay_handles_name = "_DelayHandle";
    if ((argc != 3 && argc != 4) || !(pid = strtoul(argv[1], NULL, 10)) ||
        fopen_s(&map, argv[2], "r")) return 64;
    if (argc == 4 && !(suspended_tid = strtoul(argv[3], NULL, 10))) return 64;
    while (fgets(line, sizeof(line), map)) {
        if (sscanf_s(line, " Preferred load address is %lx", &preferred) == 1) break;
    }
    if (!preferred || sizeof(segment_register) != 36) return 65;
    for (index = 0; index < 11; ++index) {
        if (!(symbols[index] = map_address(map, names[index]))) {
            fprintf(stderr, "missing symbol %s\n", names[index]); return 66;
        }
    }
    if (!(symbols[11] = map_address(map, ica_name))) {
        fprintf(stderr, "missing symbol %s\n", ica_name); return 66;
    }
    if (!(symbols[12] = map_address(map, interrupt_map_name))) {
        fprintf(stderr, "missing symbol %s\n", interrupt_map_name); return 66;
    }
    if (!(symbols[13] = map_address(map, pig_sync_name))) {
        fprintf(stderr, "missing symbol %s\n", pig_sync_name); return 66;
    }
    if (!(symbols[14] = map_address(map, delay_handles_name))) {
        fprintf(stderr, "missing symbol %s\n", delay_handles_name); return 66;
    }
    fclose(map);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    module.dwSize = sizeof(module);
    if (snapshot != INVALID_HANDLE_VALUE && Module32First(snapshot, &module)) {
        do {
            if (!_stricmp(module.szModule, "ntvdm.exe")) {
                actual = (DWORD)(uintptr_t)module.modBaseAddr;
                break;
            }
            module.dwSize = sizeof(module);
        } while (Module32Next(snapshot, &module));
    }
    if (snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
    if (!actual) return 67;
    process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!process) return 68;
    if (suspended_tid) {
        suspended_thread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, suspended_tid);
        if (!suspended_thread || SuspendThread(suspended_thread) == (DWORD)-1) return 70;
        printf("suspended-tid=%lu\n", (unsigned long)suspended_tid);
    }
    printf("pid=%lu image=%s base=%08lX preferred=%08lX non-atomic=1\n",
        pid, module.szExePath, actual, preferred);
    for (index = 0; index < 15; ++index) symbols[index] = actual + symbols[index] - preferred;
    for (sample = 0; sample < 3; ++sample) {
        DWORD ip, gr[8], cr[8], backing, length, input_initialized, interrupt_map, flags[32], delay_irq;
        DWORD heartbeat, quick_mode, pig_sync;
        LONGLONG delay_handles[16];
        segment_register sr[6];
        virtual_ica ica[2];
        if (!read_exact(process, symbols[0], &ip, sizeof(ip)) ||
            !read_exact(process, symbols[1], gr, sizeof(gr)) ||
            !read_exact(process, symbols[2], sr, sizeof(sr)) ||
            !read_exact(process, symbols[3], cr, sizeof(cr)) ||
            !read_exact(process, symbols[4], &backing, sizeof(backing)) ||
            !read_exact(process, symbols[5], &length, sizeof(length)) ||
            !read_exact(process, symbols[6], &input_initialized, sizeof(input_initialized)) ||
            !read_exact(process, symbols[7], flags, sizeof(flags)) ||
            !read_exact(process, symbols[8], &delay_irq, sizeof(delay_irq)) ||
            !read_exact(process, symbols[9], &heartbeat, sizeof(heartbeat)) ||
            !read_exact(process, symbols[10], &quick_mode, sizeof(quick_mode)) ||
            !read_exact(process, symbols[11], ica, sizeof(ica)) ||
            !read_exact(process, symbols[12], &interrupt_map, sizeof(interrupt_map)) ||
            !read_exact(process, symbols[13], &pig_sync, sizeof(pig_sync)) ||
            !read_exact(process, symbols[14], delay_handles, sizeof(delay_handles))) return 69;
        printf("nt_init_called=%lu\n", input_initialized);
        printf("sample=%lu ip=%08lX cr0=%08lX backing=%08lX length=%08lX\n",
            sample, ip, cr[0], backing, length);
        printf("ccpu-if=%lu cpu_interrupt_map=%08lX delay-irq=%08lX heartbeat=%lu quick-mode=%lu pig-sync=%lu delay-handle0=%lld\n",
            flags[9], interrupt_map, delay_irq, heartbeat, quick_mode, pig_sync,
            delay_handles[0]);
        printf("eax=%08lX ecx=%08lX edx=%08lX ebx=%08lX esp=%08lX ebp=%08lX esi=%08lX edi=%08lX\n",
            gr[0], gr[1], gr[2], gr[3], gr[4], gr[5], gr[6], gr[7]);
        for (index = 0; index < 6; ++index)
            printf("sr%lu=%04X base=%08X limit=%08X big=%u\n", index,
                sr[index].selector, sr[index].base, sr[index].limit, sr[index].big);
        for (index = 0; index < 2; ++index) {
            printf("ica%lu base=%02X irr=%02X isr=%02X imr=%02X line=%ld cpu=%ld count5=%ld\n",
                index, (unsigned)ica[index].base, (unsigned)ica[index].irr,
                (unsigned)ica[index].isr, (unsigned)ica[index].imr,
                (long)ica[index].int_line, (long)ica[index].cpu_int,
                (long)ica[index].count[5]);
        }
        print_bytes(process, backing, length, sr[1].base + ip, "code");
        if (ip >= 16) print_bytes(process, backing, length, sr[1].base + ip - 16, "code-before");
        print_bytes(process, backing, length, sr[2].base + (gr[4] & 0xffff), "stack");
        if (!(cr[0] & 1)) {
            WORD old_vector[2];
            DWORD vector_linear = sr[1].base + (gr[5] & 0xffff);
            WORD irq5_vector[2];
            print_bytes(process, backing, length, vector_linear, "cs:bp");
            if (read_exact(process, backing + vector_linear, old_vector,
                    sizeof(old_vector))) {
                printf("cs:bp-far-vector=%04X:%04X\n", old_vector[1],
                    old_vector[0]);
                print_bytes(process, backing, length,
                    (DWORD)old_vector[1] * 16 + old_vector[0],
                    "old-vector-target");
            }
            if (read_exact(process, backing + sr[1].base + 0x008a,
                    irq5_vector, sizeof(irq5_vector))) {
                printf("int0d-old-vector=%04X:%04X\n", irq5_vector[1],
                    irq5_vector[0]);
                print_bytes(process, backing, length,
                    (DWORD)irq5_vector[1] * 16 + irq5_vector[0],
                    "int0d-old-target");
            }
            print_bytes(process, backing, length, 0x00000714,
                "bios-data-40:0314");
            if (sr[1].selector >= 0x11u) {
                print_bytes(process, backing, length,
                    ((DWORD)sr[1].selector - 0x11u) * 16u,
                    "cs-near-mcb");
            }
        }
        if (!(cr[0] & 1)) {
            WORD frame[3];
            if (read_exact(process, backing + sr[2].base + (gr[4] & 0xffff), frame, sizeof(frame))) {
                printf("real-mode-stack-frame=%04X:%04X flags=%04X\n", frame[1], frame[0], frame[2]);
                print_bytes(process, backing, length, (DWORD)frame[1] * 16, "fault-segment-start");
                print_bytes(process, backing, length, (DWORD)frame[1] * 16 + 0x332, "command-map-endinit-candidate");
                { WORD value;
                    if (read_exact(process, backing + (DWORD)frame[1] * 16 + 0x203c, &value, sizeof(value)))
                        printf("command-map-envsiz-candidate=%04X\n", value);
                }
                print_bytes(process, backing, length, (DWORD)frame[1] * 16 + frame[0], "fault-candidate");
            }
        }
        if (sample != 2 && !suspended_thread) Sleep(100);
    }
    if (suspended_thread) { (void)ResumeThread(suspended_thread); CloseHandle(suspended_thread); }
    CloseHandle(process);
    return 0;
}
