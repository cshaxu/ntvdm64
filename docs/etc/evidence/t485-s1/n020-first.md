# N020 first file: USER resource declarations - S15 P13

Starting HEAD96c4d1d70. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`.
Original Source/XPSP1/NT/windows/core/w32inc/newres.h,
blob492f641b5fa984a47dac5c2c1f198e61e07361bf.
Raw three-way inputs and runs: build/t485-s15/n020-first.
[Census](n020-first-census.json).

## Original owner and complete change

XP contains identical newres.h bytes in windows/core/w32inc and shell/shell32.
The current actual consumer is USER clres.c, so use the former original owner.
The original inventory's public/internal/windows/inc/newres.h no longer exists
in XP; retire that old path, do not leave a forwarding duplicate. The XP text
changes only one comment versus the OpenNT original. All resource structures,
field order, pack(2), pack reset and RC_INVOKED condition remain original.

clres.c's existing explicit include names the new owner path. This is the only
caller edit. It remains on its own OpenNT baseline until N022 migration.
The original-to-before and original-to-after raw diff opcode intervals are
identical. All excluded bodies and #line bytes are unchanged. Therefore the
existing nonerror measurement is reusable for this path-only replacement;
this does not reclassify clres.c as XP or claim its adaptations resolved.
No algorithm, condition, guest binary or guest build change.

## Counts

newres.h before OpenNT1point/160lines, after raw XP exact0/0.
clres.c unchanged nonerror1point/6424lines, including its existing raw newline
difference; not hidden by normalization. No #error or #line changes.
Global1315files(0),763byte-exact(+1),771nonerror same(+1),544different(-1),
7745points(-1),163285lines(-160).1232XP/83OpenNT. One baseline/path transition;
no population change. N0201/7 delivered, directory not yet delivered. Overall
remains55/159 and S15 7/14. Next inventory file pif.h, after this checkpoint.

## Verification

Cached full build, runtime13 dual13-image26hash publication and
[Full24/24](n020-first-full.json) passed. [Receipt](n020-first-result.json).
Nine preceding reviewed inventory files plus newres.h reach ten; paired clres.c
also reviewed. Full checkpoint resets to zero. N0201/7 delivered, six remain;
overall55/159 and S15 7/14 unchanged. Next pif.h. The approved Win95 exit1
boundary remains unchanged; no claim of normal exit-code correction.
