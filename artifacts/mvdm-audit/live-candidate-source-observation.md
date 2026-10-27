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

## Later graph/palette observation

During the Kernel31 source-role checkpoint, the whole-tree hash assertion
detected a newer concurrent `nt_graph.c` candidate. Its previous observed hash
was `3B2E6D7959A93B05B3F3856E3A00E9455EE64DF489A36712643CB257FB5D3E86`;
the new observation is
`8D10D85B80703C3515A92E1761FB32018620179B9ED93FF52137161BD53233C8`.
The visible uncommitted delta adds a VIDEO7 DAC-width declaration and uses
6/8-bit-dependent palette conversion in both original palette loops; the
earlier chain-selection change is also still present. This is source
observation, not acceptance of its provenance, registration or behavior.

`nt_graph.c` was already partial before this newer change, so no ledger
classification changes. The [latest source-retention measurement](nt-graph-live-retention.csv)
uses the existing comment-stripping/matching-line algorithm: 1,415 of 1,424
original code lines match (99.37%). This is text retention, not restored
functional coverage. The earlier frozen-source measurement remains historical
rather than being relabelled as current. The hash assertion's initial failure
is retained as a successful detection of snapshot drift; the later checkpoint
must pass against the explicitly refreshed observation. No candidate source
is changed, compiled, committed or published by this audit.
