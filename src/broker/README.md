# broker

Versioned, fixed-width cross-process coordination contract. The legacy wire/
registry modules below remain in-process contract fixtures. S3's rpc_security
module supplies the reusable native authentication boundary; it is currently
exercised by real RPC fixtures, not yet by the three product executables.

`rpc_security.c/.h` is new finite modern transport glue, not original BaseSrv
policy. NT4 CSR port/process authentication cannot be reused without the
excluded CSR runtime. The admitted local RPC route instead requires WINNT
packet privacy and matching OS token logon LUID/session. Scope is captured from
the owning process, never copied from a command. Native state in this private
header is not wire data. Every impersonated path reverts; failed RPC reversion
denies access and falls back to public RevertToSelf, with fail-fast if neither
can restore the thread. That exceptional failure branch remains unexercised.

M0 T272 S5 disposition register:

- `wire.c` and `wire.h`: `new neutral contract`.  There is no reusable
  project-local broker source.  The fixed-size record deliberately excludes
  native pointers, handles, local surrogate IDs and guest pointers.
- `broker.c` and `broker.h`: `new neutral registry`.  It gives stable,
  monotonic broker IDs, binds each client to an already-authenticated fixed
  user key, and removes the record on disconnect.  A later transport must
  authenticate the user key using public OS facilities before dispatch; it
  cannot manufacture a cross-user route.
- `base_vdm_record.c` and `base_vdm_record.h`: `new source-shaped record
  boundary`. They are the fixed-width DOS subset of the original BaseClient /
  BaseSrv `VDMINFO` exchange: copied request/result fields, same-record
  pending/no-command state, and disconnect ownership. They deliberately omit
  CSR capture buffers, raw `HANDLE`s, guest pointers and local mapping IDs.
  The current state core is transport-neutral; `adapter-mvdm-host-out/basesrv`
  is the only permitted original-call binding, and a later public pipe/event
  transport may use this exact record contract.

Project-owned per-user cross-process coordination component. Command records
contain only versioned copied values. Required native resources use only the
separate authenticated OS-managed attachment exception, never command fields.
