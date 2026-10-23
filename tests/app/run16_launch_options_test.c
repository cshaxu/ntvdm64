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
        { L"--wait x", L"x", 1 },
        { L"-- --wait x", L"-- --wait x", 0 },
        { L"\"--wait\" x", L"\"--wait\" x", 0 },
        { L"--wait.exe x", L"--wait.exe x", 0 },
        { L"--waited x", L"--waited x", 0 },
        { L"--wait=1 x", L"--wait=1 x", 0 },
        { L"--wait \"C:\\guest apps\\winmine.exe\"  ", L"\"C:\\guest apps\\winmine.exe\"  ", 1 },
        { L"--wait", NULL, 1 },
        { L"--wait --wait x", NULL, 1 },
        { L"--wait --", L"--", 1 },
        { L"--", L"--", 0 },
        { L" \t", NULL, 0 },
        { L"", NULL, 0 },
        { NULL, NULL, 0 },
        { L"--win16-window-style x", L"--win16-window-style x", 0 },
        { L"--win16-window-style-all x", L"--win16-window-style-all x", 0 },
        { L"--win16-style.exe /x", L"--win16-style.exe /x", 0 },
        { L"\"--win16-style\" x", L"\"--win16-style\" x", 0 }
    };
    static const struct {
        const wchar_t *line, *tail, *target;
        int wait;
    } tail_cases[] = {
        { L"run16.exe cmd /c ver", L"cmd /c ver", L"cmd /c ver", 0 },
        { L"  \"C:\\Program Files\\run16.exe\" --wait cmd /c ver", L"--wait cmd /c ver", L"cmd /c ver", 1 },
        { L"\"O:\\winnt\\system32\\run16.exe\" command /c ver > out.txt", L"command /c ver > out.txt", L"command /c ver > out.txt", 0 },
        { L"\"O:\\winnt\\system32\\run16.exe\" --wait command /c \"ver >> out.txt | findstr Version\"", L"--wait command /c \"ver >> out.txt | findstr Version\"", L"command /c \"ver >> out.txt | findstr Version\"", 1 },
        { L"run16.exe \"C:\\guest apps\\tool.exe\" \"literal > >> | argument\"", L"\"C:\\guest apps\\tool.exe\" \"literal > >> | argument\"", L"\"C:\\guest apps\\tool.exe\" \"literal > >> | argument\"", 0 }
    };
    unsigned i, count = (unsigned)(sizeof(cases)/sizeof(cases[0]));
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
    if (run16_command_line_tail(NULL) != NULL) return 5;
    for (i = 0; i < sizeof(tail_cases)/sizeof(tail_cases[0]); ++i) {
        run16_launch_options parsed;
        const wchar_t *tail = run16_command_line_tail(tail_cases[i].line);
        if (!tail || wcscmp(tail, tail_cases[i].tail) ||
            !run16_parse_launch_options(tail, &parsed) ||
            parsed.wait != tail_cases[i].wait ||
            wcscmp(parsed.command, tail_cases[i].target)) {
            fprintf(stderr, "FAIL raw-tail case %u\n", i);
            return 6;
        }
    }
    {
        run16_launch_options parsed;
        if (run16_parse_launch_options(L"--win16-style command/c ver", &parsed) ||
            run16_parse_launch_options(L"--win16-style-all x", &parsed)) return 4;
    }
    printf("RUN16-LAUNCH-OPTIONS-PASS cases=%u\n", count);
    return 0;
}
