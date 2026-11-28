# S15 P60 N022 ListBox auxiliary operations - delivered

## Scope and original identity

Start HEAD2af979122/runtime68, N02230/39. Restore whole lboxrare.c from fixed
XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, blob
058f45387833216c7881a6c51f32a4490b5bb9fd; previous OpenNT object
d1b631b8e8df3f6893d80691fc9857784b640b6c. Preserve original LF/EOF bytes.

XP initializes RTL/right alignment and smooth scrolling, corrects initial
integral-height resizing and zero-size multi-column invalidation, and uses
pointer-sized notifications. Full original includes the LBEvent previously
imported as a prerequisite. No new helper, binding, lifecycle or guest change.

## Old free adaptation review

Original usercli.h UserLocalFree maps directly to HeapFree(pUserHeap,0,p).
LBSetTabStops allocates its owned tab array on that same heap. Current added
if(UserLocalFree(...)) is not needed by the XP body, which releases then clears
the pointer without interpreting a LocalFree-style result. Valid live storage
has the same success behavior. Do not claim a new recovery contract for invalid
heap pointers. Other replacement/destruction paths already use unconditional
release; there is no separate retry owner. Existing actual Win16 probe sets
64 tab stops, resets to defaults and later destroys the list. Restore XP exactly.
The old NEVER branch no longer exists in XP; no artificial exclusion retained.

## Mirror census

File5points/40lines ->0/0. Full population1329/786/797/532/8072/70667 ->
1329/787/798/531/8067/70627; delta0/+1/+1/-1/-5/-40.
1298XP/31OpenNT. N02231/39 after delivery; nine mirrors since P51 Full24/24.
No residual functional, format, #error or #line difference in this file.

## Directory-order correction

Current census confirms lboxctl3.c is still OpenNT, despite P58/P59 next-file
notes skipping it. It was not counted as migrated. Finish this running delivery,
then process lboxctl3.c before lboxvar.c; preserve the complete scope.

## Final verification and delivery

runtime69 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow, genuine SOL Options and Win16 controls including
tab-stop reset, scrolling and destruction pass. [Receipt](n022-lboxrare-result.json),
[census](n022-lboxrare-census.json). No original guest rebuild or modification.
N02231/39; overall57/159 and S15 directories9/14 unchanged. Nine completed
mirrors since P51 runtime60 Full24/24; no new Full or S/T closure claimed.
Global1329/787/798/531/8067/70627; delta0/+1/+1/-1/-5/-40.
Baselines1298XP/31OpenNT. Original bytes match, no residual #error or #line.
