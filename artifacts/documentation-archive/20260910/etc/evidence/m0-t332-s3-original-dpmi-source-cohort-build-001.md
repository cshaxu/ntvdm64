# M0 T332 S3 — Original DPMI source-cohort build

## Selected cohort

The final product graph selects the original DPMI32 common manifest bodies:
`debug`, `dpmi32`, `dpmiint`, `dpmiselr`, `buffer`, `data`, `int21map`,
`modesw`, `register`, `savestat`, `stack`, `vxd` and `xmem`; it also selects
the original CPU40-compatible `dpmimemr` and `dpmimscr` bodies.  They are
archived in `original-mvdm-dpmi32.lib` and linked by
`original-softpc-process.exe`.

The complete original table retains its twenty-five entries.  The old kernel
VDM/V86 `i386_SOURCES` trio remains a mirror/evidence cohort rather than a
silently substituted implementation.  Its `53:03` fast BOP and `53:0E`
debug-register entries have the explicit S4 monitor/debugger disposition in
the S1 ledger.

## Formal result

With the MSVC environment explicitly loaded, Ninja rebuilt and linked the
same final product target on both independent graphs:

- `build/M0-T319/S3/termination-x86/original-softpc-process.exe`
- `build/M0-T319/S3/termination-x64/original-softpc-process.exe`

Both commands completed their `original-mvdm-dpmi32.lib` inclusion and final
`link.exe` step without `/FORCE`.  The first attempt made without the MSVC
environment only produced uniform `cl.exe` process-not-found failures; it was
a tool-environment failure, not source evidence, and is not retained as a
DPMI defect.

## Scope of proof

This is a source-selection and final-link proof.  It does **not** claim that
DOSX entered protected mode, that a DPMI stack/IRET/fault route executed, or
that `VdmSetInt21Handler` is available.  Those are S4's source-defined
runtime boundaries.
