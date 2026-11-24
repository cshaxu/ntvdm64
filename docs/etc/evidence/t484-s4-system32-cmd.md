# T484 S4 P2: CMD system32 package composition

## Ownership and implementation

The old tools/build/Stage-System32ProductPackage.ps1 producer is removed.
Its current interface is tools/stage-system32.cmd with the same named input
options. CMD owns fresh build-only destination checks, baseline copy, explicit
input selection, source/destination hash comparison, native PE checks and UUID
stamping, flat-image removal, Registry overlay conflict checks, license copy
and final manifest publication. Failed work retains diagnostic files but removes
the completion manifest. Existing output roots are rejected before modification.

Existing HASH.EXE and ntvdm-package-id provide hashing, UUID and PE operations.
CMake configures the single CMD implementation from an LF-authored template
into CRLF runtime form. An LF/CRLF control proved CMD CALL-label position errors
in the LF candidate: two images were omitted despite success. This was rejected
by eleven-row comparison, not waived. The wrapper bootstraps native tools before
calling the configured file; no Python/PowerShell/JavaScript build interpreter
or second packaging implementation remains in this entrypoint.

The native CMake configuration also prepares the two license text fragments
with their existing CRLF/LF conventions and without input BOMs. CMD concatenates
them bytewise. Earlier copy-path and license-format candidate failures remain
in the build evidence; the final package comparison requires exact bytes,
including the license. No original guest or winnt.zip is modified.

## Tests

Four current observation tests now call test/observation/Invoke-System32Stage.ps1,
an assertion-runner argument/error adapter only. It invokes CMD and cannot
produce a package itself. The existing full staging test now checks eleven
images including VCDEX instead of its stale ten-image assertion. It retains
one package identity, overlay byte preservation, mixed-layout rejection and
conflicting-overlay rejection without a completion manifest. Added controls
cover wrong native-worker architecture, missing Hook64, outside-build worker,
missing VCDEX, zero identity and existing-output preservation.

## Evidence and limitations

build/t484-s4-system32/equivalent/run.ps1, PID10352, exited0 on2026-11-24
15:26:51Z. It compares the old producer saved from the pre-change revision with
the current CMD output using the same UUID and actual eleven build inputs.
Every manifest field matches; every package file other than JSON formatting
matches SHA256, including original media and combined license. The existing
CMD staging positive/Registry/mixed-layout negatives also passed.

Final expanded tests and cached full build run in
build/t484-s4-system32/delivery (PID12472); result.json is the final-code terminal
witness, with full-build.log and11 raw/22 published hash checks. Governance and
ledger reconciliation run separately. This is package acceptance, not a new
Full guest regression. Other shared staging and addon publication remain S4.
The existing P193/P215 runtime limitations remain unchanged.

Final-code PID12472 passed at2026-11-24 15:29:57Z, including expanded
negative tests, cached full build and11 raw/22 dual-publication hash checks.
Governance PID12108 passed. Product bytes are unchanged, so existing exact
dual publication is retained. Census is324/355 disposed,31 pending;24
supplemental preparation chains remain. No mirror diff is introduced.
