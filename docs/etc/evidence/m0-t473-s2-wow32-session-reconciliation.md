# M0 T473 S2 — WOW32/session source reconciliation

## Product shape

The r27 graph selects 78 original WOW32 sources directly into `WOW32.DLL`.
They remain under `src/mvdm/wow32`; the target manifest, original DEF and
resource remain canonical there.  `src/wow32-dll` contains only DLL-local
modern-boundary bindings.  The parent `ntvdm.exe` retains exactly three
worker-local files: `wow_gdi_alias.c`, `wow_user_runtime.c` and
`wow_user_session_binding.c`.

That split is material.  The two USER runtime files own the one worker TLS /
current-session association.  The DLL imports the narrow current/enter/leave/
context/worker-active ABI rather than linking `session.lib`, CCPU or an
alternate worker-state allocator.  `wow_gdi_alias` is similarly a parent
guest-address/alias boundary, not a second DLL state provider.

## Ownership decisions

| Area | Original owner | Modern boundary | S2 disposition |
| --- | --- | --- | --- |
| W32 initialization, dispatch, thunks, task managers and Winsock | selected original WOW32 source units | target-local public USER/GDI/registry/ABI calls | Retain the original bodies and their explicit target bindings.  A binding may translate a live HWND, native handle or bounded guest lease, but cannot implement a second WOW dispatcher or task scheduler. |
| Recursive callback frames | original `wcall16`/`wcall32` flow and CCPU simulation | `wow_callback_frame_lease` with numeric CPU40 address/lease operations | Retain.  Original code owns frame layout, stack transfer, return and cancellation; the binding only prevents a raw session pointer from surviving the recursive native call. |
| USER object/window/menu/control views | selected original WOW32 client bodies | page-domain/object-table and live-HWND conversion | Retain provisionally as a source-shaped modern USER view.  Native USER owns actual HWND/resource state; the view can publish/retire only explicit project-owned guest aliases and must never turn a modern token into an NT4 private pointer. |
| Task termination | original WOW `wkman` task result path | `W32TerminateTaskTicket` parent ABI and task ticket binding | Retain.  The ticket is a NTSRV/worker lifecycle identity, not an alternate task manager; normal task scheduling/termination policy remains original. |
| Async Winsock | selected original Winsock units | task serial/cancellation identity bridge | Retain.  It supplies a bounded current-task identity at completion; it cannot add a socket scheduler or independent completion queue. |

## Explicit exclusions

Many files in `src/wow32-dll` document staged or unselected recovery material.
Their presence is not an admission to the product link and they are not
silently reintroduced merely to make this inventory appear more complete.
Conversely, no selected target binding is removed merely because its source
looks non-original: its removal must prove that the original caller has a
complete modern service and no raw identity/lease crosses its boundary.

The main unresolved risk is functional, not source presence: full USER,
message, control, dialog, menu and callback lifecycle proof remains the next
runtime package stage.  Importing NT4 private USER or kernel task structures
would be a prohibited way to hide that work.  The S2 result is therefore a
clean ownership boundary, not a false claim of WOW32 behavioral closure.

## Result

No duplicate DLL/worker state provider or missing selected original WOW32
source was found.  No source deletion is justified in this group.  S3 must
take the still-partial runtime owners one coherent cohort at a time, using the
same parent-DLL ABI rather than expanding the worker dependency graph.
