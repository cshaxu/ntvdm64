# T422 S1 WOW32 direct-data mutation contract

## Question

What must make each immutable PMODE32 USER direct read valid, fresh and safe
to withdraw when no WOW32 thunk is executing?

## Inputs

`verify-wow-user-profile.ps1` pins 49 instruction-level consumer checks and
the selected `0x84000000` layout.  The current page domain establishes the
actual existing writes.  Original `desktop.c`, `user.h`, handle/client source
and selected WOW32 callers establish relationship/order evidence.

## Contract matrix

| Direct object / immutable reads | Existing state | Required authoritative producer and mutation trigger | Atomic publication / withdrawal rule | S owner and test |
| --- | --- | --- | --- | --- |
| `SHAREDINFO.psi/aheList`, `SERVERINFO.cHandleEntries` | Already guest-linear and initialized once. | Page domain on worker bootstrap. Count is profile-fixed. | Do not expose typed entries until their backing WND/CLS/owner exists; keep allocation to final withdrawal. | S2: first direct `HMValidateHandle`, invalid index/type/uniqueness. |
| `HANDLEENTRY` (`phead,pOwner,type,flags,wUniq`) | Free-list only; active publish rejects correctly. | One S2 guest-object producer owns allocation, uniqueness increment, type and native association. | Write complete guest object first, then `phead/owner`, then type/flags/uniqueness; on retirement clear typed state before reuse. | S2: stale/reused handle, callback destruction, failure before type publication. |
| TEB `pDeskInfo`, `ulClientDelta`, cached HWND/PWND | Storage exists, zero production caller. | B1 bootstrap and later selected desktop/thread switch. | Clear both caches, write delta and desktop as one suspended-CCPU context; clear cache/pair before backing release. | S2: first `GetDesktopWindow`, direct reads after switch and withdrawal. |
| `DESKTOPINFO` and root desktop WND | No guest producer. | S2 B1, before `WU32NotifyWow` returns. Root WND owns its self/parent/child/owner relationship and typed handle. | Build complete WND/DESKTOPINFO/handle graph before TEB publication. | S2: root window, top/parent/child and negative partial-bootstrap test. |
| Non-desktop guest WND/CLS graph | No guest producer. Native property records exist but are not guest objects. | S2 create/change/destroy path, paired to the native window operation and original WOW callback order. | Creation callback sees complete graph; mutation updates all direct fields before guest reentry; destruction invalidates handle before releasing backing. | S2: class, rect, style, visibility, enabled/iconic/zoomed, callback reentry. |
| `SERVERINFO` metrics/colors/cursor | Allocated but zeroed. | S2 initializes from selected public system query at B1; a native input/settings notification or explicitly bounded synchronous query publishes later changes. | A direct USER read cannot rely solely on last BOP entry. All fields in a logical snapshot update under the worker domain. | S2: metric/color/cursor normal and changed-state freshness without a thunk. |
| TEB expected Windows version | Allocated but zeroed. | Original task/version owner during B3 task initialization. | Publish before a version-sensitive guest branch, clear on task context withdrawal. | S2: two task-version cases and cleanup. |
| Shared clock page | Already 10-ms publisher-backed. | Page-domain clock thread. | Start only after mapping; stop/join before unmap. | S2: two direct reads showing monotonic/fresh value and worker loss. |
| Menu/MENUITEM graph | No producer. | S4, after S2 object/handle lifecycle. | Menu typed entry and all item/submenu links exist before guest callback/direct traversal; retire children before parent/reuse. | S4: `GetMenuState/ItemID`, mutation and owner-draw release. |
| CallCsrFlag | Dedicated writable guest byte exists. | Original USER guest condition macros. | Do not alias it with host state; release only with entire client allocation. | S2: callback/task path and worker withdrawal. |

## Consequences

The required publisher is not a periodic host-desktop mirror.  The selected
guest only needs the finite graph reachable through its own WOW task/window
objects plus the source-defined system snapshot fields.  Conversely, an
own-created-windows-only implementation is insufficient unless S2 proves the
original API's externally reachable behavior and failure result.

No native pointer may be copied into a guest record.  A native object is an
authority for the public operation; its guest representation is a separate,
bounded, source-pinned record with explicit lifetime.  The two must be
updated in the original observable order, not treated as aliases.

For rebased desktop-heap pointer fields, publication additionally uses the
source-pinned `S = C + D` / guest `C = S - D` relation from the
[client-view audit](m0-t422-s1-client-view-contract-audit.md#translation-invariant-required-for-s2).
`D` is nonzero and range-checked; `S` is a numeric server-view form, never a
worker-dereferenced native pointer.  `TEB.pDeskInfo` is the required exception:
original `SetDesktop` stores its client form `C`, alongside `TEB.ulClientDelta`.
`DESKTOPINFO.spwnd`, `HANDLEENTRY.phead`, WND links and later fields consumed
through original `REBASE*` macros use `S`; non-rebased scalar and client
pointers retain their original individual forms.

## Status

This settles the field-level production contract for all direct-reader groups;
it does not claim the producer exists.  S2 must implement and prove every row
before the corresponding capability becomes a pass.  The current zero
desktop/WND/metrics producer remains a documented non-pass.
