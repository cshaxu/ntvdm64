# S13: recursive original conditional trees and line mappings

Ten source files were preflighted against original conditional trees, then
their existing rejected arms and continuation lines were mapped to original
physical line numbers. All ten resulting trees were checked recursively and
recorded in [per-group evidence](s13-r668-tree-review.json); rejected arms
prune original children and retained leaves compare to original. Local code
outside conditional groups and complete supported-context enumeration remain
separate. [Batch evidence](s13-r668-tree-anchor-batch.json) records hashes and
original diagnostic lines.

Cached x86, x86-extra and x64 builds passed. The x86 guard suite passed 181
controls and 74 isolated MSVC error/continuation line controls passed; these
substitute a test selector and do not independently validate all source macro
combinations. Eight product images are byte-identical to the previous release
and two differ solely in parsed PE timestamps:
[comparison](s13-r668-build-image-compare.json). The prior Full selected gates
and Console/Window 17/17 are reused per owner instruction, not rerun. All ten
images were published and hash-verified at both destinations. Guest images
were neither modified nor rebuilt. [Counts](s13-r668-counts.json) keep deleted
files separate.
