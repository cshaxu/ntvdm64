# T95 S7 Stack-Pointer Diagnostic Rejection 001

## Question

Can a one-object, default-off Bochs CPU-loop diagnostic identify the ordinary
instruction that changes the post-`50:02` stack pointer from `091Eh` to the
later real-mode fault state `0003h`?

## Result

No. The r24 source-built run proves that the correct DEM close response resumes
at `61A7` with `SP=091Eh`; its later `#SS` dump has `SP=0003` and executes the
vector target at `0000:019D`. The attempted r28 logger changed only the
diagnostic compilation to use Bochs's original non-chained CPU loop, because
the accepted configuration has handler chaining enabled.

That object was not a stable replacement for the CPU archive: the original
Makefile rebuilt `cpu/libcpu.a`, and r28 stopped before the established path
(`bopObservations=0`, `commits=0`). Its log therefore cannot attribute the
stack transition to an ordinary guest instruction. r27 had already shown that
editing the copied `config.h` is worse: it invalidates broad original Makefile
dependencies and starts product-object rebuilding; it was terminated.

The logger, its focused test, and its build-root option were removed. No new
runtime behavior is retained.

## Evidence

- r24: `artifacts/analysis/t95-s7-runtime-trace-023-20260811-001/bochs.log`
- r27 retained stopped-build output:
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r27/r27-build.stdout.log`
- r28 bounded observation:
  `artifacts/analysis/t95-s7-runtime-trace-024-20260811-001/observation.json`

## Consequence

Do not add a BOP handler, a device, or an adapter workaround from this failed
trace. The remaining valid evidence still classifies the post-close stack
underflow as an ordinary-control-flow question. The next diagnostic must first
prove a same-configuration original Bochs observation facility or an explicit
CPU object/link closure; neither is supplied by a one-object CPU-loop swap.
