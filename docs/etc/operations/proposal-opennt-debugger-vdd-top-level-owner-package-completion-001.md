# Proposal: OpenNT Debugger, VDD and Top-Level Owner-Package Completion

## Purpose

Recover the OpenNT debugger, VDD, console/input, and top-level notification/service routes
that are part of declared CLI NTVDM profiles, with separate explicit
disposition for genuinely diagnostic-only or excluded facilities.

## Boundary

OpenNT remains owner of debugger/VDD lifecycle and failure contracts.  The
CLI may use supported user-mode debugging, console and device capabilities
where the original contract permits.  It must not install drivers, restore
NT4 system APIs, load historical host DLLs as a shortcut, or put VDD/DOS
semantics into Bochs.

## Admission Plan

1. **S1 — audit:** map debugger selectors, VDD dispatch, console/input
   notifications (including `cmdPushExitInConsoleBuffer`), idle/scheduler hooks
   and top-level `nt_bop.c` routes to original source and
   classify product-required versus genuinely debug-only endpoints.
2. **S2 — code complete:** implement the selected whole provider set or
   source-derived compatible capability seams; retain evidence-backed deferred
   results only for excluded diagnostic facilities.
3. **S3 — verification:** run the complete family’s positive/negative local
   regression and one bounded native observation for the selected profile.

## Exit Rule

No blanket “debugger unavailable” disposition is accepted until this audit
separates product-visible behavior from optional tooling.
