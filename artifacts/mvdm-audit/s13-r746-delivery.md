# S13: mirror branch batch r746

Ten source files received condition exclusions or original diagnostic line
mappings. The original condition frameworks and observed selected bodies are
retained. Eight files have completed condition-tree reviews within observed
worker graphs. Printer and EGA port files remain pending whole-file review;
only their identified unselected branches are recorded here. The per-file
r719-r746 evidence records source hashes, original source, witnesses and
same-directory include comparisons.

A marker placed immediately after a directive inside a multiline comment can
be swallowed by that comment. The affected EGA-video branch was explicitly
rechecked with a marker after the comment, and was selected in both observed
graphs; it was preserved. Later EGA-video witnesses used comment-aware marker
placement. See [marker evidence](s13-r739-egavide-marker-boundary.json).

Cached x86, x86-extra and x64 builds passed. [Ten-image comparison](s13-r746-build-image-compare.json) shows eight byte-identical images and two
with only PE timestamp differences. Under owner direction no Full repeat was
performed; the earlier Full evidence is r682c. All ten candidate images were
published and hash-verified in assets/release and O:/winnt/system32. Guest
images were neither modified nor rebuilt.

[Counts](s13-r746-counts.json): 1058 current C/H files in the two roots; 168
reviewed identical, 685 reviewed different and 205 pending. The 578 net deleted
C/H files relative to c97d4ee2c are separate. These are filesystem counts, not
proof of complete source provenance or all supported build contexts. The next
work follows the pending inventory in order, including files needing no edit.
