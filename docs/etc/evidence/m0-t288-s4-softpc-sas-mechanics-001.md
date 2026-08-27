# M0 T288 S4 SoftPC/SAS mechanical recovery evidence

## Scope and source decision

The exact original `v86/monitor/i386/sas.c` is retained as the source
contract. It treats `sys_addr` numeric values as low process virtual
addresses, creates and maps NT sections, and dereferences those numeric values
as native pointers. That product-shell assumption is neither pointer-width
safe on x64 nor a legal route around Bochs RAM/ROM/MMIO ownership.

S4 therefore recovers only the reached source-shaped SAS scalar/span contract
in `adapter-mvdm-host-out/softpc/mvdm_sas.c`. The facade retains the original
operation names, numeric widths, little-endian scalar results and no-op failure
direction. It uses only `adapter-bochs` typed checked-memory requests. It does
not create a SAS allocator, a memory-type table, a mapping manager, raw guest
pointer, BOP route or CPU executor.

The complete selected source disposition is recorded in
`m0-t288-s4-softpc-sas-disposition-ledger.tsv`. In particular, A20 was already
provided by the same-shaped `mvdm_a20.c`; raw pointers, memory handler tables
and CCPU descriptor stubs are explicitly not disguised as S4 success.

## Registered divergence

The historical `*_no_check` forms dereference the numeric address directly.
The new same-shaped spelling deliberately continues to apply the sole legal
Bochs range/ROM guard. This is registered as `ADAPTER-SOFTPC-011` in the
component README. It is a safety boundary, not a new SAS policy.

## Reproducible verification

The dedicated formal Ninja generator was run outside the sandbox for both
architectures:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\build\New-T288S4SasNinja.ps1 -Architecture x86 -RepositoryRoot (Get-Location).Path
ninja -C build\M0-T288\S4\x86
build\M0-T288\S4\x86\bin\sas-memory-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File .\tools\build\New-T288S4SasNinja.ps1 -Architecture x64 -RepositoryRoot (Get-Location).Path
ninja -C build\M0-T288\S4\x64
build\M0-T288\S4\x64\bin\sas-memory-fixture.exe
```

Both executables returned zero. The fixture covers scalar little-endian access,
rejected boundary reads, host-buffer string transfer, overlap-safe
forward/backward moves, byte/word fills, and the deliberate checked behavior
of an `*_no_check` call at the final byte.

This is local mechanical evidence only. It does not claim that monitor
execution, interrupt dispatch, keyboard policy or any BOP selector is enabled.
