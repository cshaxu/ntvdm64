# Proposal: OpenNT COMMAND cmdExec32 Full-Capability Closure

## Purpose

After all prerequisite owners are available, perform a complete source/ABI/
failure audit of every capability reachable from imported OpenNT `cmdExec32`,
then integrate each owner-provided capability and close one declared COMMAND
execution profile end to end.

## Boundary

This is a late COMMAND integration and closure package, not a place to
reimplement Redirector, WOW, console input, machine semantics or guest DOS.
It consumes their completed contracts and records any remaining unavailable
source behavior explicitly.

## Prerequisites

- Redirector owner package for remote/named-pipe behavior.
- WOW16 owner package for WOW execution/broker behavior.
- Debugger/VDD/top-level package for any admitted console-input behavior.
- bx machine/BIOS package for machine prerequisites.
- NTDOS/COMMAND guest EXEC-parent-return package for ordinary `54:0B` guest
  continuity.

## Admission Plan

1. Re-audit all `cmdExec32`, `cmdCreateProcess`, `cmdExec`, COMSPEC, return,
   stream, event, console, broker and failure paths against OpenNT source.
2. For each path, consume the owning package's direct interface; restore
   original code where possible and use a documented compatibility seam only
   where the historical NT4 product composition is unavailable.
3. Run complete package-local regression plus a declared native guest profile;
   prove normal child launch, completion, guest parent return, selected stream
   behavior and all admitted failure paths.

## Exit Rule

`cmdExec32` may be called fully closed only when every source-reachable path
in the declared profile has a direct, source-derived compatible, or explicitly
unavailable disposition, and the end-to-end guest profile passes without
adapter-owned DOS, Redirector, WOW or machine semantics.
