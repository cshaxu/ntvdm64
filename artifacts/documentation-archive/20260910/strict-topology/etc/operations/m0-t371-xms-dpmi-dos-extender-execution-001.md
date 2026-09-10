# M0 T371 — XMS, DPMI and DOS-extender execution

## Purpose

Recover the next dependency-ordered original execution package: XMS memory
services, DPMI/DPMI32 host paths and the selected DOS-extender handoff.  The
task starts from the existing original mirrors and their same-shaped CPU40,
Base VDM and Win32 bindings.  It does not restart BOP-wide discovery or create
a parallel provider.

## Boundaries

- Original XMS sources own XMS allocation, A20, UMB and INT 15 results.
- Original DPMI/DPMI32 sources own descriptor, protected-memory, interrupt and
  DOS-extender semantics.
- `adapter-mvdm-host-out/softpc` owns only checked mapping-manager conversion
  to CPU40/SAS operations; it cannot invent XMS/DPMI policy.
- Guest DOS/DOSX media remain immutable.  Any guest execution observation uses
  the fixed `O:\\ntvdm64` stage only after its source and formal closure.
- CPU30, Bochs, synthetic BOP outcomes, a new monitor, WOW/Redirector work,
  and x64 recovery are outside this task.

## Subtasks

1. **S1 — Original owner and execution-topology audit.** Re-read selected
   XMS, DPMI/DPMI32 and DOS-extender callers, their current production
   bindings and final CPU40/x86 graph.  Record exact operations that can be
   reached from an immutable workload, source order, mapping/handle ownership,
   returned failure paths and one complete earliest recovery cohort.
2. **S2 — Original XMS runtime cohort.** Recover the selected original
   XMS/A20/UMB/INT15 cohort through the existing mapping-manager and CPU40
   seams; add source-owner focused proof and formal-link evidence.
3. **S3 — Original DPMI/DOS-extender runtime cohort.** Recover the selected
   DPMI/DPMI32/DOSX cohort, retaining original descriptor/protected-memory and
   failure contracts; prove selected source and formal-link ownership.
4. **S4 — Frozen integration disposition.** Make only the permitted fixed
   workload observation(s), classify the earliest source-owned result and
   close without treating a trace as permission for a leaf repair.

## Exit criteria

Every reached XMS/DPMI/DOS-extender edge has an original owner/disposition;
each selected whole cohort passes focused proof and formally links into the
CPU40/x86 product; the allowed frozen observation is classified.  This task
does not claim unobserved DOS-extender, WOW, native-child or cross-family
broker behavior.
