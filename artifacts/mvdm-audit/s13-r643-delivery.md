# S13: PIG, MONITOR and CPU30 diagnostic line reconciliation

The CMOS file's two original rejected conditions were completely reviewed;
its selected lines match the pinned original after projecting out those arms
and diagnostic directives. See [r641](s13-r641-cmos-review.json).

Ten further product-observed files retain their existing PIG, hardware
MONITOR, CPU30 and segmentation exclusions, with original error, selected-arm
and continuation line mappings restored for 27 groups. Other condition groups
in those files remain pending; no hardware monitor or CPU30 path is enabled.
Exact source hashes and branch boundaries are in
[r642](s13-r642-guard-line-audit.json).

Cached x86, x86-extra and x64 builds passed. The mirror test passed 181 guard
controls and 27 isolated MSVC controls checked excluded diagnostics and
selected/continuation line mapping. Six product images are byte-identical to
the previous release and four differ only in parsed PE/debug timestamps; see
[image comparison](s13-r643-build-image-compare.json). Prior Full 17/17 is
reused under owner instruction, not rerun. Ten components were published and
hash-verified in both destinations. Guest files were not changed; complete
supported-context and remaining file review remain open.
