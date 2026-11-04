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
