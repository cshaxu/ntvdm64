# Proposal: BOP Post-Code-Complete Dependency And Lifecycle Closure

## Rebootstrap owner note

COMMAND remains an original package/library family inside
`opennt-mvdm-host`. `GetNextVDMCommand` cross-process acquisition is recovered
through the same-shaped host facade and the formal broker; local child/session
state remains session-owned and local surrogate IDs never cross IPC.

## Purpose

After all prerequisite BOP owner packages are code-complete, close the
recorded dependencies which cannot truthfully close inside a single selector
family: cross-owner execution, lifecycle, broker, completion, capability and
failure paths.  The package integrates completed owner contracts into declared
end-to-end profiles; it does not make a trace hit into a new leaf BOP task.

Imported OpenNT `cmdExec32`, including `54:01` `cmdGetNextCmd` /
`GetNextVDMCommand` command acquisition and re-entry, is the first known
ledger item. It is explicitly not the boundary of this candidate: later
owner-package closures may add similarly documented pending paths before
admission. A local shim or a code-complete dispatcher is never itself a claim
that its cross-owner lifecycle has closed.

## Queue relationship

This is the admission plan for queue candidate 5, **COMMAND `cmdExec32`
full-capability composition closure**.  It is admitted only after the
required native owner contracts are complete; candidate 10 then audits its
integrated result against the complete tracker.
## Boundary

This is a late composition and closure package, not a place to reimplement a
family owned by Redirector, WOW, console/input, machine/BIOS or guest DOS. It
consumes their completed contracts, repairs only the minimal cross-owner seam
required by a declared profile, and records any remaining unavailable source
behavior explicitly. It owns no BOP family algorithm and gives neither Bochs
nor bx-mantle COMMAND, DOS, WOW or BOP policy.

For the first ledger item, it owns recovery of the historical COMMAND
host-broker composition required by the declared profile: OpenNT command
records, request/reply ordering, re-entry accounting, wait/unblock behavior,
and the `54:01` handoff. It may not replace those with an ambient synthetic
queue. Other entries receive the same source/ABI/failure treatment, but only
after their native owner packages have declared their public composition
contracts.

## Extensible Pending-Closure Ledger

Every item added before admission must provide all of the following. This
table is the only place to expand this candidate; an entry does not authorize
implementation by itself.

| Required record | Rule |
| --- | --- |
| Reached source and BOP/service context | Name the imported caller, selector/service when applicable, original source path, and the code-complete owner package that supplied it. |
| Blocked closure | State the exact missing cross-owner contract, historical product dependency, or lifecycle transition. Do not call a general lack of runtime success a blocker. |
| Owner split | Name the owner that must complete its native behavior and the narrowly bounded composition seam this task may implement. |
| Recovery ladder | Record direct OpenNT reuse, smallest compatible seam, rejected intrusive alternatives, and why a new behavior is unavoidable if one exists. |
| Disposition before integration | Mark explicit unavailable, deferred, or source-defined failure; never leave a selected route silently successful. |
| Admission and regression | Define the declared end-to-end profile and positive, failure, cancellation and no-bypass evidence required to remove the entry. |

### Initial Ledger Entries

| Entry | Present status | Required completed owners before integration | This package's closure target |
| --- | --- | --- | --- |
| COMMAND `cmdExec32` / `54:01` / `54:08` / `54:0A` / `54:0B` | T236 provides only one local session worker, bounded re-entry and explicit WOW exclusion. | Redirector, WOW16, debugger/VDD/console-input, bx machine/BIOS, and NTDOS/COMMAND guest EXEC-parent-return. | One declared COMMAND child profile with source-shaped broker, re-entry, wait/unblock, streams and parent-return behavior. |
| COMMAND `VDMForWOW` startup and `GetWOWShortCutInfo` | T236 S5 proves a controlled unavailable stop and leaves shortcut metadata unexposed. | WOW16. | Consume an admitted WOW root, kernel command publication, shortcut producer/consumer and WOW execution contract without changing `cmdGetNextCmd` ownership. |

Additional entries may be appended only with the required record above. They
remain pending until their owner packages are code-complete; they must not
cause out-of-order leaf patches in this candidate.

## Prerequisites

- Every owner package named by every admitted ledger entry must be
  code-complete and publish a fixed, source-audited composition contract.
- The initial `cmdExec32` ledger entry requires Redirector for remote/named
  pipes; WOW16 for WOW execution/broker behavior; debugger/VDD/top-level for
  admitted console input; bx machine/BIOS for machine prerequisites; and
  NTDOS/COMMAND guest EXEC-parent-return for ordinary `54:0B` continuity.
- A source/ABI map of the historical `GetNextVDMCommand` client and its
  BaseSrv/CSR server-side counterparts. The map is an admission input, not
  evidence that private CSR APIs are usable on a modern host.

## Initial Mandatory COMMAND Broker-Recovery Work Package

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

1. Freeze the ledger at admission, verifying each entry has the required
   source/ABI/failure and owner-split record. Reject entries whose owner is
   not code-complete or whose requested result is only a trace observation.
2. Re-audit the initial `cmdExec32`, `cmdCreateProcess`, `cmdExec`, COMSPEC,
   return, stream, event, console and `cmdGetNextCmd`/`GetNextVDMCommand`
   broker paths against OpenNT source. Audit each newly admitted ledger item
   to the same standard.
3. For each path, consume the owning package's direct interface; restore
   original code where possible and use a documented compatibility seam only
   where the historical NT4 product composition is unavailable.
4. Complete the mandatory broker-recovery work package before accepting an
   end-to-end `cmdExec32` result. A retained T236 one-session continuation is
   a baseline, not a substitute for this check.
5. Run the per-entry regression and every declared composed native profile;
   prove normal flow, source-defined failure, cancellation, no-bypass and
   relevant owner handoff. Then run the complete frozen-ledger matrix.

## Exit Rule

This candidate closes only when every frozen ledger item has a direct,
source-derived compatible, or explicitly unavailable disposition in its
declared profile; its whole profile regression must pass without
adapter-owned DOS, Redirector, WOW or machine semantics. For `cmdExec32`,
this includes `54:01` command acquisition/re-entry and the historical broker
path. “The current local shim worked” is not a broker closure disposition.

The result is not the final BOP catalogue audit: the following cross-family
candidate still reconciles cardinality and integration coverage for all BOPs.
Conversely, that audit cannot replace this task's integration work by merely
classifying a known lifecycle gap.
