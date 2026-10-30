# Current diagnostic: identity of failed required window

A test-only WOW32 build adds last-error-preserving logs for failed required
root capture and local menu-owner GetWindowInfo, recording the exact full HWND
and status. It uses the earlier split-return diagnostic lifecycle object; the
patches, builder, hashes, ten-image manifest and probe output are retained.
The object-capture marker identifies the failing required root, not every
possible graph failure; the menu marker covers local owner info failure only.
Other failure paths and HWND reuse/lifetime ordering still require evidence.

Four SOL Options open/close/recovery rounds passed in a fresh isolated package;
no new marker fired. This verifies only the diagnostic package's normal path,
not a real failure or the previous 1400. Logging may affect timing. The added
failure probes have not yet been separately exercised with controlled failing
roots in this build. No production or guest files were changed or published.

## Controlled negative identity witness

A later selected-CCPU page-domain fixture linked both diagnostic objects
before the selected provider archive. A test-only fixture copy prints its
expected full HWND immediately after creating and destroying each owned test
window; all original graph and menu assertions remain. The fixture completed
with exit 0, including graph rollback/withdrawal and menu negative checks.
The required root was `050B0A06`, and the `required-root-capture` marker
reported that exact HWND and 1400. The synthetic stale menu enrollment was
`050C0A06`, and `menu-owner-info` reported that exact HWND and 1400. These
handles are unique to this run. `verify-negative.py` checks the recorded
expected identities, both marker statuses, fixture assertions and map symbols.
Input hashes bind the modified fixture, executable, map and diagnostic objects.

This verifies these two diagnostics under controlled failures and preserves
the real graph failure semantics in the selected-object fixture. The menu
failure is synthetic and this does not prove production can leave that stale
entry; neither case reproduces the historical SOL 1400 or determines its
cause. The preceding paragraph's untested-negative limitation described the
prior normal-only run, not this subsequent fixture.
