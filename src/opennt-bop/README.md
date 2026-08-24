# opennt-bop

Minimal-change mirrors of original OpenNT BOP providers, selector/service
routing and source-owned provider control flow. Preserve original names,
parameters, structures, ordering and observable failures.

It may call `opennt-host`, `adapter-win32` and `adapter-softpc` through their
declared interfaces; it must not touch Bochs objects, `bx-mantle` or `bx-core`
directly.

## Local intrusion register

Every edit to an imported BOP mirror is registered here with original path and
identity, exception identifier, necessity and focused verification. Each edited
source expression additionally carries a `DIVERGENCE:` comment naming the
replacement and reason. The future static library name is `opennt-bop`; its
public include surface is the BOP ingress/provider ABI consumed by `app`.

## Production layout

| Directory | Contents | Modification rule |
| --- | --- | --- |
| `command/`, `dem/`, `xms/`, `dpmi/`, `softpc/`, `vdmredir/` | Compiled, minimal-change OpenNT provider bodies, retained in their original owner-family grouping. | Every changed statement, include or definition is marked `DIVERGENCE:` and registered below. |
| `ingress/` | Compiled project-owned OpenNT selector routing and session/provider dispatch. | May adapt component boundaries, but may not reimplement an imported provider without a source-derived exception.  The selector-blind copied-frame ingress is in `adapter-bop`. |

Immutable originals, uncompiled mirror fragments, pre-T260 overlays and import
manifests are evidence rather than production code. They are retained under
`docs/etc/legacy_code/opennt-bop/`; the authoritative upstream comparison root
remains `refs/opennt/`.

## Local divergence register

Each row is a required nonzero difference from an identified OpenNT source.
The cited production file also contains the corresponding `DIVERGENCE:`
comment. Historic overlays are evidence only and are recorded in
`docs/etc/legacy_code/opennt-bop/`, not as a second provider.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `BOP-DIV-001` | `cmdconf.c` finds NT4's installed `command.com`. | A standalone bundle has no NT4 system installation contract. | Ask the COMMAND host provider for the admitted source-built image. | `command/cmdconf.c:514` |
| `BOP-DIV-002` | `cmddisp.c` imports the NT4 CCPU/SAS header closure. | That product-host closure is not independently linkable. | Use the fixed-width COMMAND shim while retaining the service table. | `command/cmddisp.c:9` |
| `BOP-DIV-003` | `cmdenv.c` snapshots the process environment. | Environment ownership is per CLI session. | Materialize an equivalent session-owned Unicode multisz. | `command/cmdenv.c:48` |
| `BOP-DIV-004` | `cmdenv.c` uses historical environment byte counts. | Host width differs on x64. | Make the original `DWORD` count conversion explicit. | `command/cmdenv.c:440` |
| `BOP-DIV-005` | `cmdenv.c` grows by fixed 512-byte units. | Modern bounded input may exceed one legacy increment. | Grow by the actual bounded requirement, retaining the original result contract. | `command/cmdenv.c:457` |
| `BOP-DIV-006` | `cmdenv.c` relies on source-flow initialization. | Modern `/W4 /WX` rejects the unproven local state. | Initialize only the temporary; preserve short-circuit ordering. | `command/cmdenv.c:554` |
| `BOP-DIV-007` | `cmdenv.c` counts values through historical pointer assumptions. | Counts must be width-stable on x86/x64. | Keep `DWORD` count fields explicit. | `command/cmdenv.c:567` |
| `BOP-DIV-008` | `cmdenv.c` assigns `p1` through a short circuit. | Modern flow analysis cannot prove that assignment. | Initialize `p1` without changing source traversal. | `command/cmdenv.c:725` |
| `BOP-DIV-009` | `cmdenv.c` computes an environment name length. | The OpenNT count ABI is `DWORD`, not host `size_t`. | Explicitly retain the `DWORD` conversion. | `command/cmdenv.c:734` |
| `BOP-DIV-010` | `cmdexec.c` uses the historical global `VDMInfo`. | The imported global conflicts with the admitted `cmdmisc` body. | Retain a function-local return record with the same lifecycle order. | `command/cmdexec.c:697` |
| `BOP-DIV-011` | `cmdexec.c` treats `BX:CX` redirection data as host-addressable. | A guest token cannot be a host pointer on x64. | Resolve it through the session redirection map. | `command/cmdexec.c:724` |
| `BOP-DIV-012` | `cmdexit.c` imports CCPU/SoftPC product headers. | Only the typed stop seam is needed in this composition. | Replace the include closure, preserving the imported body. | `command/cmdexit.c:11` |
| `BOP-DIV-013` | `cmdmisc.c` assigns a title length to `ULONG`. | `strlen` is native-width. | Use an explicit historical-width conversion. | `command/cmdmisc.c:122` |
| `BOP-DIV-014` | `cmdmisc.c` stores redirection information in a 32-bit CMDINFO field. | Raw pointers cannot cross that field on x64. | Use the session's opaque guest token. | `command/cmdmisc.c:152` |
| `BOP-DIV-015` | `cmdmisc.c` computes command-line length. | The original observable type is `ULONG`. | Explicitly preserve that conversion. | `command/cmdmisc.c:318` |
| `BOP-DIV-016` | `cmdmisc.c` publishes `pRdrInfo` in CMDINFO. | Host pointer width is not guest ABI width. | Publish a fixed guest token in the original slot. | `command/cmdmisc.c:371` |
| `BOP-DIV-017` | `cmdmisc.c` writes the historical `0xff` sync marker. | Plain signed conversion is warning-prone on x64. | Make the same 8-bit value explicit. | `command/cmdmisc.c:376` |
| `BOP-DIV-018` | `cmdmisc.c` walks a multisz with pointer-to-`ULONG` casts. | Such casts truncate x64 pointers. | Keep bounded source traversal using native-width arithmetic. | `command/cmdmisc.c:649` |
| `BOP-DIV-019` | `cmdpif.c` declares ForceDos-only locals. | Their owning branches are absent. | Retain declaration order but silence unused-local diagnostics. | `command/cmdpif.c:31` |
| `BOP-DIV-020` | `cmdpif.c` assigns a `MAX_PATH` length to a 16-bit CMDINFO field. | Modern compilation requires a visible narrowing boundary. | Explicitly convert the bounded original value. | `command/cmdpif.c:231` |
| `BOP-DIV-021` | `cmdpif.c` assigns a bounded command path size. | The destination retains a 16-bit original ABI. | Explicitly convert the bounded original value. | `command/cmdpif.c:264` |
| `BOP-DIV-022` | `cmdredir.c` uses K&R forms and diagnostic locals. | `/W4 /WX` diagnoses them. | Preserve semantic body while applying only warning-clean declarations. | `command/cmdredir.c:15` |
| `BOP-DIV-023` | `mem_size.c` includes Insignia/CPU/SAS product headers. | They are not a standalone modern closure. | Use the narrow SoftPC memory-size facade. | `softpc/mem_size.c:4` |
| `BOP-DIV-024` | `suballcp.h` imports private NT headers. | They are unavailable outside NT4 composition. | Use `opennt_xms_compat.h` plus the same-shaped `adapter-softpc` facade. | `xms/suballcp.h:2` |
| `BOP-DIV-025` | `suballoc.c` imports private NT headers. | They are unavailable outside NT4 composition. | Use `opennt_xms_compat.h` plus the same-shaped `adapter-softpc` facade. | `xms/suballoc.c:2` |
| `BOP-DIV-026` | `xms.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the local XMS compatibility header and `adapter-softpc` machine facade. | `xms/xms.c:2` |
| `BOP-DIV-027` | `xmsa20.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the local XMS compatibility header and `adapter-softpc` machine facade. | `xms/xmsa20.c:2` |
| `BOP-DIV-028` | `xmsblock.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the local XMS compatibility header and `adapter-softpc` machine facade. | `xms/xmsblock.c:2` |
| `BOP-DIV-029` | `xmsdisp.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the local XMS compatibility header and `adapter-softpc` machine facade. | `xms/xmsdisp.c:2` |
| `BOP-DIV-030` | `xmsmisc.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the local XMS compatibility header and `adapter-softpc` machine facade. | `xms/xmsmisc.c:2` |
| `BOP-DIV-031` | `xmsumb.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the local XMS compatibility header and `adapter-softpc` machine facade. | `xms/xmsumb.c:2` |
| `BOP-DIV-032` | `nt_pif.c` and COMMAND's `cmdpif.h` expose different historical `PIF_DATA` definitions. | Direct linking would cross incompatible layouts. | Keep the host parser source unchanged and translate only its compatible result through a separately named BOP composition unit. | `command/opennt_pif_composition.c:33`, `command/opennt_pif_composition.h` |
| `BOP-DIV-034` | COMMAND flushes historical LPT open state. | No LPT controller model is admitted. | Preserve a no-controller outcome rather than claiming a flush. | `command/opennt_command_composition.c:297` |
| `BOP-DIV-035` | COMMAND publishes console bytes through BaseSrv/CSR. | The NT4 product broker is absent. | Retain the session publication boundary. | `command/opennt_command_composition.c:307` |
| `BOP-DIV-036` | COMMAND probes an old 1 KiB DOS environment span. | That limit is not the source provider contract. | Read the exact bounded multisz extent. | `command/opennt_command_composition.c:866` |
| `BOP-DIV-037` | COMMAND materializes historical SAS pointers. | Raw aliased host access is unsafe. | Use bounded copied spans with alias rejection. | `command/opennt_command_composition.c:908` |
| `BOP-DIV-038` | `demdir.c` imports the historical DEM/SoftPC/OEM product-header closure. | That closure cannot compose independently with the modern component graph. | Split only the include boundary between BOP composition and same-shaped adapter-win32 OEM calls. | `dem/demdir.c:13` |
| `BOP-DIV-039` | `demfile.c` imports its historical OEM/CCPU/VDMREDIR header closure. | That product-header closure cannot independently compose with the modern component graph. | Preserve the imported provider body and route only the include boundary through declared component facades. | `dem/demfile.c:19` |
| `BOP-DIV-040` | `demgset.c` imports historical DEM/SoftPC/OEM product headers. | That product closure cannot independently link in the component graph. | Retain the imported body; isolate BOP selection/DTA state, same-shaped public-Win32 queries and host drive admission at declared boundaries. | `dem/demgset.c:31` |
| `BOP-DIV-041` | `demsrch.c` imports its historical DEM/SoftPC/NT/VDD product-header closure. | The original host closure is not independently composable. | Retain the imported search provider body and route the include boundary through declared component APIs. | `dem/demsrch.c:17` |
| `BOP-DIV-042` | `demmisc.c` creates a VDD user hook. | The NT4 VDD broker is not independently composable. | Retain the original call boundary as an explicit deferred no-op; VDD/debugger recovery owns any future provider. | `dem/opennt_demmisc_composition.c:60` |
| `BOP-DIV-043` | `demmisc.c` emits VDD/debugger module notifications. | Their NT4 receivers are absent from the CLI composition. | Preserve no-success/no-fabricated-notification behavior at the original source boundary. | `dem/opennt_demmisc_composition.c:66` |
| `BOP-DIV-044` | `demmisc.c` obtains debugger console input. | Guest-visible debugger input needs its separate console/input owner lifecycle. | Retain the explicit deferred boundary; do not synthesize input. | `dem/opennt_demmisc_composition.c:76` |
| `BOP-DIV-045` | `demmisc.c` resets host floppy hardware. | FDC/DMA/CMOS belongs to the machine owner, not a DEM provider. | Do not report a fabricated controller reset. | `dem/opennt_demmisc_composition.c:87` |
| `BOP-DIV-046` | `demdasd.c` uses recursive SoftPC execution. | A bounded Bochs composition cannot re-enter the original SoftPC worker at this call boundary. | Retain the named source boundary and explicit non-success outcome. | `dem/opennt_demdasd_ioctl_composition.c:75` |
| `BOP-DIV-047` | `demerror.c` / `demlock.c` reach CCPU/SAS state and NTDLL Unicode/symbolic-link helpers through the NT4 product closure. | Only a bounded same-shaped adapter closure is independently composable. | Keep provider/error ordering in the imported bodies; use the scoped `adapter-softpc` and adapter Win32 seams. | `dem/opennt_demerror_lock_composition.c:23`, `dem/opennt_demerror_lock_composition.c:85` |
| `BOP-DIV-048` | `demfile.c` imports `VrInitialized` as a VDMREDIR-DLL function-pointer import. | Static composition cannot define a DLL import pointer in each original translation unit. | Preserve the original pointer call shape; its single definition is owned by the composition unit. | `dem/demfile.c:25` |
| `BOP-DIV-049` | `demhndl.c` imports `VrInitialized` as the same VDMREDIR-DLL function-pointer import. | Static composition requires the single shared import definition. | Preserve the original external pointer declaration; the composition unit supplies it. | `dem/demhndl.c:21` |
| `BOP-DIV-050` | `demlabel.c` imports the companion DEM file/OEM/CCPU header closure. | That historical product-header closure cannot independently compose. | Use the shared declared composition header without changing the imported label-provider body. | `dem/demlabel.c:14` |
| `BOP-DIV-051` | `LoadVdmRedir` populates the VDMREDIR DLL import after dynamic loading. | The standalone product statically links the re-rooted provider rather than loading the historical DLL. | Retain the function-pointer indirection and bind it once to the private static provider entry. | `dem/opennt_demfile_composition.c:22` |
| `BOP-DIV-052` | `demfcb.c` imports its historical DEM/SoftPC/NT/VDD product-header closure. | The original host closure is not independently composable. | Retain the imported FCB provider body and route the include boundary through declared component APIs. | `dem/demfcb.c:22` |
| `BOP-DIV-053` | `VDDTerminateUserHook` walks the NT4 private VDD user-hook list. | The standalone product does not recreate a VDD callback broker. | Preserve the named source call boundary as a deferred no-op; it cannot report fabricated callback completion. | `dem/opennt_demsrch_fcb_composition.c:32` |
| `BOP-DIV-054` | `demdasd.c` closes floppy/FDC state through the NT4 device product shell. | FDC/DMA/CMOS is a separate machine owner, not a host-volume capability. | Retain explicit unsupported failure rather than claiming a controller close. | `dem/opennt_demdasd_ioctl_composition.c:100` |
| `BOP-DIV-055` | `demdasd.c` opens native `\\DosDevices` paths and FAT-specific FSCTLs. | Those private NT opens are unsuitable for the public modern Win32 host boundary. | Use declared documented-handle capability calls while retaining the original BPB/geometry failure contract. | `dem/opennt_demdasd_ioctl_composition.c:114` |
| `BOP-DIV-056` | `vrdisp.c` dispatches through the NT4 VDMREDIR product-global CCPU frame. | A modern session cannot retain product-global CPU state or raw guest pointers. | Preserve the original 0x00–0x31 service table and range rule; bind each call only to its copied synchronous frame callback. | `vdmredir/vrdisp.c` |
| `BOP-DIV-057` | `vrmslot.c` imports NT4 VDMREDIR/CCPU header closure. | That product closure cannot independently compose with the modern component graph. | Preserve the original body and register call shape through the declared `adapter-softpc` CCPU/SAS facade. | `vdmredir/vrmslot.c:11` |
| `BOP-DIV-058` | `vrmslot.c` allocates private arbitrary 16-bit mailslot IDs from `Handle16Bitmap`. | The product has one session-owned opaque host-handle manager and must not introduce a second handle allocator. | Keep the original `VR_MAILSLOT_INFO` list and lifecycle, but obtain an already range-checked opaque token from the shared manager; the list never interprets a native `HANDLE` as a guest ID. | `vdmredir/vrmslot.c:52`, `ingress/redir_native_session.c:442` |
| `BOP-DIV-059` | `VrTerminateMailslots` cleanup closes a raw process handle kept in its mailslot record. | Native handles must remain private to the session mapping manager. | Retain the close call shape while the existing CCPU/SAS facade resolves and releases the matching opaque token. | `vdmredir/vrmslot.c:125`, `vdmredir/vrmslot_compat.h:15` |
| `BOP-DIV-060` | `VrTerminateMailslots` receives its PDB as a function argument from the historical NetResetEnvironment caller. | The generic BOP ingress supplies only a copied CPU frame. | A BOP-owned one-line bridge obtains the same 16-bit PDB from `AX` before calling the retained source helper. | `vdmredir/vrmslot.c:192`, `ingress/redir_native_session.c:599` |
| `BOP-DIV-061` | `VrDeleteMailslot` returns its private 16-bit ID to `VrpFreeHandle16` after closing the raw record handle. | The product's sole opaque manager owns the token allocation and retirement. | The retained close boundary releases the same token; no private bitmap release remains. | `vdmredir/vrmslot.c:143` |
| `BOP-DIV-062` | `VrMakeMailslot` obtains its ASCIZ name by dereferencing an NT4 flat SAS pointer. | A guest pointer may not escape the copied modern CCPU/SAS call. | Copy the bounded input through the existing facade before the retained source ordering continues. | `vdmredir/vrmslot.c` |
| `BOP-DIV-063` | `VrMakeMailslot` reserves an ID in its private bitmap before creating a native mailslot. | The product permits only its one session-owned opaque handle manager. | Publish the successfully-created handle through that manager, retaining the original 16-bit result and all following record initialization. | `vdmredir/vrmslot.c` |

The migration evidence is
[`m0-t260-s5-opennt-bop-layout-migration-001.md`](../../docs/etc/evidence/m0-t260-s5-opennt-bop-layout-migration-001.md).
