# M0 T281 S5 — original host-control disposition evidence

## Question

Can the complete original `softpc.new/host/src` control plane be divided into
whole-owner recovery groups without re-auditing individual BOP leaf services or
silently reintroducing a SoftPC machine executor?

## Inputs

- `docs/etc/operations/m0-t276-s12-host-src-disposition-ledger.tsv`, the
  completed 56-source owner audit;
- `docs/etc/operations/m0-t277-effective-package-cycle-ledger.tsv`;
- `docs/etc/operations/m0-t280-s2-ingress-machine-facade-ledger.tsv`; and
- the reproducible S5 generator and verifier in `tools/governance/`.

## Procedure

`New-T281HostControlDispositionLedger.ps1` assigns each T276/S12 role group
one final S5 disposition and emits a TSV. `Test-T281HostControlDisposition.ps1`
requires exactly 56 rows, unique source paths, one of the four admitted
dispositions, and both next-owner and boundary-owner fields.

## Observations

The generator and verifier passed. The complete partition is:

- 23 `adapter-prerequisite` sources: 10 need the adapter-softpc/
  adapter-bochs mechanical facade, 7 need adapter-win32 host capability, 4
  need adapter-vdm-monitor, 1 needs the session/monitor group, and `nt_bop.c`
  needs adapter-bop/adapter-softpc plus complete provider groups;
- 18 `bochs-replacement` sources: original machine-device/profile execution
  remains excluded because Bochs is the sole machine/device owner; and
- 15 `specialist-owner` sources: VDD/debugger, original unobserved build
  auxiliaries, or cross-owner launch/startup/video composition.

No source is `eligible-next` without first closing one of those named
boundaries.

## Interpretation

This is a complete planning partition, not a claim that the 23 prerequisite
boundaries are implemented. The next recovery packet must select a whole
source-proven prerequisite package—most directly the adapter-softpc/
adapter-bochs mechanical facade—rather than compile an all-host aggregate or
activate a BOP selector. The Bochs-replacement set remains outside all MVDM
control libraries, preventing a parallel device or CCPU executor.

## Follow-up

Use this ledger to admit the next whole owner package in Queue order. VDD,
debugger, unobserved original build auxiliaries, and cross-owner composition
require their own complete package proposals before activation.
