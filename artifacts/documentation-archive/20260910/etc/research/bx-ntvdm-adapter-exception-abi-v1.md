# BX/NTVDM Adapter Exception ABI v1

## Question

What is the smallest cross-island representation for the verified generic
Bochs exception seam, before any OpenNT service owner is admitted?

## Contract

`src/bx-ntvdm-adapter/bx_ntvdm_exception_abi.h` defines two C11 records:

- `bx_ntvdm_exception_event_v1` (40 bytes): source identity, ABI version and
  length, event kind, CPU id, exception vector, error code, and fault RIP.
- `bx_ntvdm_exception_result_v1` (32 bytes): source identity, ABI version and
  length, explicit disposition, and resume RIP.

The records contain only fixed-width copied values. They contain no host or
guest pointer, C++ object, allocator ownership, function pointer, register
class, memory mapping, service selector, guest marker byte, or OpenNT name.
`PASS_THROUGH` means original Bochs delivery; `RESUME` authorizes only the
supplied resume RIP; `STOP` reserves a future typed termination record.

`bx_ntvdm_exception_abi.c` is the first mechanical consumer. It rejects null,
wrong-magic, wrong-version, wrong-length and wrong-kind events; it can form a
zeroed pass-through result or an explicit resume result. It does not inspect
instruction bytes or guest memory.

## Validation

`tests/bx-ntvdm-adapter/bx_ntvdm_exception_abi_layout_test.c` fixes the record
sizes and a generic vector-6/resume example. It is a default-disabled modern
test target and does not link Bochs or OpenNT. On 2026-08-10 it compiled and
ran successfully with MSYS2 UCRT64 GCC under
`artifacts/build/abi-v1/bx-ntvdm-exception-abi-layout-test.exe` (exit code 0).
No CMake executable is installed in the current environment, so this is a
direct C11 compiler result rather than a claimed CMake result.

The same test was rerun after adding the consumer and checks valid/invalid
event handling plus both result dispositions; it again exited with code 0.

## Limits And Follow-up

This is not yet a callback installation ABI. The Bochs-side registration
function remains an internal build seam because exporting a raw function
pointer across compiler/CRT islands would violate the architecture. The next
step is a Bochs-island wrapper that converts its internal request/response to
these records, followed by an adapter-side mechanical consumer. Only then may
an original OpenNT owner be evaluated.
