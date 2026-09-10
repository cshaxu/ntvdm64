# M0 T398 S2 — ERRORPANEL copied request/reply recovery

## Question

Can the selected original SoftPC `ERRORPANEL` carrier cross into the app as
copied data, preserve its source-defined reply contract, and leave termination
with the existing original/session completion path? This record covers only
the Win32/x86 `ntvdm32.exe` product profile. Native x64 is neither built nor
an acceptance input.

## Inputs

- Original `softpc.new/host/inc/error.h` and
  `softpc.new/host/src/nt_error.c`, especially `ErrorDialogBoxThread` and
  `ErrorDialogEvents`.
- `adapter-mvdm-host-out/softpc/mvdm_softpc_error_interaction.{h,c}`.
- `app/error_interaction.{h,c}` and the app bind/unbind site in `app/entry.c`.
- `tests/adapter-mvdm-host-out/softpc/t398_s2_error_interaction_fixture.c`.
- `tests/app/t398_s2_error_interaction_window_fixture.c` and
  `tools/build/New-T398S2ErrorInteractionNinja.ps1`.
- `tests/mvdm-host/t398_s2_original_errorpanel_worker_fixture.c`, which uses
  the source-layout private `ERRORDIALOGINFO` carrier against the original
  worker source.
- Generated x86 graph under `build/M0-T398/S2/error-interaction-x86/`.

## Procedure

1. Compared the source handler's four icon modes and response handling with
   the copied request/reply shape.
2. Bound a fixture presenter, asserted copied message/options/edit data,
   returned `RMB_EDIT`, then independently returned `RMB_IGNORE` from an edit
   request. The fixture also asserts unavailable-presenter fallback, exclusive
   bind, and fallback after unbind.
3. Compiled and ran that fixture under MSVC Win32/x86 `/MT`.
4. Generated the dedicated x86 fixture graph, then executed its focused
   adapter, app-window and original-worker runs. The worker is compiled from
   the unmodified original `nt_error.c` under the exact generated product
   `host_cflags`; `/Gy` is test-only function-section isolation, so the
   narrow harness can call `ErrorDialogBoxThread` without composing its
   unrelated historic helper families.
5. Recompiled the changed adapter, app presentation, and original `nt_error.c`
   units using the exact commands emitted by the product x86 graph.
6. Ran all 451 exact commands emitted for `original-softpc-process.exe` in
   the generated Win32/x86 graph.  The resulting PE is
   `build/M0-T398/S2/error-interaction-x86/original-softpc-process.exe`, SHA-256
   `f1c3f81d793198d32ed685429e4509547a581fc0b3f87240b97296022f0a66a0`.
   The sole link diagnostic is the historical `.EXP` output-name mismatch;
   there is no compiler error or unresolved external.
7. Staged that PE as `ntvdm32.exe` with the current x86 `VDMREDIR.DLL`,
   `WOW32.DLL`, and selected immutable media.  A Console-owning observer
   ran two independent 30-second source-shaped observations from the fresh
   short root `O:\\t398`: (a) `system32\\WOWEXEC.EXE` with the exact
   original-ERRORPANEL Ignore predicate enabled, and (b)
   `system32\\WRITE.EXE` with the passive ordinary guest-dialog census.
   Both use `--without-diagnostics`: T397 proves that the observer's default
   diagnostic environment changes this particular bootstrap before WOW32.

## Observations

- The original option word contains `RMB_ABORT=1`, `RMB_RETRY=2`,
  `RMB_IGNORE=4`, `RMB_ICON_INFO=8`, `RMB_ICON_BANG=16`,
  `RMB_ICON_STOP=32`, `RMB_ICON_WHAT=64`, and `RMB_EDIT=128`. The original
  UI defaults its resource icon to Stop.
- `ErrorDialogEvents` clears `pEdit` for Abort, Retry, and Ignore/Cancel;
  only Edit-OK returns entered text. The first S2 implementation mistakenly
  copied the edit carrier only for `RMB_EDIT`. It is corrected: every selected
  app reply copies its bounded returned carrier, whose default is empty.
- The request marks an edit control only when both source `RMB_EDIT` and an
  edit carrier are present; a non-null pointer alone is not an edit request.
- The app-owned public Win32 surface now renders the source-selected system
  icon and only source-selectable controls. It receives no original HWND,
  guest pointer, or edit pointer.
- The generated fixture graph's adapter and window tests both pass under
  x86 `/MT`. The latter creates the actual app presenter window and closes it
  by the source-equivalent `WM_CLOSE → RMB_IGNORE` route; it proves the app
  message loop, not merely a mock callback.
- The original-worker fixture also passes. It supplies the exact source
  `ERRORDIALOGINFO` field layout, calls the original `ErrorDialogBoxThread`,
  and closes the actual app surface. The asserted result is the original
  worker's `dwReply=RMB_IGNORE`, after it has trimmed the source title from
  `"  source worker title  "` to `"source worker title"`. Thus the focused
  proof crosses the selected original request carrier, rather than only a
  hand-written adapter caller.
- This focused harness intentionally emits `LNK4088` while retaining
  unresolved symbols in *unreached* functions of the one historic
  `nt_error.c` translation unit. It does not substitute for the product link:
  the separately recorded exact 451-command x86 product build resolves the
  full unit normally and has no unresolved external. The fixture calls no
  unresolved helper and its executed `ErrorDialogBoxThread` route passes.
- The changed adapter, app presentation, and `nt_error.c` units compile in
  the generated x86 product graph with no diagnostics at changed lines.
  Historical OpenNT/SDK and original-source warnings remain outside this
  change.
- The full x86 link completes all 451 generated commands.  Its direct-run
  transcript is
  `build/M0-T398/S2/error-interaction-x86/t398-s2-x86-direct-build.log`.
- The initial long stage was correctly rejected by the existing app package
  admission before SoftPC startup: the source `shell=` value repeats its
  root, so a 24-character root still exceeds NTDOS's 63-character `commnd`
  carrier.  The fresh `O:\\t398` stage has a seven-character root and passes
  that admission.
- The initial diagnostic-bearing observations are rejected as product
  evidence.  They stopped before WOW32 in CPU40's original `HLT` event wait,
  precisely matching T397's known observer-environment effect.  The optional
  HLT report selector itself changes the stopped instruction path and produces
  no admissible normal-product conclusion.
- At the same valid short root in the required diagnostic-free container,
  `WOWEXEC.EXE` loads `O:\\t398\\WOW32.DLL` and shows the visible original
  `WOWExecClass` window (28 modules).  It does not construct an ERRORPANEL in
  the 30-second observation, so the exact Ignore predicate makes zero
  selections.  This is a valid negative reachability result, not proof of an
  error-panel reply.
- The diagnostic-free `WRITE.EXE` run loads WOW32 (35 modules), creates the
  `MSWRITE_MENU` class and the visible standard `#32770` dialog titled
  `Write`.  Its text is the original guest low-memory message, `Not enough
  memory for Write to complete this operation...`; its child set is `OK` plus
  the original static controls.  Thus the guest-owned dialog remains on its
  original route and is not presented by T398's app ERRORPANEL surface.
  The reports are `O:\\t398\\t398-s2-errorpanel-ordinary.txt` and
  `O:\\t398\\t398-s2-write-ordinary.txt`.

## Interpretation

The copied boundary has a finite request/reply ABI and preserves the source
button/result, icon-selection, edit-capacity, and edit-clear behavior without
letting the app own termination. `ErrorDialogBoxThread` retains the existing
post-reply WOW notification and original caller path; the app only returns a
source reply.

## Boundary and closure disposition

The current host occasionally stalls when Ninja itself executes a graph before
spawning a compiler. The generated graph was therefore executed command by
command; it now supplies the required full x86 product link and all three
focused fixtures. The ordinary WRITE non-regression is met. No ordinary
diagnostic-free workload constructed an ERRORPANEL during the bounded
observation, but the source-worker fixture proves the selected original
request/reply carrier itself without inventing a guest fault or a product
trigger. Therefore S2's finite copied-boundary exit criterion is met.

This is not a claim that an organic host-error workload was observed, nor a
migration of guest dialogs, `RcMessageBox`, or private WOW hard errors. Any
future runtime reachability investigation requires a newly admitted packet;
it cannot reopen x64 work or manufacture an ERRORPANEL trigger under T398.
