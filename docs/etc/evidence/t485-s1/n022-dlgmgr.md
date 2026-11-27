# S15 P41 N022 dialog manager — delivered

Cached build, runtime46 dual publication and targeted tests passed.
N02212/39 delivered; S15 9/14 and overall57/159 directories remain unchanged.
[Receipt](n022-dlgmgr-result.json), [census](n022-dlgmgr-census.json).

## Inputs and original review

XP SP1 fixed revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/client/dlgmgr.c, blob
9dde726a0a75d848e89389629aaf760a7b472b89. OpenNT registered original
windows/core/ntuser/client/dlgmgr.c, blob4dac85bf608b3053a5018d9eb3201ae0abc927aa.
Both objects were read as raw Git bytes and compared with the complete current
file. XP has1450lines. Local review products are under build/t485-s15/n022-dlgmgr.

Restore XP focus revalidation, pointer-width result/cast contracts, integer
conversion, monitor placement, accessibility notifications, modal message-box
copy handling and input-language dispatch. The previous paired RepositionRect
now belongs to its whole-file XP baseline. No original guest or media changes.

## Removal and retention decisions

| Surface | Decision and evidence |
| --- | --- |
| Six omitted helpers | Restore GetDlgCtrlID, SetDlgItemInt, CheckDlgButton, GetDlgItemInt, CheckRadioButton and IsDlgButtonChecked. No evidence requires deleting them. Private body symbol bindings avoid defining imported USER32 symbols; existing external product call routes do not change. |
| WOW_ORIGINAL_DIALOG_MANAGER source guards | Remove them from the mirror. The product compiles one complete manager; only the modal test needed a subset. Extend the existing exact-source CMake projection to that subset and EOF layout tail, without a maintained algorithm copy. |
| xxxRestoreDlgFocus | Use XP's own ValidateDialogPwnd revalidation instead of the added early-return guard. The keyboard fixture must model completed original cleanup accurately: clear WFDIALOGWINDOW when PDLG is removed. Leaving that bit set falsely bypasses original validation. Original keyboard retirement tests passed. |
| Native dialog initialization | Retain wow_dialog_host_initialize before initial PDLG allocation. The native HWND owns the real dialog procedure/result slots; original private PDLG allocation alone does not initialize that native owner. |
| Procedure/result dispatch | Retain the existing native A/W procedure and result-slot owner and WM_NOTIFYFORMAT boundary. XP UserCallDlgProcCheckWow in client/clmsg.c depends on private guah lightweight hooks, MapKernelClientFnToClientFn and pfnWowDlgProcEx; the rehost instead already calls the native procedure through the scoped WOW callback route. No second dispatch or callback registry. |
| Message-box private procedure identity | Omit only the comparison to MB_DlgProc in WM_INPUTLANGCHANGEREQUEST. That function belongs to unimported XP client/msgbox.c; MVDM WU32MessageBox/WU32MessageBoxIndirect call native MessageBox/MessageBoxIndirect. Those native message boxes are not created by this recovered modal owner. Do not invent a sentinel procedure or import a second message-box engine. Retain the rest of the original input-language branch and original modal WFMSGBOX copy branch. |
| DM_SETDEFID and capture cancellation | Retain existing post-callback retirement checks protecting PDLG access, covered by real-hidden-HWND keyboard and modal boundary fixtures. No new queue/state or speculative extra checks. |
| Original extracted tail | Preserve already admitted TellWOWThehDlg/GetEditDS/ReleaseEditDS, SetDialogPointer, _GetDlgItem and the existing narrow lifecycle providers. Their original provenance remains beside their bodies. No additional extraction. |

Native binding uses the existing NotifyWinEvent owner, native IME system metric,
original x86 pointer conversions, and PUSIF_SNAPTO captured from the same native
preference already used by the previous code. Original SYSMODALWINDOWS
remains the sole excluded branch. Diagnostic mapping
checked1173matching original lines without mismatch. No extra source conditions.

## Final census and verification

Nonerror54points/480lines against OpenNT becomes10points/180lines against XP:
-44points/-300lines. Current population1329files,782byte-identical,
793nonerror-identical,536nonerror-different,7822points/117106lines.
Baselines1278XP/51OpenNT. No mirror additions/removals; byte-sensitive line
endings, EOF and #line remain counted. Final source/package hashes were verified and the global ledger updated.

Completed verification: cached full build, runtime46 dual publication/hash check,
40control cases including focus/modal retirement and original event ordering,
real CCPU window-borrow and SOL Options display/close/parent recovery, governance,
commit and push. One mirror in this batch; nine since P36 Full24/24.
The next tenth-file checkpoint and S closure still require Full.

All40 control cases, real CCPU window-borrow lifecycle and actual SOL Options
display/close/parent recovery passed on runtime46. Initial fixture builds failed
before executing cases: one restored helper needed a rejecting native-call
double, and modal projection needed the two existing source foreground macros.
Both were fixed in test wiring; no runtime46 product code changed. The final
retry is retained alongside the initial failures. Source projection uses the
same original definitions, not copied replacement algorithms. Publication26hash
and built WOW32 identity were rechecked after tests. No overall S/T closure.
