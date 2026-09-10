# M0 T251 S4 P1 — mailslot owner-group closure

The source-derived Redirector provider now implements the complete retained
`vrmslot.h` mailslot owner group.  OpenNT retains the guest `mailslot.asm`
callers and `VR_MAILSLOT_INFO` ABI but not the VDMREDIR provider bodies; the
new bx-vdm body therefore follows their data ownership and observable
register contracts while using only public Win32 mailslot APIs.

- `57:0B` creates a public local mailslot from a checked OEM guest string;
- its native handle is published only through the existing shared opaque
  handle manager;
- a provider-local 16-bit index models OpenNT `VR_MAILSLOT_INFO.Handle16`;
- `57:0A` retrieves bounded public mailslot information; and
- `57:09` verifies the original current-PDB ownership field, releases the
  opaque host token, and returns the recorded buffer/selector values.
- `57:0C` preserves non-destructive guest semantics with a record-owned
  copied message: Win32 has no peek primitive for mailslots, so the provider
  consumes at most one host message into private memory and exposes it again
  to the following `57:0D` read.  No host pointer reaches guest state.
- `57:0D` copies the retained message to checked `ES:DI` guest RAM and then
  consumes the record-owned copy.
- `57:0E` reads the original eight-byte `DosWriteMailslotStruct` from checked
  guest RAM and writes its checked far-buffer through a newly opened public
  mailslot endpoint.  The legacy timeout is retained as an input layout only:
  public synchronous `WriteFile` has no matching per-message timeout.
- `57:0F` implements `NetResetEnvironment` ownership cleanup by releasing
  every mailslot belonging to the original `AX` DOS PDB.

`t251-s3-redir-ingress-fixture.exe`, rebuilt and executed through formal-r3
outside the sandbox, proves initialize → create → info → write → peek → read
→ delete, then fresh create → per-PDB terminate.  It also proves that a
terminated handle returns `ERROR_INVALID_HANDLE`.  No host handle is stored
in guest RAM or returned in a guest register.

The remaining S4 async pipe group (`57:23/24`) is intentionally separate:
the retained `DOS_ASYNC_NAMED_PIPE_INFO` requires later guest `int 5Ch` ANR
completion.  A raw guest callback or a fabricated synchronous success would
violate this provider boundary, so its source/ABI/failure map remains active
S4 work rather than being mislabeled as a mailslot gap.
