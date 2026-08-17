# T225 S8 Primary Source-Built Input Availability

## Question

Are the four physically distinct original-toolchain guest-artifact roots
available for the required primary-sequence installation cross-validation?

## Inputs and procedure

The new `tools/build/Stage-T225S8PrimarySourceBuiltInputs.ps1` requires each
stage root, its declared build manifest, its expected artifact path, and the
locked byte/SHA-256 identity before it creates an output directory. It does
not accept `refs/opennt`, the OpenNT-tree reference sequence, or an arbitrary
same-hash file as a substitute.

A negative invocation against the repository root required:

- `artifacts/toolchain-runs/ntio-tools16-opennt-v1/ntio-build-manifest.json`;
- equivalent NTDOS, COMMAND and SHARE manifests and artifacts.

## Observation

The first required manifest is absent. The tool refused before output staging:
`Primary stage manifest missing for ntio` at the exact declared primary root.
No reference artifact was copied and no fixture or guest trace was run.

## Interpretation

The current checkout has the approved secondary OpenNT-tree reference sequence
and matching hashes, but lacks the primary source-built artifact roots. They
remain distinct provenance sequences. The present result is an artifact
restoration gate, not a BOP, machine, or profile failure.

## Follow-up

Restore all four original-toolchain run roots with their manifests, then run
the stager and the existing Ninja-linked Direct/Readonly installation fixture.
Overlay and retired Virtual must continue to reject. Only after this
cross-validation closes may a separately admitted native integration
observation be considered.