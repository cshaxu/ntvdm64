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
