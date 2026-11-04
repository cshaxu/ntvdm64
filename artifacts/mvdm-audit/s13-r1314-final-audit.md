# S13 mirror condition and line audit: current evidence

This is a structured audit checkpoint, not a claim that every original line or
every supported compilation context has been certified. The per-file
[checklist](s13-r1314-file-audit.csv) records both SHA-256 identities and
branch evidence. The [branch checklist](s13-r1308-branch-checklist.csv)
contains one row per current condition arm. The [summary](s13-r1314-summary.json)
gives the same accounting. The source paths and original identities are those
of the existing 1,056-file S13 census; guest sources were not changed.

On the current source, 182 files have identical bytes to their recorded OpenNT
baseline and 874 differ. All originals in this census were accessible. The
7,602 current arms include 3,199 structurally matched historical hits, 144
current hits, 4,005 `#error` exclusions, 242 empty/structural arms, and 12
retained arms without an observed host hit. A hit in any admitted observed
context is sufficient evidence to retain that arm; absence of a hit is not
itself proof that it may be removed. The current branch rows were compared
against all parsed source directives; one stale nested row was removed after
its containing unsupported arm was replaced.

The current preprocessing probes and their limitations are recorded in
[r1311](s13-r1311-targeted-preprocessing.json),
[r1312](s13-r1312-targeted-preprocessing.json), and
[r1313](s13-r1313-extra-contexts.json). Identical shadow and baseline
preprocessor output establishes that instrumentation did not change the
observed output for those contexts; it is not a runtime test or coverage proof
for an unobserved configuration.

A structural scan of all 4,005 arms labelled `excluded-error` found no
remaining code or nested conditional inside those arms: their bodies contain
only `#error`, optional `#line`, comments, and whitespace. This checks their
shape, not whether every exclusion was justified or every line anchor is
correct.

The x86 cached root build, the separately invoked NTVDM worker and WOW32
graphs, and the x64 cached build passed. The coherent ten-image package was
published and hash-verified at both release destinations. Against the previous
package, eight images are byte-identical and NTVDM/WOW32 differ only in their
four-byte PE timestamps ([comparison](s13-r1315-build-image-compare.json)).
Per owner instruction, runtime tests were not repeated for this comparison;
the earlier Full result is not new runtime evidence for the present audit.

The 12 not-observed arms affect six files: `wowcomm.h`, `wowfax.h`,
`wowinfo.h`, `wowmmcb.h`, `gdpvar.h`, and `wownt32.h`. The first four are
shared with original WOW16 guest sources, whose compilation is prohibited in
this task. The latter two include guards are suppressed in observed host
contexts but their files are still referenced by actual include chains. This
is a disposition for further proof, not a claim that these arms hit or are
safe to remove. `wowcomm.h` and `gdpvar.h` are byte-identical originals.

The branch checklist separately identifies error-line mappings that have
supporting original anchors and those that remain ambiguous. It does not
certify the line of every surviving selected statement in the 874 changed
files. Byte identity does establish physical source identity for the 182
identical files, but does not establish that all their arms were hit, nor that
different build contexts select the same arms. The remaining ambiguous rows
and files are counted in the machine-readable summary and must not be counted
as completed original-line checks.

An additional [line scan](s13-r1316-zero-branch-line-scan.json) checked the
373 differing files where neither the current nor original file contains a
conditional group. It compared unique, unchanged lines and applied sequential
`#line` directives. In 58 files, 6,231 of 45,695 matched lines had different
logical positions from the original; the other 315 files showed no mismatch
among these unique matches. These are source-level findings, not proof that
all matches compile or that every differing position is an observable runtime
failure. Nor does the absence of a unique-line mismatch certify every line.
The per-file checklist includes these two counts. The remaining files with
conditions require checks using the actual selected preprocessing contexts;
this zero-branch scan does not cover them.

The first [ten-file line batch](s13-r1317-line-batch.json) subsequently
restores original positions for every unchanged line block in those files;
all non-`#line` bytes are identical to the previous source. This reduces the
58 flagged files to 48. Local additions have no original line equivalent.
The cached x86 root, NTVDM worker, WOW32 and x64 builds passed; the ten-image
package was published and verified at both destinations. Eight images are
identical to the previous package and NTVDM and WOW32 differ only in PE
timestamps ([comparison](s13-r1317-build-image-compare.json)); per owner
instruction, this does not require a repeated runtime test.

The [second ten-file batch](s13-r1318-line-batch.json) restores matching
original line positions in another ten files, leaving 38 files from this scan
with observed differences. The cached x86 root, NTVDM worker, WOW32 and x64
builds passed, and the complete package was published and hash-verified at
both destinations. Eight images are identical and NTVDM/WOW32 differ only in
PE timestamps ([comparison](s13-r1318-build-image-compare.json)); owner policy
therefore permits reusing the previous runtime-test evidence.

The [third ten-file batch](s13-r1319-line-batch.json) restores matching
original line positions in another ten files, leaving 28 files from this scan
with observed differences. The cached x86 root, NTVDM worker, WOW32 and x64
builds passed and the full package was published to both destinations. Eight
images are identical and NTVDM/WOW32 differ only in PE timestamps
([comparison](s13-r1319-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [r1336 batch](s13-r1336-line-batch.json) restores matching selected-code
locations in another ten files. Their original excluded-only conditional
trees and matching code positions were independently rechecked in the
[tree record](s13-r1336-tree-recheck.json), retaining their existing local
adaptations. The condition/matching-code count is now 501 differing files,
with 373 still pending. Cached x86 root, NTVDM worker, WOW32 and x64 builds
passed; the full package was verified at both destinations. Eight images are
identical and WOW32 differs only in PE timestamp. NTVDM's twelve additional
changed bytes are diagnostic `__LINE__` immediates traced to eight error
calls ([proof](s13-r1336-diagnostic-line-proof.json)); runtime tests were not
repeated under owner-approved line-only policy.

The [r1333 batch](s13-r1333-line-batch.json) records references for the final
25 entries classified as ambiguous excluded anchors in the current branch
checklist, and corrects selected-source and continuation positions in the
same ten files. A local-only guard at original EOF is labelled as such; it
does not imply an original excluded arm existed. This resolves that specific
checklist category, not the full condition-tree or selected-line audit. Build
passed for cached x86 root, NTVDM worker, WOW32 and x64; the full package was
verified at both destinations. Eight images are byte-identical, WOW32 differs
only by timestamp, and NTVDM differs by one diagnostic `__LINE__` byte plus
timestamps. The [binary/source proof](s13-r1333-diagnostic-line-proof.json)
traces the immediate from 1414 to 1419 at the original-owner error call.
Runtime tests were not repeated under the owner-approved line-only policy.

The [r1332 excluded-anchor batch](s13-r1332-line-batch.json) maps 30
additional local-profile guards to original statements, branch entries or
the corresponding original `BACK_M` macro, and corrects eight continuation
positions. This leaves 25 ambiguous excluded anchors across five files;
other condition and line audits remain open. Cached x86 root, NTVDM worker,
WOW32 and x64 builds passed; the complete package was verified at both
destinations. Eight images are identical and NTVDM/WOW32 differ only in PE
timestamps ([comparison](s13-r1332-build-image-compare.json)); runtime tests
were not repeated under owner instruction.

The [r1331 excluded-anchor batch](s13-r1331-line-batch.json) records fourteen
original statement or branch-entry locations for excluded local-profile
guards in ten files. The remaining ambiguous excluded-anchor count is 55;
other branches remain under review. Cached x86 root, NTVDM worker, WOW32 and
x64 builds passed; the full package was published and verified at both
destinations. Nine images are identical and NTVDM differs only by PE
timestamp ([comparison](s13-r1331-build-image-compare.json)); runtime tests
were not repeated under owner instruction.

The [r1330 anchor batch](s13-r1330-line-batch.json) corrects twelve selected
source anchors in ten condition-bearing WOW, SDK and USER files against
original statements and context. Other branches remain under audit. Build and
publication passed for cached x86 root, NTVDM worker, WOW32 and x64 builds;
the full package was verified at both destinations. Eight images are
byte-identical and NTVDM/WOW32 differ only in PE timestamps
([comparison](s13-r1330-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [r1329 anchor batch](s13-r1329-line-batch.json) corrects another twelve
selected-source anchors in ten condition-bearing files, based on multiple
exact original statements and surrounding context. Other branches remain
under audit. Cached x86 root, NTVDM worker, WOW32 and x64 builds passed and
the full package was published to both destinations. Nine images are
byte-identical; NTVDM has two changed diagnostic `__LINE__` immediate bytes
plus timestamps. The [binary/source proof](s13-r1329-diagnostic-line-proof.json)
identifies both `DisplayErrorTerm` calls and the exact old/new line values;
runtime tests were not repeated under owner-approved line-only policy.

The [r1328 anchor batch](s13-r1328-line-batch.json) corrects another 20
selected-source anchors in ten condition-bearing files using multiple exact
original statements and context. Other branches remain under audit. Build and
publication passed for cached x86 root, NTVDM worker, WOW32 and x64 builds;
the full package was verified at both destinations. Nine images are identical
and NTVDM differs only in PE timestamp
([comparison](s13-r1328-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [r1326 anchor batch](s13-r1326-line-batch.json) corrects 12 selected
source anchors across ten condition-bearing files using unique original
statements and surrounding context. An otherwise tempting `cpu_vid.h`
candidate was rejected because a similarly spelled declaration belongs to a
different original structure. Other branches remain under audit. Build and
publication passed for cached x86 root, NTVDM worker, WOW32 and x64 builds;
the package was verified at both destinations. Eight images are identical
and NTVDM/WOW32 differ only in PE timestamps
([comparison](s13-r1326-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [r1325 anchor batch](s13-r1325-line-batch.json) corrects 32 source-line
anchors in ten further condition-bearing files against unique exact original
statements. This does not certify the rest of those files. Build and
publication passed for cached x86 root, NTVDM worker, WOW32 and x64 builds;
the complete package was verified at both destinations. Nine images are
byte-identical and NTVDM differs only in PE timestamp
([comparison](s13-r1325-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [sixth batch](s13-r1322-line-batch.json) aligns matching code in the
remaining eight condition-free files previously flagged by the scan and
restores two excluded video-branch anchors. Original comment-only line
positions remain uncertified for 18 of these condition-free files; the
condition-bearing files have separate remaining audit work. Build and
publication passed for cached x86 root, NTVDM worker, WOW32 and x64 builds;
the complete ten-image package was verified at both destinations. Eight
images are identical and NTVDM/WOW32 differ only in PE timestamps
([comparison](s13-r1322-build-image-compare.json)). Runtime tests were not
repeated under owner instruction.

The [r1327 condition-free review](s13-r1327-condition-free-code-review.json)
then checked all exact matching code blocks in the remaining 315 differing
files from that scan. The original and current versions have no conditional
groups; 312 have matched code and no observed logical-line mismatch, and three
contain only comments. Their includes remain referenced by project sources.
Together with the prior 58 corrected files, all 373 differing condition-free
files now have this condition/line review. The review does not establish the
behavior of local edits or certify comment-only line positions. The other 501
differing files contain conditional groups and remain under separate review.

The [r1335 batch](s13-r1335-line-batch.json) restores selected-code logical
positions in ten files with registered local differences. Their
[original condition trees and matching code](s13-r1335-tree-recheck.json) were
rechecked after editing; each current condition is an excluded error. Local
edits are retained and this source-position audit is not a runtime-behavior
proof. The condition/matching-code count is now 491 differing files, with 383
still pending. Cached x86 root, NTVDM worker, WOW32 and x64 builds passed; the
full package was verified at both destinations. Nine images are identical
and NTVDM differs only in PE timestamp
([comparison](s13-r1335-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [r1334 original-tree review](s13-r1334-excluded-only-review.json)
examined all 501 condition-bearing differing files with a conservative
criterion: the original and current condition trees must match, every current
arm must be an excluded `#error` with its original branch and continuation
anchors, and the complete code outside those arms must match the original
projection with matching logical code-line positions. 108 files meet that
criterion. One further file passes the tree and matching-line checks but has
local outside-arm changes, so is not counted as complete. The other 392
require retained-branch context or further review. Thus 481 differing files
have completed condition/selected-original-code review and 393 remain; this
does not assert runtime behavior for excluded paths.

The [r1323 anchor review](s13-r1323-anchor-review.json) additionally checks
16 existing `#line` anchors against the specific original statement or branch
entry they represent, including project-local selector cases. The remaining
ambiguous error-anchor count is 71. It also found an actual mismatch in
`cpu_vid.h`: both the selected and excluded generic `str_read` mappings need
to refer to original line 91 rather than unrelated original lines 95/102;
that source correction was included in the subsequent
[r1324 batch](s13-r1324-line-batch.json), together with other directly
matched anchor corrections. Other conditions in those files remain subject
to the per-file audit; changing one anchor does not certify a whole file.
Cached x86 root, NTVDM worker, WOW32 and x64 builds passed; the complete
package was published at both destinations. Nine images are byte-identical
and NTVDM differs only by PE timestamp
([comparison](s13-r1324-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [fifth ten-file batch](s13-r1321-line-batch.json) restores original
positions for matching code lines in another ten files. The batch uses newline
boundaries rather than treating form-feed characters as line breaks, and
avoids inserting directives inside comments or parenthesized expressions.
Comment-only lines are not certified. Eight files from the original
condition-free scan remain flagged. Cached x86 root, NTVDM worker, WOW32 and
x64 builds passed; the complete package was published to both destinations.
Eight images match byte-for-byte and NTVDM/WOW32 differ only by PE timestamp
([comparison](s13-r1321-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The [fourth ten-file batch](s13-r1320-line-batch.json) restores positions in
another ten files, leaving 18 files from the condition-free scan with observed
differences. The cached x86 root, NTVDM worker, WOW32 and x64 builds passed,
and the complete package was published to both destinations. Eight images are
identical and NTVDM/WOW32 differ only in PE timestamps
([comparison](s13-r1320-build-image-compare.json)); runtime tests were not
repeated under owner instruction.

The r1337 batch aligns original matching-code logical positions in ten more
files and rechecks their excluded-only condition trees. The current inventory
is 182 byte-identical files, 511 differing files with condition and matching-code
review, and 363 differing files still requiring that review, out of 1,056.
These results do not prove runtime behavior of local adaptations, nor that
every supported configuration has been exercised. Two identical files retain
branches without an observed host hit and require separate context review.
Evidence: [batch](s13-r1337-line-batch.json),
[tree recheck](s13-r1337-tree-recheck.json).

Cached x86 root, NTVDM worker, WOW32 and x64 builds passed. The complete
ten-image package was published and verified at both destinations. Eight
images are byte-identical to the prior package and NTVDM/WOW32 differ only
in PE timestamps ([comparison](s13-r1337-build-image-compare.json)); runtime
tests were not repeated under the owner-approved unchanged-image rule.

The [r1338 pending-file tree survey](s13-r1338-pending-tree-survey.json)
compares all 363 remaining differing files against their recorded original
hashes. It finds 274 with matching conditional-tree shape and 89 with at least
one shape difference. These categories are triage only: neither establishes
retained-body correctness, actual hit coverage, nor original logical line
mapping. Four pending files have recorded unobserved retained arms. No file is
marked complete based on this survey.

The [r1339 arm detail](s13-r1339-matched-tree-arms.json) expands the 274
pending files whose tree shape matches: 4,720 branch arms are associated with
the existing checklist; 1,822 retained leaf bodies match the original ignoring
line directives, while 55 differ. These comparisons do not validate local
patches or effective logical lines under each compilation context and do not
change any file completion status.

The [r1340 retained leaf diffs](s13-r1340-retained-leaf-differences.json)
provide exact original/current differences for the 55 differing leaf arms in
25 files. Inline divergence identifiers and ledger filename matches are
recorded as leads only; they are not proof that the full change or its
behavior has been reviewed. No file-completion status is changed.

The r1341 ten-file line batch fixes original matching-code positions; four
excluded-only files passed their direct tree recheck. Six files with selected
branches passed only an observed-context matching-line check, so remain
pending full review. The inventory is 182 byte-identical, 515 differing files
with condition/matching-code review and 359 differing files pending, of 1,056.
The [batch](s13-r1341-pending-line-batch.json),
[excluded recheck](s13-r1341-pending-tree-recheck.json), and
[observed-context recheck](s13-r1342-context-recheck.json) have the details.
Cached x86, NTVDM, WOW32 and x64 builds passed; all ten images were published
and verified. Eight match the previous package byte-for-byte; NTVDM and WOW32
differ only in PE timestamps ([comparison](s13-r1341-build-image-compare.json)).
Per owner instruction no unchanged-image runtime tests were repeated.

The [r1343 strict review](s13-r1343-strict-ready.json) records 57 additional
files whose original conditional trees, excluded-arm anchors, retained leaves
and matching-code logical lines on the observed selected projection pass.
No source changes were needed. This is not a runtime proof for local code or
for unobserved configurations. The inventory is 182 byte-identical / 572
differing files with this structural and matching-code review / 1,056 total;
302 differing files remain.

The [r1344 batch](s13-r1344-batch.json) aligns observed selected-path
matching code in ten files; their [recheck](s13-r1344-context-recheck.json)
passes. One additionally passed [strict tree review](s13-r1344-strict-review.json);
the other nine remain pending. Current count: 182 byte-identical / 573
differing with structure and matching-code review / 1,056 total, leaving 301
differing pending. Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten
images published and verified. Nine match byte-for-byte and NTVDM differs only
by PE timestamp ([comparison](s13-r1344-build-image-compare.json)); tests were
not repeated under the owner-approved rule.

The [r1345 batch](s13-r1345-batch.json) aligns matching source lines on
observed selected paths in ten files; their [context recheck](s13-r1345-context-recheck.json)
passes, but the ten remain pending complete tree review. Cached x86, NTVDM,
WOW32 and x64 builds passed and all ten images were published and verified.
Eight images match byte-for-byte and NTVDM/WOW32 differ only by PE timestamps
([comparison](s13-r1345-build-image-compare.json)); runtime tests were not
repeated under the owner-approved unchanged-image rule. Counts remain 182
byte-identical / 573 differing files reviewed / 1,056 total, with 301
differing files pending.

The [r1346 batch](s13-r1346-batch.json) aligns matching source lines on
observed selected paths in ten files; [context recheck](s13-r1346-context-recheck.json)
passes. Four additionally passed [strict tree review](s13-r1346-strict-review.json);
the other six remain pending. Current count: 182 byte-identical / 577 differing
files reviewed / 1,056 total; 297 differing files pending. Cached x86 root,
NTVDM, WOW32 and x64 builds passed; ten images published and verified. Eight
match byte-for-byte and NTVDM/WOW32 differ only by PE timestamps
([comparison](s13-r1346-build-image-compare.json)); tests were not repeated
under the owner-approved unchanged-image rule.

The [r1347 batch](s13-r1347-batch.json) aligns matching source lines on
observed selected paths in ten files, including within selected branches;
[context recheck](s13-r1347-context-recheck.json) passes. All ten remain
pending complete tree review. Cached x86, NTVDM, WOW32 and x64 builds passed;
ten images were published and verified. Eight match byte-for-byte and NTVDM
and WOW32 differ only by PE timestamps
([comparison](s13-r1347-build-image-compare.json)); runtime tests were not
repeated under the owner-approved rule. Counts remain 182 byte-identical /
577 differing files reviewed / 1,056 total, with 297 differing files pending.

The [r1348 strict review](s13-r1348-strict-ready.json) records 112 more
files whose conditional trees, excluded-arm anchors, retained leaf contents,
and matching-code lines in the observed selected projection pass. Groups with
no excluded arm were admitted only with recorded selected-branch evidence.
One candidate with only one matching code line remains under review. No source
changes were needed. This does not prove unobserved contexts or runtime
semantics of local adaptations. Counts: 182 byte-identical / 689 differing
files with condition and matching-code review / 1,056 total; 185 differing
files remain.

The [r1349 batch](s13-r1349-batch.json) aligns matching source lines on
observed selected paths in ten files; [context recheck](s13-r1349-context-recheck.json)
passes. Five additionally passed [strict tree review](s13-r1349-strict-review.json);
the other five remain pending. Current count: 182 byte-identical / 694 differing
files reviewed / 1,056 total; 180 differing files pending. Cached x86 root,
NTVDM, WOW32 and x64 builds passed; ten images published and verified. Nine
match byte-for-byte and NTVDM differs only by PE timestamp
([comparison](s13-r1349-build-image-compare.json)); tests were not repeated
under the owner-approved unchanged-image rule.

The [r1350 batch](s13-r1350-batch.json) aligns matching source lines on
observed selected paths in ten files; [context recheck](s13-r1350-context-recheck.json)
passes. Two additionally passed [strict tree review](s13-r1350-strict-review.json);
the other eight remain pending. Current count: 182 byte-identical / 696 differing
files reviewed / 1,056 total; 178 differing files pending. Cached x86 root,
NTVDM, WOW32 and x64 builds passed; ten images published and verified. Nine
match byte-for-byte and NTVDM differs only by PE timestamp
([comparison](s13-r1350-build-image-compare.json)); tests were not repeated
under the owner-approved unchanged-image rule.

The [r1351 batch](s13-r1351-batch.json) aligns matching source lines on
observed selected paths in ten files; [context recheck](s13-r1351-context-recheck.json)
passes. Four additionally passed [strict tree review](s13-r1351-strict-review.json);
the other six remain pending. Current count: 182 byte-identical / 700 differing
files reviewed / 1,056 total; 174 differing files pending. Cached x86 root,
NTVDM, WOW32 and x64 builds passed; ten images published and verified. Eight
match byte-for-byte and NTVDM/WOW32 differ only by PE timestamps
([comparison](s13-r1351-build-image-compare.json)); tests were not repeated
under the owner-approved unchanged-image rule.

The [r1352 batch](s13-r1352-batch.json) aligns matching source lines on
observed selected paths in ten files; [context recheck](s13-r1352-context-recheck.json)
passes. Eight additionally passed [strict tree review](s13-r1352-strict-review.json);
the other two remain pending. The separately held cpucstbs empty projection
was [reviewed](s13-r1352-cpucstbs-review.json). Current count: 182 byte-identical /
709 differing files reviewed / 1,056 total; 165 differing files pending. Cached
x86 root, NTVDM, WOW32 and x64 builds passed; ten images published and verified.
Eight match byte-for-byte and NTVDM/WOW32 differ only by PE timestamps
([comparison](s13-r1352-build-image-compare.json)); tests were not repeated
under the owner-approved unchanged-image rule.

The [r1353 continuation review](s13-r1353-continuation-proof.json) verifies
14 additional files without a physically adjacent continuation #line: each
selected continuation has a later matching code line at its original logical
position. Three cases lacking such a match remain pending. No source change
was made. This is not proof of unobserved contexts or local runtime behavior.
Counts: 182 byte-identical / 723 differing files reviewed / 1,056 total; 151
differing files remain.

The [r1355 EOF review](s13-r1355-eof-continuation-review.json) resolves three
previously unpaired continuation cases: after the matching conditional groups
there is no further source code, only blanks and closing directives. The
working inventory currently includes a pending, unbuilt gdpvar.h exclusion;
no release or submission is claimed for it.

The r1365 ten-file batch includes the [shadowed GDP guard](s13-r1354-gdp-guard-review.json),
line alignment for [int21map](s13-r1356-int21map.json), [c_oprnd](s13-r1359-c-oprnd.json),
[vrnmpipe](s13-r1361-vrnmpipe-line.json), [DPMI data](s13-r1364-dpmi-data-line.json),
and [five files with local groups](s13-r1365-batch.json). Existing local token
streams were preserved where reflowed. GDP passed the product build and its
original guard is now excluded; other partial audits remain pending. Counts:
181 byte-identical / 730 differing files reviewed / 1,056 total; 145 differing
files pending. Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten images
were published and verified. Eight match byte-for-byte and NTVDM/WOW32 differ
only by PE timestamps ([comparison](s13-r1365-build-image-compare.json));
per owner instruction tests were not repeated.

The [r1366 batch](s13-r1366-batch.json) aligns matching source lines in ten
files with additional local conditional groups; [observed context recheck]
(s13-r1366-extra-group-recheck.json) passes. These files remain pending complete
tree review. Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten images
published and verified. Nine match byte-for-byte and NTVDM differs only by PE
timestamp ([comparison](s13-r1366-build-image-compare.json)); tests were not
repeated under the owner-approved rule. Counts remain 181 byte-identical /
730 differing files reviewed / 1,056 total, with 145 differing files pending.

The [r1367 batch](s13-r1367-batch.json) aligns matching source lines in ten
files with additional local conditional groups; [observed context recheck]
(s13-r1367-extra-group-recheck.json) passes. These files remain pending complete
tree review. Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten images
published and verified. Eight match byte-for-byte and NTVDM/WOW32 differ only
by PE timestamps ([comparison](s13-r1367-build-image-compare.json)); tests were
not repeated under the owner-approved rule. Counts remain 181 byte-identical /
730 differing files reviewed / 1,056 total, with 145 differing files pending.

The [r1368 review](s13-r1368-extra-strict.json) pairs original conditional
groups separately from local added groups in 14 files; selected original
direct bodies, excluded-arm evidence, recorded local-group states and
matching-code lines on the observed projection pass. Local behavior and
unobserved contexts are not certified. No source change was needed. Counts:
181 byte-identical / 744 differing files reviewed / 1,056 total; 131 differing
files remain.

The [r1369 batch](s13-r1369-batch.json) aligns matching source lines in ten
files with additional local conditional groups; [observed context recheck]
(s13-r1369-extra-group-recheck.json) passes. Nine additionally passed the
[strict tree review](s13-r1369-extra-strict.json); one remains pending.
Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten images published
and verified. Eight match byte-for-byte, NTVDM differs only by PE timestamp,
and WOW32 has three changed assertion-line immediate bytes plus timestamps,
independently attributed in [proof](s13-r1369-diagnostic-line-proof.json).
Per owner instruction tests were not repeated. Counts: 181 byte-identical /
753 differing files reviewed / 1,056 total; 122 differing files pending.

The [r1370 registered-group review](s13-r1370-registered-groups.json) pairs
original and local groups in three further files and attributes each selected
direct difference to its exact existing registration. Matching original-code
line positions use the recorded observed projection. No runtime equivalence
or unobserved context is claimed. Counts: 181 byte-identical / 756 differing
files reviewed / 1,056 total; 119 differing files remain.

The r1376 ten-file batch includes the [USER group batch](s13-r1371-batch.json),
[c_main](s13-r1372-cmain.json), [nt_event](s13-r1373-ntevent.json),
[DPMI header](s13-r1374-dpmiheader.json), [VDM client](s13-r1375-vdm.json),
and [xt.h](s13-r1376-xt-line.json). Four USER files passed strict tree review;
the other files remain pending complete review. Cached x86 root, NTVDM, WOW32
and x64 builds passed; ten images were published and verified. Eight match
byte-for-byte, WOW32 differs only by timestamp, and NTVDM has four changed
diagnostic line immediate bytes plus timestamps, attributed in
[proof](s13-r1376-diagnostic-line-proof.json). Per owner instruction tests
were not repeated. Counts: 181 byte-identical / 760 differing files reviewed /
1,056 total; 115 differing files pending.

The r1382 batch aligns diagnostic positions in ten files. Cached x86 root, NTVDM, WOW32 and x64 builds passed; the ten-image package was published and verified. Eight images are byte-identical to the preceding package and NTVDM and WOW32 differ only by PE timestamp ([comparison](s13-r1382-build-image-compare.json)); per owner instruction runtime tests were not repeated. Current recorded totals: 181 byte-identical, 768 differing files with condition and observed matching-code review, and 107 differing files still pending further review, out of 1,056. Identical bytes alone do not prove every retained arm was hit; five files have 11 retained arms without observed host hits.

The [r1384 suballocation review](s13-r1384-suballoc-review.json) checks the original condition shell, registered MSVC adaptation, both retained observed arms and two source-derived line projections; no source change was required. Totals: 181 byte-identical / 769 differing files reviewed / 1,056 total; 106 differing files remain pending. The line projection is not a compiler diagnostic observation.

The [r1385 ROM BASIC audit](s13-r1385-rom-basc-review.json) verifies the excluded SEGMENTATION body and identical positions of every remaining original line. Counts: 181 identical / 770 differing files reviewed / 1,056 total; 105 differing files pending. The equipment-file audit found its existing MVDM-HOST-DIV-325 diagnostic hook omitted from the registration path list; the path was added, while that file remains pending full review.

The [r1386 equipment audit](s13-r1386-equipmnt-review.json) verifies the excluded SEGMENTATION body, registered local observer and original logical positions of the unchanged nonblank lines. Counts: 181 identical / 771 differing files reviewed / 1,056 total; 104 differing files pending. This is source-derived evidence, not a new compiler diagnostic test.

The [r1387 int21map review](s13-r1387-int21map-review.json) checks the original tree, 14 excluded arms, one retained arm and 2,987 source-matched lines. Counts: 181 identical / 772 differing files reviewed / 1,056 total; 103 differing files pending. This is source-derived and historical compilation evidence.

The [r1389 natural-line review](s13-r1389-natural-line-review.json) verifies 11 condition trees and their selected direct bodies and line projections; three other candidates remain held for local differences. Counts: 181 identical / 783 differing files reviewed / 1,056 total; 92 differing files pending. This is source-derived and historical compilation evidence, not a fresh compiler diagnostic test.

The [r1390 video review](s13-r1390-video-registered-diffs.json) records and attributes all direct retained-arm differences in three video files to existing registrations; source-projected matching lines were already checked in r1389. Counts: 181 identical / 786 differing files reviewed / 1,056 total; 89 differing files pending. No new runtime or compiler diagnostic test.

The [r1391 review](s13-r1391-registered-diffs.json) records eight files with retained-arm local differences and attributes each to its existing mirror registration, preserving full branch diffs. Counts: 181 identical / 794 differing files reviewed / 1,056 total; 81 differing files pending. Evidence is source-derived and historical, not new runtime or compiler diagnostic testing.

The [r1392 review](s13-r1392-registered-diffs.json) records eight further files with retained-arm local differences, their existing mirror registrations and full branch diffs. Counts: 181 identical / 802 differing files reviewed / 1,056 total; 73 differing files pending. Evidence is source-derived and historical, not new runtime or compiler diagnostic testing.

The [r1393 event review](s13-r1393-ntevent-review.json) checks original groups, selected direct bodies and matching source positions with two local groups. The [candidate tree inventory](s13-r1393-pending-tree-candidates.json) does not prove that every unmatched signature is missing; duplicate or reordered groups require review. Counts: 181 identical / 803 differing files reviewed / 1,056 total; 72 differing files pending.

The [r1394 printer review](s13-r1394-printer-review.json) checks 25 historically hit and 22 excluded arms and matching-source lines, including naturally aligned excluded anchors. Counts: 181 identical / 804 differing files reviewed / 1,056 total; 71 differing files pending. Evidence is source-derived and historical.

The [r1395 local-group review](s13-r1395-local-groups-review.json) checks four files, records retained direct diffs and complete local-group bodies, and links their registrations. Counts: 181 identical / 808 differing files reviewed / 1,056 total; 67 differing files pending. Evidence is source-derived and historical.

The [r1396 local-group review](s13-r1396-local-groups-review.json) checks three files and records direct diffs and local-group bodies. The previously unlisted V7 stride/wrap change is registered as DIV-841 with its introducing commit and verification boundary. Counts: 181 identical / 811 differing files reviewed / 1,056 total; 64 differing files pending. Evidence is source-derived and historical.

The [r1397 local-group review](s13-r1397-local-groups-review.json) checks two host CPU and EMS files and records direct diffs and local-group bodies against existing registrations. Counts: 181 identical / 813 differing files reviewed / 1,056 total; 62 differing files pending. Evidence is source-derived and historical.

The [r1398 local-group review](s13-r1398-local-groups-review.json) checks x87 layout and host interrupt-hook conditional structures and retains full local-group bodies. Counts: 181 identical / 815 differing files reviewed / 1,056 total; 60 differing files pending. CPU40 protected-monitor behavior remains partial under DIV-473; this condition audit is not functional certification.

The [r1399 WOW32 review](s13-r1399-wow32-review.json) checks original condition trees, selected matching-source positions and five registered local groups. Counts: 181 identical / 816 differing files reviewed / 1,056 total; 59 differing files pending. This is not fast-BOP or callback functional certification.

The [r1400 CCPU review](s13-r1400-cmain-review.json) checks original condition trees, selected matching-source positions and nine registered local groups. Counts: 181 identical / 817 differing files reviewed / 1,056 total; 58 differing files pending. CPU40 event ordering and monitor behavior remain subject to existing separate validation boundaries.

The [r1401 keyboard controller review](s13-r1401-keyba-review.json) checks original condition trees, selected matching-source positions and nine registered local groups. Counts: 181 identical / 818 differing files reviewed / 1,056 total; 57 differing files pending. Functional adaptations remain under their separate existing evidence.

The r1406 ten-source batch inserts logical line anchors only; [nturtl](s13-r1402-nturtl-lines.json) completes a guarded local subset review, while other paths remain pending. Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten images published and verified. Eight images match byte-for-byte, NTVDM and WOW32 differ only in PE timestamps ([comparison](s13-r1406-build-image-compare.json)); per owner instruction runtime tests were not repeated. Counts: 181 identical / 819 differing files reviewed / 1,056 total; 56 differing files pending.

The [r1407 ten-source batch](s13-r1407-function-lines.json) adds only logical line anchors. Cached x86 root, NTVDM, WOW32 and x64 builds passed; ten images published and verified. Eight images match byte-for-byte, NTVDM and WOW32 differ only in PE timestamps ([comparison](s13-r1407-build-image-compare.json)); per owner instruction runtime tests were not repeated. Other lines and trees in these files remain pending. Counts: 181 identical / 819 differing files reviewed / 1,056 total; 56 differing files pending.

The [r1411 host CPU review](s13-r1411-hostcpu-review.json) checks condition structures and registered original-profile adaptations; the independent fixture reports original reg size 2 and current size 4. That layout difference remains a documented non-equivalence, not a functional certification. Counts: 181 identical / 820 differing files reviewed / 1,056 total; 55 differing files pending.

The r1413 ten-file batch removes the [unreachable WOW32 header](s13-r1412-wownt32-removal.json), guards its former include and aligns [eight source locations](s13-r1413-function-lines.json). Cached x86 root, NTVDM, WOW32 and x64 builds passed; WOW32 dependencies no longer include the removed file. Ten images were published and verified: eight byte-identical and two PE timestamp-only ([comparison](s13-r1413-build-image-compare.json)). Per owner instruction runtime tests were not repeated. Current retained census: 181 identical / 820 differing files reviewed / 1,055 total; 54 differing files pending.

The [r1415 tape review](s13-r1415-tape-review.json) checks twelve excluded and ten observed retained arms, including a naturally aligned error anchor. Counts: 181 identical / 821 differing files reviewed / 1,055 total; 53 differing files pending. Evidence is source-derived and historical.
