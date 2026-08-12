# T95 S7 r36 IRET/TF Runtime Result

## Question

Does the first post-close `INT 1` loop arise because the adopted Bochs 2.6
`IRET16` implementation changes the return frame, or because the original
OpenNT NTIO `FIRET` path receives a frame whose flags already contain TF?

## Inputs

- Fresh root:
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r36`.
- Immutable observation:
  `artifacts/analysis/t95-s7-runtime-trace-029-20260811-001`.
- Binary SHA-256:
  `A5D8EC3252011EADC4A46B09C5FA820F0F27FEF70E479380EA73E766BF44CF05`.
- Registered default-off exception `BX-TRACE-050`, limited to the original
  `src/bochs/cpu/ctrl_xfer16.cc` real-mode IRET16 path after its three
  original `pop_16` operations.
- Source-built NTIO map:
  `artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/ntio.map`.
- Original OpenNT sources:
  `src/opennt/base/mvdm/inc/vint.h` and
  `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm`.

## Procedure

1. Ran the focused IRET-TF boundary check, the existing real-mode-vector and
   unique-object closure checks, and documentation governance verification.
2. Generated the absent r36 root once, then built once with MSVC/x86 using
   `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`.
3. Ran one 15-second observation.  The observer performed no guest inspection
   beyond the original IRET frame pops and recorded only frames whose popped
   FLAGS already had bit `0x0100`.
4. Correlated the resulting vector target and the generated NTIO map/source.

## Observations

- The build linked `cpu/ctrl_xfer16_trace.obj` before the intact CPU archive.
  The only compiler warning was the inherited narrowing warning in
  `cpu/exception.cc`; `ctrl_xfer16.cc` compiled without a new warning.
- The observation was watchdog-terminated after 15 seconds (`exitCode: 1`),
  preserving 39 BOP observations and 10 committed adapter transactions—the
  same reached capability count as r30/r34/r35.
- The first TF-bearing return is:

  ```text
  old-cs=0070 old-ip=01ec old-ss=00a7 old-sp=08f5
  target-cs=0000 target-ip=0043 flags=0346
  ```

  There are 163 such logs.  The later repeated rows return from the same
  `0070:01ec` site to successive `0000:` addresses with TF still present.
- r35's `INT 1` vector target remains `0070:018b`.  The NTIO map names
  `0000:018b` `intret`; its next symbol, `strategy`, begins at `0000:01ec`.
- `msbio1.asm` defines `intret: FIRET`.  `vint.h` defines `FIRET` to inspect
  the saved FLAGS word; when bit `0x0100` (TF) is set it takes its `b` branch
  and executes an ordinary `iret`.  It deliberately does not virtualize that
  return path.

## Interpretation and Confidence

High confidence: the observed trap loop is not evidence that Bochs created
TF, altered the IRET target, or requires an INT1/IRET/CPU repair.  The first
observed source-owned NTIO `FIRET` already receives FLAGS `0346`, including
TF, and its documented implementation therefore chooses native `iret`.
Bochs then correctly delivers the next single-step `INT 1` through the
existing IVT entry.

The frame origin before NTIO receives it remains unproven.  The first target
is `0000:0043`, so the next investigation must source-map that low-memory
control path and its frame construction before admitting any repair.  It must
not add a device, synthesize IVT/ROM state, clear TF, intercept INT1, or add
a BOP/adapter endpoint.

## Follow-up

Retain `BX-TRACE-050` only until the producer of the first `00a7:08f5` frame
is source-correlated.  A later observation, if admitted, must be a
source-backed, default-off Bochs-mechanical diagnostic of that producer or a
read-only placement/map correlation; it cannot change execution semantics.
