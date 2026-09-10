# Proposal — Mirror-component conformance reconciliation

## Objective

Bring every original-code mirror component and its corresponding private
overlay, one component per subtask, into the current mirror-component review
standard. This is a source-layout and ownership repair: it preserves behavior
and does not itself recover a new OpenNT service or Bochs machine feature.

## Scope and sequence

1. **S1 — `bochs-core`.** Reconcile every retained Bochs difference with the
   pinned Bochs 2.6 original; register local markers; return original or
   source-subset port-space bodies now stranded in `adapter-bochs` to the
   mirror/overlay boundary; retain only self-authored machine assembly in the
   adapter.
2. **S2 — `opennt-guest`.** Recompute source/artifact ownership and reconcile
   every guest source divergence with its selected OpenNT original.
3. **S3 — `opennt-host`.** Independently recheck T269's final inventory,
   including private binding reachability, marker/register identity and any
   source-body owner edge subsequently affected by S1/S2.
4. **S4 — `opennt-bop`.** Consume the former BOP mirror-convergence candidate:
   classify every BOP mirror body and private BOP overlay without changing a
   BOP route or provider semantics.
5. **S5 — `opennt-softpc`.** Classify firmware, ROM and machine-contract
   inputs; no unadmitted machine composition or device enablement is allowed.
6. **S6 — `opennt-utils`.** Classify each selected original utility input and
   remove or relocate non-mirror material.

## Common completion rule

For its sole component, each S must prove every production file is exactly one
of: byte-identical selected original; locally marked and README-registered true
subset; locally marked and README-registered same-shaped modification below the
50-percent retained-line limit; or private overlay code reached only through
the matching mirror. A greater-than-three-line new semantic intrusion must be
implemented below that component's overlay root. No adapter, test, app or
other mirror may directly consume an overlay.

## Required records

Each S produces a source identity/disposition ledger, changes the matching
README exception register where required, scans external overlay consumers and
source-body ownership, and records focused build/fixture results. Every
proposed source move uses `git mv` when ownership is pure.

## Non-goals

- No new BOP, OpenNT-host, guest, CPU, firmware, device, Win32 or CLI ability.
- No trace-driven change and no feature enablement.
- No generic compatibility/overlay component.
- No change to another mirror component in a component-specific S.

## Admission

Owner instruction: “请你现在以新的T任务治理清楚，每个S任务治理一个镜像组件。”
This proposal is admitted as M0 T270. S1 alone is active initially.
