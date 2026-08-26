# M0 T277 S5 — SoftPC host-control source-form family plan

## Scope

S5 reviews all 197 future-host correlations whose caller resides under the
original `softpc.new/host/src` root. These 197 calls occur in 26 original
source files and name 76 distinct symbols. The review is divided by original
host-control subdomain, not by individual trace events:

- top-level BOP, startup, stop and session/event control;
- CCPU/SAS/PIC/CPU mechanical facade calls, whose executor remains Bochs;
- serial, printer, disk/floppy and resource/configuration endpoint calls;
- mouse, display and console endpoint calls;
- monitor/VDD/debugger-facing calls; and
- ordinary CRT/resource helpers.

## Required decisions

For every correlation, cite the original provider source form and determine
exactly one owner: original host package, `adapter-bop`, `adapter-softpc`,
`adapter-win32`, `adapter-bochs`, `adapter-vdm-monitor`, `adapter-vdd`,
`adapter-debugger`, `session`, `broker`, native CRT, or Bochs-replacement
exclusion. A historical CCPU/SAS or device executor may retain its source
interface shape but must never become a second machine executor.

## Exclusions

No `softpc.new` translation unit is enabled, copied into an adapter, altered,
compiled or linked by S5. No BOP selector is enabled, no Bochs behavior
changes, and no host endpoint action occurs. The audit may name an adapter
only where original caller/provider forms demonstrate its boundary.

## Closure evidence

The generated ledger will cover all 197 stable T277/S2 work IDs once, retain
an original provider citation, state the non-pointer/mapping rule, preserve
the historical caller-side failure semantics, and keep every row explicitly
implementation-disabled. Alternate SoftPC CPU execution, private NT4
product-shell behavior and unavailable monitor/VDD/debugger branches must be
recorded as exclusions with named successor packages.
