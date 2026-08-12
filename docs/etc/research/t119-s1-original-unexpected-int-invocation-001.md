# T119 S1 original unexpected-int invocation 001

## Result

T119 closes source/build integration of the single reached machine BOP handler.
The frozen T119 r3 x86 derivative links the unchanged OpenNT
`unexpected_int` object, the machine-composition selector-`02h` call, and its
three mechanical compatibility wrappers. It has not been executed.

## Inputs and procedure

- OpenNT source object: `base/mvdm/softpc.new/base/system/unexp_nt.c`, built
  in the existing clang-cl/x86 projection. Its semantic undefined imports are
  only `_inb`, `_outb`, and `_c_sas_store`.
- Bochs change: default-off `BX_NTVDM_ENABLE_MACHINE_COMPOSITION` in
  `cpu/exception.cc`, which receives only a generic handled/declined result.
  It contains no selector or BOP decoding and, on handled, resumes at copied
  fault RIP plus three using the pre-existing exception-loop transfer.
- Component: `bx_ntvdm_machine_bop_v1.c` recognizes only selector `02h`,
  invokes `unexpected_int`, and supplies dynamically scoped wrappers to
  existing Bochs PIC-port and ordinary-RAM owners.
- Build command: `New-T98S1CurrentAdapterEngineDerivative.ps1` with
  `-MachineComposition`, then MSVC x86 `nmake /f
  ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe`.

## Observed evidence

`artifacts/build/current/t119-unexpected-int-link-r3/` was freshly generated.
Only Bochs `cpu/exception.o`, the machine component object, the admitted
adapter/CLI objects, and the final link were rebuilt; the retained Bochs
archives were not rebuilt. The final link succeeded.

The linker map assigns `_unexpected_int` to `unexp_nt.c.obj`, and assigns
`_inb`, `_outb`, `_c_sas_store`, and
`_bx_ntvdm_machine_bop_v1_execute_unexpected_int` to
`bx_ntvdm_machine_bop_v1.obj`. The Bochs probe is in `exception.o`.

Artifact SHA-256:

- executable: `F58EB208CADF1A9D8B2A33ECA6465A793E850EF72A8A00B60A787B0AE9250389`
- map: `B58A8C4769E4121A747754B8C512C67B7018D77CA604A6B0A8BB168CC8EEAE5B`

`Test-BochsMachineCompositionBoundary.ps1` and
`Test-BochsUdInterceptBoundary.ps1` both passed.

## Limitation and disposition

The link reports retained-image warning `LNK4098` for `msvcrt.lib`; adding the
standard `vcruntime.lib` resolves the handler's SEH support import. This is a
toolchain-island warning, not evidence of a safe runtime CRT combination.
T119 therefore proves link closure only. T120 is the sole admitted follow-up
and permits one bounded observation of this unchanged image; it may not repair
or mask any runtime outcome.
