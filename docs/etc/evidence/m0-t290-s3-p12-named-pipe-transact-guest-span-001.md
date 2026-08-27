# M0 T290 S3 P12 — Original named-pipe transaction guest-span composition

The formal x86/x64 `vrnmpipe.c` fixture now executes the unchanged original
`VrTransactNamedPipe` body.  A fixture-only public Win32 message-mode pipe
peer reads the two-byte request and returns a two-byte response; it is not a
Redirector production worker or broker.

The fixture publishes the client handle only through the current session's
opaque host-resource identity.  It opens the existing bounded pointer scope
for the original `DS:SI` transmit and `ES:DI` receive arguments.  The source
body retains its own event, `RememberPipeIo`/`ForgetPipeIo`, overlapped wait,
`GetOverlappedResult`, `CX` byte-count and carry/error ordering.  On success,
the receive lease commits `ok` to guest RAM and the identity is released
before session disposal.

Both formal Ninja architectures exit `0`.  No selector ingress, raw guest
pointer, raw host handle, production worker, completion interrupt, second
broker or mapping manager was added.
