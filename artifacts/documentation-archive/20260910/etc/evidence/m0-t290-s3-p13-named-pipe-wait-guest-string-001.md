# M0 T290 S3 P13 — Original named-pipe wait guest-string composition

The formal x86/x64 `vrnmpipe.c` fixture now executes the unchanged original
`VrWaitNamedPipe` body. A local public Win32 pipe instance is created in its
listening state. Its complete NUL-terminated name is copied to guest RAM and
the existing session pointer scope supplies the original `DS:DX` string
argument; original `BX:CX` timeout composition is retained.

The original body calls `WaitNamedPipe`, returns source-shaped `AX = 0` and
clear carry, then releases the bounded scope before pipe/session teardown.
Both formal Ninja architectures exit `0`. No selector ingress, raw guest
pointer, raw host handle, async worker, callback, broker or mapping manager is
introduced.
