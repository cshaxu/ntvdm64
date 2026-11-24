# T484 S3 P191: native generic-thunk fixtures

The focused generic-thunk runner no longer assembles its NE image or compiles
its native DLL through an external CompilerDriver. That unused argument is
removed; no current repository caller passes it. It reuses the existing generic
NE output and a shared CMake DLL recipe with explicit normal/cdecl profiles.
The existing TEST_GUEST_EXCEPTION profile remains in that same recipe and keeps
its prior output location, flags and exports. No product or original guest
source is changed. Runtime assertions and isolation remain unchanged.

## Actual verification

Owned normal-user-environment run `build/t484-s3-generic-native/run.ps1`,
PID3500, exit0, completed both real Win16 cases:

- normal: three guest phases, zero errors, three releases;
- mismatch/cdecl: three guest phases, zero errors, three releases;
- both retain expected nested callback counters in the54-byte guest report.

The NE image exactly matches the old assembler command. Repeated native builds
preserve both DLL timestamps. The current published11-image package was copied
to an owned source root and each case used the existing private-desktop physical
runtime stage. No Computer Use or manual desktop interaction occurred.
Normal-user environment values were not recorded. This is focused runtime
coverage, not a new Full suite result.

Cached full11 raw/22 published hash checks are recorded by delivery-result.json.
The final closure script also rebuilds the existing exception fixture target,
checks its image hash against the pre-build image, and runs documentation,
ledger, final caller parsing and diff gates. No unchanged host republish is
required. Owned package-source cleanup is recorded separately after proving
no process executes within it.

All18 extra direct-NASM sites found in P183 now have native production or a
retired interpreted entry. A fresh `& nasm` scan of test/integration has no hits;
this is only that direct-call class, not proof of all indirect build edges.
Fixed census remains316/355 disposed,39 pending. Nineteen supplemental caller
chains still retain shared configuration/staging reconciliation, distinct from
the retired observer producer. S3 and later S4/S5 remain unfinished.
