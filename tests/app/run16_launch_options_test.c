#include "../../src/run16-exe/launch_options.h"
#include <stdio.h>

int main(void)
{
    static const struct {
        const wchar_t *input, *target;
        int wait;
    } cases[] = {
        { L"command/c ver", L"command/c ver", 0 },
        { L"  --wait\t\"a b.exe\"  /x \"a\\\"b\"", L"\"a b.exe\"  /x \"a\\\"b\"", 1 },
        { L"cmd.exe /c echo --wait", L"cmd.exe /c echo --wait", 0 },
        { L"--wait command.com /c \"ver >> out.txt\"", L"command.com /c \"ver >> out.txt\"", 1 },
        { L"-- --wait x", L"--wait x", 0 },
        { L"--wait -- --wait x", L"--wait x", 1 },
        { L"\"--wait\" x", L"\"--wait\" x", 0 },
        { L"--wait.exe x", L"--wait.exe x", 0 },
        { L"--waited x", L"--waited x", 0 },
        { L"--wait=1 x", L"--wait=1 x", 0 },
        { L"--wait \"C:\\guest apps\\winmine.exe\"  ", L"\"C:\\guest apps\\winmine.exe\"  ", 1 },
        { L"--wait", NULL, 1 },
        { L"--wait --wait x", NULL, 1 },
        { L"--wait --", NULL, 1 },
        { L"--", NULL, 0 },
        { L" \t", NULL, 0 },
        { L"", NULL, 0 },
        { NULL, NULL, 0 }
    };
    unsigned i;
    for (i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        run16_launch_options parsed;
        int ok = run16_parse_launch_options(cases[i].input, &parsed);
        if (ok != (cases[i].target != NULL) || parsed.wait != cases[i].wait ||
            (ok && wcscmp(parsed.command, cases[i].target)) ||
            (!ok && parsed.command != NULL)) {
            fprintf(stderr, "FAIL launch-options case %u\n", i);
            return 1;
        }
        /* No allocation or re-serialization: preserve the exact target tail. */
        if (ok && (parsed.command < cases[i].input ||
            parsed.command > cases[i].input + wcslen(cases[i].input))) return 2;
    }
    if (run16_parse_launch_options(L"x", NULL)) return 3;
    puts("RUN16-LAUNCH-OPTIONS-PASS cases=19");
    return 0;
}
