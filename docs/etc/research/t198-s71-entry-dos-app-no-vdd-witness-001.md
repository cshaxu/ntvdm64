# M0 T198 S71: DEM App-Entry No-VDD Witness

## Result

The active composition now calls the existing exact `50:36` no-VDD provider.
It resumes after four bytes with no CPU delta; it does not read the PDB,
launch a host process, modify the stack/entry, or implement VDDs.  Original
NTDOS `$Exec` retains ownership of the following guest transfer.

## Clean source-built witness

The complete MSVC x64 `/MT` run at
`artifacts/build/t198-s71-entry-dos-app-novdd-r1` returns finite-run status
`0`, which `bx_ntvdm_finite_run.h` defines as
`BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET`.  There is no subsequent BOP and no
generic-UD controlled stop.  The terminal CPU snapshot is:

```text
CS:IP=0032:5a82
```

This is an instruction-budget terminal, not a successful app/CLI completion
or a guest HLT claim.

## Reconciliation and next frontier

The same post-`50:36` location is retained in prior bounded evidence
(`t137-s1-prefetch-predecessor-observation-001.md`).  Separately,
`t161-s1-extended-native-post-observation-001.md` demonstrates that a longer
unchanged-class observation can continue through COMMAND work to an existing
controlled `FE` target boundary.  Therefore the current result is not
evidence for a missing BOP provider, a Bochs defect, or a host process launch
requirement.

S72 must audit the finite execution objective, budget and controlled-stop
contract before another runtime extension.  It must not add a BOP handler or
device merely to force progress.

