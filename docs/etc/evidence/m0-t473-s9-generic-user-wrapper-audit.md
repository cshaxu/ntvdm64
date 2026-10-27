# T473 S9 — generic public-USER wrapper identity audit

## First source result

S8 deliberately did not change the generic wrappers in
`wow_user_task_lifecycle.c`: their contracts mix null, pseudo, foreign and
multiple HWND parameters.  The first S9 source sweep proves that a bounded
subcohort nevertheless needs separate review.  Original `wuwind.c` calls
`MoveWindow`, `SetActiveWindow`, `SetParent`, `SetWindowPos` and `ShowWindow`
with `HWND32`/`HWNDIA32`; original `wuser.c` calls `SetFocus` and `SetCapture`
with `HWND32`.  `walias.h` defines both forms as signed 16-bit carriers.

The current `WOW_NATIVE_CALL` macro forwards its `HWND` parameters unchanged
to modern USER.  It therefore cannot distinguish such a carrier from a full
host HWND.  This is the same numerical-width hazard resolved for menu/dialog
operations in S8, but must not be fixed by converting the entire macro family.

## Parameter contract matrix

| Original operation | Required target treatment | Other HWND/result treatment | Reason not to use a blanket resolver |
| --- | --- | --- | --- |
| `MoveWindow`, `ShowWindow`, `SetActiveWindow`, `SetFocus`, `SetCapture` | non-null current-worker owned target | Returned prior HWND is converted only by the existing original return thunk | Calls can synchronously change focus/activation/capture; result lifetime is not the input lifetime. |
| `SetParent` | non-null owned child | parent may be null; non-null parent requires a separately proved owned/public rule; returned old parent has independent conversion | Child and parent have different null and cross-owner meanings. |
| `SetWindowPos` | non-null owned target | `hwndInsertAfter` preserves native pseudo handles (`TOP`, `BOTTOM`, `TOPMOST`, `NOTOPMOST`) and only otherwise follows a proven mapping rule | A pseudo handle is an API instruction, not a guest object identity. |
| Message/default/MDI wrappers | no conclusion yet | target can be null, broadcast, foreign or paired with a client HWND; message payload may encode an HWND | Their receiver and return contracts are not reducible to this subcohort. |

## Required proof before repair

1. Trace every selected call into the wrapper and classify each `HWND`
   parameter as owned, null-permitted, pseudo, external or returned.
2. Prove that native full handles already held by internal adapters remain
   accepted, while a fabricated wider handle and a foreign clipboard
   observation are rejected from the owned route.
3. Verify `SetParent` and `SetWindowPos` independently, including a null
   parent and the four documented Z-order sentinel values.
4. Retain original algorithms, `GetLastError`, native-call unwind and
   published-return behavior.  Do not change guest source or reconstruct
   private USER state.

## First repair and evidence

The selected narrow-carrier wrappers now call the existing
`wow_user_window_native_handle` projection before modern USER.  That function
maps an original narrow carrier through the published object table and leaves
an already-full host HWND alone, which is required by retained native-only
adapter callers.  It does not create a second handle table.

`SetParent` separately maps its nullable parent; `SetWindowPos` preserves the
four Z-order pseudo handles before conversion.  The focused x86 lifecycle
fixture passed with short-carrier `MoveWindow`, `ShowWindow`, all four
Z-order values and `SetCapture`/`ReleaseCapture`, plus its existing menu,
callback, retirement and full-HWND regressions.

The same private-desktop fixture cannot be the activation/focus mapping
witness: `SetActiveWindow`/`SetFocus` retain real host thread input attachment
past the fixture's expected desktop close edge even when reset to null.  This
is a host test-container limitation, not evidence that the carrier conversion
failed.  A separate non-desktop-owning witness is required before treating
those two wrappers as runtime-accepted.  Guest media is unchanged.

The complete cached component build passed with ten images and 1,519
intermediates; its second build was idle.  `build/t473-s9/package-r1` retains
the immutable S8 media/configuration and replaces only the coherent ten host
images.  Its manifest hashes were verified after publication to both
`assets/release` and `O:\winnt\system32`.
