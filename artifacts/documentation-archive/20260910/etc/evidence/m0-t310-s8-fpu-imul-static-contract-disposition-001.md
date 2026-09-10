# M0 T310 S8 — FPU and IMUL static-contract disposition

## Reviewed source forms

The static ABI inventory selected five apparent pointer-to-fixed-width forms
in the original CCPU executor:

- `softpc.new/base/ccpu386/fpu.c`: `CVTR80FPH` at lines 980/982/986 and
  `CVTFPHR80` at line 1145/1147/1151;
- `softpc.new/base/ccpu386/imul.c`: `IMUL32` at line 179.

## Source result

`fpu_c.h` defines `FPSTACKENTRY` as a host `double` plus the original
pointer-sized tag/padding fields. The four `fpu.c` forms cast a pointer only
to select an in-place `FP80` or `FPHOST` layout view and then read or write a
fixed-width exponent or mantissa field into `IU32 munger`. No pointer value
is converted to `IU32`, stored as a guest identity, returned across an ABI or
sent to a device.

`IMUL32` passes `&top` and `&result`, two local `IS32` result slots, to the
original `mul64(IS32 *, IS32 *, IS32, IS32)` declaration. The definition in
`c_mul64.c` has the same four-parameter contract. This is a same-process
native pointer use; it is not a 16/32/64-bit guest or host identity boundary.

## Disposition

All five records are source-shaped fixed-width arithmetic/layout operations.
They remain byte-identical mirror code. No cast, warning suppression,
overlay, adapter or mapping-manager route is authorized.

The bounded CCPU fixture builds `fpu.c`, `imul.c` and `c_mul64.c` on both x86
and x64, proving the reviewed ABI contracts compile and link. It does not
exercise FPU instruction semantics; such instruction behavior remains part of
the S8 executor behavior matrix and cannot be inferred from this disposition.
