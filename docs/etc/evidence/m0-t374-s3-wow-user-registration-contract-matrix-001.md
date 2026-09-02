# M0 T374 S3 — Original WOW USER registration-contract matrix

## Scope and method

This is a complete audit of `PFNWOWHANDLERSOUT` in the original
`opennt-abi/.../wowuserp.h:119-143`.  The structure contains **20 callback
fields plus `dwBldInfo` metadata**.  Original
`windows/core/ntuser/client/client.c:2256-2331` stores every incoming WOW
callback, populates each output field, then returns the private `gSharedInfo`
pointer.  A repository-wide current-adapter search found no implementation of
any output field or target; this is a current-source observation, not an
assumption that a public API is absent.

## Complete output matrix

| Field | Original target assigned by `client.c` | Original implementation family | Current source disposition |
| --- | --- | --- | --- |
| `dwBldInfo` | `(WINVER << 16)` plus debug bit | private USER build/shared-info contract | overlay-required; it cannot be substituted for returned `gSharedInfo` identity. |
| `pfnCsCreateWindowEx` | `_CreateWindowEx` | NTUSER client `ntstubs.c` | overlay-required private window creation contract. |
| `pfnDirectedYield` | `DirectedYield` | NTUSER kernel `taskman.c` | overlay-required task scheduling contract. |
| `pfnFreeDDEData` | `FreeDDEData` | NTUSER DDE client `hdata.c` family | overlay-required DDE object lifetime contract. |
| `pfnGetClassWOWWords` | `GetClassWOWWords` | NTUSER client `ntstubs.c` | overlay-required class-private-data contract. |
| `pfnInitTask` | `InitTask` | NTUSER kernel `queue.c` | overlay-required task/window registration contract. |
| `pfnRegisterClassWOWA` | `RegisterClassWOWA` | NTUSER client registration path | overlay-required WOW class/layout contract. |
| `pfnRegisterUserHungAppHandlers` | `RegisterUserHungAppHandlers` | NTUSER client registration path | overlay-required hung-app notification contract. |
| `pfnServerCreateDialog` | `InternalCreateDialog` | NTUSER client `clres.c`/dialog family | adapter-backed candidate; public dialog APIs cannot be selected until the original callback/identity sequence is retained. |
| `pfnServerLoadCreateCursorIcon` | `WowServerLoadCreateCursorIcon` | NTUSER client `clres.c` | adapter-backed candidate; resource and handle identity need the complete contract. |
| `pfnServerLoadCreateMenu` | `WowServerLoadCreateMenu` | NTUSER client `clres.c` | adapter-backed candidate; resource and menu identity need the complete contract. |
| `pfnWOWCleanup` | `NtUserWOWCleanup` | NTUSER kernel `ntstubs.c` | overlay-required task/window cleanup contract. |
| `pfnWOWFindWindow` | `WOWFindWindow` | NTUSER client `ntstubs.c` | overlay-required WOW class/window lookup contract. |
| `pfnWOWGetIdFromDirectory` | `WOWGetIdFromDirectory` | NTUSER client `rtlres.c` | adapter-backed candidate; source-defined resource-directory parsing can remain original only with bounded input identity. |
| `pfnWOWLoadBitmapA` | `WOWLoadBitmapA` | NTUSER client `clres.c` | adapter-backed candidate; resource/bitmap identity requires the complete contract. |
| `pfnWowWaitForMsgAndEvent` | `NtUserWaitForMsgAndEvent` | NTUSER kernel `ntstubs.c` | overlay-required USER queue/event contract. |
| `pfnYieldTask` | `NtUserYieldTask` | NTUSER kernel `ntstubs.c` | overlay-required cooperative task-yield contract. |
| `pfnGetFullUserHandle` | `GetFullUserHandle` | NTUSER client `handles.c` | overlay-required USER handle expansion contract. |
| `pfnGetMenuIndex` | `NtUserGetMenuIndex` | NTUSER kernel `ntstubs.c` | overlay-required menu-private-data contract. |
| `pfnWowGetDefWindowProcBits` | `WowGetDefWindowProcBits` | NTUSER client shared-info path | overlay-required default-procedure bitmap contract. |
| `pfnFillWindow` | `FillWindow` | NTUSER client dialog/control family | adapter-backed candidate; public drawing is insufficient without original dialog/window state. |

## Decision

The 20 callback outputs cannot be split into independent “easy API” work:
all are returned in one `PFNWOWHANDLERSOUT` object, and `W32Init` exposes that
object to the full original WOW32 provider product before `W32Dispatch`.
Public dialog, resource, cursor, menu, bitmap, resource-directory and drawing
APIs may be capability backends for seven named rows, but none is authorized
to stand in for the private table or its object/task identity.

The smallest next implementation cohort is therefore a **same-shaped USER
registration overlay** that preserves the original 20-callback table,
metadata/result ordering, incoming callback capture and returned shared-info
contract.  It must supply either a complete source-shaped behavior for every
row or an original-source-defined unavailable result before `W32Init` reports
success.  A partial table, null callback, generated “success” `gSharedInfo`,
or static `W32Dispatch` activation is rejected.

The Base `RegisterWowBaseHandlers` lifetime remains coupled but separate: its
original storage and original `GlobalFree` consumption must be imported as one
Base-client source slice.  It is not an alternative implementation of this
USER registration cohort.

## Mapping-manager implications

No output callback transports a guest pointer by casting it to a host pointer.
Guest frames remain in the existing bounded lease path.  User/GDI/DDE/window
objects are host identities and require the session host-resource mapping
instance if an original 16/32-bit field represents them.  This matrix creates
no additional mapping manager.

## Follow-up

S4 may implement only the full selected registration overlay and the coupled
Base hook lifetime, with every original source divergence registered.  It may
not select a service leaf, stage WOW media or run a product observation.
