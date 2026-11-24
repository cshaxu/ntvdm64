# T484 S4 FCB preparation

P20 completes the FCB preparation chain without a separate tool. The shared
CMD physical stage accepts an optional single TestSubdirectory leaf, requires
actual TestFiles, rejects separators/traversal and refuses an already copied
subdirectory. Default TestFiles placement is unchanged. Creation/copy remains
inside the original owned-stage success/failure and marker boundaries.

The existing declarative guest-api producer generates three fixed two-byte AB
files (AAA/BBB/CCC.TST). The FCB caller stages them and its cached COM directly
into tests/DFCB01, preserving the original private-directory contract. It no
longer creates/copies guest inputs through PowerShell. Guest source, FCB
assertions and result collection are unchanged.

## Verification

Owned modern pwsh/current-token Userenv PID7328 in build/t484-s4-fcb ran
2026-11-24T18:31:03Z to18:32:43Z, exit0. Cached full build passes.
The existing package-identity suite verifies UUID/machine/manifest validation,
all-input preflight, refresh, flat/system32/default/selected staging, Unicode
ownership and default TestFiles behavior. Added assertions verify subdirectory
byte identity, no root leakage, seven invalid leaf forms, missing TestFiles,
existing-directory refusal and preservation of the source collision sentinel.
Expected rejection messages are asserted negatives, not waived failures.

The real independent FCB COM returns receipt37, three unique AAA/BBB/CCC TST
names, EOF FF and missing-name FF. Its40-byte FCB1 report is checked and retained.
This does not claim FCB read/write or mapping-change coverage.
All11 raw product images and22 dual-published hashes remain unchanged.
No new Full regression, original guest rebuild or product modification.

## Accounting

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease13 to12. These are overlapping scopes, not24 distinct files.
S4 remains active and S5 final clean acceptance remains pending. Final parser,
governance and diff checks precede the containing P20 commit and push.
