# S13: original line mappings and one identical header review

Ten C/H files received original diagnostic line mappings for fifteen existing
excluded branches. The conditional directive sequences were checked against
the original and selected code was not replaced. Other condition groups in
these ten files remain pending; see [the guard evidence](s13-r656-guard-line-audit.json).
The original include guard in `mapembed.h` was reviewed separately; see
[its review](s13-r655-mapembed-review.json).

Cached x86, x86-extra, and x64 builds passed. The x86 retained-guard controls
passed 181 checks, and isolated MSVC tests passed fifteen excluded, selected,
and continuation line controls. The first direct test invocation lacked the
MSVC environment; the successful run used the existing x86 context script.

Of the ten release images, six are byte-identical to the previous release and
four differ only in PE timestamps; the parsed comparison is recorded in
[the image comparison](s13-r656-build-image-compare.json). The previous Full
17/17 is reused under the owner's instruction; it was not rerun for this
batch. All ten candidate images were published and SHA-256 verified in both
release destinations. No guest image was modified or rebuilt.

File counts and the separately reported deletions are in
[the census summary](s13-r656-counts.json). Complete supported-context
enumeration and the remaining per-file audits remain open.
