# T152 S1 original fast-read execution-path audit 001

## Question

Does the T149 `50:42` / no-`50:16` sequence conform to original source, and
can it explain why the eventual transfer reaches outside the staged COMMAND
file?

## Original contract

`handle.asm:458-468` first calls `FastOrSlow`. On its fast branch it invokes
`SVC_DEMFASTREAD` (`50:42`) and uses `DEMREAD` (`50:16`) only if the fast BOP
returns CF set. `FastOrSlow` (`handle.asm:564-579`) selects that fast branch
for a non-pipe handle when `MIPS_BIT_MASK` is clear.

This is intentional for historical i386: `vint.h` defines the bit as `0x400`,
and `nt_msscs.c:124-127` clears it during original i386 DOS-emulation
initialization. For `50:42`, the original DEM table explicitly selects
`demNotYetImplemented` (`demdisp.c:165`); its body (`demdisp.c:230-241`) does
only `setCF(0)`. It neither reads the file nor copies guest data. The normal
`demRead` contract is distinct: it takes a handle/count/DS:DX buffer and
performs the host read/write operation (`overlay/.../demhndl.c:93-128`).

## T149 correspondence

Immediately before the `50:36` application transfer, the ordered trace has:

`50:12 open -> 50:42 -> 50:42 -> 50:02 close -> 50:36 transfer`.

There is no intervening `50:16`. Both `50:42` observations resume with
`delta=00`, including no copied result or RAM transaction. That matches the
original CF-clear/no-I/O `demNotYetImplemented` contract exactly: the DOS
kernel does not take its CF-set slow-read fallback, and this service cannot
populate the COM load buffer. The same path then transfers with
`DS:SI=0BC1:0100`; T151 proves the later `0BC1:FFFF` location is outside the
source-built file interval.

## Result

This is not an adapter ingress classification error, a BOP `50:42` identity
mistake, a Bochs memory/device defect, or an omitted `MIPS_BIT_MASK`
initialization. It is a missing historical **x86 fast-I/O composition**:
the visible original dispatcher declares the service unavailable/no-op, while
the original x86 guest path treats CF-clear as a completed fast read.

The current adapter faithfully preserves that original-unavailable result,
which is source-correct but insufficient to make this CLI profile load the
file. Replacing it with `50:16`, setting CF to force a fallback, or changing
the fixed MIPS state would each change a different original contract and is
therefore rejected.

## Disposition

The appropriate next package is a whole provider-design audit for the
`DEMFASTREAD`/`DEMFASTWRITE` composition gap: recover every available caller,
input/output register and memory contract, select an explicit CLI capability
backend, and specify failure/short-read semantics before implementation. It
must be source-derived, plane-gated and session-owned; it may not be a
service-number special case in Bochs or a hidden alias to normal `DEMREAD`.
No source or runtime change is made by T152.
