# Proposal — Bochs-core diff and overlay minimization

## Objective

Reduce the retained divergence from the pinned Bochs 2.6 mirror and reduce the
private `bochs-core-overlay` implementation surface without changing guest,
BOP, OpenNT, SoftPC, or device behavior. The task is an ownership correction:
external machine configuration and lifecycle decisions belong in
`adapter-bochs` (or an already-declared specialist adapter), while only code
that must access a Bochs private CPU, memory, or device object remains a
private Bochs-core overlay hook.

The baseline is the pinned Bochs 2.6 tree and the per-file register in
`src/bochs-core/README.md`; `src.old/` remains comparison-only evidence.

## Required boundary

- `adapter-bochs` remains the sole production caller of `bochs-core` and owns
  Bochs-only construction, configuration selection, lifecycle ordering and
  opaque mechanical requests.
- `bochs-core` remains an upstream mirror. A retained changed line must be an
  original include/build adjustment, a true subset crop, or the smallest
  declaration/call boundary needed to reach a private overlay.
- `bochs-core-overlay` is private to `bochs-core`: no adapter, app, session,
  fixture, or OpenNT source may include, link, or call it.
- `adapter-softpc` keeps only same-shaped historical SoftPC/CCPU interfaces;
  it never gains a Bochs type, object, global, or private-state operation.
- `adapter-bop`, `app`, and `session` may receive only their already-owned
  copied-record, composition, or lifecycle decisions. They may not absorb
  Bochs CPU, RAM, port-space, PIC, keyboard, or firmware semantics.

## Proposed subtasks

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Produce a line- and symbol-level disposition ledger for every current `bochs-core` divergence and every overlay body. Classify each as required private hook, external composition candidate, optional diagnostic, true-subset crop, or removable. | Every retained/non-retained line group has pinned-source evidence, one final owner, dependency proof and a diff/line-count baseline. |
| S2 | Move Bochs-only configuration and lifecycle decisions that do not access private Bochs state into `adapter-bochs`; preserve only opaque typed calls back into `bochs-core`. | No migrated code introduces OpenNT/DOS/VDM/WOW/Win32/BOP vocabulary into either Bochs component; `adapter-bochs` remains its sole caller. |
| S3 | Re-evaluate `minimal_port_space`, PIC and keyboard lifecycle against upstream minimal composition. Reuse an upstream path where it closes without SIM/plugin/product-shell coupling; otherwise retain the smallest private member definitions. | Every device-lifecycle body is either upstream-composed or registered as irreducible private Bochs state access; no duplicate assembly remains. |
| S4 | Re-evaluate CPU observation and callback fragments. Delete observers that are not required by an admitted product contract; retain `#UD`, profile, resume and copied-state mechanics only when a public outer boundary cannot replace them. | Every surviving CPU hook has a named consumer and default-off/failure behavior; no diagnostic-only body remains in the formal product closure. |
| S5 | Re-evaluate minimal-memory and checked-RAM bodies. Move only allocation/configuration decisions outward; retain bounds checks and private memory-table operations in the overlay. | The mirror exposes only the minimum call/declaration seam, while private-state operations remain overlay-private and have focused x86/x64 verification. |
| S6 | Re-measure all mirror diffs and overlay code, refresh exception registers and build manifests, and run focused x86/x64 formal fixtures. | Diff and overlay reduction are evidenced; all remaining differences meet mirror rules; documentation governance and `git diff --check` pass. |

## Non-goals

- No new BOP selector/provider, guest trace, OpenNT service, SoftPC semantic,
  hardware device, firmware behavior, or host capability.
- No direct adapter access to Bochs private state and no public overlay ABI.
- No movement performed only to improve a metric: an owner move must preserve
  the original mechanical behavior and reduce an actual mirror intrusion.
- No new generic adapter or compatibility root.

## Acceptance measures

The task records the before/after number of divergent `bochs-core` files,
changed lines, overlay code lines, and private hook entry points. Success is
not a numeric target alone: every retained core difference must be necessary
and minimal, while every externalizable configuration or lifecycle decision is
owned by `adapter-bochs` or its existing correct component. Formal Ninja
verification remains x86 `/MT` and x64 `/MT`; the task may not use a build or
runtime input from `src.old/`.
