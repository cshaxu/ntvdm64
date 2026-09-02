# M0 T375 — Pure DOS CPU40/SoftPC boot and COMMAND baseline

## Purpose

Prove the first real DOS execution baseline before any resumed WOW/Win16 work:
the selected CPU40/SoftPC x86 product starts from immutable executable-relative
media, reaches original `COMMAND.COM`, and completes one declared built-in.
This is a source-first vertical package, not a restart of trace-driven BOP
repair. The existing DEM, COMMAND, XMS/DPMI and Redirector static closures are
inputs; they are not runtime proof.

## Product boundary

- The only machine route is original SoftPC `CPU_40_STYLE` / CCPU40 on
  Win32/x86. CPU30 and Bochs are excluded.
- DOS, NTIO, NTDOS and COMMAND remain immutable guest media loaded through
  their original guest contracts. The app may select their
  executable-relative roots; it must not synthesize a loader or modify bytes.
- Host-side work retains original SoftPC/MVDM ownership. A public modern API
  can appear only behind the source-shaped original owner or its named adapter.
- This task stops before guest external-program `$Exec`, MZ relocation, parent
  return, native children and all WOW/Win16 service paths.

## Ordered subtasks

1. **S1 — Boot/COMMAND topology and fixed-container admission.** Reconcile
   the formal CPU40/x86 product, executable-relative stage, immutable media,
   console ownership and original call path from `ntvdm.c` through SoftPC
   startup, NTIO/NTDOS and first COMMAND acquisition. Identify one earliest
   actual terminal or completion and select the smallest source-owned S2
   cohort. No production repair occurs in S1.
2. **S2 — Earliest original startup-owner closure.** Recover the one complete
   original SoftPC/MVDM host cohort selected by S1 (for example console,
   display, timer, termination or an already linked startup service) through
   the recovery ladder, formally link it and prove its positive/negative local
   contract. A different owner requires a revised S brief; no leaf patching.
3. **S3 — Fixed original COMMAND built-in observation.** Rebuild only the
   selected graph, stage the identity-proven media and make one bounded,
   non-debug observation. Prove either original COMMAND built-in completion or
   the next exact source-owned terminal. The latter selects an additional
   complete S only if it remains inside this package boundary.
4. **S4 — Baseline closure and next-package handoff.** Record the confirmed
   runtime result, update the BOP/runtime ledger only for reached owners, and
   hand off to pure-DOS `.COM`/MZ `$Exec`/parent-return work. Do not start that
   next package here.

## Completion standard

T375 closes only when a source- and identity-proven fixed runtime container
shows original `COMMAND.COM` completing a declared built-in, or when a finite
unavailable original pre-COMMAND terminal is recorded with its exact owner and
all smaller same-owner source/facade routes exhausted. A link, fixture, BOP
ingress, or timeout without source attribution is insufficient.
