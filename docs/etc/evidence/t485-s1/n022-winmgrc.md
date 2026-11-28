# S15 P68 N022 client window helpers

## Scope and source

Start HEAD83b5a9674/runtime78, N02238/39. Fixed XP SP1 revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/client/winmgrc.c, blob
998e102edb1d8a72d7e8f0038e8e196f77e19e1a,2147lines.
Previous OpenNT blob a8ad2b8ec78fe5236ec1ebeef5cde4c949724f64.
Original bytes and the previous local file are in build/t485-s15/n022-winmgrc.
No guest source or binary is changed or rebuilt.

## Restored owners and coupled bindings

Restore XP FChildVisible, GetAppCompatFlags, GetAppCompatFlags2 and the complete
RealAdjustWindowRectEx algorithm. The latter now has its original name, DWORD
style argument and GETAPPVER input. Remove the former precomp rename of
RealAdjustWindowRectEx to _AdjustWindowRectEx and the temporary GetAppVer macro.
The original dlgbegin.c caller now reaches the real XP algorithm directly.
The older _AdjustWindowRectEx ABI remains a synchronous one-line call to that
same algorithm: the XP guah/SRVIF_HOOKED theme dispatch provider is not installed
in this standalone USER slice. There is no new table, hook or second algorithm.
GetAppCompatFlags retains the existing opaque thread-pointer spelling; XP
explicitly ignores this argument and reads the task's existing client view.

Also restore complete TakeWindowSnapshot, AnimInc and AnimDec, although they
have no current production callers. The snapshot uses the existing window
view plus public GDI/message operations; the two arithmetic helpers compose
with existing MultDiv. Their unused status is not evidence for deletion.
Original constants and intervening comments remain with these recovered units.

## Whole-file omitted boundaries

| XP range | Original functionality and current boundary |
| --- | --- |
| 17-53 | GetWindowWord public entry. Native USER owns HWND storage; WOW WU32GetWindowWord converts guest parameters and calls the enrolled native provider. Do not define a competing USER export over the private retained control view. |
| 67-225 | AdjustWindowRectEx/GetClassNameW, focus/capture, AnyPopup, input state, coordinate mapping and popup queries. Public USER owns desktop and thread state; wow_user_discovery.c publishes returned identities. The local edit context has no cCaptures and task view has no pClientThreadInfo; NtUserGetThreadState and its selectors are not a provided original service. Public adjustment still uses the native API; internal original dialog adjustment uses RealAdjustWindowRectEx. |
| 226-544 | PtiWindow/process lookup, scroll APIs, active-window/cursor and menu queries. Private kernel handle ownership and THREADINFO are absent; actual modern queries use wow_native_GetWindowThreadProcessId and the existing native scroll/discovery providers. Original scrolling requires private NtUserSetScrollInfo/NtUserSBGetParms and guah dispatch. Public exports must not be replaced by lookups over borrowed WOW-only state. |
| 592-681 | Unicode/process/hung-window queries, CreateSystemThreads and PtiCurrent. Native USER owns these public identities and kernel threads. PtiCurrent's current EDIT binding is only thread-ID equality, not a THREADINFO pointer. CreateSystemThreads requires NtUserCallOneParam/SFI_XXXCREATESYSTEMTHREADS; importing system-thread creation is outside this client cohort. |
| 760-897 | ShowWindowNoRepaint/AnimateBlend operate private SetWindowPos flags and NtUserUpdateLayeredWindow. Existing native animation owner retains modern window lifetime and publication; no replacement kernel/window service is introduced. |
| 1013-2147 | AnimateWindow/smooth-scroll/public-scroll and GUI/server-side/destroy queries. Original animation requires private THREADINFO, saved-under/DC state and NtUserCallHwndParamLock; native lifecycle/scroll providers already own actual HWND operations. Last public predicates must query actual native window state, not substitute a borrowed local snapshot. |

These unconditional omissions remain visible differences, without invented
#error guards. No original #error branch existed in the previous selected file.
No new kernel flag, window state machine, global hook or fake provider is added.

The initial whole-original product compile reported C2039 for cCaptures and
pClientThreadInfo, C2061 for PTHREADINFO/PtiCurrent, C2037 for the opaque USERAPIHOOK
members, and C2491 for the imported CreateSystemThreads definition. Those
observations agree with the actual provider/view declarations and product map.
The later full-original-compile.log is a timestamp-cache no-op, not evidence
that original restoration compiled; it does not supersede the initial errors.
Final source and the actual final cached build are checked independently.

## Diff accounting

Previous raw OpenNT-relative count:1point/725lines. Previous local bytes against
XP:1point/2271lines; normalizing only their CRLF to XP LF gives13points/2063lines.
Current XP candidate:7points/1894lines. On the same normalized XP baseline this
reduces6points/169lines. Raw mixed-baseline census increases6points/1169lines
because XP added native USER functionality outside this slice and LF exposes
separate remaining omissions. It is not1169lines of new adaptation code.
The LF restoration falls under the standing format-recovery approval; the one
small synchronous dispatch binding falls under the standing small-point approval.
Original logical-line audit passes, no new conditional exclusion framework.

## Verification and delivery

Final cached build and runtime79 dual13-image26hash publication pass.
Controls40/40, real CCPU standard-class lifecycle, SOL Options and genuine
Win16 controls pass. Initial fixture link failures were corrected in tests:
the creator mock now uses the XP RealAdjustWindowRectEx name; native-only
window-view fixtures bind SendMessage to the public host API, without a guest
message owner. No source was removed to make tests link.
[Receipt](n022-winmgrc-result.json), [census](n022-winmgrc-census.json).

Current census1330files/789byte-identical/800nonerror-identical/530different/
7971points/66379lines; delta0/0/0/0/+6/+1169.1307XP/23OpenNT.
N02239/39 delivered, S15 10/14 directories and overall58/159.
Nine completed mirror files since P61 runtime70 Full24/24; next file triggers Full.
No original guest change or S/T closure. Next N023 in DIR order.

## Follow-up diagnostic mapping

Review found that P66 rtlinit.c places four #line directives inside excluded
DBG arms. Selected non-DBG lines therefore do not execute those mappings.
No product algorithm is affected. Correct the positions in the next batch,
count that changed mirror file toward the Full checkpoint, and verify the
selected preprocessor path; the prior textual-only mapping check is insufficient.
