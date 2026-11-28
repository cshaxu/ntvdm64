# S15 P51 N022 multiline EDIT creation - delivered

## Scope and inputs

Start HEADa7a437941, runtime59 delivered. Whole edmlonce.c migrates from
OpenNT blob2c00a91882b2e685bb9c48a1fada9045118802f1 to XP SP1
blob6994d2a36049e9e02f040f24c222e5a7fbe15326 at pinned revision
a2f6d7c93aa4f11efc51147566d5961c59a8ef89. Before nonerror1point/233lines.
Global1329files/784byte-identical/795nonerror-identical/534different/
7831points/94922lines. No original guest changes/rebuilds.

## Complete original restoration

Use exact XP bytes without exclusions or adaptations. MLCreate's signature
was already paired by P49. XP ECCreate in edecrare.c403-405 initializes format
before MLCreate consults it; the old duplicate initialization and obsolete
SUPPORT_LPK framework are absent in XP. Preserve original allocation failures,
scroll/wrap configuration, font and initial-text setup. gHighestUserAddress
is bound to the existing host address-limit provider, as detailed below.
No new algorithm, callback, state or dependency. Original source is directly
composable; no replacement implementation is needed.

## Final verification and delivery

runtime60 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow, genuine guest SOL Options and Full24/24 pass.
[Receipt](n022-edmlonce-result.json), [census](n022-edmlonce-census.json).
N02222/39; overall57/159 and S15 directories9/14 unchanged. P51 is the new
completed ten-file Full checkpoint. No original guest change or S/T closure.

## Direct host binding

Pinned XP client/clinit.c initializes gHighestUserAddress from system basic
information. This standalone control graph does not compose that private USER
initialization. Its existing wow_edit_native_highest_user_address obtains the
same host maximum user address through GetSystemInfo and preserves last error.
Bind the XP spelling to this existing provider in control-client/precomp.h,
with ULONG_PTR shape. No second variable, cache or initialization lifecycle.
The first build exposed the missing declaration at edmlonce.c95 (C2065);
retain that failed attempt. The repaired final build passed before publication.


## Mirror accounting

edmlonce.c OpenNT1point/233lines becomes exact XP0points/0lines. There are no
#error exclusions, #line insertions, newline or EOF differences from XP.
Global before:1329files/784byte-identical/795nonerror-identical/534different/
7831points/94922lines. After:1329/785/796/533/7830/94689.
Delta:0/+1/+1/-1/-1/-233. Baselines1289XP/40OpenNT.
