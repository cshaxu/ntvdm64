# N020 WOW GDI declarations - S15 P18

Starting HEAD f99cb9d82. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, original
Source/XPSP1/NT/public/internal/windows/inc/wowgdip.h,
blob15c02b0baadaa7e327cdbf8dba1a2ad625063ac3.
Raw before/XP inputs: build/t485-s15/n020-wowgdi; three-way research:
build/t485-s15/n020-review. [Census](n020-wowgdi-census.json).

## Entire-file review

Restore complete original XP bytes. Relative to normalized old OpenNT, XP
changes the copyright notice; all seven GDI service declarations remain
unchanged. Actual includes are WOW wdib.c, wgdi.c, wgdi31.c and wow32fax.c.
No callback layout, implementation, calling convention, include path or provider
changes. Keep unused declarations. There is no #error arm or #line anchor.
No original guest changes or rebuilding.

## Mirror census

Registered OpenNT before1nonerror point/38lines; XP after0/0 and byte-exact.
Global1318files,767byte-exact,777nonerror same/541different,
7727points/157354lines. Delta0files,+1exact,+1same/-1different,-1point/-38lines.
1240XP/78OpenNT; one baseline transition, no population change.

## Verification

Cached full build, runtime18 dual13-image26hash publication, output/font
contracts, real Win16 GDI marker and WINMINE/WRITE visible windows passed.
[Receipt](n020-wowgdi-result.json). No gameplay or deployed fax-driver claim.
N0206/7 delivered, next wowuserp.h. Overall55/159 and S15 7/14 unchanged.
Eight files since P13 Full24/24 including three required original headers;
no repeat Full.
