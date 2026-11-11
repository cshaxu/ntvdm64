/* Actual product dispatcher object; sentinel handlers prove routing only. */
#include <stdio.h>
static unsigned selected, calls;
extern int CmdDispatch(unsigned long);
void cmdExitVDM(void) { selected=0; ++calls; }
void cmdGetNextCmd(void) { selected=1; ++calls; }
void cmdComSpec(void) { selected=2; ++calls; }
void cmdSaveWorld(void) { selected=3; ++calls; }
void cmdGetCurrentDir(void) { selected=4; ++calls; }
void cmdSetInfo(void) { selected=5; ++calls; }
void cmdGetStdHandle(void) { selected=6; ++calls; }
void cmdCheckBinary(void) { selected=7; ++calls; }
void cmdExec(void) { selected=8; ++calls; }
void cmdInitConsole(void) { selected=9; ++calls; }
void cmdExecComspec32(void) { selected=10; ++calls; }
void cmdReturnExitCode(void) { selected=11; ++calls; }
void cmdGetConfigSys(void) { selected=12; ++calls; }
void cmdGetAutoexecBat(void) { selected=13; ++calls; }
void cmdGetKbdLayout(void) { selected=14; ++calls; }
void cmdGetInitEnvironment(void) { selected=15; ++calls; }
void cmdGetStartInfo(void) { selected=16; ++calls; }
int main(void)
{
    unsigned i;
    for (i=0;i<17;++i) {
        selected=99;calls=0;
        if (!CmdDispatch(i) || selected!=i || calls!=1) return 1;
    }
    puts("COMMAND_ORIGINAL_OBJECT_17_ROUTES_OK");
    return 0;
}
