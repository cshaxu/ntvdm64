# Proposal: Bochs machine foundation

## Purpose

Recover the smallest source-audited Bochs 2.6 machine closure and its sole
project-facing mechanical caller, `adapter-bochs`.

## Prerequisites

The repository and neutral runtime foundation candidate is closed. This task
uses its owner manifest and does not implement imported MVDM interfaces.

## Boundary

`bochs-core` remains an original mirror with only registered minimum
intrusions. `adapter-bochs` is the only production caller and owns Bochs-only
construction, bounded run, stop, resume and opaque mechanical endpoints. A
minimal `app` shell may create and destroy this machine, but no OpenNT, DOS,
WOW, BOP, SoftPC or Win32 service meaning enters either Bochs component.

## Proposed subtasks

1. **S1 — Bochs provenance and mirror recovery.** Compare the existing
   `src.old/bochs-core` mirror with the admitted Bochs baseline; carry forward
   exact or registered compliant files, and register every subset,
   same-shaped divergence and private overlay.
2. **S2 — Mechanical lifecycle.** Audit and reuse the existing
   `adapter-bochs` mechanics where their new owner/dependency boundary is
   compliant; recover CPU, RAM, firmware, default port space, reset and
   bounded run/stop/resume solely through that adapter.
3. **S3 — Minimal app machine shell.** Reuse the existing app composition only
   after owner review, then retain only what is needed to create a
   session-owned opaque machine endpoint and destroy it deterministically.
4. **S4 — Boundary and architecture verification.** Build focused machine
   fixtures on x86/x64 and prove no production consumer other than
   `adapter-bochs` calls `bochs-core`.

## Exit criteria

- A minimal machine can be created, reset, run for a finite budget and stop on
  both host architectures.
- `bochs-core` has no dependency on project components.
- `adapter-bochs` contains no OpenNT/DOS/VDM/WOW/Win32/BOP vocabulary.
- All Bochs changes and overlays meet mirror audit rules.

## Non-goals

This task does not load guest images, decode BOP, emulate historical SoftPC
interfaces or claim DOS execution.
