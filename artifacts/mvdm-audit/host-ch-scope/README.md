# Provisional host C/H scope snapshot

`inventory.csv` enumerates the 1,815 unique `.c`/`.h` paths in the four recorded shard CSVs. `observe.py` computes it from those CSVs, with each source file hashed in `evidence.json`; it reads and hashes the same input bytes, rejects duplicate paths, and does not change the shared ledgers. Run from the repository root with `python artifacts/mvdm-audit/host-ch-scope/observe.py`. Two consecutive runs against the same input bytes produced byte-identical outputs. Running it against later shard states intentionally creates a **new** snapshot; compare the input hashes before interpreting counts or overwriting a frozen result. This is not a replacement for the authoritative per-file ledgers.

The 1,270 `host-candidate` rows exclude the path buckets below for triage only. Their recorded ledger statuses group as 277 pending, 716 partial, 224 excluded, and 53 full. These numbers describe recorded statuses, not demonstrated runtime coverage or a final scope decision. In particular host-candidate includes unselected historical architectures, fax/VDD code and standalone module boundaries that need source and build ownership review. Mirror diffs require partial classification under the project rule.

The 473 `role-pending` rows (`wow16/`, `dos/v86/`) are **not excluded** from the task: some guest trees can contain host-consumed headers, shared ABI declarations, build inputs, or dual-use source. Each such case needs per-file consumer and build-dependency evidence before classification. In particular a path name or historical guest role alone cannot rule out a host dependency.

The 72 `tool-or-scaffold-candidate` rows (`vdmutils/`, `vdmexts/`, `bde/`, `ieuvddex/`, `v86/scaffold/`) are provisionally separated as utility, debugger-extension or scaffold paths, not asserted unnecessary: verify per-file linkage and any shared runtime dependencies. `v86/monitor/` remains a host candidate because the original monitor describes itself as user-mode host code; `dbg/` and `vdmdbg/` also remain candidates as possible host DLL capability.

Output counts and classifications are evidence-bound snapshots. They are neither final scope approval nor evidence that every listed capability executes or passes tests.

The initial snapshot at 0ccfd0acf used historical reporting_class for shards 1–3, so its status counts were incorrect. The corrected observer reads audit_status, decision and review_class respectively; candidate scope counts did not change.

The [GFI provider review](gfi-alternative-provider-review.md) distinguishes compiled empty objects from usable device behavior. The [DEM lock object fixture](../host-dem/demlock-selected-object-review.md) exercises real host locks with controlled guest-register and error seams; guest and lifecycle boundaries remain open.

The [guest-tree consumer snapshot](guest-tree-host-consumer-review.md) compares ambiguous C/H paths to four recorded build/dependency sets; zero exact hits do not establish all configurations or justify bulk exclusion.

The [GFI header review](gfi-header-provider-review.md) traces selected and alternative declarations. The [DEM directory fixture](../host-dem/demdir-selected-object-review.md) tests selected object success and controlled errors without claiming guest integration.

Replaying check-mirror-diffs.py against the recorded inventory and retention ledger found 202 host-candidate C/H mirror-diff rows and zero with a non-partial status. This only checks recorded classifications, not behavioral recovery or completeness of diff detection.

The [CD-ROM provider review](cdrom-provider-boundary-review.md) separates unselected original interfaces from independent MSCDEX/VDD behavior. The [DEM IOCTL object fixture](../host-dem/demioctl-selected-object-review.md) exercises seven controlled drive-type results without claiming real device or guest coverage.

The [input alternate headers](keyboard-mouse-alternate-header-review.md) and [display profiles](display-profile-interface-review.md) retain per-profile boundaries. DEM selected-object fixtures also cover [current-directory failure](../host-dem/demcwd-selected-object-review.md) and [cross-process lock retirement](../host-dem/demlock-process-selected-object-review.md); guest integration remains open.
