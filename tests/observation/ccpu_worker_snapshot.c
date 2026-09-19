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
    DWORD pid, actual = 0, preferred = 0, symbols[7], index, sample;
    HANDLE snapshot, process;
    MODULEENTRY32 module = {0};
    FILE *map;
    char line[1024];
    const char *names[] = {"_CCPU_IP", "_CCPU_GR", "_CCPU_SR",
        "_CCPU_CR", "_Start_of_M_area", "_Length_of_M_area", "_nt_init_called"};
    if (argc != 3 || !(pid = strtoul(argv[1], NULL, 10)) ||
        fopen_s(&map, argv[2], "r")) return 64;
    while (fgets(line, sizeof(line), map)) {
        if (sscanf_s(line, " Preferred load address is %lx", &preferred) == 1) break;
    }
    if (!preferred || sizeof(segment_register) != 36) return 65;
    for (index = 0; index < 7; ++index) {
        if (!(symbols[index] = map_address(map, names[index]))) {
            fprintf(stderr, "missing symbol %s\n", names[index]); return 66;
        }
    }
    fclose(map);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    module.dwSize = sizeof(module);
    if (snapshot != INVALID_HANDLE_VALUE && Module32First(snapshot, &module)) {
        if (!_stricmp(module.szModule, "ntvdm.exe")) actual = (DWORD)(uintptr_t)module.modBaseAddr;
    }
    if (snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
    if (!actual) return 67;
    process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!process) return 68;
    printf("pid=%lu image=%s base=%08lX preferred=%08lX non-atomic=1\n",
        pid, module.szExePath, actual, preferred);
    for (index = 0; index < 7; ++index) symbols[index] = actual + symbols[index] - preferred;
    for (sample = 0; sample < 3; ++sample) {
        DWORD ip, gr[8], cr[8], backing, length, input_initialized;
        segment_register sr[6];
        if (!read_exact(process, symbols[0], &ip, sizeof(ip)) ||
            !read_exact(process, symbols[1], gr, sizeof(gr)) ||
            !read_exact(process, symbols[2], sr, sizeof(sr)) ||
            !read_exact(process, symbols[3], cr, sizeof(cr)) ||
            !read_exact(process, symbols[4], &backing, sizeof(backing)) ||
            !read_exact(process, symbols[5], &length, sizeof(length)) ||
            !read_exact(process, symbols[6], &input_initialized, sizeof(input_initialized))) return 69;
        printf("nt_init_called=%lu\n", input_initialized);
        printf("sample=%lu ip=%08lX cr0=%08lX backing=%08lX length=%08lX\n",
            sample, ip, cr[0], backing, length);
        printf("eax=%08lX ecx=%08lX edx=%08lX ebx=%08lX esp=%08lX ebp=%08lX esi=%08lX edi=%08lX\n",
            gr[0], gr[1], gr[2], gr[3], gr[4], gr[5], gr[6], gr[7]);
        for (index = 0; index < 6; ++index)
            printf("sr%lu=%04X base=%08X limit=%08X big=%u\n", index,
                sr[index].selector, sr[index].base, sr[index].limit, sr[index].big);
        print_bytes(process, backing, length, sr[1].base + ip, "code");
        if (ip >= 16) print_bytes(process, backing, length, sr[1].base + ip - 16, "code-before");
        print_bytes(process, backing, length, sr[2].base + (gr[4] & 0xffff), "stack");
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
        if (sample != 2) Sleep(100);
    }
    CloseHandle(process);
    return 0;
}
