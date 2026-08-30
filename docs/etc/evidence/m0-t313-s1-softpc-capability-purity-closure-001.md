# M0 T313 S1 — SoftPC Capability And Mirror-Purity Ledger Closure

## Result

S1 closes the single whole-package inventory pass required by T313.  The
ledger records eleven selected machine/lifecycle cohorts, their exact original
root families, existing formal x86 CCPU40 selection, retained adapter or
overlay form, and the only local witness that can complete each cohort.

The authoritative machine inventory is
[the S1 ledger](m0-t313-s1-softpc-capability-purity-ledger-001.tsv).  It is
derived from the current formal `source-manifest.json` emitted by
`New-T310OriginalSoftpcNinja.ps1`, rather than an inferred directory scan.
That manifest selects 131 original CCPU sources plus the original BIOS,
keymouse, system, disks, video/C-VID, communications, DOS, host-root and
support cohorts.

## Purity Review

- A literal production-source scan of `src/mvdm-host` and
  `src/mvdm-host-overlay` found no `bochs`, `adapter-bochs` or `src.old`
  reference.  Register names such as `EBX` are CPU vocabulary, not retired
  Bochs references.
- The formal generator retains those three names only in its negative
  `forbiddenInputs` assertion.  It is therefore a prohibition, not a build
  dependency.
- The overlay root has 17 files.  Eleven selected mirror source files contain
  registered `DIVERGENCE:` markers.  They are not treated as automatically
  acceptable: S2--S5 must reread the portion each cohort reaches and either
  remove it, replace it with an available original source/interface, or retain
  it with its existing source-contract register entry.
- No unregistered non-original source or stale machine route was found in this
  inventory pass.  Consequently there is no safe speculative deletion in S1;
  the owner's immediate-cleanup direction is now a required action in every
  later owner cohort, not deferred debt.

## Ordered Transfer

- S2 owns CPU/FPU/SAS, EMS/UMB, firmware and source-shaped reset.
- S3 owns PIC/PIT/DMA/timer and input controller lifecycle.
- S4 owns video/C-VID, storage/floppy and communications endpoint lifecycle.
- S5 owns the bounded execution-to-teardown witness and the final reread of
  all divergences reached by S2--S4.

## Limitation

The selected formal graph and this ledger prove selection and ownership, not a
guest workload or device-runtime success.  x64, CPU30/MONITOR, Bochs, kernel
VDM and CSRSS remain outside T313.
