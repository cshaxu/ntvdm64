# N021 SDK first three - S15 P20

Starting HEADfb4650f78. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`.
All originals are Source/XPSP1/NT/public/sdk/inc/<filename>:
- callconv.inc: `86ca9c6292bb594597c4819f2f1ff7a3498af732`.
- devioctl.h: `d357c7c2389af1086ee890c100e14597721b3820`.
- dlcapi.h: `923a1401a3a8b8df86153636d3ffbd6d2d478ec9`.

Raw originals/before bytes and branch evidence: build/t485-s15/n021-first.
[Census](n021-first-census.json).

## Ordered whole-file review

1. callconv.inc already matches XP bytes and is unchanged. Existing assembly
   consumers include RTL largeint/movemem and WOW callpr32/fastwow. Do not
   recount its baseline as newly migrated.
2. devioctl.h restores complete XP bytes. The local extra INFINIBAND device
   constant has no source/test consumer; no runtime dispatch is removed.
   Preserve original XP DEVICE_TYPE_FROM_CTL_CODE, access definitions and
   all unused device constants. Current consumers include nt.h and VCDEX.
3. dlcapi.h restores XP pragma-once and both adapter-name/number declarations.
   No competing local declarations/providers were found. Six redirector
   consumers retain their include spelling and existing LLC contracts.
   Preserve the two existing original C++ excluded single-line arms; they
   occupy the original line and need no #line additions. No algorithm change.

## Mirror metrics

| File | Before nonerror points/lines | After | Byte-exact XP |
| --- | ---: | ---: | --- |
| callconv.inc | 0/0 | 0/0 | yes, already migrated |
| devioctl.h | 1/270 | 0/0 | yes |
| dlcapi.h | 1/1816 | 0/0 | no, two original excluded arms only |

Before uses individually registered OpenNT for the two newly migrated files
and XP for callconv.inc. LF/EOF are counted, not normalized out. No #line
remains in the three files. Global1318files,768byte-exact,779nonerror same/
539different,7729points/154750lines. Delta0files,+1exact,+2same/-2different,
-2points/-2086lines.1243XP/75OpenNT; two baseline transitions, no additions.

## Verification

Final cached full build, runtime20 dual13-image26hash publication, RAP11/11,
both existing redirector lifecycle fixtures and real guest NetAPI passed.
[Receipt](n021-first-result.json). This does not claim physical DLC adapter
traffic acceptance. N0213/17 delivered, next nt.h; overall56/159 and S15 8/14
unchanged. Three reviewed files since P19 Full24/24; no repeat Full.
No original guest modification or rebuilding.
