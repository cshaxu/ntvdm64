# M0 T290 S3 P14 — Original CallNamedPipe structured guest-span composition

The formal x86/x64 `vrnmpipe.c` fixture now executes unchanged original
`VrCallNamedPipe`. The original packed `DOS_CALL_NAMED_PIPE_STRUCT` remains a
24-byte physical guest ABI: its four far-pointer fields are explicitly
four-byte `DWORD` values rather than host-width pointer typedefs. This is
registered as `MVDM-SUPPORT-DIV-002`; the source body still uses its original
`READ_FAR_POINTER`, field names, argument order and error sequence.

Before entering the source body, the existing Redirector pointer scope is
given five exact transient leases: structure, pipe name, transmit bytes,
receive bytes and returned count. The scope resolves the original 16:16
values, commits only the two output spans, and verifies every prepared lease
was consumed. A fixture-only public Win32 message-pipe peer exchanges the
two-byte request/response; it is not production Redirector machinery.

Both formal Ninja architectures exit `0`. No raw guest alias, raw host handle,
selector ingress, async worker, completion interrupt, broker or mapping
manager is introduced.
