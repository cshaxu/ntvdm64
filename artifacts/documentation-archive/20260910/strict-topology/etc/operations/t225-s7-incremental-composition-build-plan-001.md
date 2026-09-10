# T225 S7 incremental composition-build plan

## Scope

S7 creates a reproducible local build cache for the current CPU5/P, x64, `/MT`
composition. It is a build-system prerequisite for the paused T225 S6 fixture;
it changes neither guest inputs nor runtime semantics.

## Module products

| Module | Product | Ownership rule |
| --- | --- | --- |
| bx-core | static library/object set | Minimal Bochs execution mechanics only. |
| bx-mantle | static library/object set | Minimal Bochs machine assembly and mechanical bridge. |
| bx-vdm | static library/object set | VDM adapter, profiles and host-service composition. |
| opennt-host | static library/object set when independently composable | Original host providers or admitted compatibility seams; never guest images. |
| cli | static library/object set | CLI declaration and launch contract. |
| target | freshly linked executable | Always linked from verified module products; never cached as an acceptance result. |

## Cache identity and reuse

Each reusable object/library has a manifest containing its source hash, complete
transitive include hash list, compiler path/version, architecture, CRT,
language mode, defines and ordered include roots. A cache hit requires exact
identity. Any source/header/toolchain/configuration mismatch recompiles the
affected module and relinks the target. Cache files live only below the S7
working `build/` root and are not artifact, source, runtime or release inputs.

## Verification sequence

1. Cold compile and link a focused bx-vdm target.
2. Repeat unchanged and prove object-cache hits plus a fresh target link.
3. Change one declared local source in a disposable verification copy; prove
   that only its owning module recompiles and the target relinks.
4. Re-run the normal source unchanged and prove the cache identity restores.
5. Record manifest, commands, outcomes and timing; then resume S6.

## Boundaries

The cache does not decide component ownership, suppress compiler diagnostics,
reuse a previous executable, admit prebuilt binaries or alter Bochs/OpenNT/
guest behavior. Failure to compute a complete dependency closure is a cache
miss, never permission to reuse an object.