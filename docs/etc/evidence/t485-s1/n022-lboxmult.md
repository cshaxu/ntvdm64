# S15 P59 N022 multi-column ListBox - delivered

## Scope and source

Start HEAD7ac6d7405/runtime67, N02229/39. Restore lboxmult.c byte-for-byte
from fixed XP a2f6d7c93aa4f11efc51147566d5961c59a8ef89, blob
6343456496dcfdd0d952bc3616974bea4be45ef4. Prior OpenNT blob
0eab021a9d003bc0e437e19e9a1119e8c6505ee2. Full original source including LF/EOF.
No conditional exclusion, shim, extra algorithm, #line or original guest change.

XP adds right-aligned line/page/top/bottom scrolling, thumb-position conversion
and restores fSmoothScroll at SB_ENDSCROLL. P56/P58 already provide the original
LBIV fields and xxxNewITop path. No production prerequisites need changing.
Extend the existing independent Win16 probe with a32-item multi-column list,
column width40, one-column horizontal movement and return, then end-scroll.
It checks the actual multi-column handler, not only a visible scrollbar; it
is not a claim of RTL guest coverage. All prior controls checks remain.

## Accounting

File OpenNT1point/222lines -> XP0/0. Global1329files/785byte-identical/
796nonerror-identical/533different/8073points/70889lines ->
1329/786/797/532/8072/70667. Delta0/+1/+1/-1/-1/-222.
1297XP/32OpenNT. N02230/39 on delivery; eight files since P51 Full24/24.

## Final verification and delivery

runtime68 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow, genuine SOL Options and Win16 controls including
multi-column horizontal scrolling pass. [Receipt](n022-lboxmult-result.json),
[census](n022-lboxmult-census.json). No original guest rebuild or modification.
N02230/39; overall57/159 and S15 directories9/14 unchanged. Eight completed
mirrors since P51 runtime60 Full24/24; no new Full or S/T closure claimed.
Global1329/786/797/532/8072/70667; delta0/+1/+1/-1/-1/-222.
Baselines1297XP/32OpenNT. Original bytes match, no residual #error or #line.
