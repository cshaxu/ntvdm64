# M0 T372 S2 — Redirector source-first interface closure

## Original DLL contract

The original `src/mvdm-host/vdmredir/sources` defines `TARGETNAME=vdmredir`
and `TARGETTYPE=DYNLINK`. Its matching original `vdmredir.def` exports exactly
these ten names:

1. `VrDispatch`
2. `VrInitialized`
3. `VrReadNamedPipe`
4. `VrWriteNamedPipe`
5. `VrIsNamedPipeName`
6. `VrIsNamedPipeHandle`
7. `VrAddOpenNamedPipeInfo`
8. `VrConvertLocalNtPipeName`
9. `VrRemoveOpenNamedPipeInfo`
10. `VrCancelPipeIo`

`nt_bop.c:514-584` retains the original dynamic load contract. It resolves the
first nine names before setting `VdmRedirLoaded`; `VrCancelPipeIo` remains an
original DLL export used by the provider/lifecycle path, not an `nt_bop.c`
load prerequisite. A failed `SafeLoadLibrary("VDMREDIR")` or missing required
name follows the original failed-load result: close the partial module,
retain unloaded state and return `ERROR_INVALID_FUNCTION` from BOP-7.

The future product build must therefore emit a `VDMREDIR.dll` with this
original export set and stage it beside the executable. It may use the current
original source selection and same-shaped adapters; it must not replace
`LoadVdmRedir` with static linkage or an app-owned selector switch.

## External-interface disposition

| Original provider family | Original bodies | First usable recovery rung | Current/required boundary | Deferred or unavailable extent |
| --- | --- | --- | --- | --- |
| DLL lifecycle and local broker | `vrdll.c`, `vrinit.c`, `vrmisc.c` | original body + same-shaped binding | session lifecycle and existing Redirector adapter; DEM/COMMAND consume it | no second command broker; VDD-only lifecycle remains source-defined failure |
| synchronous named pipe | `vrnmpipe.c`, `vrputil.c` | original body + same-shaped binding | public pipe API, session host-resource IDs, bounded guest leases | remote/private pipe behavior remains group-owned until S3 proof |
| mailslot | `vrmslot.c` | original body + same-shaped binding | public mailslot API, session host-resource IDs and leases | async delivery waits for S4 |
| async named pipe/completion | `vrnmpipe.c`, `vrinit.c`, `vrnetb.c` | original body + same-shaped binding | staged copied records, session cancellation and original machine-shaped completion call | no persistent guest alias, guest callback or direct IRQ ownership |
| local NetAPI identity results | `vrnetapi.c`, `vrremote.c` | original body + public Win32/NetAPI binding | bounded OEM result copy and session leases | source-only remote/RAP result paths await S5 disposition |
| NetBIOS and DLC/LLC | `vrnetb.c`, `vrdlc*.c` | original body where public API suffices; otherwise explicit original failure | existing `netb`/SoftPC mechanical declaration shapes | `DLCAPI`, private ACS/kernel/VDD and physical-completion paths are not silently emulated |
| window/mode/interrupt helpers | `vrdlc5c.c`, `vrdlcpst.c`, `vrremote.c` | original body + same-shaped facade only when reached | session + CPU40/SoftPC mechanical edge | no private monitor/kernel-VDM or GUI shell recreation |

The existing adapter functions are already limited to the required same-shaped
boundary work:

- `mvdm_redirector_handle_from_words` resolves a 32-bit session resource ID;
- `mvdm_redirector_mailslot_{publish,resolve,release}` maps the original DOS
  WORD identity to the same session resource manager;
- `mvdm_redirector_{copy_ansi_to_guest,copy_wide_to_guest,write_cd_names}`
  use bounded synchronous guest leases; and
- `mvdm_redirector_async_*` snapshots worker data and acquires a fresh lease
  only at completion.

They retain provider call shape without becoming policy providers. The
mapping-manager ownership is unchanged: host-resource identity belongs to the
session's host-resource instance, guest addresses to its guest-memory instance
and completion records to its completion instance. No raw native handle,
pointer or guest alias crosses an asynchronous lifetime.

## Rejected alternatives

- **Static-linking into `nt_bop.c`: rejected.** It changes the original
  `LoadVdmRedir` availability/error behavior and defeats the original DLL
  boundary.
- **New app or COMMAND Redirector dispatcher: rejected.** `vrdisp.c` already
  owns source ordering and unsupported-service behavior.
- **Reviving Bochs/adapter-bochs delivery: rejected.** CPU40/SoftPC owns the
  machine-shaped completion edge; Redirector owns neither a CPU nor selector
  mechanics.
- **Private CSR/BaseSrv, kernel VDM, RAP or DLC recreation: rejected.** The
  retained original branch must be reached through the source-defined
  unavailable/failure outcome where a public same-shaped facade cannot exist.

## S3 implementation boundary

S3 may construct the original DLL image, exports and stage contract, then
exercise only lifecycle, local broker and synchronous named-pipe groups. It
must prove that the completed image loads through `LoadVdmRedir` without
enabling unrelated mailslot, async, NetAPI, NetBIOS or DLC branches. S4/S5
retain their package-owner groups as set by T372.

## Verification

Reviewed original `vdmredir.def`, `sources`, `nt_bop.c`, all current redir
adapter headers/bodies, the session mapping-manager contract and the generated
CPU40/x86 graph. This evidence updates former documentation that called the
machine edge `adapter-bochs`: no such product component is selected; the
current boundary is CPU40/SoftPC through `adapter-mvdm-host-out/softpc`.

S2 is closed as an interface-design and source-recovery record. It performs
no selector enablement, DLL build, stage copy or runtime observation.
