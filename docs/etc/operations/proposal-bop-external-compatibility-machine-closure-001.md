# Proposal: External Compatibility, Machine And Legacy-Composition Dependency Map

## Purpose

Record the BOP tracker’s hard-dependency topology after the
no-external-dependency layer.  It coordinates selector-blind machine/BIOS
mechanics, modern x64 compatibility for historical host composition, DPMI,
Redirector, WOW16, VDD/debugger, and NTDOS EXEC/PSP/parent return; it is not a
single implementation package.

## Boundary

This is not permission to recreate NT4 private subsystems. Bochs remains a
selector-blind machine; OpenNT guest and host source remain semantic owners;
`bx-vdm` only hosts fixed-width records and named compatibility seams. Every
NT4/private/obsolete API follows the tracker’s modern Win32/x64 decision rule:
reuse original source first, use an equivalent public-API shim only when it
preserves the observable contract, otherwise retain original failure/defer or
obtain an owner decision.

## Candidate relationship

This document is a dependency map, not a queue candidate and not an admission
brief.  The queue now admits the following bounded packages separately: first
the `BOP 12h` conventional-memory reset/query component; then keyboard/IVT/
INT 15; guest EXEC; Redirector; DPMI; WOW16; debugger/VDD; evidence-admitted
remaining machine devices; and finally `cmdExec32` cross-owner composition.
Each package freezes its own tracker rows and exit rule before it obtains a
numeric T identifier.

## Workstreams

| Workstream | Tracker dependency class | Supporting proposal |
| --- | --- | --- |
| SoftPC conventional memory | earliest reached machine dependency | `proposal-softpc-bios-conventional-memory-recovery-001.md` |
| bx machine/BIOS device work | later P7 mechanics, individually admitted | `proposal-bx-machine-bios-selector-owner-package-completion-001.md` |
| XMS | P7 A20/extended-memory/UMB/INT15 | `proposal-opennt-xms-owner-package-completion-001.md` |
| guest EXEC/parent return | P8 NTDOS PSP/arena/JFN lifecycle | `proposal-ntdos-command-guest-exec-parent-return-001.md` |
| DPMI | P9 protected mode/LDT/IDT/exception/memory | `proposal-opennt-dpmi-owner-package-completion-001.md` |
| Redirector | P10 IPC/network/remote streams | `proposal-opennt-redirector-owner-package-completion-001.md` |
| WOW16 | P11 NE/DPMI/WOW host composition | `proposal-opennt-wow16-owner-package-completion-001.md` |
| VDD/debug/top-level | P12 events, console/input and notifications | `proposal-opennt-debugger-vdd-top-level-owner-package-completion-001.md` |
| cross-owner lifecycle | P13 only after native owners are complete | `proposal-opennt-command-cmdexec32-full-capability-closure-001.md` |

## Planning rule

For every later workstream, freeze its target tracker rows and make an
API/ABI/machine decision ledger: direct reuse, minimal public-API shim,
source-defined failure/defer, or owner decision.  Complete one original-owner
package at a time; after it closes, update its rows and run at most one bounded
native observation.  A trace hit never creates an unplanned leaf patch.
