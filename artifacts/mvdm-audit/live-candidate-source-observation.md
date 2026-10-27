# Live candidate source differences

The frozen S10 inventory remains unchanged. A later read-only sweep observes
two originally identical video files changed by the concurrent owner:
`v7_ports.c` and `vga_mode.c`. Their current bytes therefore require partial
classification under the owner's mirror-diff rule, even before that candidate
has been reviewed or accepted. This audit neither changes nor commits those
production source files.

`live-candidate-source-retention.csv` records original/live hashes and the
existing source-line measurement algorithm's results: 515/516 code lines
(99.81%) and 347/350 (99.14%). These are source retention estimates, not
restored functional coverage. The candidate changes cursor scaling and packed
scanout decisions; original-owner behavior, divergence registration and relevant
guest acceptance remain the candidate owner's obligations. A functional
restoration fraction cannot be inferred from those small diffs.

The six current source/registry drifts are retained in `initial-summary.json`.
Existing original differences stay partial; no changed source is promoted to
full by this metadata checkpoint. The sealed package used for earlier guest
tests remains its own identity and is not proof about these uncommitted bytes.
