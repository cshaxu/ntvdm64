# Adapter Instruction-Window ABI v1

## Record

`bx_ntvdm_instruction_window_v1` is a 32-byte pointer-free observation:
versioned header, `valid_bytes`, and a maximum 15-byte copied buffer. It has
no guest address, host pointer, physical mapping, decoded instruction, or
execution disposition.

The capture API initializes an otherwise-valid zero-length record when the
source pointer is unavailable or the requested count is too large. This is
deliberately indistinguishable from an unavailable observation to downstream
policy: it must cause pass-through, never a retry or a memory read.

## Bochs capture rule

The registered `BX-ABI-006` seam captures only from the current
`eipFetchPtr` page when `prev_rip + eipPageBias` is within
`eipPageWindowSize`. The count is capped at 15 bytes and at the bytes
remaining in that same page. A page split produces a shorter record; it does
not fetch the second page.

This keeps fetch, page translation, and any resulting faults wholly within
native Bochs. The adapter receives a copy only after Bochs has already
obtained the current page.

## Boundary

This ABI does not recognize a marker, selector, service, or OpenNT function.
It is suitable only as an observation prerequisite for a later adapter-owned,
fail-closed comparison. The default dispatch validates it and returns
pass-through.

## Verification

The direct UCRT C11 layout/negative test confirms the fixed 32-byte form,
normal capture, overlength rejection to a valid empty record, null-source
rejection to a valid empty record, and reserved-byte rejection. The exception
layout test confirms a valid window preserves pass-through and an invalid
window prevents dispatch.

The separate r10 native `make bochs.exe` attempt stopped before adapter code,
in original `iodev/devices.cc`, because the pinned old configure output's
`#define inline inline` is rejected by the currently installed GCC 16 C++
headers. The r9 and r10 configure outputs have the same macro. This is
retained as toolchain evidence only; no unrelated Bochs compatibility patch is
admitted for it.
