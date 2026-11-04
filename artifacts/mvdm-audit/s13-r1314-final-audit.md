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
