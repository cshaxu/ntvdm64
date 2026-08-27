#include <nt.h>

BOOL VrIsNamedPipeName(LPSTR name);

VOID WaitIfIdle(VOID) {}
USHORT getAX(VOID) { return 0u; }
USHORT getBX(VOID) { return 0u; }
USHORT getBP(VOID) { return 0u; }
USHORT getCX(VOID) { return 0u; }
USHORT getDX(VOID) { return 0u; }
USHORT getSI(VOID) { return 0u; }
USHORT getDI(VOID) { return 0u; }
USHORT getDS(VOID) { return 0u; }
USHORT getES(VOID) { return 0u; }
VOID setAL(UCHAR value) { (void)value; }
VOID setAX(USHORT value) { (void)value; }
VOID setBX(USHORT value) { (void)value; }
VOID setCX(USHORT value) { (void)value; }
VOID setDX(USHORT value) { (void)value; }
VOID setSI(USHORT value) { (void)value; }
VOID setDI(USHORT value) { (void)value; }
VOID setDS(USHORT value) { (void)value; }
VOID setES(USHORT value) { (void)value; }
VOID setCF(ULONG value) { (void)value; }
VOID setZF(ULONG value) { (void)value; }
LPVOID _inlinePointerFromWords(WORD segment, WORD offset)
{
    (void)segment;
    (void)offset;
    return NULL;
}
VOID VrRaiseInterrupt(VOID) {}
VOID VrQueueCompletionHandler(VOID (*routine)(VOID)) { (void)routine; }
WORD VrpMapLastError(VOID) { return ERROR_NOT_SUPPORTED; }

int main(void)
{
    char remote[] = "\\\\server\\PIPE\\queue";
    char remote_slash[] = "\\\\server\\pipe/queue";
    char local[] = "\\pipe\\queue";
    char missing_name[] = "\\\\server\\pipe";
    char ordinary[] = "C:\\queue";

    if (!VrIsNamedPipeName(remote) || !VrIsNamedPipeName(remote_slash) ||
        VrIsNamedPipeName(local) || VrIsNamedPipeName(missing_name) ||
        VrIsNamedPipeName(ordinary)) return 1;
    return 0;
}
