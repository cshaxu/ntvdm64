# M0 T290 S3 P8 — Original named-pipe handle-state register composition

The existing formal `vrnmpipe.c` x86/x64 fixture now also executes original
`VrGetNamedPipeHandleState`.  A local public Win32 pipe handle is published
only into the active session's existing `host_resource` mapping, then its
opaque 32-bit identity is supplied in the source-defined `BP:BX` words.
The original body resolves that identity through the same-shaped Redirector
handle binding, calls the public pipe APIs and clears CF.

No native `HANDLE` crosses guest state.  The test releases the identity,
unbinds and disposes the session after the call.  No selector ingress is
enabled.  The remaining register-plus-guest-buffer forms (`VrGetNamedPipeInfo`,
`VrPeekNamedPipe`, `VrTransactNamedPipe`, `VrCallNamedPipe`, and
`VrWaitNamedPipe`) require the already-declared bounded guest pointer scope;
they remain the next S3 group rather than a reason to create another mapper.

The P6 x86/x64 Ninja commands were rerun and both fixture executables exited
`0`.
