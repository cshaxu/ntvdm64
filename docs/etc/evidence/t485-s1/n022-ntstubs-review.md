# S15 P64 N022 client ntstubs migration - delivered

## Fixed source and current scope

Current delivered HEAD72f7c46d8/runtime72. This is client/ntstubs.c, not the
previously retired kernel/ntstubs.c. XP SP1 revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/client/ntstubs.c, blob
1debffc3f0955d76831a892d7205451c2341e3dd,3050 lines. Current whole-file
OpenNT baseline blob8b6cbd785ffe0588c24f03e99110f38208c44939.
Pre-migration nonerror metric1point/2475lines.

## Verified migration decisions

| Current route | XP evidence | Required disposition |
| --- | --- | --- |
| GetClassWOWWords | ntstubs.c140-168 uses KPBYTE and GCLP_WOWWORDS | Keep complete original query; update the existing borrowed-class view ABI. No new class registry. |
| GetControlBrush | Absent from client file; usercli.h1658 aliases NtUserGetControlBrush | Remove old client wrapper and object targets. Bind existing scoped brush provider, preserving native-call/borrow lifetime. |
| GetControlColor | Absent from client file; usercli.h1659 aliases NtUserGetControlColor | Remove old client wrapper and object targets; retain current native color boundary. |
| _SetWindowLong | XP _SetWindowLongPtr908-1066; usercli.h1425 maps to _SetWindowLong on x86 | Use original pointer-width body and x86 alias. Preserve actual HWND-owned dialog procedure seam; no private modern CPD layout. |
| _DefSetText | ntstubs.c1494-1518 retains LARGE_STRING capture and service call | Restore complete original body and mapping. |

XP kernel/ntstubs.c5028 onward validates NULL HDC and the inclusive
WM_CTLCOLORFIRST..WM_CTLCOLORLAST range before xxxGetControlBrush/Color.
The local brush provider currently retains HWND state but does not own this
validation. Removing the old wrapper must place the original validation at
that existing service boundary. Color already reaches installed native USER.
Do not silently drop the old NULL-HDC behavior or call a provider outside its
borrowed window scope. Existing brush fixture requires no provider call on
NULL HDC and covers reentry, invalid window, retained parent, callback exception
and retirement. Extend it for the XP message range, not a second fixture.

Current production targets are separate class-query, text, color, brush and
private-word object contexts in tools/build/wow32/units.cmake. The control
suite additionally composes native and original dialog-procedure contexts.
Remove the two obsolete object targets and their link references together;
update tests to the same existing providers. Original guest is not modified.

## Whole-file disposition

| XP range | Boundary and disposition |
| --- | --- |
| 1-23 | Original notice, CLIENTSIDE, dbt.h, ntsend.h, cfgmgr32.h and unused remote-keyboard declaration retained. precomp selection remains per current independent object context. csrhlpr.h is omitted: its CSR heap debug services are within the original DBG arm, and no selected body uses it. |
| 25-138 | Session color policy, internal FindWindow, per-logon keyboard/MUI policy and DDE event packet transport remain owned by installed USER or existing outer task/DDE paths. Their private SERVERINFO/CLIENTINFO and NtUser service contracts are not provided by the class-query borrowed view. IMM_PER_LOGON arms retain original exclusion. |
| 140-168 | Complete XP GetClassWOWWords restored. |
| 170-907 | NtUserInitTask, shared kernel memory handles, hooks/notifications, private message-pump and menu/window/input entry stubs require their original session-side service/capture contracts. Those functions are not the existing host's task, hook, menu or message providers. Do not replace those owners while importing this client slice. Original MESSAGE_PUMP_HOOK excluded; unconditional omitted code remains visible as mirror diff. |
| 908-1066 | Complete XP pointer-sized setter, with existing HWND-owned dialog-procedure seam. Original BUILD_WOW6432/DBG exclusions remain. |
| 1068-1490 | Original _WIN64 setter and DBG CSR diagnostics excluded under original guards. Other keyboard translation, region, internal-text, window-station and cursor-object stubs remain native system boundaries; they consume private NtUser/CLIENTINFO layouts rather than the retained x86 window view. |
| 1494-1518 | Complete XP _DefSetText restored. |
| 1520-2105 | Private versioned window creation, keyboard installation, synthetic input and NtUserMessageCall transport remain outside this slice. Existing window creation/enrollment and message capture own the product path; replacing them requires their separately registered owner migration, not duplicate public/internal entrypoints here. |
| 2107-2161 | All three original character-position helpers restored, although not currently called. They compose through public code-page functions and the already selected WCSToMBEx provider. No evidence warrants deleting them. |
| 2163-2421 | Original LATER reconversion arm excluded, not a new product condition. |
| 2423-3050 | Remaining private message-call capture, broadcast and device-notification/PNP client RPC are native system boundaries. No copied service manager or second message transport is admitted. Original trailing DBG arm is excluded; unconditional omissions remain recorded as diff. |

This does not claim complete USER32 import. Historical range admission is
m0-t420-s40-wow-user-profile (GetClassWOWWords/window creation) and
m0-t447-s3-control-source-audit (control/service/dialog boundaries).
The actual selected callers/providers and build variants were rechecked this P.
No new conditional framework hides an unconditional omitted original service.
The source function inventory is a navigation aid, not a C parser or test result.

## Implemented changes and verification

The two obsolete client wrapper bodies and both production/test object targets
are removed. Existing service aliases retain native-call/window-borrow ownership.
The brush provider validates DC and message range at its existing service edge;
the fixture adds lower/upper out-of-range checks without a new test framework.
Original x86 _SetWindowLongPtr alias and fixed-width kernel-pointer identity
are supplied at existing binding headers. No new state machine or guest change.

Final file metric30points/2236lines (ten original excluded arms), versus
1point/2475lines under OpenNT. Delta+29points/-239lines; restored LF exposes
separated actual differences. Global1329/788/799/530/8108/66388;
1302XP/27OpenNT. Source line mapping passes. Residual ranges are above.
Initial build failed due to an omitted local #endif; corrected before DWLP_USER.
Second build exposed x86 setter aliases in a dialog-only block; moved to the
shared binding. Initial controls39/40; corrected fixture cleanup count and
retried only the failed brush fixture. No product change after publication.
Cached full build, runtime73 dual13-image26hash publication, controls40/40,
real CCPU window-borrow, SOL Options and genuine Win16 controls pass.
[Receipt](n022-ntstubs-result.json), [census](n022-ntstubs-census.json).
N02235/39; S15 9/14 directories, overall57/159.
P61 runtime70 Full24/24 remains checkpoint; three files since it.
