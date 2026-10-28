# Existing classified entries: quality audit rows 101–300

**Coordinator reconciliation:** this is the original proposal, not the final
decision. The three COMMAND sources have mirror differences and remain
`partial` as required by the T objective. The 81 source/data rows with only
scoped role evidence and two unbound images have been reopened; objects,
maps and recipes require separate role-by-role judgment.

Using the same current four-shard, path-sorted partial/excluded snapshot as the first-100 audit, `sample101-300-observer.py` captures the next 200 exact entries with current per-row evidence and limitations. The entire slice is original DOS guest command families: COMMAND 80, DEBUG 37, EDLIN 22, EXE2BIN 17, FASTOPEN 6 and GRAPHICS 38.

195 entries are labeled `excluded`/`整体排除`; their recorded rationale limits the finding to historical guest source/build/physical artifact identity or non-selection as a host input, and explicitly leaves guest function, image lineage, configuration or provider unproved. Under the agreed *overall capability* classification rule these do not support blanket exclusion. The observed narrower fact about a historical intermediate or recipe may be retained as separate file-role evidence, but the associated capability-level status should be **pending**. This does not mean old binaries should be rebuilt, linked or deleted.

Five entries are partial: `dos/v86/cmd/command/init.asm`, `rdata.asm`, `stub.asm`, `dos/v86/cmd/fastopen/FASTOPEN.EXE` and `dos/v86/cmd/graphics/GRAPHICS.COM`. Their documented evidence is source identity/difference, generic COMMAND execution, or image comparison, while exact source/image-to-loaded-runtime behavior remains explicitly unbound. Under the agreed standard these are likewise **pending** until a matching image and relevant function witness establish actual reachability. Generic COMMAND startup does not prove a particular old source body or alternate language build was used.

`existing-classifications-sample101-300-corrections.csv` gives all 200 exact paths, original shard/status, recorded evidence and its limitation, and the scoped reason for recommending pending. The CSV is a proposed *status-vs-evidence reconciliation*, not 200 reproduced defects; it preserves each original note and does not change the shared ledger. Other agents may audit other disjoint ranges. No product, guest, external source or runtime was modified or tested.

Recompute corrections from the frozen sample with `PYTHONDONTWRITEBYTECODE=1 PYTHONIOENCODING=utf-8 python artifacts/mvdm-audit/nine-way/part-8/sample101-300-corrections-observer.py`. The original sampler now refuses to overwrite the frozen sample; use a fresh destination for any later live-ledger audit. Source-ledger and generator hashes are in the frozen baseline JSON.
