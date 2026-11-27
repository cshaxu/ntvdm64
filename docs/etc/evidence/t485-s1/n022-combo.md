# S15 P36 N022 combo migration — delivered

Base commit: 21fee549984a74ff0dae7c42aa80c671118e6de2.
XP commit: a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
combo.c XP blob: f79ac6886fbdfe6b6022d1004565272f9b45bef6.
OpenNT blob: de000921ecc635a2e282a7d6472863ada31abd35.

## Source and dependency disposition

The complete combo.c algorithm is XP original. Five COLOR_HOTTRACKING arms
and one LATER arm preserve their original guards with #error bodies; each
#line resumes after endif, including the selected false path. The old literal
#if 0 declaration no longer exists in XP. No behavior is removed to ease build.

CBOX/COMBOWND move from the old usercli.h location to XP's inc/user.h owner;
there is one structure definition. Original CBNcCreateHandler,
xxxCBCreateHandler and xxxCBCalcControlRects supply the paired XP creation,
ANSI input and right-to-left placement behavior. The rest of comboini.c,
user.h and usercli.h remain OpenNT-baselined prerequisites, not completed
whole-file migrations. Original DbcsCombine is restored in editec.c, using
the already bound PeekMessageA and existing queue, with no new byte buffer.
THREAD_CODEPAGE reuses OpenNtUserThreadCodePage and native keyboard layout.

XP CB_GETCOMBOBOXINFO calls the existing retained-state info provider. The
no-edit case is not missing a child: original comboini sets spwndEdit to the
combo itself. Native GetComboBoxInfo must not inspect a different private
control implementation. Existing scoped native calls now also cover
AnimateWindow and NotifyWinEvent. Monitor geometry and UI-effect preferences
are inputs to the existing per-call context; no second lifecycle owner.

Source review found registered_init_task accepted but discarded compat_flags
and user_flags supplied by XP wkman.c. Both now flow through the existing
lifecycle provider to queue.c's task/client state before task insertion.
The XP COMPATFLAGS2_FORWOW mask is preserved. It excludes ANIMATIONOFF;
this change does not invent that flag for WOW. Original GetAppCompatFlags2
is restored in winmgrc.c. No native PEB/private CLIENTINFO reinterpretation.

The combo-only _CreateWindowEx boundary adopts XP's thirteen arguments and
RTL styles. The generic fourteen-argument WOW creation boundary is unchanged.
The obsolete rejected-WOW-words combo test is removed because the parameter
no longer exists; other failure and ownership assertions remain.

## Paired message-table correction

The real CB_GETCOMBOBOXINFO message initially failed because the old server.c
message array filtered it before XP ComboBoxWndProcWorker. The entire original
XP gawComboWndProc array is restored, including mouse-leave/style/UI-state
messages. No filter bypass or alternative dispatch. server.c remains OpenNT
whole-file baselined; its new unit has XP diagnostic mapping and restores the
following OpenNT line. The production event wrapper uses the existing HWND
conversion and synchronous scope. No guest file was changed or rebuilt.

## Delivery and verification

Cached full build passed. runtime41 is dual-published to assets/release and
O:/WINNT/SYSTEM32 with26 verified hashes for13 images. Control39/39, task order,
task lifecycle, native creation, window borrow, process retirement, actual SOL
Options and final-package Full24/24 passed. The task tests retain passing
unchanged code/ABI evidence from before the final combo-array-only correction.
The final package proves the combined product. Failed attempts remain in the
receipt; none counts as a pass. The native creation test's obsolete NT4 tag
was corrected to the existing XP constant, without relaxing production checks.

## Mixed-baseline census

| Metric | Before | After | Delta |
| --- | ---: | ---: | ---: |
| files | 1329 | 1329 | +0 |
| byte_identical | 781 | 781 | +0 |
| nonerror_identical | 792 | 792 | +0 |
| nonerror_different | 537 | 537 | +0 |
| nonerror_points | 7848 | 7853 | +5 |
| nonerror_lines | 123546 | 119031 | -4515 |

| File | Nonerror points before/after | Nonerror lines before/after |
| --- | ---: | ---: |
| src/non-mvdm/windows/core/ntuser/client/combo.c | 1 / 6 | 4601 / 6 |
| src/non-mvdm/windows/core/ntuser/client/comboini.c | 1 / 1 | 1251 / 1275 |
| src/non-mvdm/windows/core/ntuser/client/editec.c | 1 / 1 | 5819 / 5834 |
| src/non-mvdm/windows/core/ntuser/client/usercli.h | 1 / 1 | 4693 / 4638 |
| src/non-mvdm/windows/core/ntuser/client/winmgrc.c | 1 / 1 | 711 / 725 |
| src/non-mvdm/windows/core/ntuser/inc/user.h | 1 / 1 | 5161 / 5223 |
| src/non-mvdm/windows/core/ntuser/kernel/queue.c | 1 / 1 | 5200 / 5211 |
| src/non-mvdm/windows/core/ntuser/kernel/server.c | 1 / 1 | 3010 / 3017 |
| src/non-mvdm/windows/core/ntuser/kernel/userk.h | 1 / 1 | 10214 / 10216 |

Only combo.c switches whole-file baseline to XP: six remaining #line differences
outside six excluded arms. Eight paired dependencies retain OpenNT whole-file
baselines; original XP units do not claim their whole files migrated. Totals:
1273XP /56OpenNT. LF restoration exposes six dispersed mapping points instead
of one whole-file block; byte-sensitive line/EOF differences are not discarded.
No new #error branch is invented for an unconditional original omission.

[Receipt and attempts](n022-combo-result.json), [census](n022-combo-census.json).
