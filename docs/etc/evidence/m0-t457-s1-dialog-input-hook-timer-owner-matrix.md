# M0 T457 S1 — dialog, input, hook and timer owner matrix

## Result

S1 closes the admission ledger for W06.  The formal x86 WOW32 graph directly
selects the retained original providers `wudlg.c`, `wumsg.c`, `wucaret.c`,
`wutmr.c`, `wuhook.c`, and `wowhooks.c`; it does not select a project-created
dialog manager, input router, timer engine, or hook registry.  The remaining
work is a finite runtime acceptance/repair question, not a reason to duplicate
those original owners.

The recorded graph is `build/.ninja/msvc-x86-release/wow32/build.ninja`:
`wucaret` at line 64, `wudlg` at 69, `wuhook` at 71, `wumsg` at 74, `wutmr` at
78, and `wowhooks` at 81.

## Authority matrix

| Family and selected entry | Original behavior owner | Modern resource owner | Adapter boundary | Normal, nested, and retirement rule | W06 disposition |
| --- | --- | --- | --- | --- | --- |
| Dialog creation/end/default control processing: `WU32DialogBoxParam`, `WU32EndDialog`, `WU32IsDialogMessage`, `DefDlgProcWorker` in `wudlg.c` | Original dialog template conversion, `DLGDATA`, callback procedure and dialog/control ordering | HWND, messages, native dialog window and focus | Existing T447 dialog context/finalization/call bindings only | The original path owns converted template memory and callback order; T447's bound finalizer owns the one retirement edge | **Transferred T447 behavior.** S2 may exercise it, but must not make a second dialog implementation. |
| General message conversion: `WU32GetMessage`, `WU32PeekMessage`, `WU32TranslateMessage`, `WU32CallMsgFilter` in `wumsg.c` | Original message conversion, DDE packing and callback sequencing | Native queue and USER dispatch | Existing call-owned message lease/finalizer | `MSGPARAMEX` is call-owned and unwound in the original caller's `__finally`; nested conversion may not escape it | Retained T447 execution foundation; S2 consumes it for keyboard/caret evidence only. |
| Caret: `WU32CreateCaret`, `DestroyCaret`, `Get/SetCaretPos`, `Hide/ShowCaret`, blink getters/setters in `wucaret.c` | Original 16→32 handle/argument conversion | Native USER owns the shared system caret, shape, owner and hide count | `wow_user_private_access.h` maps only to public native calls; no project caret state exists | The original caller invokes native USER directly.  A task may never retain a copied caret identity; focus/window retirement remains native USER's responsibility | **W06 S2.** Verify actual guest caret/input behavior and repair only a source-shaped call boundary if one is observed. |
| Timers: `WU32SetTimer`, `WU32KillTimer`, `WowOriginalTimerFunc`, `DestroyTimers16` in `wutmr.c` | Original `TimerList`, 27 fixed callback slots, 55 ms clamp, Excel null-HWND matching, cancelled-but-queued callback handling, and `CallBack16` conversion | Native USER timer queue | `wow_timer_callback.c` enters existing task/borrow scopes then calls `WowOriginalTimerFunc` unchanged | `KillTimer` removes an active slot but preserves the linked record for a queued callback. `WowOriginalFreeTask` calls `DestroyTimers16` before hook/resource cleanup | **W06 S3.** No scheduler replacement is admissible; exercise set/kill/reentry/task-loss against this owner. |
| Hook install/remove/next: `WU32SetWindowsHookInternal`, `WU32SetWindowsHookEx`, `WU32Unhook*`, `WU32CallNextHookEx` in `wuhook.c` | Original `HOOKSTATEDATA`, 16-bit procedure/module identity, legacy return-handle convention and `W32GetThunkHookProc` conversion | Native USER hook registration and chain continuation | `wow_hook_native_binding.c` has only the same-process module/null-module compatibility seam | Original source decides global/process/thread scope and persists its hook record. A native binding failure leaves `InUse` false. `W32FreeOwnedHooks` retires the task's records | **W06 S3.** Complete real registration/removal/task-loss evidence without another hook registry. |
| Native hook callback: all 32 stubs in `wowhooks.c` → `WU32StdHookProc` | Original hook-code-specific conversion, guest-stack allocation and `DefHookProc` behavior | Native USER's invocation/chaining | `wow_hook_callback.c` supplies task/borrow scope, enrolled HWND roots and restores `vHookParams`, current-index, DDE and origin state around the original call | Creation is enrolled before the original callback and committed only after non-veto completion; all saved original globals are restored on nested/exception exits | Existing T447 callback-entry foundation is retained; S3 only proves the complete registration/removal routes use it. |

## Source-first ladder

1. **Direct composition:** all six original providers above are in the formal
   production graph.  `wutmr.c` retains both its table and callback bodies;
   `wowhooks.c` retains the 32 native callback stubs and original converter.
2. **Smallest same-shaped seams:** `W32TimerFunc` is a scoped entry wrapper
   around `WowOriginalTimerFunc`; `WU32StdHookProc` is a scoped wrapper around
   `WowOriginalStdHookProc`; caret calls map to public USER APIs.  None owns a
   duplicate timer, hook, caret, dialog, or message state.
3. **External intrusion:** none selected.
4. **New behavior:** none admitted.

The native hook module adjustment is documented in source as
`ADAPTER-WOW-090`: it changes only the native registration argument required
for retained original fixed stubs.  It neither determines hook type nor
creates a callback table.

## Callback and task-loss order

`wkman.c::WowOriginalFreeTask` runs `DestroyTimers16(ptd->htask16)` before
`W32FreeOwnedHooks(ptd->htask16)`.  That is the selected original task-loss
order.  `wow_task_termination_retire` only reports the already-retired task to
NTSRV; it is not a second USER cleanup path.  The owner sequence therefore is:

```text
native USER callback → scoped adapter entry → retained original converter
→ guest callback → original/finally cleanup → task retirement
→ original DestroyTimers16 → original W32FreeOwnedHooks → NTSRV notification
```

Failure before a scoped entry is native USER failure.  Failure during a scoped
entry leaves through the adapter's `__finally`, restoring borrow/task/context;
the original provider remains responsible for its own conversion allocation
and return value.

## Focused evidence

| Check | Artifact | Result | Scope limitation |
| --- | --- | --- | --- |
| Native hook registration argument/handle boundary | `build/t457-s1/hook/manual.stdout.txt` | `WOW_HOOK_NATIVE_BOUNDARY errors=0 calls=24 ... handles=76/76` | This is a public-native seam fixture; it does not claim real guest hook execution. |
| Original timer callback nested/exception/denied-entry guard | `build/t457-s1/timer/manual.stdout.txt` | `WOW_TIMER_CALLBACK_GUARD errors=0 calls=30 nested=1 exception=1 denied=2` | It proves the retained callback entry contract, not full guest application acceptance. |
| Input group lifetime fixture | `build/t457-s1/input/membership.stderr.txt` | **Non-pass before WOW32 behavior:** `INPUT_NATIVE_CONTROL own_Z=0 before_group=1` | The fixture requires the current desktop to own native keyboard state for `Z`; it did not reach group attachment/WOW logic.  This is an environment precondition, not a product pass or a source defect. |

## Finite S2 admission

S2 owns only actual dialog/input/caret acceptance at the selected original
entries.  It must prove a real Win16 dialog/control callback, keyboard-driven
dialog processing, caret creation/motion/show/hide and normal/cancel task
retirement.  A failure may add only the smallest source-shaped adapter seam at
the exact observed original call boundary.  It may not alter guest media,
replace the retained dialog manager/message loop, or preempt S3's hook/timer
registration and task-loss work.

