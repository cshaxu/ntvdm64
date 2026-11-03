# S13: further original condition trees

Ten sources were preflighted against their original conditional trees and
received original diagnostic and continuation mappings. All ten resulting
trees were checked recursively and registered in [per-group evidence](s13-r670-tree-review.json).
Error-only arms prune original children; retained leaves compare to original.
Local code outside conditional groups and complete supported-context
enumeration remain separate. [Batch evidence](s13-r670-tree-anchor-batch.json)
records hashes and original diagnostic lines.

Cached x86, x86-extra and x64 builds passed. The x86 guard suite passed 181
controls; 74 isolated MSVC error and continuation line controls passed using a
test selector rather than independently testing every original macro profile.
Eight images are byte-identical to the previous release and two differ only
in parsed PE timestamps: [comparison](s13-r670-build-image-compare.json).
Previous Full selected gates and Console/Window 17/17 are reused per owner
instruction, not rerun. All ten images were published and hash-verified at
both destinations. Guest images were not modified or rebuilt.
[Counts](s13-r670-counts.json) keep deleted files separate.
