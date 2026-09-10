# M0 T396 S35 — private USER class-registration audit

## Question

Can the first reached original WOW32 class-registration call be recovered
without recreating a private NT4 USER/CSRSS/Win32k service?

## Contract

The exact output-table field in
`src/opennt-abi/source/public/internal/windows/inc/wowuserp.h` is:

```c
typedef ATOM (WINAPI *PFNREGISTERCLASSWOWA)(
    PVOID lpWndClass, LPDWORD pdwWOWstuff);
```

Original `src/mvdm-host/wow32/wuclass.c:543` prepares a native `WNDCLASS` and
a `WC` record carrying the 16-bit window-procedure address, then invokes that
field at line 628.  It deliberately tags the procedure with `WNDPROC_WOW`.
This is not the public `RegisterClassA(WNDCLASSA *)` contract: its second
parameter establishes per-class WOW state used after registration.

The resulting state has material downstream consumers:

- `walias.c` obtains server-owned `PWC` through
  `pfnGetClassWOWWords`, and reads window state through `GCL_WOWWORDS` and
  `GWL_WOWWORDS`.
- `wuwind.c`, `wmdisp32.c`, and `wmsg16.c` use that state to retain the
  original 16-bit procedure and choose the message thunk.
- `wowuserp.h` defines the same field family as the private
  `PFNWOWHANDLERSOUT` table returned by `UserRegisterWowHandlers`.

The S34 fixed-stage observation maps the live call to `WU32RegisterClass` and
then into modern USER/Win32u.  It does not return within the fixed 30-second
bound, but it also produces neither an exception nor a null-call fault.

## Source-recovery ledger

| Rung | Disposition |
| --- | --- |
| Original source reuse | Rejected for product composition. The original owner is NT4 USER/Win32k/CSRSS server state, which source policy names as an explicit stopping boundary. The repository retains its declarations and the original MVDM client, but not a composable USER server package. |
| Smallest same-shaped adapter | Not yet implementable. A facade must preserve the `PWC`/`PWW` private storage, `GCL_WOW*`/`GWL_WOW*` retrieval, `WNDPROC_WOW` dispatch, and subsequent message-thunk lifecycle; a one-call public `RegisterClassA` wrapper cannot do so. |
| External-code intrusion | Rejected. No approved external runtime owner supplies the NT4 USER server contract, and the source policy prohibits substituting a comparison project or importing USER/Win32k/CSRSS. |
| Newly authored behavior | Rejected in this packet. A new class/window/message service would be a broad product-defined UI policy and private-service recreation, precisely the S35 stop condition. |

## Result and follow-up

S35 proves the field signature, its immediate source consumer and its
multi-file lifetime dependency.  The correct successor is the already queued
**WOW16 service, UI and host integration activation** package, which must
first choose a bounded UI surface and define how original WOW class/window
state is represented without claiming a private USER-server reimplementation.

No CPU, DPMI, BOP, guest-media, app-routing or generic USER behavior is
changed by this audit.  The observed live wait is not a Win16 lifecycle pass;
the recovered end-to-end frontier remains original
`MS_bop_1 → W32Dispatch → WU32RegisterClass`.
