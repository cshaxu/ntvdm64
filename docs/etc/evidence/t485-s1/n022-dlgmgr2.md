# S15 P42 N022 dialog keyboard — delivered

Cached full build, runtime47 dual publication,40control cases, real CCPU
window-borrow lifecycle, SOL Options and Full24/24 passed.
[Receipt](n022-dlgmgr2-result.json), [census](n022-dlgmgr2-census.json).
N02213/39 delivered; S15 9/14 and overall57/159 directories. Ten-file Full
checkpoint completed; this does not close S15 or T485.

## Inputs and original review

XP revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/client/dlgmgr2.c, blob
4da9947dcfba77bf869f8cde8b9c35d69b125b91. Registered OpenNT original blob
b67bca343d0ff1ba5ca9294911e7100945aa3873. Raw Git objects were compared against
the complete pre-edit source. The XP original has779lines.

Restore XP's repeated-control traversal stop, synchronous-message validation,
keyboard-cue updates, RTL arrow navigation and pointer-width casts. Original
FE_SB, FE_IME and DBG branch conditions retain their #error exclusions. The
obsolete OpenNT LATER exclusion is removed: XP makes that validation unconditional.

## Retained boundaries and actual providers

| Surface | Disposition and evidence |
| --- | --- |
| Target WM_GETDLGCODE callback | Keep the existing fnid retirement check and shared unlock exit. The callback can retire the target before button-state access; existing keyboard lifecycle cases exercise this boundary. |
| Button focus callback | Keep the existing button borrow scope and finally release. Original BST_DONTCLICK update remains inside that lifetime. No new state machine or callback registry. |
| Message classification | Reuse XP MessageTable from clmsg.c/messages.h and the exact TESTSYNCONLYMESSAGE expression. PROTOS_ONLY changes generated thunk material in msgdef.h, not the MessageTable definition. No duplicate product table or hand-maintained message list. |
| Keyboard preference | Existing dialog_message captures and restores wow_dialog_context; capture already reads native keyboard preferences and copies the lifecycle class atoms. Reuse TEST_KbdCuesPUSIF and the existing native UI-state boundary. |
| Button class identity | XP compares the system button atom with CLS.atomNVClassName. The retained pre-XP CLS view exposes atomClassName, initialized from the registration's immutable native atom by class.c and retained with the window's class reference. Bind IS_BUTTON to that field, preserving class-atom comparison rather than the older FNID predicate. No versioned-class namespace is introduced. |
| Test-only source guards | Remove WOW_ORIGINAL_DIALOG_KEYBOARD guards from the mirror. Production compiles the whole file; helper-only navigation tests use exact-source CMake projections. Keyboard tests link the same original messages.h table through generated msgdef.h. |

Keyboard fixture inputs now establish the existing dialog/class context and
exercise native rejection versus WOW marking for synchronous messages, including
WM_DEVICECHANGE's wParam bit. Original ANSI conversion and callback retirement
tests remain. No original guest or media changes.

The first control build failed at link, before any cases executed: the restored
UI-state branch required three test providers absent from the older fixture.
The fixture now checks the UI-state query/change contract and observes actual
hide-focus clearing; GetAppCompatFlags2 reads its explicit client-info input.
The second control run passed all40cases. Product inputs did not change after
publication. Both attempts remain under the local evidence directory.

## Final statistics

Nonerror35points/61lines against OpenNT becomes10points/25lines against XP:
-25points/-36lines. Original logical-line check covers606matching lines with no
mismatch. Three original excluded branch bodies remain excluded.

Final full population:1329files,782byte-identical,793nonerror-identical,
536nonerror-different,7797points/117070lines. Before:1329/782/793/536/7822/117106.
Delta:0/0/0/0/-25/-36. Baselines:1279XP/50OpenNT. Final source and package hashes were rechecked after verification. The
global ledger now reflects this delivery. No guest or media changes.
