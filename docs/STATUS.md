# Project Status

## Current Work

**Active: T094 — S5 historical software-monitor host-closure investigation.**

## Active Packet

| Field | Record |
| --- | --- |
| Objective | Establish the next caller-proven historical owner needed for a clean OpenNT CCPU/SoftPC composition; do not create a replacement runtime. |
| Identifier | `T094`; the completed historical crosswalk ends at `T093` (`M92`). |
| Scope | Static selector/dynamic reachability and host-owner closure for the retained `BIOS[]` route. |
| Non-goals | External backend integration, reduced BIOS tables, unresolved-link suppression, device/video shims, or a new BOP/SVC dispatcher. |
| Current evidence | `etc/research/ccpu-bios-closure/S5-FULL-NOSM0-OWNER-SEAM-LEDGER.md` and `etc/research/ccpu-bios-closure/S5-BIOS-STATIC-DYNAMIC-OWNER-LEDGER.md`. |
| Applicable rules | `rules/EXECUTION.md`, `rules/ARCHITECTURE.md`, `rules/CODING.md`, and `etc/operations/policy/source-policy.md`. |
| Verification | Retain exact source/build/probe commands and the first reachable boundary; run focused fixtures without adding them to the default runtime graph. |
| Stop conditions | Stop when the next requirement lacks a reached historical caller, owner, data-layout evidence, or bounded failure behavior. |
| Exit criteria | Record an evidence-backed owner disposition or a fail-closed recovery admission with a focused positive and negative fixture. |

## Current Technical Baseline

- The supported control-flow model is `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS`.
- `ntdos64-run` remains an owned non-invasive Windows CLI. PE targets use direct host handoff; DOS MZ/COM requires an explicit user-selected engine; NE/WOW is rejected.
- `ntvdm64`, Bochs, PCjs, and later source trees are read-only comparison inputs, never build, link, runtime, or acceptance dependencies.
- Existing expanded status material is retained at `planning/status.md` as a pre-governance historical ledger. It is not a current authority.

## Recent Closures

- S4 recorded bounded reconstructed-monitor mechanics and retained them default-disabled; it did not admit device, BOP, DEM, or runtime behavior.
- S5 reached the original NTIO/NTDOS loader and early host-boundary research records, while clean complete-`BIOS[]` composition remains blocked by documented static closure.
- The native-image handoff record separated NT4 COMMAND `/z` protocol from the runner's host PE dispatch; it made no guest-runtime claim.
- Historical completed work is indexed by `T001`--`T093` in [history/task-index.md](history/task-index.md).
