# M0 T282 S8 — COMMAND owner dependency partition

Partition every original COMMAND service and reached helper into a complete
next-owner implementation order. This is a package-level source/ABI audit,
not a trace-led leaf implementation.

## Objective

Use the exact 11 original `dos/command` translation units, dispatcher order
and their reached non-COMMAND calls to record one disposition each: direct
original composition, existing same-shaped adapter prerequisite, named later
owner package, or explicit unavailable source branch.

## Required records

- Original source file, handler/helper and any `54:xx` service association.
- Reached external interface and its owner: public Win32, adapter-win32,
  adapter-softpc, adapter-vdm-monitor, session, app, broker, Redirector,
  WOW, VDD/debugger, machine or guest lifecycle.
- Earliest recovery rung, retained original ordering/layout/failure behavior,
  and the next package that can legally implement the unit.
- A dependency order that identifies complete next slices rather than a list
  of individual trace hits.

## Non-goals

No source-body modification, BOP route, command dispatch enablement, guest
trace, machine change, guessed capability or new adapter.

## Exit criteria

Every original COMMAND handler and reached helper has one source-backed
disposition and no dependency is left anonymous. The resulting ordered slices
must be sufficient to admit the next implementation S without reopening this
audit. Documentation governance and `git diff --check` pass.
