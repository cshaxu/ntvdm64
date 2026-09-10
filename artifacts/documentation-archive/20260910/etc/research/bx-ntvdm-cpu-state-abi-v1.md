# BX/NTVDM CPU State ABI v1

## Purpose

`bx_ntvdm_cpu_state_abi.h` defines a copied x86 architectural snapshot for a
future, separately admitted bridge transaction. It exists because the original
`MS_bop_0 -> DemDispatch` path observes general registers, IP, flags, and
segment selectors while the historical CPU context is live. The earlier
exception event intentionally contains none of those values.

## Record

The 72-byte C11 record contains a magic/version/size prefix, a declared
execution-mode tag, `EAX..EIP`, `EFLAGS`, and the six x86 selector values.
It has no pointer, C++ object, descriptor cache, page table, memory mapping,
allocator ownership, BOP selector, service byte, OpenNT function address, or
writeback operation. A selector is a copied identifier, not permission to
resolve or alter a descriptor.

Only `REAL`, `PROTECTED`, and `V8086` tags are valid. The initializer zeroes
all fields; the validator rejects null, ABI mismatch, unsupported mode, size
mismatch, and nonzero reserved data.

## Boundary

This record is deliberately not embedded into exception ABI v1 and is not
copied into exception ABI v1. `BX-ABI-005` now copies it beside that event in
the same-island generic `#UD` bridge, where the adapter validates it and
returns only pass-through. It does not recognize an instruction, read guest
memory, invoke an OpenNT owner, or authorize a CPU-state commit. Those
operations require a later transaction contract with explicit instruction
phase, memory effects, reentrancy, result ownership, and rollback rules.

Thus this is a type-level prerequisite, not evidence that BOP, DEM, NTIO, or
NTDOS runs. It introduces no Bochs or OpenNT source intrusion and needs no
exception-register entry.

## Validation

`tests/bx-ntvdm-adapter/bx_ntvdm_cpu_state_abi_layout_test.c` fixes the
72-byte layout using a real-mode-shaped `CS:EIP` and `DI` sample, and rejects
invalid mode/reserved cases. On 2026-08-10 it compiled and ran with MSYS2
UCRT64 GCC under
`artifacts/build/adapter-tests/bx-ntvdm-cpu-state-abi-layout-test.exe` with
exit code 0.

The r9 native Bochs adapter fixture also validated a real runtime copy at a
generic `UD2` boundary and observed the state-snapshot pass-through log. It is
not BOP or OpenNT runtime evidence.
