# S15 P49 N022 multiline EDIT - delivered

## Scope and fixed inputs

Start HEAD4d210084e; P48 runtime57 delivered. Complete client/editml.c migration.
XP blob283d14308cd14c1a7896761b6f011b6894ab7ee0 at fixed SP1 revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89; OpenNT blob
1db43d5cb3ab73456237466ff57fd1978a8450f3. Prior nonerror1point/7554lines.
Global1329files/784exact/795nonerror same/534different/7824points/107385lines.

## Whole-file restoration and provider

Restore complete XP multiline layout, selection, DBCS, IME, undo and message
handling. Original DBG, literal0, UNDO_CLEANUP and LATER branches stay excluded
with #error/#line. Preserve original typed LPK hooks; no invented conditions.
Paired usercli.h window procedure declaration retains old whole-file baseline.

Original kernel/cursor.c750-761 zzzHideCursorNoCapture sets cursorNULL only
without capture and when appcompat/effect policy permits. Current x86 win32u
exports NtUserHideCursorNoCapture; reuse it through the existing single native
export resolver and symbolic SFI binding, not an NT4/XP numeric syscall index.
No duplicated cursor policy, queue or state machine. Existing thread scope
and original editor own invocation; installed USER owns cursor lifetime.

## Final verification and delivery

runtime58 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and genuine guest SOL Game > Options pass.
[Receipt](n022-editml-result.json), [census](n022-editml-census.json).
No guest change. N02220/39; overall57/159 and S15 directories9/14 unchanged.
Eight completed mirrors since P42 Full24/24. Next editsl.c. No S/T closure.

## Direct dependency findings

MLCreate's HWND argument is unused in the old provider and absent in XP.
Pair its declaration/provider with the XP caller, retaining its original
remaining body and OpenNT baseline until whole-file migration. ECUpdateFormat
already comes from P48 editec.c; restore its declaration, not another algorithm.
FRE_RIPMSG0 is empty in original w32err.h899 for the existing retail profile
without PRERELEASE/USER_INSTRUMENTATION; use that exact selected macro.

Current native USER32 exports EnterReaderModeHelper. It enters modal input,
so it is reached through the existing WOW_NATIVE_MAPPED_CALL boundary: convert
HWND, begin native call, invoke provider, finally restore. Installed USER owns
its reader-mode resources/message loop; no local loop or duplicate state.
The native control fixture now checks HideCursorNoCapture leaves both capture
and the cursor intact when this thread owns capture, restoring prior inputs.
First build exposed the three paired declaration/provider gaps above; retained
in build.log. Final build and package acceptance passed as recorded above.

## Nonerror mirror accounting

| File | Before points/lines | After points/lines | Baseline |
| --- | ---: | ---: | --- |
| src/non-mvdm/windows/core/ntuser/client/editml.c | 1/7554 | 6/6 | OpenNT to XP |
| src/non-mvdm/windows/core/ntuser/client/edmlonce.c | 1/233 | 1/233 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/usercli.h | 1/4706 | 1/4708 | OpenNT to OpenNT |

Global1329files,784byte-identical,795nonerror-identical,534different,7829points/99839lines. Delta0/0/0/0/+5/-7546. Baselines1287XP/42OpenNT. Restoring LF exposes6actual diagnostic #line blocks rather than one CRLF whole-file block. editml.c has no nonexcluded algorithm alteration. Six original unselected branches remain #error. Paired files retain OpenNT baseline. Raw newline/EOF differences remain counted.
