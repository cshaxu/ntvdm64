# M0 T397 S1 — WOW16 UI/service owner-package audit

## Question

Which parts of the complete admitted WOW32 provider can move through public
Win32 independently, and which require the NT4-private USER server contract
that blocks the first live Win16 UI call?

## Complete provider scope

T396's selected provider remains the complete 77-body, eight-subdomain
original cohort. T397 does not reduce it to the first trace hit: 12
thunk/frame bodies, 10 message/window-procedure bodies, 13 GDI/window-object
bodies, 9 kernel/task/DOS-session bodies, 17 USER-interface bodies, 5
shell/OLE/printing bodies, 8 multimedia bodies, and 3 network bodies. The six
C bodies excluded by `sources` remain excluded.

## USER output-table ledger

`W32Init` calls `UserRegisterWowHandlers(&pfnIn, &pfnOut)`. The exact
20-function `PFNWOWHANDLERSOUT` table is consumed as follows:

| Output field | Direct original consumer(s) | Contract disposition |
| --- | --- | --- |
| `pfnCsCreateWindowEx` | `wuwind.c` | Private class/window creation; carries WOW window state and callback dispatch. |
| `pfnDirectedYield`, `pfnYieldTask`, `pfnWowWaitForMsgAndEvent` | `wkgthunk.c`, `wkman.c` | Private USER task scheduling/message wait; not a generic host wait. |
| `pfnFreeDDEData` | `wumsg.c` | Private DDE ownership/cleanup. |
| `pfnGetClassWOWWords`, `pfnRegisterClassWOWA` | `walias.c`, `wuclass.c` | Private `PWC` class storage plus 16-bit procedure identity. |
| `pfnInitTask`, `pfnWOWCleanup` | `wkman.c`, `wuser.c` | Crosses USER task/server lifecycle. |
| `pfnRegisterUserHungAppHandlers` | `wkman.c` | Existing narrow no-broker registration direction only; no callback producer. |
| `pfnServerCreateDialog` | `wudlg.c` | Server-created dialog plus WOW window-procedure callback. |
| `pfnServerLoadCreateCursorIcon`, `pfnServerLoadCreateMenu`, `pfnWOWLoadBitmapA`, `pfnWOWGetIdFromDirectory` | `wucursor.c`, `wumenu.c`, `wuser.c` | UI-resource services coupled to private handle/class state. |
| `pfnWOWFindWindow`, `pfnGetFullUserHandle`, `pfnGetMenuIndex` | `wuman.c`, `walias.h`, `wmsg16.c` | Private 16/32 handle and class/menu identity mapping. |
| `pfnWowGetDefWindowProcBits`, `pfnFillWindow` | `wuman.c`, `wuser.c` | Private thunk metadata and window-state fill contract. |

The table is not a set of independent public API aliases. After
`RegisterClassWOWA`, original `walias.c` reads `GCL_WOWWORDS`/`GWL_WOWWORDS`;
`wmdisp32.c`, `wmsg16.c`, and `wuwind.c` then use those values to select and
invoke 16-bit window-procedure/message thunks.

## Recovery ladder

| Rung | Result |
| --- | --- |
| Original source | The WOW32 client cohort is fully composed. The corresponding USER/Win32k/CSRSS server is deliberately not: source policy prohibits recursive composition. |
| Same-shaped adapter | Possible only for a bounded, complete replacement of one retained client contract, including state ownership, procedure callback, message conversion, teardown and failure behavior. A single public Win32 call is insufficient. |
| External intrusion | Not admitted. No external project is a product dependency or supplies the original private server contract. |
| New behavior | Requires a registered public-Win32 WOW-window projection exception with explicit session-owned class/window/handle state and callback bridge. |

## Result and follow-up

Shell/OLE/printing, multimedia, and network bodies remain candidates for
separate source-first audits, but cannot bypass the class/window/message
contract or prove a Win16 lifecycle. S1 selects a bounded session-owned
public-Win32 WOW-window projection design audit for S2. It must define a fixed
ABI, ownership, callback/message ordering, teardown, public mapping, negative
behavior, and focused acceptance while excluding a generic USER/CSRSS
replacement.
