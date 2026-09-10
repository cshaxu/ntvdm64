# M0 T333 S3 P2 — Complete original Redirector package build and lifecycle correction

## Question

Does the original `VDMREDIR` package compile as one source-selected unit on
both required host-width rows, and what is the actual predecessor for its
original lifecycle?

## Inputs

- The complete original `src/mvdm-host/vdmredir/sources` manifest: 16 C
  translation units plus the original `vdmredir.def` and `vrdll.c` entry.
- Original `softpc.new/host/src/nt_bop.c` (`LoadVdmRedir`) and
  `softpc.new/host/src/nt_msscs.c` (`VDDInstallUserHook`).
- Existing source-shaped thread, guest-span and opaque-identity adapters.

## Procedure

1. Correct the manifest reader so a `SOURCES` block followed by `C_DEFINES`
   is read as the original list rather than rejected or hand-subsetted.
2. Add only retained original ABI declaration roots required by the complete
   package (`ds`, `net`, private DLC and RPC-translation headers).
3. Reuse the existing cdecl-to-WINAPI thread facade for the second original
   cdecl worker (`VrDlcEventHandlerThread`), preserving the call expression,
   body and error branch.
4. Generate isolated x86 and x64 Ninja graphs and build
   `original-mvdm-redir.lib` without `/FORCE`.

## Observations

- Both `build/M0-T333/S3/redir-full-x86/original-mvdm-redir.lib` and
  `build/M0-T333/S3/redir-full-x64/original-mvdm-redir.lib` built successfully
  from all 16 original C units.
- Existing compiler warnings remain visible. They are not suppressed and this
  result does not claim width-safe runtime execution of the DLC/RAP branches.
- The initial P1 five-unit library was insufficient as a lifecycle model:
  original `nt_bop.c` uses `SafeLoadLibrary("VDMREDIR")` and `GetProcAddress`.
  The original contract is therefore a separately packaged DLL, not a static
  library linked into the process.
- The prior S2 VDD row was too broad. The reached
  `VDDInstallUserHook` function is already original `nt_msscs.c`; its immediate
  precondition is the VDMREDIR DLL module handle. It must not be replaced by a
  Redirector-local VDD shim. `VrInitialize` still reaches original NetBIOS and
  DLC initialization, whose runtime capability remains unenabled.

## Interpretation

The full original provider source is now selected as one package. S3 remains
open for the distinct original DLL entry/link/load composition decision. A
static library gives compilation evidence only; it neither invokes
`VrDllInitialize` nor satisfies the original deferred loader.

## Follow-up

Audit and construct the original `VDMREDIR.DLL` build/entry/load contract
without creating a replacement lifecycle provider. Any unresolved NetBIOS or
DLC driver dependency retains its original failure boundary and is handled by
its owner cohort.
