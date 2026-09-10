# M0 T312 S1 live Bochs dependency audit

## Question

Which remaining Bochs references are live product dependencies, and which
original SoftPC/CCPU40 package must replace them before deletion?

## Findings

- The selected T310 formal CCPU40 graph already forbids Bochs input and does
  not compile or link a Bochs object.
- Eight production source files still include or name `adapter-bochs`.
  They are concentrated in an earlier self-authored machine facade rather
  than in an imported MVDM mirror.
- The only true product lifecycle caller is `app/machine_shell.c`; it still
  deliberately rejects the selected SoftPC backend and creates a Bochs
  lifecycle. This is the principal S2 transfer.
- Register, SAS, real-mode stack and protected-frame facade files must return
  to their original CCPU40/SAS/DPMI32/monitor owners. None authorizes a second
  machine abstraction.
- All listed Bochs-specific build scripts and fixtures are retired experiment
  evidence. They do not participate in the selected formal graph and are S3
  removal/rehome candidates.

## Result

The complete disposition is recorded in
`m0-t312-s1-live-bochs-dependency-ledger.tsv`. S2 can now replace the live
facade calls package-by-package; it must not delete the Bochs source roots
until those replacements compile in the selected graph.
