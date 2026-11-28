# S15 P62 N022 variable-height ListBox

## Scope and identity

Start df0ce6764044762dab63fac6b9283b904b1064ab/runtime70.
Restore windows/core/ntuser/client/lboxvar.c from XP SP1
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, blob
94b748462d0328c6d5f12b09a2dad346bf3c3209. Previous OpenNT blob
86d22c19ef0fa7403de0b1f761757503df32158b.

All five height/page/scroll functions remain original. XP differs from the
current normalized file only in copyright, a comment typo and blank whitespace.
No algorithm, ABI, provider, exclusion, diagnostic mapping or guest change.
No new shim or state. Entire file becomes byte-identical to XP, including LF/EOF.

## Production consumers

lboxrare.c uses the height get/set helpers for item-height messages and sizing.
lboxctl2.c uses them for item rectangles, visible counts, page navigation and
scroll-distance calculation. Their current XP LBIV/LBItem/LBODItem contracts
already match; no paired-header or implementation change is needed.

## Census

File1point/466lines to0/0. Current delivery starts at
1329/787/798/531/8072/68258. Final census and hashes are recorded on verification.
N02232/39 delivered before this batch. P61 Full24/24 is the current checkpoint.

## Final verification and delivery

runtime71 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow, genuine SOL Options and Win16 controls pass.
[Receipt](n022-lboxvar-result.json), [census](n022-lboxvar-census.json).
Initial publication used legacy powershell.exe without Get-FileHash after
successful staging. Re-staging correctly refused the existing root. The copy/hash
phase alone resumed under pwsh; no rebuild or repeat of passing tests.
These existing tests do not claim coverage of every variable-height owner-draw
branch. Original algorithms, ABI and guest binaries remain unchanged.
N02233/39; S15 directories9/14, overall57/159. One file since P61 Full24/24.
Global1329/788/799/530/8071/67792; delta0/+1/+1/-1/-1/-466.
Baselines1300XP/29OpenNT. Exact XP bytes; no residual #error or #line differences.
