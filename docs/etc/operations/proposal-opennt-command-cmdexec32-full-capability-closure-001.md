# Proposal: OpenNT COMMAND cmdExec32 Full-Capability Closure

## Purpose

After all prerequisite owners are available, perform a complete source/ABI/
failure audit of every capability reachable from imported OpenNT `cmdExec32`,
then integrate each owner-provided capability and close one declared COMMAND
execution profile end to end.  This explicitly includes the `54:01`
`cmdGetNextCmd` / `GetNextVDMCommand` command-acquisition and re-entry
protocol: it is not merely a prior local-session shim or an assumed external
service.

## Boundary

This is a late COMMAND integration and closure package, not a place to
reimplement Redirector, WOW, console input, machine semantics or guest DOS.
It consumes their completed contracts and records any remaining unavailable
source behavior explicitly.  It does own recovery of the historical COMMAND
host-broker composition required by the declared profile: OpenNT's command
records, request/reply ordering, re-entry accounting, wait/unblock behavior,
and the `54:01` handoff.  It may not replace those with an ambient synthetic
queue or give Bochs COMMAND/BOP meaning.

## Prerequisites

- Redirector owner package for remote/named-pipe behavior.
- WOW16 owner package for WOW execution/broker behavior.
- Debugger/VDD/top-level package for any admitted console-input behavior.
- bx machine/BIOS package for machine prerequisites.
- NTDOS/COMMAND guest EXEC-parent-return package for ordinary `54:0B` guest
  continuity.
- A source/ABI map of the historical `GetNextVDMCommand` client and its
  BaseSrv/CSR server-side counterparts.  The map is an admission input, not
  evidence that private CSR APIs are usable on a modern host.

## Mandatory COMMAND Broker-Recovery Work Package

The previous T236 S2 closure establishes only the smallest one-session
composition: a session-owned command record, bounded re-entry count, and
local continuation that lets the imported `cmdGetNextCmd` body proceed.  It
does **not** claim the complete historical `BaseSrvGetNextVDMCommand` broker.
This package must close that distinction rather than hide it.

1. Audit and attempt to retain the relevant OpenNT structure from
   `base/win32/client/vdm.c`, `base/win32/server/srvvdm.c`, and
   `public/internal/base/inc/vdmapi.h`, alongside imported COMMAND callers.
   Record the exact request/reply records, state transitions, re-entry
   increments/decrements, current-directory/environment exchange, wake/block
   ordering, cancellation, and source failure results.
2. Keep directly composable OpenNT translation units and layouts.  Where the
   private CSR/BaseSrv transport cannot compose on a modern host, introduce
   only the smallest session-owned public-Win32 transport seam that preserves
   the recorded request/reply contract.  It must use copied fixed-width
   records and the existing opaque-handle manager; it may use public events,
   waits and pipes, but cannot expose raw handles, guest pointers, or host
   process globals.
3. Recover the declared profile's external-command producer and consumer
   behavior through that contract.  A one-session product profile need not
   claim multi-VDM scheduling, but it must not call a local in-memory shortcut
   “BaseSrv recovery”.  Multi-session scheduling is either an explicitly
   source-backed extension with its own regression or an explicit unavailable
   disposition.
4. Verify `54:01` before, during, and after `54:08`/`54:0A` child execution:
   normal command delivery, re-entry, wait/unblock, cancellation, no-command
   result, failure propagation, current-directory/environment updates, and
   parent-return interaction.  The original service bodies remain the owners
   of COMMAND semantics.

## Admission Plan

1. Re-audit all `cmdExec32`, `cmdCreateProcess`, `cmdExec`, COMSPEC, return,
   stream, event, console, `cmdGetNextCmd`/`GetNextVDMCommand` broker and
   failure paths against OpenNT source.
2. For each path, consume the owning package's direct interface; restore
   original code where possible and use a documented compatibility seam only
   where the historical NT4 product composition is unavailable.
3. Complete the mandatory broker-recovery work package before accepting an
   end-to-end `cmdExec32` result.  A retained T236 one-session continuation is
   a baseline, not a substitute for this check.
4. Run complete package-local regression plus a declared native guest profile;
   prove normal child launch, command acquisition, completion, guest parent
   return, selected stream behavior and all admitted failure paths.

## Exit Rule

`cmdExec32` may be called fully closed only when every source-reachable path
in the declared profile has a direct, source-derived compatible, or explicitly
unavailable disposition, **including the `54:01` command-acquisition/re-entry
and historical broker path**.  The end-to-end guest profile must pass without
adapter-owned DOS, Redirector, WOW or machine semantics.  “The current local
shim worked” is not a closure disposition for the historical broker; the
proposal requires a source ledger, a retained or minimal-compatible transport,
and the stated command lifecycle regression.
