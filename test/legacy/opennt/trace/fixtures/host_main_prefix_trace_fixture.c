/*
 * Default-disabled trace of the original SoftPC host_main entry prefix.
 * The local host_applInit definition is a fail-closed observation stop at
 * main.c's first historical host boundary; it supplies no host behavior.
 */
#include <setjmp.h>
#include <string.h>

extern int host_main(int argc, char **argv);
extern int *pargc;
extern char **pargv;

static jmp_buf host_main_stop_environment;
static int host_main_observation;

void host_applInit(int argc, char **argv) {
    if (pargc != 0 && *pargc == argc && pargv == argv && argc == 2 &&
        argv != 0 && strcmp(argv[1], "-f") == 0) {
        host_main_observation = 1;
    }
    longjmp(host_main_stop_environment, 1);
}

int main(void) {
    char program_name[] = "ntvdm";
    char internal_flag[] = "-f";
    char *argv[] = {program_name, internal_flag, 0};

    if (setjmp(host_main_stop_environment) == 0) {
        (void)host_main(2, argv);
        return 1;
    }
    return host_main_observation == 1 ? 0 : 2;
}
