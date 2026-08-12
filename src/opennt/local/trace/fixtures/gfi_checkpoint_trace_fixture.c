/* Trace-only check of the gfi_init -> startup checkpoint control transfer. */
#include <setjmp.h>

extern void gfi_init(void);
extern jmp_buf ntdos64_config_prefix_stop_environment;
extern void ntdos64_startup_prefix_trace_checkpoint(int stage);

int main(void)
{
    int stop_result = setjmp(ntdos64_config_prefix_stop_environment);

    if (stop_result == 0) {
        gfi_init();
        ntdos64_startup_prefix_trace_checkpoint(2);
        return 2;
    }

    return stop_result == 142 ? 0 : 1;
}
