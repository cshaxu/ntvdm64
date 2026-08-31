# M0 T333 S4 — Redirector async adapter composition

## Result

S4 closes the admitted local mailslot and asynchronous named-pipe boundary.
The original `mvdm-host/vdmredir` package still owns request decoding, its
queue, the worker loop, completion order, `VrQueueCompletionHandler`, and the
ICA interrupt request.  The Redirector adapter owns only the durable host-side
state that the original NT4 flat guest aliases cannot safely represent.

## Source and ownership result

- `vrmslot.c` keeps its original 16-bit handle allocation and mailslot policy.
  `adapter-mvdm-host-out/redir/mvdm_redirector_mailslot.c` resolves the opaque
  record through the existing session host-resource mapping instance.
- `vrnmpipe.c` retains original `DOS_ASYNC_NAMED_PIPE_INFO` queue ownership and
  original completion control flow.  `ADAPTER-REDIR-004` stages write data or
  read data and takes a fresh session guest-memory lease only at submit or
  completion.
- The original source confirms that `pBytesTransferred` and `pErrorCode` are
  32-bit *far-address carriers* but point to `WORD` results.  The adapter
  therefore preserves the exact original two-byte writes; it does not truncate
  a 32-bit result.
- The former `mvdm-host-overlay/vdmredir/mvdm_redirector_async.[ch]` material
  is now `adapter-mvdm-host-out/redir/{include,mvdm_redirector_async.c}`.  No
  source policy, queue policy, interrupt policy, or BOP routing moved with it.

## Validation

- The formal x86 Ninja graph compiled all original `vdmredir/sources` units
  and the new `redirector-bindings.lib` together:
  `build/M0-T333/S4/redir-adapter-x86`.
- The focused copied-span fixture built and exited successfully on both x86
  and x64:
  `build/M0-T333/S4/redir-async-{x86,x64}/bin/redirector-async-contract-fixture.exe`.
  It proves write snapshotting, read staging, original two-`WORD` completion
  writes, stale-session rejection, and worker stop state.
- The local fixture generator now canonicalizes its build root, so Ninja never
  resolves the generated MSVC environment script relative to its own build
  directory.

## Deliberate limits

This is not a claim that a physical CPU40 interrupt can yet be delivered to a
guest callback, that NetBIOS/DLC/RAP are enabled, or that `VDMREDIR.DLL` is
linkable as a final product.  Those are respectively source-shaped unavailable
or later S5/S6 owner work.
