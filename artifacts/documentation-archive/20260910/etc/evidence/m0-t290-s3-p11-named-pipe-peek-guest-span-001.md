# M0 T290 S3 P11 — Original named-pipe peek guest-span composition

The same formal x86/x64 original `vrnmpipe.c` fixture now executes
`VrPeekNamedPipe`.  The fixture writes three bytes into a local public pipe,
publishes only the client handle's session-owned opaque identity, then opens
the existing bounded DS:SI write scope.  The unchanged original source
resolves the identity, peeks into the checked bounce span, returns original
BX/CX byte counts and DI connected status, and the scope commits the three
bytes back to guest RAM.  A subsequent original `VrReadNamedPipe` consumes the
same bytes.

Both formal Ninja architectures exit `0`.  No selector ingress, raw pointer,
raw handle, asynchronous request, completion interrupt or new mapping manager
is introduced.
