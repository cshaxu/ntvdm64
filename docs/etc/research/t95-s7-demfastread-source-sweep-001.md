# T95 S7 DEMFASTREAD Source-Recovery Sweep 001

## Question

Does the imported OpenNT MVDM source tree contain a recoverable historical
implementation or registration path for the reached `50:42`
`SVC_DEMFASTREAD` BOP?

## Inputs

- The immutable runtime evidence root
  `artifacts/analysis/t95-s7-runtime-trace-009-20260811-001/`: its retained
  log reaches `C4 C4 50 42` with `AX=4001`, `BP=4e53` and records pass-through.
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm`.
- `src/opennt/base/mvdm/dos/dem/demdisp.c`.
- `src/opennt/base/mvdm/inc/{DOSSVC.INC,dossvc.h,vint.h}`.
- `src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c`.

No external source, guest media, build, or runtime execution was used.

## Procedure

1. Searched the imported MVDM tree for the exact symbolic service names
   (`SVC_DEMFASTREAD`, `DEMFASTREAD`, `fastread`, and `fast_read`), including
   the historical SoftPC host subtree.
2. Read the call site and `FastOrSlow` branch in `handle.asm`.
3. Read the DEM dispatch-table entry and the behavior of
   `demNotYetImplemented`.
4. Cross-checked the fixed-state flag definition and the i386 initialization
   performed by `nt_msscs.c`.

## Observations

- `DOSSVC.INC:82` and `dossvc.h:158` define `SVC_DEMFASTREAD` as `42h`.
- `handle.asm:459-466` calls `FastOrSlow`, executes `SVC DEMFASTREAD` on the
  fast branch, and invokes `DEMREAD` only if that BOP returns CF set.
- `handle.asm:564-579` selects the fast branch for a non-pipe handle when
  `MIPS_BIT_MASK` is clear.
- `vint.h:40` defines that bit as `0x400`; `nt_msscs.c:124-127` clears it on
  i386. The reached x86 branch is therefore intentional.
- `demdisp.c:165` maps the service to `demNotYetImplemented`; its definition
  at `demdisp.c:230-241` merely clears CF. The matching fast-write entry is
  likewise a placeholder.
- The exact-symbol/source sweep finds no alternative `DEMFASTREAD` owner,
  fast-I/O worker, or registration site in the imported OpenNT MVDM and
  `softpc.new` trees. The ordinary `demRead` implementation in `demhndl.c`
  is a different, slow-path host service with a distinct transfer contract.

## Interpretation and Confidence

High confidence that the imported source set does not contain a directly
recoverable fast-read implementation. The evidence proves a missing historical
x86 host-composition capability, but does not prove that no such code ever
existed in an unavailable NT build component. In particular, `demNotYetImplemented`
returning CF clear means that substituting `DEMREAD`, or forcing CF set, would
change the original DOS-kernel branch rather than restore it.

## Decision and Follow-up

`50:42` stays observed and pass-through. It is neither a Bochs machine
requirement nor a reason to alter the private namespace token or the fixed
MIPS-state word. A future implementation requires a separately admitted,
source/ABI/failure-derived fast-I/O contract; until then the next work item is
to retain the generic listener and classify the next actually reached BOP,
not to add a broad dispatcher or an ambient host filesystem.
