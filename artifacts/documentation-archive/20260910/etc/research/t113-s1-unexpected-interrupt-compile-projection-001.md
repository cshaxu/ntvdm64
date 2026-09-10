# T113 S1 unexpected-interrupt compile projection 001

## Question

Can the unmodified original `unexp_nt.c` be compiled as an x86 historical
object without adding a shim, linking any product, or invoking Bochs?

## Inputs and procedure

The source is `src/opennt/base/mvdm/softpc.new/base/system/unexp_nt.c`.
A fresh root `artifacts/build/current/t113-unexp-compile-projection-r3` used
the standalone `src/opennt` CMake project, Visual Studio x86 environment,
`clang-cl.exe -m32`, and `CMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY`.
The only built target was the existing
`ntdos64-opennt-system-provider-objects` OBJECT target. It has no link step.

Two earlier fresh roots are retained as configuration evidence: r1 revealed
the compiler probe's default x64 link mismatch; r2 used invalid `/m32` syntax
for clang-cl. Neither is source evidence. r3 corrected both conditions.

## Observation

The r3 command completed with exit code zero and reports:

```text
[  0%] Building C object .../unexp_nt.c.obj
[100%] Built target ntdos64-opennt-system-provider-objects
```

It produced no executable or library link. The compiler emitted historical
header warnings (210 across the object target), but no error for
`unexp_nt.c`. The retained log SHA-256 is
`B2741CC719AD20927460B16354617B41181D42E886B5583C4027C01570F7BFB2`.
The fresh `unexp_nt.c.obj` SHA-256 is
`270BDD2A959879B35E493739289A3B8E7DD3D30F7ADCD0BA46CF087183E534B2`.

`llvm-nm --undefined-only` reports exactly these semantic unresolved imports:

```text
_c_sas_store
_inb
_outb
```

The two security-cookie imports are compiler runtime support, not original
handler semantics. `llvm-nm --defined-only` exports only `_unexpected_int`.

## Interpretation and follow-up

The original handler has a proven compile closure in the documented x86
historical island; it is not yet linkable or runnable. T114 audits the three
original platform imports and Bochs equivalents before any compatibility shim
or Bochs exception can be considered.
