# N022 clmenu.c migration review

Pinned XP blob c55123fd3f0a97de7ebeece02234b44cda614c5a;
OpenNT blob cd37b39b8a942607264ed9c8946b1613a4c6c991.
Whole file reviewed. Old MEMPHIS selections do not exist in XP: retain the
unconditional XP code, including ValidateMENUITEMINFO and ValidateMENUINFO.
No invented exclusion guard. Current candidate has no #error exclusion.

Original XP changes retained: ULONG_PTR IDs/data; ULongToPtr zero extension;
MIIM_FTYPE/STRING split; six-argument NtUserThunkedMenuItemInfo call;
MENUEX right-order propagation; native 95-sized MENUITEMINFO conversion;
validation masks, bitmap compatibility and original invalid-parameter flow.

Three residual points: include binding; original SetMenu forwarding omitted
because existing native wrapper owns registration/refresh; GetMenuInfo private
MENU-field reads omitted because menu objects belong to modern USER32. Modern
GetMenuInfo remains available. No shadow MENU or replacement parser introduced.
Retained InternalInsertMenuItem forwards through existing native insertion;
validation uses native IsMenu/GetObjectType at original handle-check boundary.

XP kernel mnchange.c _SetMenuFlagRtoL only sets MFRTL; the one-HMENU syscall ABI
in XP ntuser.h is also exported by current native win32u.dll (dumpbin checked).
The binding resolves that native provider; missing service reports
ERROR_CALL_NOT_IMPLEMENTED. No version/program special case or queue.

Existing resource fixture now covers original old-size conversion, immutable
input flags, invalid mixed masks/states and MENUINFO return/error behavior.
MENUEX right-order parsing and actual native RTL entry are exercised alongside
existing ordinary/owner-draw/error cleanup cases. Resource projections use
existing function-body extraction for three actual source functions, not an
open-ended suffix that would pull unrelated new validation definitions.

Before: 1 point/868 nonerror lines against OpenNT. Candidate: 3 points/49 lines
against XP; +2points/-819lines. Whole census candidate1329files,781byteexact,
792nonerror same/537different,7807points/126894lines;1270XP/59OpenNT.
LF restoration exposes three actual blocks. No source-total-line metric used.

First build failed for three missing original private constants; restored
MFT_NONSTRING, MIM_MASK and MAXUSHORT with original values. Final cached build, dual publication, menu resource/validation/RTL and real SOL Options passed.
The first real RTL test rejected MFT_RIGHTORDER because B still used OpenNT
MFT_MASK/MFT_OLDAPI_MASK. Both now use original XP values36B64/6B64;
runtime34 is superseded by runtime35 after rebuild and revalidation.
The native lifecycle link exceeded CMD8191 characters (actual8222); existing
link inputs now use a CMake-generated response file in unchanged order.

[Receipt](n022-menu-result.json), [census](n022-menu-census.json).
N0224/39; overall57/159; S15 9/14. Nine changed mirror files since P31 Full;
next ordered clmsg.c reaches the next ten-file Full checkpoint.
