# M0 T290 S3 P9 — Original named-pipe information guest-span composition

The formal x86/x64 `vrnmpipe.c` fixture now executes original
`VrGetNamedPipeInfo` with all historical boundary forms intact:

- `BP:BX` carries only the session-owned opaque identity of a local pipe
  handle; the existing Redirector handle binding resolves it locally.
- `DS:SI` resolves only while the existing Redirector pointer scope is active.
  That scope acquires a checked write bounce lease from the one session
  `guest_memory` mapping instance, and commits it only after the original body
  returns.
- The unchanged source body obtains Win32 pipe state, writes the original
  packed `PIPEINFO` fields and copies its recorded original pipe name.  The
  fixture proves CF clear, `cbName`, and the copied name after scope commit.

No raw pointer or HANDLE becomes guest-visible, and no second mapping manager,
selector ingress, async pipe, callback or IRQ behavior is introduced.  Both
formal Ninja fixture architectures exit `0`.
