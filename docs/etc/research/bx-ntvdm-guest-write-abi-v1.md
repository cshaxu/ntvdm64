# BX/NTVDM Guest Write ABI v1

## Purpose

This is the type-level preflight for one proposed physical guest-memory write.
It complements the CPU-state snapshot without exposing native Bochs memory or
authorizing a memory operation yet.

## Record And Validation

`bx_ntvdm_guest_write_v1` is a fixed 40-byte record containing an ABI prefix,
zero-required flags, physical guest address, byte count, and an offset into a
separate copied payload region. It contains no host pointer, guest pointer,
`BX_MEM_C`, CPU object, mapping, page-table information, or payload bytes.

Preflight accepts only a correct ABI record whose complete guest range lies in
the declared physical aperture and whose complete payload interval lies in the
declared payload extent. Its subtraction-based checks reject overflow without
performing a write. The descriptor permits a zero-length range but grants no
special mutation behavior.

## Atomicity Boundary

The required future sequence is:

1. validate all proposed state and memory records before mutation;
2. have the native Bochs owner validate its own memory applicability;
3. perform the selected writes; and
4. commit the explicitly permitted CPU result only after successful writes.

This record implements only step 1 for a single write. It has no batch/order
semantics, no rollback claim, no native Bochs read/write seam, and no BOP,
DEM, DOS, or OpenNT dispatch behavior. Separate read and write admissions are
still required by the guest-memory seam research.

## Validation

`tests/bx-ntvdm-adapter/bx_ntvdm_guest_write_abi_test.c` fixes record size and
tests valid full-range input plus payload-underflow, aperture-overrun, flags,
and null rejection. On 2026-08-10 it compiled and ran with MSYS2 UCRT64 GCC as
`artifacts/build/adapter-tests/bx-ntvdm-guest-write-abi-test.exe` (exit 0).
