# T473 S8 — public USER identity and retirement recovery

## Decision

The selected original WOW32 menu and dialog callers retain their algorithms
and their historical `HWND32` carriers. At the one modern-USER boundary they
now resolve an already-published, current-worker full `HWND`; modern USER owns
that resource. The project object table is the sole authority for the
carrier-to-resource projection. It is not a private USER reconstruction.

This corrects a real mixed-identity defect: `USER32`/`HWND32` in the selected
original callers is a zero- or sign-extended 16-bit carrier, while a current
host `HWND` is wider. Passing the carrier to modern USER could target an
unrelated host window with the same low word. Resolving it once, rejecting a
fabricated full value, and passing only the published full handle prevents
that substitution.

## Owner matrix

| Concern | Owner | S8 boundary rule |
| --- | --- | --- |
| Menu operations and ordering | Original `wumenu.c` / `wusercli.c` | `GetMenu`, `GetSystemMenu`, `DrawMenuBar`, `HiliteMenuItem`, `SetMenu` and popup tracking receive their existing original carrier, then use the owned full native handle only at the wrapper boundary. |
| Dialog construction algorithm | Original dialog callers | `wow_dialog_native_create_window` resolves a non-null parent through the same owned mapping before its existing `win32u` call. A null parent remains a native null parent. |
| Actual HWND/menu/dialog lifetime | Modern USER | No modern handle, pointer or token is interpreted as an NT4 private object. |
| Published mapping and retirement | Project public-USER object table | It is a bounded projection only; mapping removal/handle reuse remains subject to the existing ownership and retirement rules. |
| Foreign clipboard/window observation | Existing public-observation path | Deliberately separate: it may observe an external native window. The new *owned* resolver rejects it rather than silently admitting it to an original-owned call. |

## Exact caller trace and rejection rule

`walias.h` defines `HWND32` through the signed 16-bit `USER32` form. The
selected original calls in `wumenu.c` and `wusercli.c` therefore pass that
carrier to the redirected public-USER APIs. `wow_user_window_owned_handle`
accepts either its unsigned short form, its signed short form, or the exact
currently published full handle. It rejects a null input when the native API
does not permit null, an absent current runtime, an unowned low-word mapping,
or a wider value that is not the published full handle. It sets the existing
native failure code instead of inventing a second identity channel.

After `SetMenu` succeeds, the wrapper refreshes the published menu edge using
the exact full handle for both `GetWindowInfo` and `GetMenu`. That preserves
the existing native result and last-error transfer while preventing the former
"resolve one HWND/query another" mismatch. The dialog parent adaptation has
the same one-way shape: original caller -> owned projection -> native service.

## Evidence

The selected production x86 target rebuilt and linked `WOW32.DLL` from all
155 selected sources. The focused `menu-resource` route passed with the
current source: 16 original resource cases, seven parser cases, 64 invalid
repeats, tracking cancellation/reentry/unlock, mutation publication, detached
retirement, native close return, rollback and original thunk entry all report
success. The direct mapping witness reports
`WOW_NATIVE_CLASS_CCPU ... owned_handle=1` and covers short signed/unsigned,
exact full, fabricated full and foreign-observation rejection.

The complete control-boundary cohort built 144 targets and passed all 29
fixtures. It covers dialog creation/context/finalization/end/keyboard and
mnemonic paths, EDIT and button lifetime/drawing resources, controls, default
window behavior, list/scroll/control drawing and native-owner probes. These
are source-level current fixtures, not a claim that every historical USER API
has been exercised in a real guest program.

The complete ten-image component build passed after the version metadata was
advanced to the active T473 identity; its immediate second build was idle.
The final control run also passed its RPC request/reply, native GUI route,
version-negotiation rejection, real modern EDIT/Ctrl+Q, CMD echo, DOS MEM,
frontend-loss and selected-worker retirement routes. Its final direct
completion/input observer timed out while NTSRV, NTCON, NTVWM and its native
child were still alive. This is retained as an **unpassed test observation**;
the user explicitly directed that this verification not block the S8 delivery.
It is not counted as proof of the direct completion/input path.

## Finite continuation boundary

`wow_user_task_lifecycle.c` contains a separate generic public-window wrapper
family (`SetWindowPos`, `SetParent`, messages, default procedures and related
two-HWND calls). It has different null, pseudo-handle, foreign-target and
return-conversion contracts. It is not silently swept into S8 merely because
it shares the word "HWND"; it is the next finite public-USER caller group if
source review establishes a carrier/native mismatch. S8 is complete once its
coherent ten-image delivery is published under the recorded verification
exception; it does not claim that later group.

Guest media is unchanged.
