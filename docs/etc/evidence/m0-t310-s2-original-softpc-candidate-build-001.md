# M0 T310 S2 original SoftPC candidate build — 001

## Question

Can the exact selected original `ccpu386` source membership and the first
whole SoftPC host-control roots compile as isolated MSVC `/MT` candidates on
both supported host architectures, without mixing any Bochs execution object
into the target?  Does the owner-directed NTVDMx64 `fmstubs.c` patch body
remain present and build-visible rather than being silently omitted?

## Inputs

- `src/mvdm-host/softpc.new/base/ccpu386/sources`: 130 selected C sources.
- Selected original host roots: `nt_cprgs.c`, `nt_cpu.c`, `nt_aorc.c`,
  `nt_reset.c`, `nt_error.c`, `nt_msscs.c`, `sim32.c`, `nt_sas.c`, `nt_mem.c`,
  and `nt_umb.c`.
- `src/mvdm-softpc-patch/x86/prod/` selected generated carriers.
- Byte-identical `patches/common/fmstubs.c` and the selected
  `patches/minnt/callconv.patch` evidence.
- `tools/build/New-T310OriginalSoftpcNinja.ps1`.

## Procedure

Outside the repository sandbox, generate the architecture-specific graph and
run the candidate target with MSVC `/MT`:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x64
ninja -C build/M0-T310/S2/softpc/x64 -j1 original-softpc-candidate

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86
ninja -C build/M0-T310/S2/softpc/x86 -j8 original-softpc-candidate
ninja -C build/M0-T310/S2/softpc/x86 -j1 ntvdmx64-softpc-patch-evidence.lib
```

The graph forbids `src.old`, `bochs-core`, and `adapter-bochs` inputs.

## Observations

- x64 compiled and archived all 130 CCPU translation units, the ten selected
  original host roots, and the small modern Win32 binding library.
- x86 compiled and archived the same 130 CCPU translation units and ten
  selected original host roots after the registered two-site `WINAPI`
  correction for `ErrorDialogBoxThread`.
- x86 compiled and archived the byte-identical `fmstubs.c` in the separate
  `ntvdmx64-softpc-patch-evidence.lib`. Its source still contains the original
  `int 3` breakpoint forms; no hook was reimplemented or enabled.
- The original `nt_mem.c` compiles, but its historical host-pointer/VDM-address
  casts remain warnings. They are a later mapping-manager binding issue, not
  evidence that raw addresses are safe.
- `fmstubs.c` is intentionally not an x64 runtime object: its historical MSVC
  inline-assembly breakpoint syntax is x86-only. The body remains imported
  evidence and its individual hook dispositions are tracked in `SPC-IF-017`.

## Interpretation

The selected original CPU and host-control source bodies now have an isolated
compile/archive closure on both x86 and x64. This is not a runnable original
SoftPC backend: it does not link a full executable, resolve every historical
device/monitor/Win32 form, bind the mapping-manager memory seams, or select a
session backend. The next S must use the recorded physical interfaces rather
than replacing CCPU or hiding raw-address issues.

## Follow-up

- Classify and bind the unresolved original-link forms by physical owner.
- Recover the `nt_mem`/`sim32` mapping-manager boundary without retaining a
  host pointer in MVDM-visible state.
- Decide each `fmstubs.c` hook from its original caller; retain breakpoint
  failure until a source-shaped implementation is proven.
