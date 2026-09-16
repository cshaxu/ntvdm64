# M0 T419 S1--S3 x87 recovery evidence

## Inputs

- Current baseline: `6e64db15f` before this packet.
- Comparison baseline: SoftPC `ea7e982`, recovery source commit
  `2c05238335a22d125e0e35f71692c3b2a1b0b15d`.
- Affected original mirror owners: `host/inc/cfpu_def.h` and
  `base/ccpu386/fpu.c`.

## S1 audit

The pre-correction MSVC/x86 layout probe reported `FP64.hiword` at offset zero
and `mant_lo` at offset four. Its byte-representation checks read wrong
FP32/FP64 fields and read exponent 24575 from the extended `-1.5` carrier.
That is a host-C bitfield/member-order defect: x86 guest byte order itself was
not changed.

The source comparison found two functional `fpu.c` hunks after layout repair:

1. `CVTFPHI64` aliases an `FPH`/`double` through `FPHOST *`; SoftPC copies the
   representation into `FPHOST` first.
2. `FIST m64int` converts the unrounded stack value while existing M16I/M32I
   paths first call `npx_rint`.

The remaining current-only `fpu.c` text was two x64-era explanatory comments;
the executable statements already matched SoftPC.

## S2/S3 recovery and checks

`cfpu_def.h` now uses SoftPC's exact little-endian declarations for `FP32`,
`FP64HI`, `FP80SE`, `FP64`, `FP80` and `FPU_I64`. The structural fixture uses
the production `insignia.h`, `host_def.h`, `cfpu_def.h` inclusion order and
passes on MSVC x86. It checks known FP32/FP64 values, the FP80 `-1.5` byte
carrier, and signed FPU_I64 word order.

`fpu.c` now has the two exact SoftPC T60 conversion hunks. It compiles with
the formal x86 define/include profile; pre-existing warnings remain warnings.
The post-change, whitespace-normalized source comparison reports no `fpu.c`
text difference. `cfpu_def.h` differs only by SoftPC's standalone `fenv`
branch, deliberately excluded because this product uses the NTVDM `_controlfp`
thread/process control contract.

## Formal x86 build and closure

The default PATH selected a MinGW Ninja after VS initialization and stalled on
the build lock. The generator now resolves the native Ninja executable before
writing its runner. This host still cannot dispatch native Ninja build jobs
reliably, so the exact 507 commands emitted by `ninja -t commands` were run
serially in the generated VS x86 environment. The serial fallback needed all
Ninja output directories pre-created; after that, it produced `run16.exe`,
`basesrv.exe`, `ntvdm.exe`, `VDMREDIR.dll`, `x87-layout-fixture.exe`,
`ccpu-halt-reset-test.exe`, `original-external-memory-test.exe` and
`cvidc-vector-binding-fixture.exe`.

All four fixtures passed. The three original CCPU/SAS/C-VID fixtures reported
their established PASS markers, including `AX=beef` for HALT/reset. The four
product files were hash-verified after deployment to `O:\winnt`.
