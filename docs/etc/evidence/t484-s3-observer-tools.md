# T484 S3 P13: native observation tools

Generate-ObservationNinja.mjs and Generate-CleanConsoleCommandIntegrationNinja.mjs
are replaced by `tools/build-fixtures.cmd <x86|x64> observation-tools`.
Five unchanged observer sources build on both architectures; the unchanged
clean-console integration helper remains x86-only. C11, warning/error policy,
source includes and explicit system libraries follow the original profiles.
The shared fixture linker supplies its standard map and unused-code removal.
No product algorithm or original guest is changed.

All11 executables compile. The input-milestone test passes on both architectures,
checking fresh/partial/stale echo, cursor/tail state and mismatched window PID.
The other9 instances return their defined missing-argument error codes. These
are actual negative invocation checks, not GUI or guest integration acceptance.
This batch does not claim that every observer scenario was rerun.

No executable consumer still names either removed generator. The independent
BuildConsoleRegression.cmd is already a direct CMD/compiler path and remains;
its existence is not hidden as an eliminated build edge. Documentation names
the canonical native target and retains the frontend-width requirement.

Cached full host build PID22680 passes. All11 raw host inputs and22 hashes at
the two publication locations are unchanged; no new product image needs to be
copied. Package d8983d2b-3fe1-4d18-8ea9-64613c84346c remains current. Mirror and
original guest changes are0. [Receipt](../../../artifacts/build-migration/t484/s3-observer-tools/result.json)
records output identities and actual expected/observed return codes; adjacent
product-build receipt records the full build. The containing commit is P13's
delivery identity. Remaining test producers and S4/S5 acceptance are pending.
