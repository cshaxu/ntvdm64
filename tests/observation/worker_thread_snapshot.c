/* Reuse the existing test observer's bounded suspend/read/resume stack
 * capture. This does not launch a workload or alter product code. */
#define main native_console_observer_main
#include "native_console_observer.c"
#undef main
#include <stdlib.h>

int main(int argc, char **argv)
{
    DWORD target;
    if (argc != 2 || !(target = strtoul(argv[1], NULL, 10))) return 64;
    record_modules(stdout, target);
    record_thread_ips(stdout, target);
    return 0;
}
