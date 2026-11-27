# N020 GDI driver declarations - S15 P16

Starting HEADd774ac72f. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`.
Source/XPSP1/NT/public/oak/inc/winddi.h,
blob57a6fd53a537421addbf507c98ce2940d3a61f84. Raw originals and branch mapping:
build/t485-s15/n020-ddi; three-way research: build/t485-s15/n020-review.
[Census](n020-ddi-census.json).

## Actual owner, declarations and selected branch

Move the complete original XP header to public/oak/inc and retire the obsolete
public/internal/windows/inc path. Add the OAK path to the existing original
WOW source profile; do not add a forwarding header. Ninja's actual wow32fax.c
object dependencies confirm this XP OAK file, not the modern SDK copy.
The independently compiled compat/wow_fonts.c still uses the host SDK header;
its source/profile and provider behavior are unchanged.

WOW's ConvertGdiInfo uses the original PGDIINFO layout and size, converting
Win16 fields into the selected original structure. Preserve all XP definitions,
including newly added driver declarations, GUID and DirectDraw/D3D declarations,
FLOATL and user-mode conditional alternatives. No new driver implementation,
private GDI backend or broad feature-selection macro is introduced.

The one existing excluded FLOATOBJ arm follows XP's original condition
_X86_ && !USERMODE_DRIVER. The earlier OpenNT _X86_ guard names the same
selected exclusion; no new arm is excluded. Preserve XP's separate FLOATL
condition including BUILD_WOW6432 rather than reusing this guard. Two post-arm
#line anchors restore original XP diagnostic positions. No guest changes.

## Mirror metrics

OpenNT before1nonerror point/5020lines; XP after2points/2lines, only #line.
The old whole-file CRLF difference joined the two real line-mapping changes
into one block. Restoring original LF exposes them: +1point, -5018lines.
This is the owner's standing approval for original newline/line-map restoration,
not an extra functional adaptation. One original #error arm remains, so raw
bytes are not identical. Global1315files(0),763byte-exact(0),772nonerror same(0),
543different(0),7729points(+1),158246lines(-5018).1235XP/80OpenNT.
One baseline/path transition, no population change.

## Verification

Cached full build and runtime16 dual13-image26hash publication passed.
Existing embedding31, fax135 and font16 assertions, real Win16 GDI marker,
font compatibility cases and visible WINMINE/WRITE windows passed.
[Receipt](n020-ddi-result.json). Native fax failure fixtures do not prove a
deployed legacy fax driver; window observations do not prove gameplay.
N0204/7 delivered, three remain; overall55/159 and S15 7/14 unchanged.
Three files since P13 Full24/24; no repeat Full. Next wingdip.h.
