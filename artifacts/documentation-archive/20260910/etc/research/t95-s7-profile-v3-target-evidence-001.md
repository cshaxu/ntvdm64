# T95 S7 Profile-Pinned Target Evidence

## Implemented Scope

The modern GCC CLI/profile island now recognizes only the exact first-profile
v3 pair `ntdos64-byob-profile-v3` / `nt4-en-us-command-smoke-v3`. It requires
the existing NTIO, NTDOS and COMMAND components plus one required target
component. The target name is closed to `TARGET.COM` or `TARGET.EXE`; its
ordinary BYOB descriptor supplies exact size and SHA-256 verification.

`guest_target_placement` must exactly be `\\TARGET.COM` or `\\TARGET.EXE`
respectively and use the declared COMMAND drive. The selected descriptor and
placement are copied into `byob_profile_selection` only after the same
identity verification used for every other BYOB component.

`byob_target_selection_matches` compares the runner's normalized local target
pathname against the normalized `BYOB root + selected target filename`. It
does not open content or pass a path onward. The adapter must independently
re-verify and load content with `byob_image_load_exact` before materialization.

## Verification

Fresh root `artifacts/build/t95-s7-profile-v3-target-c11-001` was configured
with CMake 4.1.0 and GCC 16.1.0. Its five-object `byob-profile-test` passed
v1/v2 preservation, v3 target selection, identity verification and invalid
guest target placement rejection.

Fresh root `artifacts/build/t95-s7-target-identity-c11-001` built and ran the
three-object `byob-target-selection-test`, which accepted the selected
`TARGET.COM`, rejected another file, and rejected a mismatched descriptor.

## Deferred Composition

The runner deliberately does not call the guard yet. It still puts the raw
target path after the engine's `--` separator. Calling the guard without
replacing that old handoff would give a false impression that host paths had
been removed. The next cohesive change must define a copied, path-free launch
declaration containing target kind and normalized DOS tail, have the runner
pass only it through its child-only environment, and have the adapter reject
its absence or malformed form before session installation. Only then can the
old target argument leave the engine invocation.
