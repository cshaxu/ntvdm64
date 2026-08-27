# M0 T291 S3 P4 — WOW callback guest-frame composition

## Question

Can the staged WOW callback preserve the original `CBVDMFRAME` layout and
lease ordering without retaining an NT4 process-wide VDM pointer alias?

## Inputs

- Canonical `mvdm-host/wow32/wcall16.c`, specifically its non-fast
  `CallBack16` frame construction and cleanup sequence.
- Canonical `mvdm-support/inc/wow.h` `CBVDMFRAME` / `PARM16` declarations.
- The S3 bounded WOW pointer scope and numeric task/frame projection.

## Implementation

`mvdm_wow_callback_guest_frame` is the exact numeric 38-byte packed portion
of the original `CBVDMFRAME`: task, return ID, local BP, 16-byte `PARM16`,
procedure, original stack, AX/DX and the two general-use words retain their
source order.  It is not a native pointer-bearing host structure.

The transaction now preserves this order:

1. copy the current numeric task frame and acquire its six-byte source prefix;
2. validate/copy `wTDB` and `wLocalBP`, then release that source lease;
3. select the aligned callback stack using the existing source-shaped task
   projection;
4. acquire, populate, flush and release exactly one callback-frame lease;
5. dispatch the existing copied session control record;
6. write and reread AX/DX through a fresh bounded callback-frame lease; and
7. restore the numeric task stack and end the pointer scope.

The temporary route remains a copied result handoff.  It does not claim that
a Win16 callback instruction stream has already run.  P5 owns the separate
`SETVDMSTACK` / `host_simulate` / `VDMSTACK` machine interval.

## Verification

The formal `New-T291S3WowCallbackTransactionNinja.ps1` graph rebuilt and ran
on both architectures:

- MSVC x86 `/MT`: passed;
- MSVC x64 `/MT`: passed.

The fixture uses a bounded in-memory guest backing to prove original task and
local-BP copying, computed callback-frame address, the packed task/return-ID
and AX/DX writes, returned DX:AX value, and oversized-parameter rejection.
Its protected-span stub is unreachable: the dedicated S3 P1 fixture remains
the evidence for protected pointer resolution.

## Exclusions

No original WOW body, selector, guest load, fast monitor, raw pointer/HANDLE,
CCPU executor or new mapping manager enters this part.  No CPU stack or
instruction pointer is changed by P4.
