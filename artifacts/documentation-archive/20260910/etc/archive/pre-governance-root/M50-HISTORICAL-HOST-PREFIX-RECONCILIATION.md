# M50 Historical Host Prefix Reconciliation

Status: corrected read-only reconciliation, 2026-08-08. This record makes no
runtime, shim, or `ntvdm64` change.

## Reconciled Prefix

The current executed source-backed **cross-profile CCPU fixture** prefix is:

```text
config()
  -> sas_term()
  -> sas_init(vdmMemorySize * ONEKB)
     -> ccpusas4.c
     -> host_sas_init (bounded fixture aperture)
  -> rom_init/copyROM
  -> InitNtCpuInfo()
  -> fail-closed stop
  -> sas_term() cleanup
```

The prefix was independently revalidated by the default-disabled
`ntdos64-opennt-config-sas-prefix-trace-fixture`, as recorded in
`R3-CONFIG-PREFIX-TRACE-LEDGER.md`. It executes neither guest code nor
IVT/BDA construction, BIOS selector dispatch, BOP/SVC, DEM, NTIO, or NTDOS.

## Profile Boundary

The fixture's next caller is `config.c:539`, under `CPU_40_STYLE`:

```c
sas_init(vdmMemorySize * ONEKB);
InitNtCpuInfo();
```

`host/src/nt_cpu.c:265` does not provide a machine operation. It binds a
non-x86 `CPU_40_STYLE` debugger register view to generated monitor state:
`GDP_PTR`, fixed GDP offsets, `GLOBAL_*` state, and `ConstraintR*` bits.

The original `obj.vdm/CDEFINE.INC` establishes that this is not the NT4 x86
production profile. Its `$(386)` branch selects `MONITOR`, `C_VID`, and
`X86GFX`; only the non-x86 branch selects `CPU_40_STYLE` and optionally CCPU.
The source-backed stop therefore proves a cross-profile ABI mismatch. It does
not identify a missing NT4 x86 generated header.

## Decision

`InitNtCpuInfo` is the immediate stop for this cross-profile probe, not a
recovery gate for a native x86 startup. It must remain fail-closed: no
`GDP_PTR` alias, borrowed architecture/profile offsets, no-op success, or
synthetic BIOS/BOP/DEM/DOS response is permitted.

The actual NT4 x86 historical profile requires the V86 monitor path. Because
the research runner explicitly excludes V86 and is x64 software-emulated, its
future backend must be newly governed and must use historical source as an
oracle rather than claim CCPU/`InitNtCpuInfo` restoration.

## Next Unblocked Work

The cross-profile fixture cannot become the historical x86 runtime. Productive
work is either metadata-only BYOB admission or a separately governed software
backend; neither makes a historical V86 path executable.
