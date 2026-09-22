# T421 S1 DLL compilation-boundary ledger

## Question

What do the current x86 product graph and deployed package actually compose
for `VDMREDIR.DLL` and `WOW32.DLL`, and which non-mirror bindings belong to a
DLL target versus the one `ntvdm.exe` worker?

## Inputs

- [`src/mvdm/vdmredir/sources`](../../../src/mvdm/vdmredir/sources),
  [`vdmredir.def`](../../../src/mvdm/vdmredir/vdmredir.def), and the 15
  selected original C bodies beneath that same canonical mirror path.
- [`src/mvdm/wow32/sources`](../../../src/mvdm/wow32/sources) and
  [`wow32.def`](../../../src/mvdm/wow32/wow32.def): the original manifest
  declares 86 C bodies, one resource and `W32DllInitialize`.
- Fresh generated graph
  `build/M0-T421/S1/formal-x86-r1/build.ninja`, generated on 2026-09-22 by
  `New-T310OriginalSoftpcNinja.ps1` with the pinned Node 22 and x86 inputs.
- Current target bindings under `src/ntvdm-exe/redir/` and
  `src/ntvdm-exe/wow/`, plus the original dynamic loader in
  [`nt_bop.c`](../../../src/mvdm/softpc.new/host/src/nt_bop.c).
- Read-only package inspection of `O:\winnt\VDMREDIR.dll` and
  `O:\winnt\WOW32.dll` with x86 `dumpbin`.

## Procedure

1. Parsed both original `sources` manifests and the fresh Ninja graph from
   source through object, archive and final target.
2. Ran `ninja -C build/M0-T421/S1/formal-x86-r1 -n VDMREDIR.dll`; the dry run
   reaches the real `ntvdm.exe` parent link and then the VDMREDIR DLL link.
   It is a graph proof only: no current package artifact was changed.
3. Inspected declared exports and the deployed DLL import/export tables.
4. Searched every current redirector/WOW binding for session/TLS ownership and
   compared it with the formal inputs.

## Observations

### VDMREDIR is a current formal DLL

| Layer | Current selected content | Ownership/disposition |
| --- | --- | --- |
| Canonical mirror | 15 original C bodies (`vrmslot` through `vrdll`), `vdmredir.rc`, and `vdmredir.def` | Remain in `src/mvdm/vdmredir/`; never move or duplicate. |
| Original archive | `original-mvdm-redir.lib` | Source audit carrier; the final DLL consumes the objects directly, preserving the historical DLL target shape. |
| DLL-local finite bindings | `mvdm_redirector_guest_copy.c`, `mvdm_redirector_remote_unavailable.c`, `mvdm_redirector_dll_entry.c` in `redirector-bindings.lib` | Move in S2 to `src/vdmredir-dll/`. They respectively translate bounded numeric guest copies, expose the source-proven unavailable RAP boundary, and adapt the original `VrDllInitialize` to x86 stdcall. |
| Worker bindings | `mvdm_redirector_worker_copy.c`, `mvdm_redirector_async.c` | Stay in `src/ntvdm-exe/`: they acquire worker-owned guest-memory leases and own no DLL state. |
| Final link | 15 redirector objects + resource + `ntvdm.lib`, `redirector-bindings.lib`, original NetAPI/RTL and existing finite SoftPC bindings | No `session.lib` or CCPU archive is named on the VDMREDIR link edge. The required worker functions are imports from `ntvdm.exe`, not a copied static worker state. |

The live original loader calls `SafeLoadLibrary("VDMREDIR")` and resolves the
ten names in `vdmredir.def`.  The deployed `O:\winnt\VDMREDIR.dll` dated
2026-09-22 exports exactly those ten names.  Its imports include bounded
redirector worker-copy/async functions and current-thread operations from
`ntvdm.exe`, which is the intended parent-DLL ABI; it does not prove or imply a
second `session.lib` copy.

### WOW32 is source-present, but not a current formal target

The original WOW32 manifest is complete in the canonical mirror: 86 C bodies,
one resource, `wow32.def`, `TARGETTYPE=DYNLINK`, and
`DLLENTRY=W32DllInitialize`.  The fresh T421 graph, however, contains no
`WOW32.dll` target and no object compiled from `src/mvdm/wow32/`.  Its product
phony lists only `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`, and
`VDMREDIR.dll`.

The graph does build exactly two WOW-adjacent worker sources into
`wow-worker-bindings.lib`, then links that archive into `ntvdm.exe`:

| Binding | Current disposition | Why |
| --- | --- | --- |
| `wow_user_runtime.c` | Retain in `src/ntvdm-exe/` | Contains the process worker's `__declspec(thread)` current-binding state. |
| `wow_user_session_binding.c` | Retain in `src/ntvdm-exe/` | Attaches/detaches that state to the sole worker session. |
| `wow_user_object_bindings.c` | Fixture-only input; no product move in S3 without a proved DLL consumer | It is linked only by `wow-page-domain-test.exe`. |
| Remaining 18 `ntvdm-exe/wow/*.c` files: `wow_bitmap_bindings`, `wow_callback_frame_lease`, `wow_class_client_bindings`, `wow_class_words_binding`, `wow_cleanup_bindings`, `wow_dialog_creation_binding`, `wow_private_user_compat`, `wow_sim32_pointer_compat`, `wow_task_event_binding`, `wow_task_profile_binding`, `wow_user_borrow_scope`, `wow_user_message_bridge`, `wow_user_private_access`, `wow_user_registration_bridge`, `wow_user_task_lifecycle`, `wow_user_thunk_scope`, `wow_window_creation_binding`, `wow_window_words_binding` | Not formal-product inputs; retain as explicitly unadopted WOW-recovery material | T421 must not silently turn partial experiments into a runtime provider. Their functional closure remains the queued WOW32 recovery task. |

The generator writes `generated/ntvdm-wow32-provider.def`, an intended parent
import ABI that includes the two runtime operations, selected memory/descriptor
operations and existing redirector worker exports.  It is consumed by the
`ntvdm.exe` link but has no matching current WOW32 target.  Therefore it is an
unconsumed prospective contract, not evidence that current production loads a
WOW32 provider.

`nt_bop.c` still has the original `SafeLoadLibrary("WOW32")` and resolves
`W32Init`/`W32Dispatch`.  `O:\winnt\WOW32.dll` exists, but its 2026-09-14
timestamp predates the 2026-09-22 current `ntvdm.exe` and VDMREDIR package.
It is a retained S42 experiment (27 exports), not an artifact generated by the
current formal graph and not an acceptance claim for this product state.

## Boundary conclusion

The no-second-worker invariant is presently true for the one formal DLL:
VDMREDIR imports narrow operations from the parent and has no direct session or
CCPU executor archive dependency.  It is not yet demonstrable for WOW32 in
the current product because no current WOW32 target exists.  S3 must create
that target from the original mirror manifest and make it import, rather than
link, the two worker-local runtime operations.  It must not revive any
fixture-only or unselected WOW experiment merely to make the target link.

## S2/S3 move manifest

- **S2:** create `src/vdmredir-dll/`, relocate only the three DLL-local
  redirector binding bodies/headers and their target manifest/build contract;
  keep the two worker bindings under `src/ntvdm-exe/redir/`.
- **S3:** create `src/wow32-dll/` only after converting the complete original
  WOW32 manifest into a target edge.  It owns target manifest, resource/DEF,
  entry binding and finite imports to `ntvdm.exe`; it cannot own session/TLS,
  CCPU, guest-memory leases, BaseSrv or original WOW bodies.
- **S4:** remove the generated provider definition if it is not consumed by
  the new current target, or prove it is the exact target import contract;
  remove every stale worker-tree alias after the new owners compile.

## Confidence and follow-up

High confidence for VDMREDIR: the source manifests, fresh graph, dry-run final
link, original loader, and deployed PE table agree.  High confidence that the
current WOW32 target is absent: the fresh graph has neither WOW32 target nor
canonical WOW32 object, while the only discovered package DLL is older than
the current package.  S1 deliberately makes no behavioral claim and changes
no source or guest media.  S2 is now implementation-ready; S3 requires the
explicit target design recorded here before any source movement.
