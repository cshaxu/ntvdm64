# N020 PIF declarations - S15 P14

Starting HEAD16b78ce46. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`.
Original Source/XPSP1/NT/public/internal/shell/inc/pif.h,
blobf700f91bd69bd6860c22e498563d9bd65d001fdc. Raw inputs: build/t485-s15/n020-pif.
[Census](n020-pif-census.json).

## Original owner, actual consumer and ABI

Both XP public/internal/shell/inc/pif.h and shell/published/inc/pif.h are
byte-identical. Retain the internal declaration owner and retire the obsolete
public/internal/windows/inc path. The actual selected consumer is the AMD64
PIF addon reader/editor. Its only include-directory selection follows the new
owner. The NTVDM COMMAND consumer already uses the separate XP mvdm/inc/pif.h;
it is unchanged. No original guest modification/rebuild.

XP moves PROPPRG and public flags into shlobj.h and explicitly includes it.
Preserve that original dependency, unused declarations and HANDLE/LONG_PTR
property-service signatures; do not copy the old declarations into compat.
The addon calls none of those property services: it reads/writes STDPIF,
W286PIF30, W386PIF30 and WNTPIF31 disk records under the original pack(1).
Existing byte-for-byte PIF edit tests verify the selected persisted layouts.

All seven pre-existing #error arms remain: missing LF_FACESIZE/FAR,
UNICODE record groups, non-WINNT maximum group and PIF_PROPERTY_SHEETS.
Six retain identical structural keys. XP changes the property declaration
parent from !NTVDM && WINAPI to WINAPI; its child PIF_PROPERTY_SHEETS exclusion
follows that exact XP parent. No artificial conditional or added exclusion.
Only five post-branch #line anchors remain, mapped to XP physical lines.

## Metrics

OpenNT before15nonerror points/19lines; XP after5points/5lines, all #line.
Raw file is not identical because seven existing branches remain excluded.
Global1315files(0),763byte-exact(0),771nonerror same(0),544different(0),
7735points(-10),163271lines(-14).1233XP/82OpenNT. One baseline/path transition,
no added/removed mirror population. No retained functional adaptation.

## Verification

Cached full host build, addon PIF build, runtime14 dual13-image26hash
publication and PIF utility publication passed. Existing create/show/update,
malformed-input, checksum/legacy, five complete memory byte comparisons,
invalid-input no-write and structured root-update tests passed against the
published tools. [Receipt](n020-pif-result.json).
N0202/7 delivered, five remain. Overall55/159 and S15 7/14 unchanged.
One file since P13 Full24/24; no repeated Full. Next winconp.h.
