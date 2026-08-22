# Proposal: External Compatibility, Machine And Legacy-Composition BOP Closure

## Purpose

Resolve the BOP tracker’s hard dependencies after the no-external-dependency
layer: selector-blind bx machine/BIOS mechanics, modern x64
compatibility for historical host composition, DPMI, Redirector, WOW16,
VDD/debugger, and NTDOS EXEC/PSP/parent return.

## Boundary

This is not permission to recreate NT4 private subsystems. Bochs remains a
selector-blind machine; OpenNT guest and host source remain semantic owners;
`bx-vdm` only hosts fixed-width records and named compatibility seams. Every
NT4/private/obsolete API follows the tracker’s modern Win32/x64 decision rule:
reuse original source first, use an equivalent public-API shim only when it
preserves the observable contract, otherwise retain original failure/defer or
obtain an owner decision.

## Workstreams

| Workstream | Tracker dependency class | Supporting proposal |
| --- | --- | --- |
| bx machine/BIOS | P7 mechanical prerequisites | `proposal-bx-machine-bios-selector-owner-package-completion-001.md` |
| XMS | P7 A20/extended-memory/UMB/INT15 | `proposal-opennt-xms-owner-package-completion-001.md` |
| guest EXEC/parent return | P8 NTDOS PSP/arena/JFN lifecycle | `proposal-ntdos-command-guest-exec-parent-return-001.md` |
| DPMI | P9 protected mode/LDT/IDT/exception/memory | `proposal-opennt-dpmi-owner-package-completion-001.md` |
| Redirector | P10 IPC/network/remote streams | `proposal-opennt-redirector-owner-package-completion-001.md` |
| WOW16 | P11 NE/DPMI/WOW host composition | `proposal-opennt-wow16-owner-package-completion-001.md` |
| VDD/debug/top-level | P12 events, console/input and notifications | `proposal-opennt-debugger-vdd-top-level-owner-package-completion-001.md` |
| cross-owner lifecycle | P13 only after native owners are complete | `proposal-opennt-command-cmdexec32-full-capability-closure-001.md` |

## Admission plan

1. Freeze the target tracker slice and make an API/ABI/machine decision ledger
   for each dependency: direct reuse, minimal public-API shim, source-defined
   failure/defer, or owner decision.
2. Complete one workstream at a time with original owner source, its declared
   machine or host seam, and full local family regression.
3. After each workstream, update its BOP/dependency rows and run one bounded
   native observation; do not create a service patch from a trace hit.

## Exit rule

Each selected hard dependency has a tested owner closure or explicit,
source-proven unavailable disposition. Remaining cross-owner lifecycle work
is transferred only to the post-code-complete proposal, then candidate 3
reconciles the full tracker.
