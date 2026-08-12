# T95 S6 Narrow Startup-Transaction Build Attempt 001

## Question

Does the frozen narrow startup-transaction object list link once in a fresh
MSVC/x86 copied root, without selecting the broad adapter runtime or product
build target?

## Inputs

- Fresh root:
  `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r1`.
- Its generator manifest SHA-256:
  `EA6D9B6AB57AB8ACD2499BE5568FA66C783CD26B3DEBB44369FA548DA9E882B2`.
- Its generated shim SHA-256:
  `D60ED187AE2EF23616A949233DC17729566A5B4D0A55CE8A4B99C282BE6EE8B8`.
- BuildTools 2022 x86 environment and the sole admitted target:
  `nmake.exe /f ntdos64-native-startup-transaction.mak
  ntdos64-native-startup-transaction.exe`.

## Procedure

The generator manifest and shim hash were verified before one invocation.
The target output was captured as `narrow-startup-transaction-build.log`; its
exit code was recorded as `narrow-startup-transaction-build.exitcode`. No
second invocation or change in that root was made.

## Observations

The one invocation reached the final link and exited `2`. It compiled the
declared bridge-side sources, rebuilt the copied `cpu/exception.o`, and
produced the copied-root `cpu/libcpu.a`. The output executable was not
produced; the map exists only as a zero-byte failed-link byproduct.

The linker reported precisely two unresolved symbols:

```text
bx_ntvdm_exception_abi.obj:
  bx_ntvdm_cpu_delta_v1_initialize
cpu/libcpu.a(exception.o):
  bx_ntvdm_cpu_result_v2_resume
```

Source ownership is direct and narrow:

| Missing symbol | Provider source/object | Direct required consumer |
| --- | --- | --- |
| `bx_ntvdm_cpu_delta_v1_initialize` | `bx_ntvdm_cpu_delta_abi.c` / `adapter/bx_ntvdm_cpu_delta_abi.obj` | `bx_ntvdm_exception_abi.c` pass-through result initialization |
| `bx_ntvdm_cpu_result_v2_resume` | `bx_ntvdm_cpu_result_v2.c` / `adapter/bx_ntvdm_cpu_result_v2.obj` | narrow `cpu/exception.cc` result application |

The retained log SHA-256 is
`F3D2D5092C3E86B49945915B8BDCA9ED56BE23B87C0E32F4F038E94A0B91BA4B`; the
exit-record SHA-256 is
`B194C1A2E67AE3FE8D895048914D2BB306A353BC09CC65142E27DA505182B8F7`.

## Interpretation

This is a rejected **r1 link closure**, not an external compiler failure and
not evidence for enabling another Bochs device, configuration feature, OpenNT
object, SoftPC object, service, or broad adapter operation. The only observed
defect is that the frozen list omitted two generic fixed-width result helper
objects that the selected narrow ABI already uses. The rejected r1 root is
preserved unchanged and is never retried.

## Follow-up

A separate admission must first correct and statically verify the closure
ledger and generator to include exactly those two providers. Only then may a
new fresh root receive one new build attempt. The correction must retain all
existing rejected-object scans and must not widen the host-service or Bochs
feature boundary.
