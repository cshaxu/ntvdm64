# M0 T299 S2 — first-degree call-site identity normalization plan

## Correction

T299 S1's 801-row interface ledger is a useful spelling index, not a final
function-identity ledger. Different original files can declare or call the
same spelling with distinct ABI, static scope, macro expansion or product
variant. The raw caller records were retained, but they must now drive the
identity.

## Required result

For every raw first-degree call that remains external after T298 macro
normalization, locate its original implementation candidates.  Each found
implementation is an independent one-degree function identity keyed by source
root, path, file hash, definition line and signature—even where the spelling
is identical.  The caller is linked to every compatible implementation
identity, but no same-spelling implementations are merged.

Only after a concrete implementation identity is located may its original
function body be scanned.  Its direct non-macro calls become second-degree
candidate rows whose caller is that exact implementation identity.  A call
without a located implementation has no fabricated second-degree expansion.

All inputs remain original-source-only; current mirrors, adapters, outputs and
`src.old` are prohibited. The former 801-row ledger remains a spelling index
and S1 evidence, but no longer authorizes a second-degree frontier.
