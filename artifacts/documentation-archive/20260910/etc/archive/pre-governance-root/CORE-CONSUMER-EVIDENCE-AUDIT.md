# Core Consumer Evidence Audit

Status: M49 read-only audit, 2026-08-08. This record evaluates the current
`ntvdm64` source tree as a possible independent consumer of the two neutral
capabilities in `CORE-DELTA-HANDOFF-RECORD.md`. It changes neither repository.

## Result

No independent consumer is currently evidenced for either configured
decode-point transition or paused ordinary-RAM multi-span patch transaction.
The external adapter therefore remains insufficient to request either core
extension by itself.

## Candidates Rejected

| Candidate | Current source evidence | Why it does not satisfy the gate |
| --- | --- | --- |
| Default PC/AT firmware construction | `src/vm/profile/default_profile/firmware/bios.c` writes BDA, IVT, and firmware setup bytes with existing real-mode one-span writes before execution. | It is product-owned initial-state assembly, not a paused runtime transaction. Entry-plan/ROM work already covers the relevant topology and preload class. |
| Debug target writes | `src/vm/composition/session/debug_target.c` exposes one normal debug memory write at a time. | There is no specified multi-range edit, rollback rule, or all-or-none publication requirement. Calling a sequence of ordinary writes a transaction would invent a consumer. |
| VM session pause/run control | `src/vm/composition/session/control.[ch]` owns product lifecycle state. | A pause state alone does not create a need for copied multi-span publication or a generic decoded-marker observation point. |
| Media replacement/save atomicity | VM media code and architecture documents use atomicity for host backing-file replacement. | Host file replacement has different ownership, failure and visibility semantics from guest ordinary-RAM publication. It cannot establish Capability B. |
| Existing undefined-instruction transition | `src/core/machine/undefined_instruction_transition_interface.h`. | It is intentionally a real-mode `#UD` facility; no VM product registers a general configured decode-point transition. It remains a useful design precedent, not Capability A's second consumer. |

## Consequence

The current core boundary must remain unchanged. `ntdos64` may retain its
profile/transaction fixtures and historical-source evidence, but it must not
add a local executor, serial-write substitute, BOP dispatcher, or a direct
source dependency to bypass the gate.

## Reopen Conditions

Capability A may be reconsidered only when NXVM, NXVDM, or a core-owned
facility independently needs a configured, frozen byte-pattern observation
point with the same finite outcomes and no protocol-specific meaning.

Capability B may be reconsidered only when a product has a specified paused
multi-range guest edit that must reject invalid spans before mutation and
publish all accepted bytes together. A debugger feature could qualify only
after that behavior is an actual product requirement, not a hypothetical
test consumer.

Until then, the next executable-path work is historical-host evidence and
contained-profile validation; a future core owner, not this research project,
decides whether equivalent broad reuse justifies implementation.
