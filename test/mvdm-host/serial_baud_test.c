/* Execute the production HOST_COM_BAUD arm with a bounded serial provider. */
#include <windows.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define HOST_COM_BAUD 1
#define EHS_UNSUPPORTED_BAUD 2
#define ERR_CONT 3
static struct { HANDLE handle; struct { DWORD BaudRate; } ComStates; } state;
static int forwarded, succeed, calls, errors, traces;
static char error_text[32], trace_text[80];
static BOOL FastCommSetBaudRate(HANDLE handle, int baud)
{
    assert(handle == state.handle);
    assert(state.ComStates.BaudRate == 123);
    forwarded = baud; ++calls;
    return succeed;
}
static void host_error(int error, int action, const char *text)
{
    assert(error == EHS_UNSUPPORTED_BAUD && action == ERR_CONT);
    assert(calls == 1 && traces == 0 && state.ComStates.BaudRate == 123);
    strcpy(error_text, text); ++errors;
}
#define always_trace1(format, value) do { assert(errors == 1);     sprintf(trace_text, format, value); ++traces; } while (0)
static void apply(long arg)
{
    char BaudRateStr[32];
#define current (&state)
    switch (HOST_COM_BAUD) {
#include "serial_baud_arm.inc"
    }
#undef current
}
int main(void)
{
    const long values[] = {0, 9600, 115200, LONG_MAX, -1, LONG_MIN};
    unsigned i;
    for (succeed = 0; succeed <= 1; ++succeed) {
        for (i = 0; i < sizeof(values)/sizeof(values[0]); ++i) {
            char expected[80];
            state.handle = (HANDLE)1; state.ComStates.BaudRate = 123;
            calls = errors = traces = 0;
            apply(values[i]);
            assert(calls == 1 && forwarded == (int)values[i]);
            assert(state.ComStates.BaudRate == (DWORD)values[i]);
            assert(errors == !succeed && traces == !succeed);
            if (!succeed) {
                sprintf(expected, "(%ld)", values[i]);
                assert(strcmp(error_text, expected) == 0);
                sprintf(expected, "set BAUD failed - SetBaudRate:%ld", values[i]);
                assert(strcmp(trace_text, expected) == 0);
            }
        }
    }
    puts("PASS baud forwarding, failure ordering and saved state: 12 cases");
    return 0;
}
