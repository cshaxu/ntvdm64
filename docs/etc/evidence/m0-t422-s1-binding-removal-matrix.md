# T422 S1 WOW32 binding removal matrix

## Question

For each currently selected target-local WOW32 binding, what exact original
operation does it bridge, why is the original translation unit not directly
composable, and what concrete evidence would permit deletion rather than an
unbounded accumulation of autonomous code?

## Method

Every selected `src/wow32-dll/source/*.c` file was read at the admitted
revision.  The matrix groups only functions sharing one source owner and one
runtime lifetime; it does not use a similarly named Win32 API as equivalence.
“Retain” means *pending the named receiver's evidence*, not accepted forever.

| Binding / operations | Original owner and unavailable edge | Four-rung disposition | Receiver and removal condition |
| --- | --- | --- | --- |
| `ntuser_rtl_compat`: NLS code page, alloc/free | `ntuser/rtl/chartran.c`; NT private RTL carrier is absent from the selected DLL environment. | Reuse original `chartran` body through three typed CRT/Win32 forms. | S6. Remove only if a directly composable original RTL carrier replaces all three names in the same DLL build. |
| `wow_bitmap_bindings`: scoped bitmap/DC load/shutdown | Original WOW bitmap/resource callers; native DC lifetime crosses imported image conversion. | Keep original conversion, bind only synchronous native DC/capability scope. | S3/S4. Remove when an admitted original client slice supplies the same finite DC lifetime with no host-resource leak. |
| `wow_callback_frame_lease`: linear/VP acquire and release | `wcall16.c::CallBack16`; original `FlatAddress` exposed a worker pointer. | Original callback algorithm plus bounded CCPU lease. | S2. Not removable unless original CPU40 form can acquire/commit/release all callback frames without a raw pointer escaping the call. |
| `wow_class_client_bindings` and `wow_class_words_binding`: class registration, lookup, WOW words, procedure exchange and removal | Original `client.c`, `class.c`, `cleanup.c`; private NT USER class storage is unavailable. | Reuse selected original class algorithms; bind native registration/property storage. | S2. Remove portions only when a newly admitted original source slice has a finite modern boundary and replaces the corresponding property/callback lifetime. |
| `wow_cleanup_bindings`: enrolled-window cleanup tree | Original `cleanup.c`; native USER object enumeration/locks are private. | Reuse original cleanup traversal with bounded derived native tree. | S2. Remove only after the source-shaped guest/native object producer exposes a complete finite cleanup enumeration; no success no-op is admissible. |
| `wow_dialog_creation_binding`: dialog create hooks and callback scope | Original `wudlg.c` and USER registration output; private server create-dialog call unavailable. | Keep original caller/conversion; bind public dialog creation plus temporary callback scope. | S2. Remove only if an admitted original client dialog path composes with the same callback ordering and rollback. |
| `wow_private_user_compat`: FillWindow, DWP bits, resource ID, cursor/icon, menu index/indirect menu | Separate original USER private operations in `paint`, `server/client`, `clres`, `validate` and menu sources; private server entrypoints are unavailable. | One named public equivalent per operation, retaining original caller policy. | S2/S4/S7 by operation. Delete a function only after its exact original source owner is composable; do not bulk-delete/move the facade. |
| `wow_public_user_facade`: font, glyph, ETM, network fonts, start glass, DC cache | Original WOW GDI/USER callers; not a single original host unit. | Public API binding with operation-specific source owner. | S3/S7. Split/delete only after each native resource lifetime and original error behavior are passed; no blanket “USER32 available” deletion. |
| `wow_sim32_pointer_compat`: `Sim32FlushVDMPointer` | CPU40 `softpc.h` makes the original operation a no-op, but all selected WOW32 objects were compiled against the historical external declaration. | Retain the exact no-op function shape as the smallest late-DLL ABI seam; it owns no mapping or state. | S1 proof: altered link without its object leaves 38 callers unresolved. It is not a deletion candidate unless caller headers are changed to select the original macro and the whole DLL is rebuilt/retested. |
| `wow_task_event_binding` and `wow_task_profile_binding` | Original task registration/profile readers use NT objects/Registry handles. | Retained native event; shared layered-registry open-key provider. | S2/S7. Remove only if original source can be selected with the same finite event/key ABI; otherwise minimize to the exact retained handle/key operation. |
| `wow_user_borrow_scope` and `wow_user_private_access` | Original USER window/class lock and private word access. | Scoped property-backed borrow, not a persistent host alias. | S2. Replace only with source-shaped client access retaining lock duration, stale rejection and callback-safe release. |
| `wow_user_message_bridge` | Original taskman/queue receive and wake behavior; NT queue internals unavailable. | Original scheduler policy with one public queue wake/receive bridge. | S2. Remove only if an admitted original client queue path can supply the exact wait/receive lifecycle; never replace it with a second scheduler. |
| `wow_user_object_bindings` | Original `handtabl.c` typed entry publication. | Present active-domain behavior fails explicitly rather than publishing a free-list link as `phead`. | S2. Replace the file/function with a full guest WND/owner/type/uniqueness producer and retirement path; current rejection itself must then disappear. |
| `wow_user_registration_bridge` | Original `wow32.c::W32Init` to private USER registrar. | Local ABI transport retains all 21/20 slots and explicit placeholder. | S2-S7. Shrink as slots move to direct admitted original owners; cannot delete registration transport until modern USER accepts the original ABI, which current probe disproves. |
| `wow_user_task_lifecycle` | Original `taskman`, `queue`, `cleanup`, task exit ordering; NT thread/APC/server state unavailable. | Original selected algorithms with one worker runtime and finite public wait. | S2. Retire helpers only when their original owner becomes directly composable; production must first wire B1--B5 and keep module/task cleanup distinct. |
| `wow_user_thunk_scope` | Original `W32Dispatch` USER-domain enter/leave around callbacks. | One worker runtime scope, no second TLS/lock. | S2. Remove only if original dispatch has a directly composable current worker-domain interface; preserve recursion/error cleanup. |
| `wow_window_creation_binding` and `wow_window_words_binding` | Original create/class/WND word operations require private USER object state. | Public creation plus property-backed scoped metadata; original WOW caller policy remains selected. | S2. Replace only with complete guest/native WND producer that proves create callback, mutation, reentrant destroy, stale handle and task cleanup. |

## Accounting result

The matrix covers all **21 files / 2,977 lines** named in the autonomous
inventory. The fresh altered-link experiment rejects the sole initial direct
deletion candidate: without `wow_sim32_pointer_compat.c` the linker reports
`Sim32FlushVDMPointer@16` unresolved from 38 original WOW32 objects. Thus zero
lines are currently declared removable by source resemblance alone.
The largest coupled reduction opportunity is S2's identity/lifecycle group:
16 files / 2,419 lines (callback, class, cleanup, dialog, event/profile,
borrow/message/object/private access, registration, lifecycle, thunk, window
creation/words).  This is an opportunity estimate, not a promised deletion:
S2 must separately measure every replaced binding and retain the finite
standalone parts whose historical kernel/USER dependency remains unavailable.

No mirror diff is added by this matrix.  If a receiver imports an original
OpenNT file, it must be added at its original path with provenance and its
replaced binding must be measured separately; moving autonomous code into a
mirror is prohibited.

## Private/public USER facade operation audit

The two facade files are deliberately not accepted as opaque “USER32
compatibility.”  The following operation-level pass resolves every exported
operation in them.  A `reuse` row names an original body already selected by
the caller; a `binding` row has an original server/private edge that cannot
cross into a standalone worker.  The stated receiving packet must prove the
normal, failed and cleanup path before retaining the binding.

| Current operation | Original caller / source owner | S1 disposition and receiver |
| --- | --- | --- |
| `DialogBoxIndirectParamAorW` | `wudlg.c::WU32DialogBoxIndirect`; `client/clres.c` dialog helper | Binding.  The original converted template/callback order remains in `wudlg`; public A/W dialog creation supplies only the unavailable client/server call.  S2 proves callback order and rollback. |
| `GetHFONT`, `GetGlyphOutlineWow`, `GetETM`, `GdiCleanCacheDC` | `wgfont.c`, `wgdi31.c`, `wgdi.c`, `wreldc.c`; original GDI/client paths | Binding.  Modern public GDI supplies the named DC operation; no native DC or font pointer is published to guest.  S3 proves resource lifetime and error paths. |
| `AddFontResourceTracking`, `RemoveFontResourceTracking`, `UnloadNetworkFonts` | `wgfont.c`, `wkman.c`; original remote-font tracking | Binding.  The finite owner-id/refcount list is a standalone substitute for private font tracking, not a new font provider.  S3/S7 prove repeated task release and unavailable-font behavior. |
| `ShowStartGlass` | `wow32.c`, `wkman.c`; `client/client.c` / `kernel/queue.c` presentation call | Explicit unavailable no-state presentation boundary.  It must remain void and must not synthesize focus, cursor or task success.  S2/S7 prove it cannot change task ordering. |
| `wow_private_user_fill_window` | registration output consumed by original client paint flow; `kernel/paint.c::xxxFillWindow` | Binding.  Public USER/GDI covers the observable clip/brush work; private PWND/DC locks remain unavailable.  S2/S3 prove callback, invalid DC and release behavior. |
| `wow_private_user_get_def_window_proc_bits` | `wuman.c::WU32NotifyWow`; `client/client.c::WowGetDefWindowProcBits` | Candidate original-source recovery.  The exact original client algorithm is present but depends on the original client message data/headers.  S2 must either compile that finite original body through its existing client selection or retain the verified static table with byte-for-byte output test. |
| `wow_private_user_get_id_from_directory` | registration output; `client/clres.c` resource lookup | Binding to public icon-directory selection.  The original resource name/alias policy remains in `wcuricon.c`; S4 proves invalid-resource and replacement behavior. |
| `wow_private_user_load_create_cursor_icon` | registration output; `client/clres.c::WowServerLoadCreateCursorIcon` | Candidate original-source recovery.  `clres.c` is already present as a restricted original slice, but this server call crosses unavailable private state.  S4 must extend the selected original slice only if its finite resource/DC boundary is fully proved; otherwise retain this narrow public conversion binding. |
| `wow_private_user_get_menu_index` | registration output; `kernel/ntstubs.c::NtUserGetMenuIndex` | Binding.  Private menu validation/locking is unavailable; public validation and immediate-item enumeration are the finite edge.  S4 proves stale and nested menu behavior. |
| `wow_user_load_menu_indirect`, `wow_private_user_load_create_menu` | `wcuricon.c` and registration output; `client/clres.c::WowServerLoadCreateMenu` | Binding to the already imported original Win16 menu parser with a public menu allocation carrier.  S4 proves parser conversion, owner-draw aliases and full teardown. |

This pass reduces no binding line today: none of the five candidate original
sources can be imported wholesale without private USER server/lock state.
It does establish the only allowed deletion criterion: S2--S4 may delete a
facade operation only after its exact original source range is selected,
linked and passes the same guest route and retirement contract; a similarly
named public API is insufficient.

## Follow-up

The full provider has fresh x86 link evidence. S1 therefore no longer carries
an untested deletion promise for the 24-line pointer-flush shim. Any future
macro-routing change is a material ABI/build change and must prove both the
same no-op result and all affected callers' runtime behavior. All other
removal decisions are owned by their complete receiving packet and must
include production, negative, reentry and teardown evidence.
