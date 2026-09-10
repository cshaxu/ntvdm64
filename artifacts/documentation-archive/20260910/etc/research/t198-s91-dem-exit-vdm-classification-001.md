# T198 S91 DEM ExitVDM classification 001

## Question

What does the newly reached `C4 C4 50 3D` boundary mean in the source-built
NTIO startup trace, and may the CLI profile attach a provider or ordinary
resume to it?

## Inputs and procedure

This classification reads the bounded S90 native observation, the common BOP
catalogue and DEM plane, and the pinned OpenNT definitions and dispatcher.  It
does not execute another guest, change a provider, or add a Bochs feature.

## Observations

- S90 reaches a bounded stop at `8DC8:0986`; its next bytes are `C4 C4 50 3D`.
  The preceding `5F` handoff has already selected the guest stream branch.
- `base/mvdm/inc/dossvc.h` assigns service `3D` to `SVC_DEMEXITVDM`.
  `dos/dem/demdisp.c` maps that exact table slot to `demExitVDM`, and
  `dos/dem/dem.h` declares it in the DEM interface.
- `dos/dem/demmisc.c::demExitVDM` documents input `DS:SI` as an error-message
  string and explicitly documents no exit because the VDM is killed.  The body
  invokes `RcErrorDialogBox(ED_BADSYSFILE, "config.nt", NULL)` and then
  `TerminateVDM()`.
- The reached guest sites support that meaning: `doskrnl/bios/sysinit1.asm`
  reaches the service from its `comerr` startup error path; COMMAND `init.asm`
  also uses it after its bad-version diagnostic.
- The historical termination implementation is host/product-bound:
  `softpc.new/host/src/nt_term.c` calls historical `ExitVDM`, then
  `ExitProcess(0)`.  It is neither an independently composable provider nor a
  normal guest continuation.
- The current global adapter map already classifies service `0x3d` as DEM
  miscellaneous and deferred.  `bx_ntvdm_dem_misc_plane_v1_dispatch` only
  admits its separately evidenced debug and system-symbol endpoints, so it
  does not accidentally handle `3D`.  The unrelated `C4 C4 FE` controlled-stop
  service recognizes only that three-byte form and cannot stand in for `3D`.

## Result

`50:3D` is the original fatal startup/error path, not a normal DOS termination,
not COMMAND `54:11`, and not an output or display request.  Its original
contract reads a guest message pointer, displays a historical NT UI error, and
terminates the whole VDM without resuming the guest.

The first CLI profile supplies neither the historical UI nor the host product
termination composition.  Its correct current disposition is therefore
**globally classified, explicitly deferred, and unimplemented**.  No provider,
generic stop alias, DS:SI read, state mutation, CLI result, or Bochs/mantle
change is admitted by this classification.  A future fatal-result design, if
needed, must be separately admitted as an engine-owned process result rather
than silently turning this BOP into success.

## Confidence and follow-up

High confidence in service identity, owner, input, and non-continuation: each
is directly stated by the original declaration, dispatcher, implementation,
and reached guest callers.  The S90 trace proves reachability of the boundary,
not the underlying configuration error that selected it.

The next work remains distinct: map the available source-built SHARE normal
return candidate and its prerequisite startup path.  It must not mistake this
fatal `50:3D` edge for proof of a CLI terminal result.

## Current-status supersession (T199 S37)

This T198 record preserves the original classification and the bounded native
observation; its then-current “unimplemented” disposition is superseded.
The T199 DEM package's existing `bx_ntvdm_dem_cli_unavailable_provider_v1`
selects `50:3D` as a typed controlled stop after the miscellaneous plane
declines it. This remains an adapter lifecycle result, not a UI recreation or
an ambient process termination. The source-built all-DEM r48 regression
asserts STOP with no resume RIP, register delta, or flags delta. It does not
claim that a native guest reaching this fatal path is a successful boot.
