# S15 P39 N022 dialog creation

Final source and runtime44 are verified; this receipt is included in the P39
commit. N02210/39, S15 9/14, overall57/159; next ordered file is dlgend.c.

## Inputs and procedure

[Original objects and final source identity](n022-dlgbegin-review.json),
[delivery receipt](n022-dlgbegin-result.json), [census](n022-dlgbegin-census.json).
Fixed XP SP1 revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89 was read as Git
object bytes. dlgbegin.c migrates completely to XP; dlgmgr.c and usercli.h
receive paired prerequisites and retain their registered OpenNT whole-file
baseline. No original guest, winnt.zip or unrelated product changes.

## Original algorithms and minimal retained boundaries

XP shell-font/charset selection, DIALOGEX fallback, signed coordinates,
monitor placement, per-child version/class flags, early WOW notification and
keyboard-cue initialization are retained. No modern CreateDialog replacement.

| Boundary | Final implementation and necessity evidence |
| --- | --- |
| RepositionRect | Restore the whole four-argument XP helper in dlgmgr.c and pair DM_REPOSITION and usercli.h. The old three-argument helper cannot accept XP monitor placement. |
| Monitor views | XP compares two retained PMONITOR pointers. The existing combo single overwritten slot cannot preserve their identity. The existing per-dialog scope owns stable views keyed by actual HMONITOR, freed at scope release; no global registry or alternate placement algorithm. Native context tests verify repeat identity, native work area, NULL and nested scope independence. |
| Creation | XP VerNtUserCreateWindowEx enters the existing enrolled creator with absent optional WOW words. No second creation path. |
| Notification | Restore the original XP TellWOWThehDlg body using the existing registered callback; remove WOWDlgInit's unchanged-lParam wrapper. Preserve XP notification timing. |
| Font | Keep XP font algorithm. Installed x86 GDI exports GetTextFaceAliasW, but the SDK import library does not resolve it (build-r2 LNK evidence). Reuse the existing GDI INIT_ONCE resolver with the same signature, not a substitute lookup. Native fixture exercises the actual export. |
| UI inputs | Capture native keyboard preference/cues and DPI in the existing scope. Refresh cursor only at the XP DS_CENTERMOUSE decision; no unconditional cursor access. |
| Native procedure | wow_dialog_host_set remains the actual native A/W procedure owner. Original private PDLG assignment alone does not install that procedure; retain failure cleanup without double-freeing the attached menu. |
| Retirement | Existing creator fixture destroys real hidden root/child HWNDs at callbacks. Retain promotion cleanup and checks after font, EDIT allocation, child creation and child messages. Release newly allocated EDIT segment when the root has retired. Add the same check after XP WM_CHANGEUISTATE; no new lifetime state machine. |

The initial review carried old TL/ThreadLock/Unlock scaffolding. Full call-chain
inspection showed these client macros are no-ops, not lifetime owners. Remove
all five such additions and restore XP's original DLGF_ANSI assignment. The
existing enclosing borrow scope owns lifetime. This corrects the initial
candidate rationale; the original candidate measurements remain in JSON.

Three original disabled/debug branches retain their #if 0/#if DBG conditions
and #error exclusions. #line resets after #endif preserve active diagnostics;
1194 matching original lines were checked with zero mapping mismatches.
No harmless original functions/declarations were removed for being unused.

## Non-error mirror census

| File | Before baseline, points/lines | After baseline, points/lines | Delta |
| --- | --- | --- | --- |
| dlgbegin.c | OpenNT 13/46 | XP 16/52 | +3/+6 |
| dlgmgr.c | OpenNT 40/428 | OpenNT 54/480 | +14/+52 |
| usercli.h | OpenNT 1/4638 | OpenNT 1/4638 | 0/0 |

The dlgmgr.c increase includes original XP helper/notification restoration
against its still-OpenNT baseline, not new replacement algorithms. It remains
registered until ordered whole-file XP migration. Cursor/UI guard additions
are within standing small-change approval. Whole-file XP restoration is
permanently approved; this receipt does not claim a net diff reduction.

| Whole non-guest population | Before | After | Delta |
| --- | ---: | ---: | ---: |
| Files | 1329 | 1329 | 0 |
| Byte identical | 782 | 782 | 0 |
| Nonerror identical | 793 | 793 | 0 |
| Nonerror different | 536 | 536 | 0 |
| Nonerror points | 7852 | 7869 | +17 |
| Nonerror changed lines | 117354 | 117412 | +58 |

1276 XP / 53 OpenNT baselines, versus1275/54 before. No mirror additions or
removals. Raw line endings, EOF and #line remain counted; untouched mixed bytes
in the paired files are preserved. All1329 current hashes were checked.

## Verification and delivery

Cached full build-r4 passed after resolving original macro/header declaration
ordering and SDK export binding. Runtime44 was dual-published;26 file hashes
and the release manifest match, including the built WOW32 input after tests.
Control40/40, real CCPU window-borrow lifecycle and SOL Game > Options modal
window/close/parent recovery passed on the final package.

Initial control build failed before executing fixtures: keyboard/creator
fixtures lacked newly reached monitor and foreground ABI symbols. Add explicit
test doubles that fail if unrelated branches are entered; retry passed.
Product code/package remained unchanged. Initial build and test failures remain
in the receipt; they are not erased by the retry. No Full repetition: three
mirrors in this batch, seven since P36 runtime41 Full24/24. S closure and the
ten-file checkpoint still require Full. No claim of completing the overall T.
