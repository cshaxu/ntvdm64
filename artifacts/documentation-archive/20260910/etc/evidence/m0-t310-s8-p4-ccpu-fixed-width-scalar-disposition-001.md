# M0 T310 S8 P4 — CCPU fixed-width scalar disposition

The full selected `c_main.c` warning inventory contains 111 C4244 records.
Each was read as an instruction-decoder or executor expression and has only
the original CCPU fixed-width value classes: `IU8`, `IU16`, `IU32`, `ISM32`
or `IU64`. Typical sites decode an opcode, preserve an Intel register-width
result, update an instruction pointer/count, or invoke the original 8/16/32
bit operation table.

Those records do not convert a native pointer, `HANDLE`, `size_t`,
`uintptr_t`, callback, host resource or mapping-manager identity. `IU8`,
`IU16`, `IU32` and `ISM32` remain explicitly 8/16/32 bit in the selected
SoftPC type definitions on both host architectures. The `__int64` forms are
also instruction arithmetic narrowed to an original 32-bit guest result, not
a host-address conversion.

Therefore the 111 records are retained as visible original guest-width
diagnostics, but are removed from the x86/x64 repair queue as
`not-x86-x64-fixed-guest-scalar`. No casts, warning suppression, adapter,
overlay or mapping-manager route was added. Formal x86 and x64 CCPU builds
remain the verification of the selected source profile.
