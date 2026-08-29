# M0 T310 S8 P4 CCPU fixed-width operand call disposition

## Scope

This review covers all 100 current C4057 records in
`mvdm-host/softpc.new/base/ccpu386/c_main.c` that pass an `OPERAND.sng`
address to source-era instruction function declarations expecting `ISM32 *`.

## Source reading

The original operand union declares `sng` as `IU32`; the original host type
header declares `ISM32` as signed 32-bit `long`.  The affected calls are
inside CCPU instruction decode/execute cases: the pointer denotes a 32-bit
Intel operand, not a host handle, native allocation, guest-to-host address or
function pointer.

On MSVC both x86 and x64 retain `long` and `int` as 32-bit scalar types. The
diagnostic records the historical signed/unsigned type mismatch, but no
pointer-width, representation-width, calling-convention or host-identity
transition occurs.  The original integer storage is intentionally passed to
the instruction implementation as a bit-level 32-bit operand.

## Disposition

All 100 exact current records are classified
`not-x86-x64-fixed-width-ccpu-operand-contract`. They remain in compiler
output and the complete classification ledger; they are deliberately removed
only from the x86/x64 repair worklist. No cast, warning suppression, mirror
change, overlay, mapping-manager route or adapter is introduced.
