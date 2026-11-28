# S15 P57 N022 ListBox data operations - delivered

## Scope and inputs

Start HEADc331eefc7, runtime65. Complete lboxctl1.c switches to fixed XP SP1
blobcd7f7d807ddc07e2c04751c774b8f6f1aa9ad1f7, revision
a2f6d7c93aa4f11efc51147566d5961c59a8ef89. Before OpenNT nonerror10points/82lines
includes P56 direct GrowMem/xxxLBInitStorage. Baseline1329files/785byte-identical/
796nonerror-identical/533different/8076points/77980lines,1294XP/35OpenNT.

## Complete implementation and direct dependencies

Restore all XP scroll-position, item data/text, insertion, allocation, sorting,
delete notifications and cleanup code. Preserve the original FE_SB framework;
actual control-client production flags do not select FE_SB, unlike unrelated
WOW thunk objects. Retain #error for that positive arm and #line anchors.
No original guest modification or rebuild. Existing P56 LBIV layout, LBEvent,
GrowMem and allocation contract supply the original dependencies.

Pair LBGetItemData's LONG_PTR declaration and LBItem/LBODItem ULONG_PTR data
fields in usercli.h; the fixed x86 layout remains unchanged. That header is
still a partial migration against OpenNT. Preserve all unrelated declarations.

XP Win16 US-English sorting invokes pfnWowIlstrcmp. Original wow32.c already
supplies pfnIn.pfnWowIlstrsmp=WOWlstrcmp16. Original wcall16.c owns conversion,
guest stack allocation, RET_LSTRCMP callback and result conversion. Our USER
registration copied the input structure but did not publish this original
global. Restore the assignment in its existing C-owned set-callbacks entry,
which also runs when B restores a previous callback snapshot. No separate
registration, null fallback, sorting algorithm or second lifecycle is added.
XP client.c:2813 is the original assignment; production composes its existing
registration subset in clglobal.c. The producer and consumer retain original
C2/C1 registration semantics. This adds one line within an existing diff block.

The independent Win16 controls probe now creates a sorted list, inserts Beta
then Alpha and verifies Alpha at index0. This checks real guest sorting; the
special RET_LSTRCMP branch is conditional on the original thread locale and
TIF_16BIT, so the result alone is not an instruction trace of that callback.
The original guest is immutable; only our independent test probe is rebuilt.

## Candidate accounting

lboxctl1.c10/82 ->2/2, both residuals #line. clglobal.c8/157 ->8/158.
usercli.h165/583 ->168/589. Global8076/77980 ->8071/77907,
delta-5points/-73lines;1295XP/34OpenNT. File equality counts unchanged.
Original conditions and source line mapping checked; full source body retained.

## Final verification and delivery

runtime66 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow, genuine SOL Options and Win16 controls including
sorted-list insertion/text pass. [Receipt](n022-lboxctl1-result.json),
[census](n022-lboxctl1-census.json). No original guest rebuild or modification.
N02228/39; overall57/159 and S15 directories9/14 unchanged. Six completed
mirrors since P51 runtime60 Full24/24; no new Full or S/T closure claimed.
Global1329/785/796/533/8071/77907; delta0/0/0/0/-5/-73.
Baselines1295XP/34OpenNT. Newline/EOF counted; existing #error/#line retained.
