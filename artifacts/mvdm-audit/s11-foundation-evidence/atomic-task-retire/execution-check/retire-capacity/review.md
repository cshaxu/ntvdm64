# Bounded withdrawal scratch allocation

The page-domain projection formerly allocated 65,536 `ULONG` withdrawal
entries (262,144 bytes) for every call. ADAPTER-WOW-105 counts the existing
window-record list before preparation, adds one spare entry and caps the
allocation at the original table limit. Only a currently published backing
absent from the validated view can be withdrawn. A newly prepared backing is
not yet published; a new record pinned for an existing row belongs to the
view. Guest execution and native callbacks are excluded throughout this
transaction. A bounds check fails the transaction before writing if the
assumption is violated; existing cleanup releases prepared pins and objects.
This introduces no persistent count, object owner or guest/mirror change.

The isolated candidate was built against the current integrated page-domain
source. The archive replacement used the complete stored member path; the
link log has no LNK4006/LNK4014 and a library listing showed only the selected
candidate page-domain object. Graph-snapshot, window-borrow and native-nested
fixtures passed with the candidate selected; these are bounded tests, not a
complete concurrent failure proof. The candidate and nine other selected
component inputs were staged as a coherent isolated package.

Three SOL interactions per package passed in A/B/A and B/A/B runs. One first
candidate close took 6.281 seconds; subsequent candidate and baseline closes
were generally 1.8–2.7 seconds. This is not enough to attribute or exclude a
rare candidate regression, and no reliable latency improvement is claimed.
The separate instrumented candidate logged sampled capacities of 369–382
entries (1,476–1,528 bytes) during two successful SOL interactions. This is
requested scratch allocation, not measured working set; a larger record list
can legitimately require up to the original maximum. Raw measurements,
manifests, sources, build logs and fixture outputs are retained here.

The change is now integrated in the project adapter but remains unpublished;
the production rebuild, integrated fixtures and coherent package gates must
bind their own identities. The earlier candidate's timings are not proof for
that new build, and Full acceptance remains open.

The actual integrated x86 worker was then rebuilt with no duplicate-object
warnings. Its selected graph-snapshot fixture passed, including retained and
released dead windows, duplicate-slot rejection, missing-desktop failure and
unpublished rollback. Build/fixture logs and hashes are in `integrated-graph`.
This is not a coherent package or Full acceptance.

The subsequent coherent integrated ten-image package passed Console-17,
Window-17, all 12 Control gates, three SOL Options rounds, Notepad Find, Write
Find, Program Manager New and SOL gameplay. The shared-WOW idle/dialog witness
passed with two tasks, 17 dialogs, 16 explicit closes and later idle after the
dialog task retired; it uses a freshly sealed identity recorded in its own
manifest. The guest-media contract passed for 59 immutable files, excluding
configuration. Per-run manifests and summaries are in `integrated-*` and
`guest-media-*`. These are bounded witnesses, not a Full-suite pass or proof
that the isolated 6.281-second candidate close can never recur.

Eight additional integrated-package SOL Options rounds completed. The first
open took 7.641 seconds and close 3.609 seconds; the following opens took
1.250–1.688 seconds and closes 1.421–1.843 seconds. An immediately subsequent
eight-round pre-change baseline also completed: first open/close were
2.890/2.438 seconds, with later opens 1.843–2.125 seconds and closes
2.063–2.500 seconds. Both ordered runs and manifests are in `extended-sol`.
This small non-randomized comparison does not establish the cause of the
outlier, exclude a candidate-specific regression, or show the performance
problem fixed. Preserve the observed outlier as an open performance finding.
