# redir family

Owns the historical VDMREDIR/Redirector external-product boundary. No
Redirector provider or protocol implementation is admitted.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-REDIR-001 | Historical `vdmredir.h` reconstructs an x86 host `HANDLE` directly from two guest-visible words. | The pair is a session surrogate32 on both x86 and x64; a native handle may not be placed in a guest or MVDM ABI field. | Preserve `HANDLE_FROM_WORDS` at original call sites, but resolve only through the existing `session` host-resource mapping via the SoftPC identity facade. Unknown/zero IDs return `INVALID_HANDLE_VALUE` with `ERROR_INVALID_HANDLE`. | `include/vdmredir.h`, `include/mvdm_redirector_handle.h`, `mvdm_redirector_handle.c` |
| ADAPTER-REDIR-002 | Original mailslot helpers issue arbitrary 16-bit DOS handles from a process-local bitmap. | The opaque ID must map to a native-width private record without adding another mapping-manager implementation. | The current session's existing host-resource mapping publishes/resolves/releases the private record; only nonzero identities representable as `WORD` are returned. | `include/mvdm_redirector_mailslot.h`, `mvdm_redirector_mailslot.c` |
| ADAPTER-REDIR-003 | Original NetAPI local-result bodies write strings by converting `ES:DI` into an unbounded host alias. | A current session must not expose a guest pointer without an exact acquire/release lifetime. | The helper preserves the numeric real-mode address, takes one existing synchronous guest-memory lease, copies bounded ANSI bytes, then commits/releases. Unicode NetAPI results use the original OEM target encoding before the same bounded commit. | `include/mvdm_redirector_guest_copy.h`, `mvdm_redirector_guest_copy.c` |
| ADAPTER-REDIR-004 | Original asynchronous named-pipe code retains `GetVDMAddr` aliases and reads/writes them from a worker. | A session guest lease cannot cross the BOP/worker lifetime and native aliases cannot enter durable request state. | Preserve the packed 16:16 request layout; snapshot write bytes or stage read bytes, then acquire a fresh session lease only for completion copies. The original queue, completion ordering and ICA calls remain in `mvdm-host/vdmredir/vrnmpipe.c`; this adapter owns only lifetime-safe copy/staging and stop state. | `include/mvdm_redirector_async.h`, `mvdm_redirector_async.c`; mirror `../../mvdm-host/vdmredir/vrnmpipe.c` |
