# T146 S1 single-diagnostic CPU archive closure

## Question

Can the registered BX-TRACE-056 prefetch-predecessor observation be linked
without the non-comparable direct `cpu_trace.obj` override used by T136/T137,
while retaining the T130 machine-composition and all non-CPU Bochs inputs?

## Inputs

- T130 build root: `artifacts/build/current/t130-machine-outcome-r1`.
- r5 CPU source/archive root:
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r5`.
- Registered source delta: `src/bochs/cpu/cpu.cc`, BX-TRACE-056 only.
- Generator: `tools/New-T146S1CpuArchiveDerivative.ps1`.
- Static verifier:
  `tests/bx-ntvdm-adapter/Test-T146S1CpuArchiveDerivative.ps1`.

## Procedure

1. Copy the complete T130 root to a fresh `t146-cpu-archive-r1` root.
2. Save T130's separately linked machine-composition `cpu/exception.o`.
3. Replace every CPU `.cc` source with r5, then replace only `cpu.cc` with
   the registered BX-TRACE-056 version.
4. Rebuild only `cpu/libcpu.a` under MSVC x86.  The compiler receives
   `PREFETCH_PREDECESSOR=1`; IRQ13, IRET-TF, real-mode far-jump, and stack
   transfer diagnostics each receive explicit `=0` values.
5. Restore the saved T130 `cpu/exception.o`, then link once with the retained
   non-CPU inputs.  No root/full Bochs target, device target, adapter target,
   or CLI target is invoked.

## Observations

- The rebuilt archive has 93 members and SHA-256
  `DD32A7636C946B6FB22B8BD5A21F9CD7365BB1F3F6E9F3EB27B74734E0DD3D0A`.
- The retained T130/r5 CPU archive hash is
  `E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB`.
- The final executable hash is
  `FCF8A5B7D67932B75B1D2730B15C4270971CF912B1411BF3B38247C1C519F1A8`;
  its map hash is
  `697EA40CF900CEC59FDA00FB191A777ABC89F9228251587864F3506EAD12A3C2`.
- T130's direct machine-composition exception object and T146's restored
  final object are identical:
  `DD37654286130B20E6226458D36EADD77112F8E77650B048960A422CC5473D5A`.
- The final map resolves `BX_CPU_C::prefetch` from `libcpu:cpu.o` and contains
  no `cpu_trace.obj` reference.  The static verifier also proves non-`cpu.cc`
  CPU source identity with r5 and retained T130 non-CPU input identity.
- The original `unexp_nt` CRT-island `LNK4098` warning remains.  It is the
  already-recorded T132 limitation, not a new T146 link dependency.

## Interpretation and confidence

T146 S1 closes the direct-object comparability defect: the diagnostic now
resides in a complete, source-pinned CPU archive rather than preceding a
different archive as a one-object override.  The composition's separately
approved `exception.o` remains byte-identical to T130.  Confidence is high
for static source/build/link closure.

This is not a runtime result.  It neither proves that the late fault will be
reached nor attributes its predecessor.  Any run requires a separately
admitted bounded observation with the T130 fixed profile and a watchdog.

## Follow-up

The only justified next question is whether one bounded T146 derivative run
reaches the previously observed late prefetch fault and emits the new
fault-only record.  It must not change a provider, BOP result, Bochs device,
firmware, guest image, or adapter input.
