# M0 T457 S2 — dialog, input and caret acceptance

## Result

S2 closes. A real Win16 dialog/control probe exposed one production seam in
the retained modal-dialog route; the repair scopes the existing original
dialog call binding over that route. It adds no dialog manager, input router,
caret state, scheduler, or guest change.

## Source ownership and root cause

The formal x86 graph selects the retained original providers. The modal chain
is:

```text
wudlg.c::WU32DialogBoxParam
  → wow_user_dialog_box_indirect_param
  → clres.c::DialogBoxIndirectParamAorW
  → dlgmgr.c::InternalDialogBox
  → dlgbegin.c::InternalCreateDialog
```

`InternalCreateDialog` uses original USER aliases (`gpsi`, `oemInfo` and
`rcScreen`). In this product they project the call-scoped
`wow_dialog_current` TLS context. The former public modal facade invoked the
original `DialogBoxIndirectParamAorW` without entering that scope. The first
modal real-entry therefore dereferenced null context at WOW32 RVA `0x485af`.
The formal map locates `InternalCreateDialog` at `0x100481a0`.

The modeless route already reached the equivalent scope through
`wow_user_server_create_dialog`. S2 factors that exact scope into a private
shared call lease and uses it for the modal facade. Original `clres.c` template
sizing, flags and modal loop remain authoritative. Native USER owns the HWND
and focus; the borrowed view is a temporary projection only; original dialog
code remains the sole owner of conversion and modal behavior.

## Delivery

`wow_user_modal_dialog_box` is declared in
`src/wow32-dll/include/wow_dialog_creation_binding.h` and implemented beside
the existing modeless binding. It enters task callback, public-user borrow,
dialog context and TLS scopes, invokes the original modal provider, and
unwinds them in reverse order on every result. The public facade delegates to
it. The modeless call uses the same helper, eliminating two independently
maintained context contracts.

## Evidence

| Check | Result |
| --- | --- |
| Focused modal entry fixture | `wow_dialog_creation_entry_fixture.exe`: `dialog-creation-entry failures=0` |
| Formal x86 product graph | complete `msvc-x86-release` build; following build reported `ninja: no work to do` |
| Pre-repair control | T449 package with original WOW32 faults in `DLGPROBE` at the source-proven modal entry |
| Repaired control | the same T449 package with repaired WOW32 completes all eight probe phases |
| Coherent candidate | `build/t457-s2/dialog-fixed-r4/result.txt`: `WOW_DIALOG_GUEST_OK phases=8 init=6 modal=2 rollback=1 keyboard=2 retirement=1` |
| Publication | all ten manifest images copied coherently to `assets/release` and the installed System32 target; `build/t457-s2/publication-r1.json` records strict candidate/release/installed hash equality |

The guest probe covers initialization and text controls, shared/local dialog
state, modal entry, failure rollback, cancel, edit-key delivery and one task
retirement. A stale disposable test broker caused one rejected trial before
the final run; it was identified by exact owned path, stopped, and is not
counted as product evidence.

## Boundary and handoff

S2 does not claim hook or timer registration. S3 owns retained original
`wuhook.c`, `wowhooks.c` and `wutmr.c` registration, notification, reentry,
removal and task-loss acceptance. The keyboard/caret desktop-state fixture
remains an explicit environment non-pass from S1; this real dialog probe proves
keyboard-driven dialog processing without replacing that fixture.

No guest image, guest build, configuration or media changed. The only image
diff is the rebuilt host WOW32 closure and its coherent dependent package
images, recorded by the publication manifest.
