# Project Status

## Current Work

**Active: M0 T481 S5 — combined qualification and delivery.** S1–S4 are
closed; the T remains open. See the [S4 evidence](../etc/evidence/m0-t481-s4-closure.md),
[S3 evidence](../etc/evidence/m0-t481-s3-closure.md) and the
[working plan](../etc/operations/m0-t481-multitask-completion-plan.md).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S5, Ordinary Mode. |
| Admission And Approval | Owner approved continuous execution and per-S complete delivery. |
| Objective | Audit all admitted obligations against evidence, complete cached builds and applicable integrated Win3.1/Win95, DOS and WOW regression; publish and verify the complete host and addon package and W31 driver pair. |
| Non-goals | No original guest modification/rebuild, hardware V86 monitor, unrelated 486 changes or speculative protocol expansion. |
| Reference Baseline | Candidate579 package `2f22fbfe-ee21-4df6-a59b-ef50997e42ff`, S3 Full549 16/16; S4 native002, reuse001, init002 and editor003. |
| Files And ABI Surface | Existing product, addon, integration tests, manifests and task evidence; only reached defects may require implementation. |
| Applicable Rules | Execution, architecture, coding, source policy and approved BIOS/INT16 unread-input return boundary. |
| Verification | Cached x86/x64 builds, applicable Full gates, four Win3.1 mode/DOSONLY cases, actual Win95 tests and complete package/addon/W31 hashes; preserve diagnostic versus production distinctions. |
| Expected Markers | Both VM chains complete without cross-delivery or stranded workers; guest files unchanged; package and W31 hashes agree. |
| Asset Needs | Existing Win3.1 and Win95 installations on isolated copies, cached builds, test probes and release staging. |
| Reporting Requirements | Requirement-by-requirement evidence, deviations, exact test and package hashes, release destinations, commit and push. |
| Stop Conditions | New semantic concession or original guest patch required; preserve evidence and continue independent work. |
| Exit Criteria | All original obligations evaluated without claiming unsupported paths passed; bounded coverage or approved immutable-guest exceptions stated, required tests pass and complete product is built, published, committed and pushed. |
| Original Owner Request | Complete the multitask payload; each S builds, tests, publishes, commits and pushes; update W31 for owner testing. |
| Similar-Issue Sweep | VM isolation, shutdown, input handback, window presentation, native/DOS nesting, source ownership and package coherence. |
| Temporary Evidence | `build/t481-s5`; task-owned runs, 24-hour/4-GiB review budget. Archive necessary outcomes and hashes before cleanup. |

## Current Technical Baseline

Candidate001 is published: eleven host images match both destinations;
`NTVDM64.386` and `INSIGNIA.386` match `O:/w31/PATCH` and release. Cached
x86/x64 builds, Full 16/16 and four Win3.1 mode/DOSONLY cases pass; see
[qualification](../../artifacts/t481-s5/qualification001.json),
[publication](../../artifacts/t481-s5/publication001.json), and
[obligation audit](../../artifacts/t481-s5/obligation-audit.md). The optional
DOS4GW/NESticle inherited-environment test reproduces its existing TODO failure;
the minimal environment control reaches graphics. S4 proved
production-host four native commands across two Win95 VMs, real handle reuse,
and diagnostic initialization rollback; its interactive mouse/editor test uses a
diagnostic host. Win95 observer exit code 1 and the historical direct native
batch path remain explicitly unproved as normal behavior. They must not be
reported as passing; see [S4 closure](../etc/evidence/m0-t481-s4-closure.md).

S5 delivery is complete and awaits owner acceptance; the T remains open.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.
