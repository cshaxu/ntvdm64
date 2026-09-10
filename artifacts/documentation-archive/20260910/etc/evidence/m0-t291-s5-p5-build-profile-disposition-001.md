# M0 T291 S5 P5 — Build/profile disposition audit

## Result

The build/profile ledger is an audit of future selection, not a build claim.
Each selected package now has one of two outcomes in both host architectures:

- **static-closure pending** when its reconciled file rows contain an
  `adapter-backed`, `binding-only` or `overlay-required` member. The next
  static-closure task must select its exact source list, original build inputs
  and libraries before compiling it.
- **profile-excluded** when all its selected rows are `tool-only`,
  `firmware-only`, `guest-only` or `not-host-runtime`. Its original mirror is
  retained, but it has no x86/x64 main-host target.

The accepted external Base VDM slice is static-closure pending. No row claims
a successful compile, link or local run. Its selected subset, private adapter
contracts and `srvinit.c` evidence-only relocation are prerequisites.

## Evidence

`mvdm-file-recovery-ledger.tsv` final dispositions, T276 closure, and the
current Base VDM external-file disposition audit.
