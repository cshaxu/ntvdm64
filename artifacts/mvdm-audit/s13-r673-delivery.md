# S13: non-monitor and BIGWIN context reviews

Two files completed conditional structure review. For `cpucstbs.c`, actual
x86 worker and x64 reference-x86 Ninja compile commands were used with a
temporary source copy containing a marker within the original non-MONITOR
arm. Both preprocessed outputs included the marker; the nested A3CPU arm is
already error-only. See [evidence](s13-r673-monitor-review.json).

`nt_munge.c` is byte-identical to its original and has one BIGWIN conditional
group. Both worker graphs' real commands were preprocessed and the two BIGWIN
function definitions were observed. See [evidence](s13-r673-bigwin-review.json).
These are observations of the two actual graphs, not proof of unenumerated
contexts. No product source or runtime image changed; prior published build
and Full evidence remain unchanged. [Counts](s13-r673-counts.json) report
deletions separately. Remaining contexts and files are still open.
