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
    unsigned i, count = (unsigned)(sizeof(cases)/sizeof(cases[0]));
    for (i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        run16_launch_options parsed;
        int ok = run16_parse_launch_options(cases[i].input, &parsed);
        if (ok != (cases[i].target != NULL) || parsed.wait != cases[i].wait ||
            (ok && wcscmp(parsed.command, cases[i].target)) ||
            (!ok && parsed.command != NULL) || parsed.window_style != WINDOW_STYLE_OFF ||
            parsed.window_style_explicit) {
            fprintf(stderr, "FAIL launch-options case %u\n", i);
            return 1;
        }
        /* No allocation or re-serialization: preserve the exact target tail. */
        if (ok && (parsed.command < cases[i].input ||
            parsed.command > cases[i].input + wcslen(cases[i].input))) return 2;
    }
    if (run16_parse_launch_options(L"x", NULL)) return 3;
    {
        static const struct {
            const wchar_t *input,*target;
            unsigned style;
            int wait;
        } style_cases[] = {
            {L"--win16-style command/c ver",L"command/c ver",WINDOW_STYLE_WIN16,0},
            {L"--win16-style-all --wait \"a b.exe\"  /x",L"\"a b.exe\"  /x",WINDOW_STYLE_ALL,1},
            {L"--wait --win16-style x",L"x",WINDOW_STYLE_WIN16,1},
            {L"--win16-style-all x --win16-style",L"x --win16-style",WINDOW_STYLE_ALL,0},
            {L"--win16-style",NULL,WINDOW_STYLE_WIN16,0},
            {L"--win16-style --win16-style-all x",NULL,WINDOW_STYLE_WIN16,0},
            {L"--win16-style-all --win16-style-all x",NULL,WINDOW_STYLE_ALL,0},
            {L"--win16-style-all --win16-style x",NULL,WINDOW_STYLE_ALL,0},
            {L"--win16-style --win16-style x",NULL,WINDOW_STYLE_WIN16,0},
            {L"--win16-style-all",NULL,WINDOW_STYLE_ALL,0},
            {L"\t--win16-style\t--wait\tcommand/c ver  ",L"command/c ver  ",WINDOW_STYLE_WIN16,1},
            {L"--win16-style-all --wait --wait x",NULL,WINDOW_STYLE_ALL,1}
        };
        count += (unsigned)(sizeof(style_cases)/sizeof(style_cases[0]));
        for(i=0;i<sizeof(style_cases)/sizeof(style_cases[0]);++i){
            run16_launch_options parsed;
            int ok=run16_parse_launch_options(style_cases[i].input,&parsed);
            if(ok!=(style_cases[i].target!=NULL) || !parsed.window_style_explicit ||
               parsed.window_style!=style_cases[i].style || parsed.wait!=style_cases[i].wait ||
               (ok && wcscmp(parsed.command,style_cases[i].target)))return 4;
        }
    }
    printf("RUN16-LAUNCH-OPTIONS-PASS cases=%u\n", count);
    return 0;
}
