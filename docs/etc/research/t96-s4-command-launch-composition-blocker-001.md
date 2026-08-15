# M0 T96 S4 COMMAND Launch-Composition Blocker 001

## Question

Can the pre-T96 `cmd_get_next_service` be treated as an implementation of
OpenNT `cmdGetNextCmd` (`54:01`) for the new COMMAND provider plane?

## Direct source comparison

Original `cmdGetNextCmd` in pinned
`refs/opennt/base/mvdm/dos/command/cmdmisc.c:25-369` consumes a guest
`CMDINFO` record via `GetVDMAddr` and coordinates all of the following:

- first/repeat command state and return error;
- initial/merged VDM environment and capacity retry;
- per-drive current directories and host CWD isolation;
- PIF parsing, DOS-versus-32-bit classification, command-tail/path result;
- process/session termination, console title/blocking, and event threads;
- redirection completion and inherited standard handles; and
- SCS synchronization, DOS-binary marker, drive count, code page, and guest
  result fields.

The existing `bx_ntvdm_cmd_get_next_v1_*` implementation instead accepts a
single profile target and fixed launch declaration, reads a bounded CMDINFO
subset, and emits selected writes for TARGET path/tail, drive count and SCS
registration. It does not represent the complete source protocol above.

## Decision

The existing implementation is a frozen **target-launch fixture**, not an
original COMMAND provider and not a valid source-derived general dispatcher.
It must not acquire extra conditionals to impersonate the missing protocol.
S8 must route/review it as a legacy endpoint, after the common COMMAND plane
is established.

Any future first-profile launch provider must declare a copied launch contract
covering: target identity/tail, immutable environment snapshot, guest CMDINFO
read/write ranges, session policy, declared current directories, and explicit
no-redirection/no-PIF/no-32-bit-exec capability state. It may not retain raw
guest pointers, inherit Win32 handles, query ambient CWD/environment, or
silently claim the omitted historical capabilities.

## Confidence

High for the mismatch and required omitted categories: they are direct source
observations. The shape of a later copied launch contract is a constrained
design consequence, not a claim of executable provider completeness.
