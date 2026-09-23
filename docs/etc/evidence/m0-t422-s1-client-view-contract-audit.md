# T422 S1 WOW32 client-view contract audit

## Question

What exact original guest-visible USER client view is required before the
immutable PMODE32 `USER.EXE` can run, and which part is actually missing from
the current worker?

## Inputs

- Hash-pinned `O:/winnt/system32/USER.EXE` profile and its 47 instruction
  checks in `verify-wow-user-profile.ps1`.
- Current `mvdm_softpc_wow_page_domain.c`, the original `wuman.c` caller, and
  the WOW32 registration/object bindings.
- Original `windows/core/ntuser/kernel/desktop.c::{GetDesktopView,
  _MapDesktopObject,MapDesktop}` and `inc/user.h::CLIENTINFO`, inspected as
  source of layout/order only.

## Observations

The current worker correctly reserves the immutable client ABI prerequisites:

| Required guest-visible item | Current state |
| --- | --- |
| `SHAREDINFO` and `SERVERINFO` | guest-linear allocation; `WU32NotifyWow` publishes that guest address |
| `HANDLEENTRY` | 65,536 entries × 16 bytes, with the original free-list form |
| TEB client offsets | self `+18h`, desktop `+5Ch`, delta `+60h`, cached HWND/PWND `+6Ch/+70h` |
| fixed time page | separate mapped source already present |
| `CallCsrFlag` | separate writable guest byte already present |

However, the required desktop and window-object publication has **zero
production callers**.  The only `set_client_desktop` and clear operations are
their page-domain definitions.  `pfnGetFullUserHandle` is deliberately an
`ERROR_CALL_NOT_IMPLEMENTED` placeholder.  `wow_user_window_publish` rejects
active guest publication rather than writing a native cleanup pointer into
guest `HANDLEENTRY.phead`.

That refusal is correct for the present incomplete state.  A native cleanup
record is not the original 176-byte guest WND layout, and its address is not a
guest-linear client view.  Calling it a success would cause `HMValidateHandle`
to interpret an incompatible address as WND state.

The pinned binary's first post-notification `GetDesktopWindow` follows the
original chain `FS:18 -> TEB+5C -> DESKTOPINFO+8 -> subtract TEB+60 -> WND`.
With the current zero desktop pair it reads a nonzero word from the mapped DOS
IVT as if it were a WND pointer; the prior captured page fault is therefore a
missing publication fault, not a guest or CCPU instruction fault.

The current-source S1 recheck again emitted
`WOW32_CLIENT_VIEW_GAP_CONFIRMED`, `desktop producer calls: 0`, and
`active incomplete HANDLEENTRY publication: explicit failure`.  This is
current negative evidence, not merely inherited T420 history.

## Original boundary and design conclusion

NT4's `MapDesktop` maps the desktop section into a process, records a
`DESKTOPVIEW`, and publishes the client-relative `pDeskInfo` plus
`ulClientDelta`; `_MapDesktopObject` derives each client pointer using that
same delta.  Its actual implementation depends on NT object manager, section
mapping and the USER server, so it cannot be compiled wholesale into the
standalone worker without crossing the explicit USER-server stopping boundary.

The delta is not an optional bookkeeping value.  Original
`client.c::DispatchClientMessage` asserts that it is nonzero, and immutable
`USER.EXE` subtracts it from desktop and handle-table server-view pointers.
The worker transport now rejects a zero delta before touching TEB state.  S2
must therefore allocate one controlled guest client view and publish the
corresponding nonzero server-view translation; it may not use direct guest
pointers as disguised server pointers.

S2 must therefore retain the original ordering/layout contract but bind it to
the one existing worker and public USER32 objects:

1. establish one source-shaped guest client allocation containing a real
   client `DESKTOPINFO` and guest WND representations—not host pointers;
2. publish the desktop pointer/delta together only after the desktop WND and
   its typed `HANDLEENTRY` are valid;
3. use that same publisher for create, mutation, destruction, task switch and
   worker teardown; invalidate cached HWND/PWND before change and withdraw
   before releasing backing;
4. keep native object companions solely for public USER32 operations; never
   make them the guest object plane or invent a second USER server.

This is a finite S2 implementation boundary, not a reason to bypass the
optimized USER16 reads, alter guest bytes, or add an independent window
manager.

## Translation invariant required for S2

The original `MapDesktop` source fixes the direction of the relation.  Its
server mapping is `pdesk->hheapDesktop`; its client mapping is `pClientBase`;
and it stores `ulClientDelta = serverBase - clientBase`.  Original
`_MapDesktopObject` returns `serverObject - ulClientDelta` to its client.
Consequently the standalone binding must use the following *numeric* relation
for each desktop-heap object which original client code rebases:

```
client-linear C  + nonzero delta D  = server-view number S
guest consumer: S - D = C
```

`C` is an address in the one CCPU-visible client allocation.  `S` is only the
server-view number carried by a rebased desktop-heap field such as
`DESKTOPINFO.spwnd`, `HANDLEENTRY.phead` or a WND cross-reference; it is
**not** a native pointer and no worker code may dereference it.  The worker
reaches `C` only through its bounded guest-memory lease.  Any native USER32
companion remains an opaque local association keyed by the typed guest handle,
never an input to this arithmetic.

`SetDesktop` makes one important distinction: it publishes
`CLIENTINFO.pDeskInfo` itself as the client form `C` (`serverDeskInfo - D`) and
publishes `CLIENTINFO.ulClientDelta = D`.  S2 must retain that distinction:
the TEB's `pDeskInfo` is a checked client-linear pointer, while only the
originally rebased fields *within* that desktop view use `S`.  Treating the
TEB field itself as `S` would make the first `pDeskInfo->spwnd` access start
from the wrong address.

S2 must choose one nonzero `D` per active worker view, prove that each chosen
`C + D` does not wrap 32 bits, and prove `S - D == C` for every field the
original client consumer rebases.  It must also prove that publication rejects
an out-of-range relation before it changes the TEB or a typed handle.
The value is deliberately not fixed by S1: `0x84000000` in earlier fixtures
is a test value, not evidence that it is a safe production mapping.  This
finite numeric translation preserves the original client/server view contract
without importing the NT object-manager section mapping or exposing a host
address to immutable USER.EXE.

## Reproduction

```powershell
& .\tests\observation\verify-wow32-client-view-gap.ps1 `
  -RepositoryRoot (Get-Location).Path
```

Expected output is `WOW32_CLIENT_VIEW_GAP_CONFIRMED`.  This is a passing
negative audit: it proves the known gap remains explicit rather than falsely
claiming the current allocation is a functional desktop view.

## Receiver

S2 owns `USER-VIEW-01`, `USER-HANDLE-01`, `USER-DATA-01`, and the directly
dependent task/window/callback lifecycle.  S4's menu direct-data rows consume
the resulting handle/publication contract and cannot close before S2 does.
