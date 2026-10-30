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
