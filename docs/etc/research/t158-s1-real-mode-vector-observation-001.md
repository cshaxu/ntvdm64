# T158 S1 real-mode vector observation 001

## Derivative and run

T158 generated a fresh x86 derivative from the T155/r5 inputs using
`-RealModeVectorDiagnostic`.  Its manifest lists exactly one Bochs replacement:
`cpu/exception.o`; `main.o`, `cpu/libcpu.a`, I/O, memory and GUI archives are
retained.  The rebuilt object uses the existing r5 exception macros plus only
`BX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC=1`.

One MSVC x86 link and one one-million-tick/15-second watchdog observation were
performed.  The watchdog child cleanup again reported local `Access denied`,
so no `observation.json` exists; the 65,297-byte `bochs.log` is retained and
no emulator process remained.  The log contains four FASTREAD calls and 271
existing generic real-mode vector records.

## Reached predecessor

The final EOF `50:42` transaction occurs at tick 320,848 and resumes
normally.  Its later causal chain is now observed:

```text
323,708: vector=29 old=0032:379b ss:sp=00a7:0906 -> 0070:0254
323,719: vector=10 old=0070:0260 ss:sp=00a7:08f6 -> 073b:089d
323,773: vector=01 old=0000:0043 ss:sp=00a7:08fb -> 0070:018b
... repeated vector 01 records while CS=0000 advances toward 019c
326,544: stackPrefetch ffff / third #SS
```

`INT29` followed by `INT10` is the exact historical sequence recorded by
the earlier r36/r37 audits.  `spckbd.asm:1277-1364` routes fullscreen INT10
through an original far jump to `host_int10`; `InstSpcKbd` at lines 2132-2139
copies the pre-existing IVT INT10 pointer into that field.  The r37 direct
entry record already proves that far target is `0000:0000`.

T158 independently reproduces its downstream consequence: execution begins
at low memory, repeatedly raises INT1 into original `0070:018b`, and reaches
the T156 stack failure.  The vector log cannot itself print the far-jump
operand, but the source and same-family r37 record make the causal
classification high confidence.

## Classification

This is the known **direct pre-POST execution-plan composition**: it enters
NTIO after hardware setup but before ROM BIOS POST, IVT initialization and
VGA ROM initialization.  It leaves the machine INT10 vector zero when OpenNT
saves it.  It is not a BOP-provider, FASTREAD, DEM, adapter, stack, CPU-limit
or device-model defect.

`BX-TRACE-049` made no state change and contains no BOP/service filtering.
No repair is made here.  The only aligned successor is the already designed
native POST plus deferred opaque handoff composition; it must be statically
closed against the current T154 adapter before any later run.
