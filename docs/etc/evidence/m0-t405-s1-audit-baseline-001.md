# M0 T405 S1 initial audit baseline

## Question and procedure

Establish comparison coverage before counting semantic replacements.
On 2026-09-12 inspected Git status/HEAD, CURRENT, Queue, the proposal, and
tracked membership using git ls-files grouped by source root. Static audit
only: no product execution, source repair or runtime deployment.

## Initial observations

Starting HEAD: `3d127962c82133cb6adad50f52733250bac5f11a`, clean worktree.
Ancestor `1daff0ace` preserves unaccepted research code: clean Git state is
not functional acceptance.

Independently read deployed O:\ntvdm64\ntvdm32.exe with Get-FileHash:
SHA-256 `27f8d7b5bb074838e9484877954a38dcbcfa0be64282a4a2d05c776867655107`.
This identifies the retained runtime artifact, not a rebuild of current HEAD
or a fresh runtime regression result. No deployment was changed.

| Root | Tracked files, all kinds |
| --- | --- |
| mvdm-host | 1977 |
| mvdm-host-overlay | 8 |
| mvdm-softpc-patch | 6 |
| opennt-host | 20 |
| adapter-mvdm-host-out | 112 |
| adapter-opennt-host | 1 |

These are initial membership counts, not executable-file counts, line diffs,
hack counts or the final audit denominator. App/session, ABI declarations,
build selection and provenance still need reconciliation. No tracked
opennt-host-overlay result does not imply absent non-MVDM replacements;
those bodies can be in other roots.

## Follow-up and completion limitation

Reuse existing provenance/selection ledgers, resolve each selected upstream
file, separate declarations/generated/data inputs and map actual providers
before computing upstream diffs and replacement totals. Record runtime
hashes independently. The initial seven findings are leads, not exhaustive
coverage. S1 remains active: admission and this initial record do not close it.

## Executor pass: archived topology reconciliation

The source README and old Audit-T276S1MvdmFileIdentity.ps1 refer to removed
docs/etc/operations ledger paths. Located the retained inputs under
artifacts/documentation-archive/20260910/etc/operations/ledgers instead:
m0-t274-s5-mvdm-host-topology-manifest.tsv and
m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv. No old audit script
was executed: it writes historical records and assumes a fixed denominator.

Procedure: Import-Csv with tab delimiter; index topology by target_path under
src/mvdm-host; index supplement by its full target_path; compare git ls-files
membership. For each topology pair, Test-Path the selected_source_path and
Get-FileHash both source and mirror, comparing the source to the manifest's
selected_source_sha256. Results on the frozen product-source baseline:

| Classification | Count |
| --- | --- |
| Old topology rows | 1689 |
| Current tracked host paths | 1977 |
| Current paths paired to topology | 1585 |
| Paired source missing / source hash changed | 0 / 0 |
| Paired mirror byte-identical / byte-different | 1395 / 190 |
| Additional paths listed in supplement | 385 |
| Additional paths unpaired to both inputs | 7 |
| Old topology paths absent at current host location | 104 |

The seven unpaired paths are README.md, kernel-vdm/v86/monitor/i386/
monitor_printer.c, oemuni/obj/_objects.mac, oemuni/obj/i386/file.obj,
oemuni/obj/i386/process.obj, suballoc/obj/_objects.mac, and
suballoc/obj/i386/suballoc.obj. They need their own provenance classification.

## Reviewer pass: limits and next action

Arithmetic reconciles: 1395 + 190 = 1585; 1585 + 385 + 7 = 1977;
1585 + 104 = 1689. However byte difference is not semantic difference:
190 is not a count of hacks or executable diff files. Line endings, cropped
source and true behavioral changes must be separated. Supplement membership
does not select opennt-src-2 as a replacement primary baseline; its hashes
and historical-only disposition remain to be checked. The 104 absent paths
are not established deletions until re-rooting and ownership are reconciled.
The archived union uses owner_root opennt-mvdm-host, whereas the old script
filters mvdm-host; fixing just its input directory would still be unsound.
These findings require a current inventory rather than running the old gate
and reporting its result as present coverage. Non-MVDM coverage and actual
build selection remain outstanding. No product behavior was changed.

## Reproducible coverage tool and second review

Added tools/audit/Measure-OpenNtMirrorCoverage.ps1. It reads Git membership
and the two archived manifests, validates source hashes and writes only to a
new directory beneath build. Raw CSVs and summary remain at
build/M0-T405/S1/source-coverage-002; invoke the script with a new
OutputDirectory to reproduce. It does not rewrite source or old ledgers.

The first run's relocation result was invalid: a PowerShell array-expression
precedence mistake concatenated two candidate paths. Explicit parentheses
fixed it. Re-running all comparisons retained the same host identity counts
and established 71 byte-equal relocations: 41 to mvdm-softpc-firmware and 30
to mvdm-tools. The remaining 33 old paths are in v86. Their association with
retired CPU30 monitor/scaffold is supported by the T315 history, but their
per-file deletion/source-carrier disposition remains to be finished.

All 385 supplement source hashes match the recorded source identity; 384
current files match those bytes. The exception is softpc.new/obj.vdm/obj/
i386/ntvdm.def: git diff --no-index shows one added export,
Sim32FlushVDMPointer. The current New-T310OriginalSoftpcNinja.ps1 selects
that .def via hostExportDefinition. Thus a historical-looking obj path is
not automatically inactive: this is a concrete source-selection exception
requiring audit. The old evidence-only label cannot establish current use.

Additional tracked membership: app 10, session 5, broker 7, opennt-abi 210.
No tracked files were returned for mvdm-platform-abi or adapter-mvdm-host-in
in this checkout. Reconcile declared topology with physical inputs; do not
drop the actual opennt-abi root from the coverage denominator.

## Paired physical-line baseline

Measure-OpenNtPairedDiff.ps1 verifies that coverage-input source/current
hashes still match, then runs git diff --no-index --no-ext-diff --no-textconv
--ignore-cr-at-eol --numstat with core.autocrlf=false. It also compares the
19 non-README opennt-host files to OpenNT: same relative paths except netapi
maps to upstream ds/netapi, as established by the source package registers.
Output: build/M0-T405/S1/paired-diff-001/paired-diff.csv.

| Comparison group | Text-different files | Added lines | Deleted lines |
| --- | --- | --- | --- |
| Selected MVDM topology pairs | 173 | 4378 | 1273 |
| Supplement-only pair with changed bytes | 1 | 1 | 0 |
| Non-MVDM OpenNT-host reference | 7 | 316 | 14011 |

The 190 byte-different topology pairs split into 173 text differences and
17 CR-only differences. All 1395 byte-equal topology pairs and 384 byte-equal
supplement pairs remain equal. Non-MVDM has 12 byte-equal files; its seven
text differences are client/vdm.c (+135/-3242), netapi/api/apibuff.c
(+10/-215), ntexapi.h (+20/-1832), ntpsapi.h (+25/-1280), ntrtl.h
(+117/-6337), nturtl.h (+9/-1103), and ptypes32.h (+0/-2).

Reviewer checks: ptypes32.h's two deletions are blank lines, although its
README row calls it byte-exact. Non-MVDM deletion totals largely describe
registered source subsets, not deleted hacks. The selected MVDM largest
physical deltas include modesw.c (+317/-6), cmdmisc.c (+273/-40), cmdexec.c
(+234/-52) and c_main.c (+175/-38); size prioritizes review but proves no bug.
These totals exclude overlays, patches, autonomous adapter bodies, seven
unpaired host files and other ABI/project roots. They must not be reported
as a whole-project footprint or completed semantic audit.

All 33 remaining V86 old paths are present in git diff-tree's deletion list
for bbe4b0ec0, the T310 S6 V86-monitor retirement. Together with the 71 verified
relocations this accounts for all 104 absent topology paths. The separately
reintroduced monitor_printer.c slice remains subject to its own provenance
and boundary review; retirement does not prove all original functions unusable.

## All-root denominator and added-body footprint

Export-OpenNtAuditInventory.ps1 enumerates every tracked src path, rejects an
unknown root, records SHA-256 and assigns a review category without pretending
to resolve build selection. Output is build/M0-T405/S1/all-roots-001/
inventory.csv: 5043 rows and 5043 distinct paths. Guest, firmware and tools
are separately labelled load-only/independent evidence, not lost from the
inventory or assumed to be host runtime inputs.

Physical source-line counts include comments and blanks for the explicit
C/header/assembly/include/definition/resource/patch extension list. Other
files remain inventoried but are not silently counted as source lines.

| Root | Source-text files | Physical lines |
| --- | --- | --- |
| mvdm-host-overlay | 7 | 245 |
| mvdm-softpc-patch | 5 | 1868 |
| adapter-mvdm-host-out | 103 | 11654 |
| adapter-opennt-host | 0 | 0 |
| app | 9 | 1594 |
| session | 4 | 1065 |
| broker | 6 | 433 |
| opennt-abi | 199 | 80463 |

These are added-body/declaration review denominators, not net upstream diffs
or autonomous-code totals. The overlay 245 lines independently agree with
ReadAllLines counts: demfastio.c 95, localfm.c 20, sas_overwrite_memory.c 16,
C-VID binder C/header 77/9, EOI bridge C/header 20/8. Its SAS wrapper retains
the original ntstubs.c call body; its localfm carrier omits the duplicate Sas
global. Neither whole file can be classified as an invented algorithm solely
because it sits in an overlay.

The C-VID binder additionally owns mvdm_cvidc_jump_restart and calibration
getters/setters, not only vector assignments. Compare that state against
the original qevnt.c/host accessors during S2. The current generator selects
all five overlay C bodies and compiles fmstubs.c separately with vector-default
and activity-check predicates: selected source membership is not selection of
the entire patch translation unit. adapter-opennt-host contains only README,
while BaseSrv behavior is selected from adapter-mvdm-host-out/basesrv.
These distinctions must survive the function-level classification.

Remaining S1 work includes exact ABI/source provenance, generated-header
dependencies and current graph linkage per inventoried provider. The raw
inventory explicitly labels build selection unestablished; physical coverage
alone is not final S1 acceptance or the requested full semantic audit.

## Unpaired-path reconciliation update

Resolved all seven paths previously unpaired to the two selected manifests:
README.md is project documentation; monitor_printer.c is a three-function
original-source extraction, with matching upstream hash and body comparisons
recorded in the [semantic audit](opennt-replacement-audit-20260912.md#unpaired-printer-carrier-resolved).
The other five paths are byte-identical to the corresponding paths beneath
O:/repos.external/opennt-src-2/nt/private/mvdm:

| Host-relative path | Bytes | Source and current SHA-256 |
| --- | --- | --- |
| oemuni/obj/_objects.mac | 434 | C5E265B9B66971949E40567D55727EB923A2C53DE9AACB529DF24C03D563F79F |
| oemuni/obj/i386/file.obj | 44649 | CB04F400411340CD0FF44719D4EEDAB4E550522D8412A89FA92A79F0F61DC3F2 |
| oemuni/obj/i386/process.obj | 35587 | B2704B0AF5387F717011E90E592957FF6061219C392260957EBC842BBD5B870E |
| suballoc/obj/_objects.mac | 306 | D4BB0B4F285AA32979F67755665BE35DCC17C7BDD7A0CCAA968A5CA72DC89D9D |
| suballoc/obj/i386/suballoc.obj | 23800 | 92B4E62C7AB84E413B4590EA3148B87804CD675D8D6CBC85DFFB74EDBD2801B9 |

Procedure: compare each explicit path with Get-FileHash; inspect Git path
history (d0a9204c7 re-root), original sources files, generator object/library
rules and retained r014/r015 graphs. The generator compiles file.c/process.c
and suballoc.c to build-local objects; neither retained graph references
these historical obj-directory paths. They are retained historical build
carriers, not selected object-code replacements. Hash equality proves source
identity, not independently the original import authorization or runtime
acceptance. This resolves the unpaired identity list without changing the
raw manifest-based inventory or promoting supplementary objects to inputs.

Reviewer result: do not count these five exact historical files or the three
exact printer bodies as autonomous duplicate algorithms. Remaining S1 closure
work is provider-selection/coverage reconciliation, not rediscovery of these
seven paths. Detailed semantic findings still require the combined S2/S3
owner review before any repair.

## Reproducible retained-graph membership join

Added tools/audit/Join-OpenNtRetainedGraphCoverage.ps1. It verifies all 5043
inventory hashes and tracked membership before joining the two retained
r014/r015 graphs. Absolute paths are canonicalized, including source/../
segments and Ninja's escaped drive colon. Unsupported compile-input syntax
fails rather than silently dropping a row. Graph hashes and each matched
compile rule/line are retained beside the joined coverage CSV under
build/M0-T405/S1/graph-coverage-002. The inventory itself is not overwritten.

Two runs produced byte-identical coverage.csv: 508 compile edges, 505 unique
selected paths (adapter 48, app 5, broker 3, MVDM 435, overlay 5, patch 1,
opennt-host 6, session 2). The source/../mvdm_vdm_tib.c path is correctly
selected. Four adapter C files remain unselected in this graph union, as
recorded in the semantic audit. Out-of-build and existing-output rejection
checks both passed without creating or overwriting output.

This closes reproducibility of retained compile membership, not present-day
link reachability: these graphs are historical observations, and headers,
generated carriers, link-time extraction and runtime identities are separate
evidence. The output explicitly labels non-translation-unit rows rather than
claiming unselected headers are unused. No compiler or guest was executed.

## S1 bounded conclusion and reviewer handoff

S1's inventory/baseline objective is concluded. Its original pending statements
above describe intermediate evidence, not the remaining S1 state. Requirements
are reconciled as follows:

| S1 requirement | Evidence and limitation |
| --- | --- |
| Frozen source and runtime | Source remains identical to 3d127962c under src (git diff --name-only empty); deployed SHA-256 rechecked and unchanged. WIP is explicitly unaccepted. |
| Complete membership and measurement rules | 5043 unique tracked paths, source hashes, root assignments and separate paired diff/body counts; current membership/hash verification passes. |
| Selected-source mapping and exceptions | Topology/supplement hashes and 104 absent-path dispositions accounted for; seven unpaired identities resolved above. ABI directory pairing covers all 209 files in the linked semantic audit. |
| Build-selection distinction | Reproducible canonical-path join: 505 selected paths, 508 edges. Non-selected and non-translation-unit labels explicit. Actual link/runtime reachability is not inferred from retained graphs. |
| Overlay/patch/generated carriers | All physical files inventoried and source/body counts separated. Detailed ABI-layout and generated-provider correctness are S2/S3 semantic review, not presumed from this inventory. |
| Audit assignments and unresolved inputs | Every root has an assignment. MVDM versus non-MVDM ownership is resolved per behavior during S2/S3, including mixed adapters and inactive bodies; unknown semantic provenance is retained explicitly, not excluded. |

The remaining per-provider original-owner, outgoing-boundary, link-reachability
and restoration-disposition reconciliation belongs to the already planned S2/S3
audit. No source or runtime acceptance is claimed by closing this inventory
stage. Documentation governance and diff checks passed. Local review/delivery
is still pending; prior push was unavailable, so no fully delivered P is claimed.
The existing detailed findings are preliminary audit evidence to review in
sequence, not evidence that S2/S3 were silently activated or closed.
