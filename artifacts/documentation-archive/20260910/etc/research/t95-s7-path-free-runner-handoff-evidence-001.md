# T95 S7 Path-Free Runner Handoff Evidence

## Contract

For a DOS-engine invocation, `ntdos64-run` now validates the exact v3 profile
selection, verifies that the resolved CLI target path equals the selected
BYOB target resource, and derives a copied launch declaration. The child gets:

```text
argv: <engine> --byob-profile <profile> --byob-root <root> --
environment: NTDOS64_ADAPTER_LAUNCH_KIND=com|exe
             NTDOS64_ADAPTER_LAUNCH_TAIL=<bounded ASCII tail>
```

The resolved target host pathname and all tail arguments are absent from the
engine argv. Profile/root and the previously admitted include/exclude drive
policy remain child-only environment inputs. No path is passed to adapter or
Bochs as an environment value.

## Verification

Fresh root `artifacts/build/t95-s7-path-free-runner-c11-001` used CMake 4.1.0
and GCC 16.1.0 to build `ntdos64-run` and `runner-engine-probe`. The policy
fixture created a v3 BYOB root with selected `TARGET.COM` and invoked the
production runner with `/c smoke`.

The probe accepted only six argv elements, ending in `--`, and independently
checked profile/root, `C,D,E` inclusion, `E` exclusion, launch kind `com`,
and launch tail `/c smoke` in its child environment. The policy completed with
exit zero. Its displayed usage, missing-BYOB and invalid-drive messages belong
to the fixture's intentional negative cases.

## Limit

The adapter has not yet consumed the declaration. It cannot yet materialize
the target bytes, and `BOP_CMD:01` remains pass-through. The next change must
make adapter installation require and copy the two values for a v3 selection,
and reject missing, malformed or profile-kind-mismatched values before it
marks the session installed. It must not add the target to the namespace or
route a BOP in that change.
