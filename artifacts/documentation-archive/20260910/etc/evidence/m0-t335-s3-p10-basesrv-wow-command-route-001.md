# M0 T335 S3 P10 — BaseSrv WOW command-route binding

## Result

The existing local BaseVDM adapter now preserves the reached original
BaseSrv distinction between DOS and shared-WOW command records.  It does not
load a Win16 image, start WOW32, or replace the original guest callback
trampoline.

## Original source basis

The directly relevant original source is the accepted `opennt-host` Base VDM
slice:

- `base/win32/client/vdm.c:GetNextVDMCommand` selects the WOW request through
  `ASKING_FOR_WOW_BINARY` and a sentinel console handle;
- `base/win32/server/srvvdm.c:BaseSrvGetNextVDMCommand` maintains a distinct
  WOW record path.  An empty WOW queue is a successful, zero-length,
  **nonblocking** response.  A DOS caller instead receives its DOS-record
  wait/retry path.

The original client/server translation units cannot be linked as a modern
local service because their transport is CSR capture/CSRSS state, duplicated
handles and global console/WOW records.  The existing
`adapter-mvdm-host-out/basesrv` boundary is therefore the smallest same-shaped
facade available to the selected MVDM callers.

## Binding

`base_vdm_local` retains its one copied session record but tags it with a
private `command_owner` discriminator.  The discriminator never enters
`VDMINFO`, guest state or a wire ABI:

- a DOS request can consume only a DOS record and otherwise retains the
  existing pending/wake/second-request behavior;
- a shared WOW request can consume only a WOW record and otherwise returns
  the original successful empty queue result without blocking;
- PIF and separate-WOW remain the same exact `ERROR_CALL_NOT_IMPLEMENTED`
  outcomes, because their original record and process semantics are not in
  this one-session profile.

The result keeps all original caller-facing data in `VDMINFO`: command,
application, environment, current directory, task, code page, creation flags
and status.  It introduces neither a guest pointer nor a second mapping
manager.

## Verification

The focused `base_vdm_local_fixture` was generated and run through Ninja in
the current T335 build roots:

- x86: `build/M0-T335/S3/P10/base-vdm-x86`, `ninja -C ... test` —
  `PASS: local Base VDM broker contract`;
- x64: `build/M0-T335/S3/P10/base-vdm-x64`, `ninja -C ... test` —
  `PASS: local Base VDM broker contract`.

The fixture proves the existing DOS record remains intact, an empty WOW
request is a successful empty result, a published WOW record is copied only
to a WOW request, and separate-WOW remains unavailable.  Its stale
`/C VER` assertion was corrected to the currently published `/C EXIT`
declaration; this is a test expectation repair, not a command behavior
change.

## Boundary

This packet does **not** establish the producer side for a WOW record, the
original `VDMForWOW` / `GetWowKernelCmdLine` handoff, `krnl386.exe` selection
and installation, `MS_bop_1` provider initialization, WOWEXEC notification,
GUI/USER/GDI, CSRSS transport, or a real guest callback return.  Those remain
the remaining source-chain inspection and vertical-verification work in S3.
