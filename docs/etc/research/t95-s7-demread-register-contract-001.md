# T95 S7 DEM Read Register Contract 001

## Question

Which registers may the contained adapter write when it services the reached
OpenNT `50:16` (`SVC_DEMREAD`) and `50:42` (`SVC_DEMFASTREAD`) BOPs?

## Source evidence

`upstream/opennt/base/mvdm/dos/dem/demhndl.c:93-235` implements `demRead`.
Every success path calls `setCF(0)` and `setAX(dwBytesRead)`; every error path
uses the same error convention, CF set plus AX.  It does not assign BX, CX,
DX, BP, SI, or DI.

The NTDOS call site in
`upstream/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:430-540` saves BP and
BX, places the DEM handle in AX:BP, invokes `SVC_DEMREAD` or
`SVC_DEMFASTREAD`, branches on CF, and continues the normal read path.  The
handle's high word in BP is an input to the BOP, so clearing it is especially
not a valid synthetic result.

## Observed discrepancy and correction

The retained source-built r24 trace records its final `50:42` with
`AX=4005`, `BX=0005`, `CX=0020`, `BP=4E53` and `delta=2f`.  That delta writes
AX, CX, DX, BX and BP, including a zeroed BP.  Although the zero-byte read and
CF-clear outcome are source-valid at EOF, the broad register delta is not.

`bx_ntvdm_dem_readonly_file_v1_read_service` now uses a dedicated
`read_result`: both `50:16` and `50:42` resume at `RIP+4`, write AX only, and
set CF according to the original `demRead` result.  This is an adapter ABI
correction only: it does not add a BOP, guest memory access, Bochs behavior,
device, DOS semantic, or host capability.

## Verification

The adapter-only target
`bx-ntvdm-dem-readonly-file-service-test` was rebuilt and passed from
`artifacts/build/t95-s7-bop-catalog-c11-002`.  Its fixture keeps a valid
AX:BP handle while seeding unrelated BX/CX/DX values, and asserts that both
normal and fast reads produce `gpr16_write_mask == 1` (AX) for EOF, nonempty,
and invalid-handle outcomes.

## Consequence

This corrects a source-proven corruption visible in r24 but does not by itself
prove that the later stack underflow is resolved.  The next runtime evidence,
when separately admitted, must use the same bounded workload and determine
whether execution now proceeds past the former `50:42` point before proposing
any additional BOP, Bochs prerequisite, or CLI capability.
