/*
 * Bounded, default-disabled probe of the original OpenNT host_applInit.
 * It supplies the historical internal -f argument and exits immediately
 * at a selected original lifecycle checkpoint. The deepest checkpoint is
 * after the historical event-thread creation function returns; that thread
 * remains CREATE_SUSPENDED and this fixture never resumes its event loop.
 */
#include <windows.h>
#include <setjmp.h>
#include <stdlib.h>

#ifndef NTDOS64_HOST_APPLINIT_TRACE_STOP_STAGE
#define NTDOS64_HOST_APPLINIT_TRACE_STOP_STAGE 11
#endif

extern void host_applInit(int argc, char **argv);

static jmp_buf host_applinit_stop_environment;
static int host_applinit_stop_reason;
static int host_applinit_stop_stage = NTDOS64_HOST_APPLINIT_TRACE_STOP_STAGE;

void ntdos64_host_applinit_trace_checkpoint(int stage) {
    if (stage == host_applinit_stop_stage) {
        host_applinit_stop_reason = stage;
        longjmp(host_applinit_stop_environment, stage);
    }
}

int main(int argc, char **argv) {
    char program_name[] = "ntvdm";
    char internal_flag[] = "-f";
    char stream_io_off[] = "-o";
    char *host_argv[] = {program_name, internal_flag, stream_io_off, 0};
    int result;

    if (argc == 2) {
        int parsed_stage = atoi(argv[1]);
        if (parsed_stage >= 10 && parsed_stage <= 31)
            host_applinit_stop_stage = parsed_stage;
    }

    result = setjmp(host_applinit_stop_environment);

    if (result == 0) {
        host_applInit(3, host_argv);
        return 1;
    }
    if (result == host_applinit_stop_stage &&
        host_applinit_stop_reason == host_applinit_stop_stage)
        return 0;

    /* A nonzero stage is diagnostic-only and never indicates progress. */
    return host_applinit_stop_reason > 0 && host_applinit_stop_reason < 200
               ? 20 + host_applinit_stop_reason
               : 2;
}
