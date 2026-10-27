# S10 provisional recovery worklist

This is a dependency-ordered handoff from the current evidence, not the final
S10 scope or permission to bypass its remaining source reconciliation. Entries
separate demonstrated behavioral differences from source-level risks. Stages
follow the approved T473 stage plan; a stage must establish actual owner,
original behavior, lifecycle and regression before implementing a change.

| Stage | Cohort and evidence | Established result / remaining work |
| --- | --- | --- |
| S11 | [CPU far-transfer diagnostic](shard-3/cpu-far-diagnostic-audit.md); [BOP/interrupt diagnostic inventory](shard-1/diagnostic-guest-access-inventory.md); [illegal-op diagnostic](shard-0/illegal-op-diagnostic-audit.md) | Controlled real CCPU fixture shows exception ordering changes with logging disabled; other fixtures establish extra guest reads. Recover original ordering without removing the underlying handlers; prove normal, fault, disabled/enabled and nested paths. Actual guest fault incidence remains unproved. |
| S11 | [SAS vector](shard-3/sas4gen-vector-audit.md) | Product vector contains breakpoints and field-layout differences, but reachability and consumers are unproved; trace indirect/generated users before a repair or exclusion. |
| S12 | [Disk close](shard-3/disk-close-failure-audit.md) | Actual-object failure seam demonstrates partial retirement after guest counter update failure; real reachability is unproved. Establish guest counter and resource ownership, retry/teardown and both floppy/fdisk outcomes. |
| S12 | [ForceDOS](shard-3/forcedos-original-owner-audit.md); [PIF](shard-3/pif-config-lifecycle-audit.md) | ForceDOS consumer rejects an encoded original flag; nested guest execution still needs proof. PIF state retention is a source-level question, not an established defect. Trace task and worker owners before choosing changes. |
| S13 | [ROM and CPU register owners](shard-1/rom-residency-xt-audit.md) | Required embedded ROM failures return zero and rom_init ignores the result; deployed failure reachability and outer handling are unproved. Verify required resources, protection, startup failure and register/parity ownership. |
| S13 | [Codepage transition](shard-1/standalone-tool-chain-audit.md) | Independent console carrier codepages were observed; establish original console authority and test both native and guest output before modifying propagation. |
| S14 | [Generic thunk](shard-0/wow-generic-thunk.md) | Product-object mismatch failure and real guest differential exist; candidate original assembly needs an EBX-safe ABI. Complete convention/exception/lifecycle proof and coherent package tests before publication. |
| S14 | [DDEML](shard-2/ddeml-owner-boundary.md); [WINMM callback](external-winmm-callback-boundary.md) | Historical private dependencies are identified but actual guest/provider behavior and recoverability remain pending. Trace callback identity, queue delivery and task teardown; never infer absence from an unselected provider alone. |
| S15 | [Clipboard formats](shard-2/wow-clipboard-format-failure-review.md); caret finding in [cross-shard findings](findings.md) | T460 covers text and DIB, not every format; fault/ownership paths require format-specific tests. Existing caret evidence is inconsistent and needs an actual guest witness. |
| S16 | [WOWFAX](shard-1/wowfax-trace.md); multimedia source-selection findings in [cross-shard findings](findings.md) | Object compilation does not prove reverse MCI, aux/joystick, fax installation or runtime behavior. Establish actual guest/provider route and its dependency boundary. |
| S17 | Registry identifiers and selected-versus-linked reconciliation in [cross-shard findings](findings.md) | Resolve misleading provenance and owner records without treating documentation collisions as runtime defects; unresolved unselected and external paths remain in the exhaustive audit. |

S18 removes only providers actually displaced by verified recoveries. S19
reconciles every applicable path and runs the complete product and capability
gates. Neither this list nor prior per-package acceptance converts pending
rows to full/excluded, and the 42,963 external rows still require relevance
review. Preserve original guest bytes and the owner-approved host boundaries.
