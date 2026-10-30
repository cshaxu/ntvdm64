# Current-source live VDD memory exports

The existing test VDD was loaded in an isolated package staged from the latest
ten-component release. Its callback resolved the actual NTVDM process's query
and reallocation exports by both name and ordinals, then exercised query,
allocate, reallocate and free. An oversized reallocation failed while preserving
the original allocation and free-space observation. The unmodified MEM guest
and its launcher completed successfully; outputs and VDD identities are here.

The isolated package was resealed with a fresh package identity, so its PE
hashes differ from the installed release. All ten `InputSha256` values match
the published release manifest; the staged manifest is retained. This proves
the listed paths for one live worker, not all failure modes, concurrency,
other providers or the separate SOL 1400. The VDD and registry overlay were
test-owned; no original guest file or installed configuration was changed.
