# M0 T335 S3 P3 — WOW current-task TEB binding

## Result

The selected original WOW32 task carrier can now retain the original
`CURRENTPTD()` storage form without treating a modern Windows TEB as an NT4
TEB.  The existing adapter-private, per-thread `OPENNT_SUPPORT_TEB` gains the
host-private field `WOW32Reserved`; original `wow32.h` continues to define:

```c
#define CURRENTPTD() ((PTD)(NtCurrentTeb()->WOW32Reserved))
```

Therefore original `wow32.c` and `wkman.c` remain the owners of `TD`, its
allocation, assignment, task ordering and destruction.  This binding does
not create a WOW scheduler, a second session object, or a guest-memory mapper.

## Original contract and modern boundary

NT4 placed `TD *` in the writable `WOW32Reserved` member of each thread's
TEB.  Modern public Win32 intentionally does not expose a corresponding
product slot.  The adapter's TLS TEB already carries selected historical
fields (`StaticUnicodeString`, `Vdm`) for source-compatible OpenNT code, so
the same host-private carrier is the narrow correct replacement.  It remains
a host pointer internal to the original WOW32 package and never crosses a
guest, session-resource or BOP identity boundary.

`TD` includes the source-owned `vpStack`, `vpCBStack`, task handle and
compatibility flags.  P3 does not reinterpret any of them.  In particular it
does **not** make the raw `GET*VDMPTR` macro family safe: that family still
needs its one source-wide temporary-alias lifetime contract before an
original callback body can be enabled.

## Changed locations

- `src/adapter-mvdm-host-out/win32/include/nt.h` — one adapter-private TEB
  carrier member, marked `DIVERGENCE(ADAPTER-WIN32-041)`.
- `src/adapter-mvdm-host-out/win32/README.md` — registered reason, original
  behavior, replacement and exact files.
- `tests/adapter-mvdm-host-out/wow/t335_s3_wow_callback_frame_lease_fixture.c`
  — verifies the field is initially null, writable and obtained again through
  `opennt_support_current_teb()` on the same thread.
- `tools/build/New-T335S3WowCallbackFrameNinja.ps1` — adds the already-owned
  Win32 adapter source and the original declaration-mirror include root to
  the focused graph.

## Verification

Fresh clean focused builds and fixture runs passed on both architectures:

- x86: 7 compile/link actions, exit code 0;
- x64: 7 compile/link actions, exit code 0.

The graph uses `opennt-host/public/sdk/inc` for the existing selected
`ntpsapi.h`/`ntrtl.h` declaration mirrors; it does not create a duplicate
adapter header merely to satisfy the fixture.

## Remaining S3 boundary

P3 closes only the original current-task storage precondition.  The following
remain S3 work, with no claim of completion:

1. package-local temporary-alias lifetime and commit semantics for the full
   selected `GET*VDMPTR`/`FREE*`/`FLUSH*` family;
2. the original non-fast `CallBack16` source hook using that plane;
3. a CCPU40 recursive `host_simulate` return proof.

Long-lived communication aliases, GUI/USER/GDI, WOWEXEC/CSRSS, fast WOW and
generic Win16 provider routes remain unavailable or transfer to their named
owner packages.
