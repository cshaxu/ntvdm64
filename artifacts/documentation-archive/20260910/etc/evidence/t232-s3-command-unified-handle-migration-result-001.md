# M0 T232 S3 — COMMAND Unified-Handle Migration Result

> **Superseded ABI detail:** M0 T234 replaces this record's initial low-word
> ID/high-word-zero representation with a same-width 32-bit opaque ID. See
> [the successor result](t234-s1-32bit-opaque-host-handle-abi-result-001.md).

## Question

Can the direct COMMAND `54:06` publication and `54:08` launch/redirection
path replace `handle_tokens[64]` with the T232 host-handle manager while
keeping OpenNT's `BX:CX` guest ABI and original error path intact?

## Inputs

- T232 S1 manager foundation and T232 S2 DEM migration.
- OpenNT COMMAND mirrors `cmdmisc.c`, `cmdredir.c`, and `cmdexec.c`.
- `command_misc_shim.{h,c}` and `command_native_session_shim.c`.

## Procedure

1. Replaced `command_misc_session.handle_tokens[64]` with a session-owned
   `bx_ntvdm_host_handle_manager`.
2. Published standard/redirection handles as borrowed entries and resolved
   launch-time standard-handle records through the manager.
3. Kept `BX:CX` as the original fixed register-pair layout: `BX` is zero and
   `CX` holds the opaque nonzero 16-bit identifier. A nonzero high half in a
   launch record follows the existing `ERROR_INVALID_HANDLE`/carry path.
4. Reset the manager when the native COMMAND session unbinds; borrowed
   standard handles are removed but never closed by that reset.
5. Rebuilt the focused formal targets and ran their fixtures.

## Observations

- `handle_tokens[64]`, its 64-entry capacity, and direct host-handle array
  indexing are absent from the COMMAND product route.
- `t231-s4-command-console-keyboard-direct-fixture.exe` exited zero.
- `t231-s8-command-lifecycle-direct-fixture.exe` exited zero, covering both
  a rejected nonzero-high-word launch token and a successful borrowed pipe
  standard-handle launch.
- The manager, DEM direct-session, DEM handle-import, DEM file/FCB, and
  COMMAND focused fixtures all exited zero in the shared formal tree.

## Interpretation And Confidence

COMMAND now uses the same adapter-owned manager implementation and ownership
contract as DEM. This is the explicit `BX-VDM-001` native-width host-handle
representation exception, not an assertion that OpenNT used this mechanism.
Original COMMAND provider bodies and their control/error semantics remain the
owner. Confidence is high for the published COMMAND routes. There is not yet
a native execution session that composes DEM and COMMAND simultaneously, so
this result does not claim cross-family handle sharing at runtime.

## Follow-up

T232 S4 audits all non-v1 product and formal-fixture routes, records the
absence of duplicate private tables, and closes the task. A future composed
native session must inject one manager instance if it makes DEM and COMMAND
live concurrently.
