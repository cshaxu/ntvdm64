# S15 P38 N022 combo directory — delivered

Base commit: ea35a086e2c3c14d9588a9f2187de68cbfb89721.
XP commit: a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
XP path: Source/XPSP1/NT/windows/core/ntuser/client/combodir.c.
XP blob: e759545f0ea76d3a3f7470f92f084da180cf77d0.
OpenNT blob: b4aa0c0fc56f895dca72c587cd34c8ed166b7d77.

## Source disposition

Restore all205 XP lines byte-for-byte, including the original list-child
assertion and retail FUNCLOG declarations. No mirror adaptation, #error or
#line remains in combodir.c. This completes the DIR entry omitted by P36's
next-file pointer; comboini.c is already delivered, so dlgbegin.c follows.

The XP DlgDirSelectHelper accepts an HWND instead of two borrowed PWND values.
Restore the complete original helper and DlgDirSelectExW caller in lboxctl3.c;
update its existing usercli.h prototype. Both combo and list directory selection
now use the same original helper. No compatibility forwarding wrapper, copied
algorithm, extra state or queue. Original helper geometry/text/SendMessage and
return conventions are retained. The two paired units carry XP #line positions
and restore the following OpenNT lines; both files retain whole-file OpenNT
baselines, with all previous #error exclusions unchanged.

The existing retail logging boundary adds empty FUNCLOG4/5 alongside FUNCLOG6.
Logging arguments are not evaluated, matching the existing uninstrumented
product build. No source behavior is removed to silence the compiler.

The existing real-window fixture now selects a file and directory through
DlgDirSelectComboBoxExA: file selection returns false and adds the original
trailing dot; directory selection returns true and appends the backslash.
This uses the same original combo/list state and message path, no filesystem
fixture or replacement directory algorithm.

## Census and delivery

| File | Nonerror points before/after | Nonerror lines before/after |
| --- | ---: | ---: |
| combodir.c | 1 / 0 | 410 / 0 |
| lboxctl3.c | 1 / 1 | 2396 / 2375 |
| usercli.h | 1 / 1 | 4638 / 4638 |

Global1329files,782byte-identical,793nonerror-identical/536different,
7852points/117354lines. Delta0files/+1exact/+1same/-1different/-1point/-431lines.
1275XP/54OpenNT. Line endings/EOF remain byte-sensitive; no guest changes.

Cached full build, runtime43 dual13-image26hash publication, control39/39,
window-borrow including real file/directory combo selection, and actual SOL
Options passed. Final compiled WOW32 input matches the published manifest
following fixture builds. Initial window test called the inner original directly outside its production
borrow boundary and left list data for a later assertion. The corrected test
uses wow_user_dlg_dir_select_comboA and clears its own entries; product code
and the passed control cohort are unchanged. No Full repetition this P: three mirror files changed,
four since P36's runtime41 Full24/24 checkpoint. N0229/39 delivered, including
all entries through comboini.c; next dlgbegin.c. No guest or winnt.zip changes.

[Receipt](n022-combodir-result.json), [census](n022-combodir-census.json).
