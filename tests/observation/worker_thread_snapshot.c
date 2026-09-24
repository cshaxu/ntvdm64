/* Reuse the existing test observer's bounded suspend/read/resume stack
 * capture. This does not launch a workload or alter product code. */
#define main native_console_observer_main
#include "native_console_observer.c"
#undef main
#include <stdlib.h>
#include <wct.h>

static void record_wait_chains(DWORD target)
{
    HWCT session = OpenThreadWaitChainSession(0, NULL);
    HANDLE snapshot;
    THREADENTRY32 entry = {0};
    if (!session) {
        printf("wait-chain-open-error=%lu\n", GetLastError());
        return;
    }
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    entry.dwSize = sizeof(entry);
    if (snapshot != INVALID_HANDLE_VALUE && Thread32First(snapshot, &entry)) {
        do {
            WAITCHAIN_NODE_INFO nodes[WCT_MAX_NODE_COUNT];
            DWORD count = WCT_MAX_NODE_COUNT, index;
            BOOL cycle = FALSE;
            if (entry.th32OwnerProcessID != target) continue;
            if (!GetThreadWaitChain(session, 0, WCT_OUT_OF_PROC_FLAG,
                    entry.th32ThreadID, &count, nodes, &cycle)) {
                printf("wait-chain thread=%lu error=%lu\n",
                    entry.th32ThreadID, GetLastError());
                continue;
            }
            printf("wait-chain thread=%lu nodes=%lu cycle=%u\n",
                entry.th32ThreadID, count, cycle);
            for (index = 0; index < count; ++index) {
                printf("wait-node index=%lu type=%u status=%u", index,
                    nodes[index].ObjectType, nodes[index].ObjectStatus);
                if (nodes[index].ObjectType == WctThreadType)
                    printf(" pid=%lu tid=%lu", nodes[index].ThreadObject.ProcessId,
                        nodes[index].ThreadObject.ThreadId);
                putchar('\n');
            }
        } while (Thread32Next(snapshot, &entry));
    }
    if (snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
    CloseThreadWaitChainSession(session);
}

int main(int argc, char **argv)
{
    DWORD target;
    if (argc != 2 || !(target = strtoul(argv[1], NULL, 10))) return 64;
    record_modules(stdout, target);
    record_thread_ips(stdout, target);
    record_wait_chains(target);
    return 0;
}
