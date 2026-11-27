# S15 P40 N022 dialog ending — delivered

Cached build, runtime45 dual publication and targeted tests passed.
N02211/39 delivered; S15 9/14 and overall57/159 directories remain unchanged.
[Receipt](n022-dlgend-result.json), [census](n022-dlgend-census.json).

## Three-way review

Fixed XP SP1 revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/client/dlgend.c,
blob76805728f6d9bc3e1fcba8f30e155bea12efcde9. Registered OpenNT
windows/core/ntuser/client/dlgend.c blob3f784316f920a8dda9d04245650f407c5f083a77.
Read both objects as original bytes and compare the full current file.

XP changes the EndDialog result parameter from int to INT_PTR, adds the original
retail logging invocation and updates comments/copyright. Preserve the complete
178-line original algorithm, including owner enable/activation, modal result,
focus/hide and posted wake message. Pair the production declaration, typed
callback and cast with INT_PTR; update the same test interface declarations.
The production WOW owner is x86, so the width stays32bits; this is original ABI
restoration, not a claim of x64 WOW support. Retail FUNCLOG2 has no execution
effects and is bound consistently with existing FUNCLOG4/6 diagnostics.

## Residual evidence and minimality

The one functional adaptation checks retirement after NtUserEnableWindow and
before PDLG access. The original dialog-end fixture destroys a real hidden HWND
at this callback and clears its PDLG, verifying false return without access
violation or result mutation. Removing the check would dereference retired
PDLG. Retain this two-line guard; do not add a second end/finalization path.
Other focus/hide/activate callbacks do not acquire new PDLG accesses and need no
new checks. The existing scoped borrow owns storage across the synchronous call.

Three SYSMODALWINDOWS arms remain excluded under their original conditions.
Remove redundant pre-arm #line directives; reset after each excluded arm and
after the retained guard to the original XP diagnostic coordinate.
No guest changes, extra queues, state machines or original algorithm copies.

## Final census

dlgend.c changes from OpenNT10points/15lines to XP7points/9lines after #error
body exclusion: -3points/-6lines. All other mirrors are unchanged.
Whole population1329files,782byte-identical,793nonerror-identical,
536nonerror-different,7866points/117406lines. Delta0files/0exact/0same/
0different/-3points/-6lines. Baselines1277XP/52OpenNT, versus1276/53 before.
EOF/newlines/#line remain counted. No file added or removed.

## Verification and delivery

Cached full build, runtime45 dual publication and26hash checks, existing40
control cases (including22 original EndDialog cases and modal lifecycle),
real CCPU window-borrow lifecycle and SOL Options close/parent recovery.
No full rerun this P: one mirror changed, eight since P36 Full24/24.
S closure and the ten-file checkpoint retain their mandatory Full gate.

All40 control cases, real CCPU window-borrow lifecycle and actual SOL Options
display/close/parent recovery passed on the final package. The26 published
hashes match; fixture builds did not alter the published WOW32 input. Original
diagnostic mapping checked113lines with zero mismatch. The first CMD launcher
exited before creating a build log because its relative path used forward
slashes; its absent process handle was verified before relaunch with a proper
CMD path. No compiler/test failure is hidden by this launcher correction.
Commit/push include this final verification evidence; no overall S/T closure.
