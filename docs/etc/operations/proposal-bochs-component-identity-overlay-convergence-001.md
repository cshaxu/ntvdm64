# Proposal — Bochs component identity and mirror-overlay convergence

## Objective

Converge the adopted Bochs code boundary on an auditable mirror/overlay model
without changing product behavior. Rename the two existing Bochs components,
move upstream-originated material to the Bochs mirror or its dedicated overlay,
and apply the established mirror-component audit standard to both the Bochs
and composed OpenNT host mirrors.

## Required component contract

- `adapter-bochs` is the renamed, project-authored Bochs assembly and access
  adapter. Only `app` may call it in a production dependency graph.
- `bochs-core` is the renamed adopted Bochs mirror. Only `adapter-bochs` may
  call it in a production dependency graph.
- A `*-overlay` is private to its corresponding native mirror: only `<name>`
  may call `<name>-overlay`; no other component may directly include, link, or
  invoke it.
- These rules do not license Bochs or OpenNT semantics to move into a wrong
  owner. `adapter-bochs` remains Bochs-only and must not gain OpenNT, DOS,
  VDM, WOW, BOP, or Win32 vocabulary.

The task adopts the owner-directed naming change exactly. Any future
OpenNT-facing machine API remains `adapter-softpc` work, and must reach the
machine through admitted `app`/session wiring rather than a forbidden direct
production edge to `bochs-core`.

## Packets

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Governance rename and dependency policy: revise architecture, coding, build, and component naming authorities from `bx-mantle`/`bx-core` to `adapter-bochs`/`bochs-core`; add the private-overlay rule. | All authorities express the same directed dependency rule and no candidate code is moved. |
| S2 | Perform the two root renames with `git mv`; repair production build manifests, includes, fixture paths, and documentation references. | `src/bx-mantle` and `src/bx-core` have no live production inputs; renamed graph compiles in its existing formal closure without behavior expansion. |
| S3 | Classify every source file in `adapter-bochs`. Move upstream-original, true-subset, or same-shaped modified Bochs material to the corresponding upstream-relative path under `bochs-core`, or to `bochs-core-overlay` when the mirror rule requires it. Retain only self-authored minimal Bochs assembly in `adapter-bochs`. | Every retained adapter file is project-authored, Bochs-only assembly; each moved mirror/overlay file has provenance and exception registration. |
| S4 | Audit every differing `bochs-core` file under the mirror-component standard. Move each >50-percent divergence or >3 executable-line semantic intrusion into `bochs-core-overlay`, leaving only the smallest marked mirror call boundary. | A per-file ledger records exact/subset/same-shaped/overlay disposition, changed-line measurement, local `DIVERGENCE:` markers, README registration, and focused build/fixture evidence. |
| S5 | Audit every differing `opennt-host` mirror file under the same standard. Move each >50-percent divergence or >3 executable-line new semantic body into `opennt-host-overlay`, preserving only minimal same-shaped mirror boundaries. | An equivalent host ledger, README register, source-first justification, and focused formal evidence show no oversized self-authored body remains in `opennt-host`. |

## Non-goals

- No BOP feature, guest behavior, Bochs CPU/device semantic, OpenNT provider,
  or host-capability expansion.
- No generic overlay/common/compat component and no direct consumer access to
  either overlay.
- No deletion of source merely to improve a similarity percentage; every move
  preserves provenance, a smallest call boundary, and a documented owner.

## S3 required dependency repair

The S3 source audit found existing direct `adapter-softpc` includes of
`adapter-bochs` and `bochs-core` headers.  Those edges contradict the already
admitted direction; they cannot be hidden by moving `pc_system` alone.
Before S3 can close, its final implementation pass therefore creates an
app/session-bound, copied-data machine callback contract.  `adapter-softpc`
keeps the historical SoftPC/CCPU-facing interface shape but invokes the bound
mechanical contract rather than importing Bochs objects.  `app` remains the
only production caller of `adapter-bochs`, and `adapter-bochs` remains the
only production caller of `bochs-core`.  This is a behavior-preserving
dependency inversion, not a BOP or machine-feature expansion.

## Verification

Each packet uses a source/provenance ledger, `git diff --check`, documentation
governance verification, an include/link dependency scan, and the relevant
formal Ninja target. S4 and S5 additionally measure each non-exact mirror file
against its pinned upstream baseline and prove the overlay privacy rule in the
build graph.

## Admission evidence

Owner admitted this proposal as **M0 T265 S1** on 2026-08-24. S1 is limited
to authority and dependency governance; source movement starts only after its
separate closure and the next admitted S packet.
