# S13 final candidate batch r1307

Five candidate files were checked: seven excluded-arm diagnostics already mapped to the original line, while the excluded non-EGG video branch required a line correction. [Evidence](s13-r1306-final-anchor-review.json). The owner explicitly authorized building the final batch with fewer than ten modified files.

Cached x86, x86-extra and x64 builds passed. [Ten-image comparison](s13-r1307-build-image-compare.json): eight identical, two PE timestamp-only; no tests repeated under the owner rule. Prior [r1287 Full](s13-r1287-full-evidence.json) remains the runtime evidence. All ten images were published and verified in `assets/release` and `O:/winnt/system32`. Guest files remain unchanged.

The [coverage account](s13-r1307-coverage-account.json) distinguishes original-identical files, conditional-structure evidence and the absence of a complete per-file line certification for differing files. It does not assert full line coverage or close that gap.
