# S10 P2 evidgen.h review

## Question and inputs

Second ordered S10 file: can HXP-R-00644--00646 restore the original video
access macros without changing the production CPU40 provider contract?
The original is the files.csv-pinned XP object at revision
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, path
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/evidgen.h`.
Source SHA was checked against files.csv before comparison. The
[metrics](s10-p2-evidgen-metrics.json) record both working-file identities.

## Whole-chain findings

The complete 81-slot VideoVector declaration is original. The prior wording
that these macros dispatch an incompatible vector layout was inaccurate and
is superseded here: the table remains intact. The actual boundary is which
provider ordinary host video accesses select.

* Original `base/inc/egacpu.h` declares 38 direct getter/setter pairs for
  CPU40 before including evidgen.h. Production `gfx_updt.c` calls these
  accessors, including clear_dirty and full-screen refresh invalidation.
* Original `base/cpu/src/evid/vglob.c` supplies the direct field operations.
  The current worker build.ninja selects its original translation unit as
  unit_obj_cvidc_vglob_obj. This is not a newly invented accessor algorithm.
* Original `cvidc/vglfunc.c` connects the vector setter to
  S_0417_CiSetVideodirty_total_00000036. Its original sinit013.c implementation
  writes r2, r21, r1 and additional shared generated execution scratch before
  storing dirty_total. Restoring all ordinary macros would route the host
  refresh calls through that implementation instead of vglob's direct store.
* The existing production-linked C-VID fixture explicitly calls the generated
  setter and verifies r21 changes, then exercises the direct setter from a
  refresh thread while verifying all 32 scratch locations stay unchanged.
  P1's actual run passed (build/t485-s10/p1/cvidc.exit = 0 and cvidc.log PASS).
  That is existing evidence of the provider distinction, not a new P2 run.
* The original vglob.c comment explicitly excludes latch accessors because
  they remain in EvPtrs. Both latch macros must stay usable outside the guard.

## Per-point disposition and minimality

| Frozen point | Frozen added/deleted | Conclusion |
| --- | --- | --- |
| HXP-R-00644 | 2/0 | Retain one CPU40 guard and the moved original latch setter; ordinary macros otherwise shadow direct accessors. |
| HXP-R-00645 | 0/1 | This is the other end of that move, not a removed function. The latch operation remains available. |
| HXP-R-00646 | 1/0 | End the guard before the original write/read/mark selectors. |

Deleting unused ordinary macros would create more original omissions. Leaving
the latch setter in its original position requires two guarded regions instead
of one. No wrapper, alternate state, vector truncation or algorithm copy is
needed. All three residual points remain pending owner decisions with the
corrected evidence; none is counted as restored or newly approved retain.

## Changes and verification boundary

Restore two CRLF endings to the original LF; all normalized source text,
preprocessor selections and binding remain unchanged. Byte-sensitive non-error
metrics stay **3 points / 8 added-plus-deleted lines**: both changed endings
were on already-added adaptation lines. This is not a claim of zero format
difference before editing. There are no #error bodies to exclude.

Source-line simulation checked all identical nonblank original lines in both
CPU40 selections: 93 selected CPU40 lines and 169 legacy-selection lines retain
their original logical numbers. The moved latch setter is explicitly #line132;
the ordinary setter range resumes at133 and final selectors at171.

No new build, publication, test, commit or push is claimed for P2. Per owner
batching rules this format-only change accumulates with the next complete
file. P1 results remain evidence for the unchanged functional binding, not
final-package acceptance of the next batch. Next ordered file is sevid000.c.

Current local source-review progress: 2/30 files, 7/1060 frozen points;
1/30 delivered, 4/1060 final-verified in P1. Zero fully restored frozen
points, seven evidence-backed residuals (three newly reviewed), 1053 unreviewed.
The thirty-file total stays 185 points/101302 lines, net -16/-1561 from the
fixed S10 baseline. [Current census](s10-current-census.csv) updates the header
hash without changing the immutable P1 census.
