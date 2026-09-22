# T422 S1 WOW32 direct-data owner matrix

## Question

Which immutable PMODE32 USER16 direct-memory reads already have a genuine
worker producer, which currently have only allocated zeroed storage, and what
exact publication/update/withdrawal responsibility must be closed before a
WOW32 route can be called restored?

## Inputs and procedure

The read-side ABI is hash-pinned by
`tests/observation/verify-wow-user-profile.ps1`.  It verifies 49 instruction
patterns in the immutable `USER.EXE` and emits the corresponding offsets.
The write side was inspected in the current worker page domain,
`wuman.c::WU32NotifyWow`, the target-local registration/object bindings and
the original `ntuser/kernel/desktop.c` mapping/publishing routines.  Searches
for the worker publication APIs distinguish production callers from the one
focused fixture.

No guest byte is modified by this audit.

## Current publication matrix

| Guest-visible field group | Immutable consumers | Current authoritative producer and state | Required update / withdrawal | Receiver |
| --- | --- | --- | --- | --- |
| `SHAREDINFO.psi` and `aheList` | `HMValidateHandle`; original `WU32NotifyWow` receiver | Worker page domain allocates guest-linear storage and `WU32NotifyWow` returns its address. The two pointers are initialized once. | Remain valid for the active domain; clear only while disposing the whole domain after all consumers stop. | S2 validates live objects. |
| `SERVERINFO.cHandleEntries` | `HMValidateHandle` | Worker initializes 65,536 entries. | The count is fixed for this profile; backing must outlive all typed entries. | S2. |
| `SERVERINFO` metrics, colors and cursor coordinates | `GetSystemMetrics`, `GetSysColor`, `GetCursorPos` | The page exists but these fields have no production source or refresh. Zeroed storage is not a valid producer. | Source-shaped initialization before first direct read; update on required settings/cursor changes without relying on a later WOW thunk; withdraw with the domain. | S2, with S3/S7 consumers where applicable. |
| `HANDLEENTRY` `phead/pOwner/type/flags/wUniq` | `HMValidateHandle`, `IsWindow`, every WND-based direct reader | Worker creates only the original free-list form. `wow_user_window_publish` deliberately refuses active guest publication because it has no valid guest WND. | Publish full guest WND/owner before setting typed state; invalidate/reclaim before backing reuse; reject stale uniqueness. | S2 `USER-HANDLE-01`. |
| TEB self pointer | FS-based direct reads | Worker publishes guest TEB self at `+18h`. | Constant through the active worker domain; remove with its descriptor/backing. | S2 verifies selector/task switching. |
| TEB `pDeskInfo`, `ulClientDelta`, cached HWND/PWND | `GetDesktopWindow`, `ValidateHwnd` and all WND graph readers | Transport has set/clear routines, but **zero production callers**. The current runtime therefore exposes zero desktop and delta. | Atomically invalidate caches and publish the pair after a valid desktop WND exists; republish on task/desktop switch; clear before releasing backing. | S2 `USER-VIEW-01`. |
| TEB expected Windows version | Version-correcting `GetSystemMetrics` direct path | Storage exists; no source-proven live task producer is wired. | Publish from the original task/version owner before version-sensitive direct reads; retire with task context. | S2 task initialization. |
| `DESKTOPINFO` and desktop WND | `GetDesktopWindow`, `GetTopWindow`, `GetWindow`, visibility/ancestry paths | No guest `DESKTOPINFO` or 176-byte WND producer exists. | Allocate source-pinned guest representation; publish only after desktop HANDLEENTRY is typed; keep relationships valid across callbacks; withdraw before storage release. | S2. |
| Guest WND / CLS graph | Class name, rectangles, parent/child/owner, style/state and window queries | No guest layout producer; native cleanup/WW records are explicitly not interchangeable with a guest WND. | One logical native/guest object lifecycle must update WND, CLS and relationships at create/mutate/destroy/reuse. | S2. |
| Guest MENU / MENUITEM graph | `GetMenu*`, `GetSubMenu`, `GetMenuState` | No guest MENU graph producer. | S2 must first provide valid handle/publication lifetime; then S4 publishes/mutates/releases menu records and owner-draw data. | S4. |
| Fixed clock page `0x7ffe0000` | `GetTickCount`, `GetCurrentTime` | Page domain maps a host page and updates tick/multiplier every 10 ms. | Keep mapping valid only while paging/domain is selected; stop publisher and unmap during worker teardown. S2 must test required freshness against immutable reads. | S2 X02. |
| `CallCsrFlag` byte | original USER call-server-condition macros | `WU32NotifyWow` supplies a dedicated writable guest byte. | Starts zero; guest may set/clear it; release with client allocation. This is not a substitute for any other shared-data field. | S2 callback/task paths. |

## Original-order constraint

`desktop.c::MapDesktop` establishes a process desktop view and its relocation
delta.  `SetDesktop` then publishes the client-relative `pDeskInfo` and the
same delta into the thread CLIENTINFO.  `_MapDesktopObject` derives client
pointers through that delta.  The standalone worker cannot import that
NT-object/section-mapping implementation wholesale, but S2 must preserve its
observable order:

1. create a source-pinned guest desktop/WND/handle representation;
2. publish the typed desktop object and relationship graph;
3. clear TEB cache fields, then publish `pDeskInfo` and `ulClientDelta` as
   one current-thread context;
4. allow direct reads and callbacks; and
5. clear the TEB pair and retire typed objects before releasing guest backing.

The inverse order would recreate the proven `GetDesktopWindow` fault: a zero
desktop pair lets the immutable code dereference mapped DOS IVT content as a
WND.  A native HWND, cleanup record, host TEB or arbitrary host pointer is not
an admissible replacement for any guest-linear record.

## Test allocation

S2 owns the coupled `USER-VIEW-01`, `USER-HANDLE-01`, `USER-DATA-01` and
`USER-CALLBACK-01` tests.  It must exercise bootstrap, create/query, direct
read after host-originated mutation, callback-time destruction, stale/reused
handle rejection, task switch and worker exit.  S4 cannot close menu rows
until the S2 publisher is proven.  S8 repeats all three routes (direct data,
ordinary thunk and callback) on the final WRITE/WINMINE/SOL artifact set.

## Reproduction

```powershell
& .\tests\observation\verify-wow-user-profile.ps1 `
    -RepositoryRoot (Get-Location).Path
& .\tests\observation\verify-wow32-client-view-gap.ps1 `
    -RepositoryRoot (Get-Location).Path
rg -n 'mvdm_softpc_wow_page_domain_(set|clear)_client_desktop' src tests
```

Expected audit markers are `WOW_USER_PROFILE_SOURCE_AND_BINARY_OK` and
`WOW32_CLIENT_VIEW_GAP_CONFIRMED`; the latter is a deliberately passing
negative witness, not a runtime capability pass.
