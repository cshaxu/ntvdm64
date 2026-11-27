# N020 GDI private declarations - S15 P17

Starting HEAD eeaff96f4. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, original
Source/XPSP1/NT/public/internal/windows/inc/wingdip.h,
blob e757151f39218cb0191a55b9ae6dbd6883e3e052.
Raw inputs and three-way evidence: build/t485-s15/n020-review and n020-gdi.
[Census](n020-gdi-census.json).

## Complete file and actual consumers

Restore the entire XP private GDI header at its unchanged original path.
WOW wgfont.c, wgdi.c and wkman.c consume font metrics and application
compatibility declarations; USER client/rtl and compat drawing precompiled
headers also include it. Preserve XP added display, color, drawing-stream,
font and printer declarations even when currently unused. No new algorithm,
provider, feature-selection macro or guest modification is introduced.

Both original C++ wrappers remain the existing excluded single-line arms.
Their replacement occupies the same physical line, so old redundant #line
anchors are removed. The third OpenNT excluded DBCS branch has changed in XP
to FE_SB, which the real WOW profile selects. Keep the original XP
GFRI_FONTMETRICS constant rather than carrying an obsolete exclusion into a
selected branch. No production references to that constant were found; its
original declaration is harmless and retained. Do not claim unchanged original
conditions where XP itself changed them.

## Mirror differences

Before: registered OpenNT,1nonerror point/854lines. After: XP,0points/0lines;
two excluded single-line bodies only. Raw bytes remain nonidentical because
of those #error substitutions. Newlines and original diagnostic positions
match XP without added #line. Global1318files,766byte-exact,
776nonerror same/542different,7728points/157392lines. Delta+3files,+3exact,
+4same/-1different,-1point/-854lines.1239XP/79OpenNT.

## Verification

Cached full build and runtime17 dual13-image26hash publication passed.
Existing embedding31/fax135/font16 assertions, real Win16 GDI marker,
font compatibility and visible WINMINE/WRITE windows passed.
[Receipt](n020-gdi-result.json). These are not gameplay or deployed fax-driver
acceptance. N0205/7 delivered; overall55/159 and S15 7/14 unchanged.
Seven mirror files including three required dependencies since P13 Full24/24;
no repeat Full. Next wowgdip.h.

## Required XP declaration dependency

The first real cached build failed on missing POLYPATBLT/PPB_BRUSH in USER
menu and drawing callers. XP moved these declarations from wingdip.h to
windows/core/w32inc/w32gdip.h. Import that whole original file plus its
w32w64a.h and public/internal/windows/inc/w32w64.h dependencies byte-exact.
No old declaration copy or header overlay is created. The original non-WOW64
DECLARE_KHANDLE expansion preserves HBRUSH/HBITMAP types on this product;
all unselected original WOW64 macros remain intact.

Existing compat control-client/control-drawing/control-draw-state composition
headers include the original w32gdip.h. No drawing algorithm changes. This is
a required dependency under the current migration scope, not an independent
new component. The three new mirrors each add zero diff points/lines; their
population delta is reported separately. Failed build logs are retained as
build/t485-s15/n020-gdi/initial-build.log. Final cached rebuild and targeted delivery passed.
