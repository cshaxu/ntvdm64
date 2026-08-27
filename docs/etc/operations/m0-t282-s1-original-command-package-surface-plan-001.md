# M0 T282 S1 — original COMMAND package source surface

## Purpose

Establish the complete source and boundary inventory for the original
`dos/command` owner package before any COMMAND BOP route is activated. This is
a package-level recovery step, not a `54:xx` leaf implementation sequence.

## Original package

The canonical `sources` file selects these eleven translation units:

- `cmd.c`, `cmddata.c`, `cmddisp.c`, `cmdexec.c`, `cmdexit.c`, `cmdmisc.c`;
- `cmdpif.c`, `cmdredir.c`, `cmdconf.c`, `cmdkeyb.c`, `cmdenv.c`.

The resulting owner interface covers `54:00` through `54:10`, whose original
dispatcher order is already recorded in the prior COMMAND service ledger.

## S1 outcome

Create a reproducible x86/x64 static compile-surface probe and a one-row-per-
translation-unit ledger. Each row must record source identity, reached missing
interface family, owner boundary, source-first rung, and disposition:

- directly composable using existing declaration-only boundaries;
- blocked on a named adapter prerequisite; or
- transferred to a separately named specialist owner package.

The probe may compile declarations or objects. It must not link an executable,
enable `CmdDispatch`, enable a `54:xx` selector, call a provider, invoke
Bochs, or substitute a new COMMAND implementation.

## Boundaries

The audit must distinguish original local calls from the already-known
boundaries: adapter-win32 host process/environment/console capability,
adapter-softpc CCPU/SAS/guest-memory facade, adapter-vdm-monitor/session
notification, adapter-bop ingress, Redirector/broker transport, and WOW/VDD
specialist products. A public Win32 API is only a backing capability; it does
not replace an available original COMMAND algorithm.

## Exit criteria

All eleven original source paths and all seventeen service entries are covered
once; each non-composable interface has one named owner; x86/x64 probe results
are retained; and the next S can select one whole coherent source group
without re-auditing a leaf BOP.
