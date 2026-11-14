# Proposal — Unified build.cmd build and release

## Status and owner objective

Owner request dated 2026-11-13: add a candidate at the head of Queue to
clean up and establish `tools/build.cmd`, build all eleven product components
and all add-on components in dependency order, and publish to `assets/release/`.
This is an unnumbered candidate; [Queue](../states/QUEUE.md) owns its position.
The currently active task is unchanged. This update performs no product build
or release.

## Scope and contract

- Provide `tools/build.cmd` as the documented top-level build/release entry.
  Resolve the repository from the script location and initialize the required
  toolchains explicitly. Reuse the existing CMake/Ninja source-owner graphs,
  stable caches and component producers rather than inventing a parallel graph.
- Build the eleven current product images: `run16.exe`, `ntsrv.exe`,
  `ntcon.exe`, `ntvdm.exe`, `ntvwm.exe`, `ntmon.exe`, `WOW32.DLL`,
  `VDMREDIR.DLL`, `VCDEX.DLL`, `nthook32.dll` and `nthook64.dll`.
  Preserve each component's admitted architecture and dependency boundary.
- Cover every independently authored `src/addon` build product, including
  native utilities, MOUSE101/MOUSE31, compat95, insignia-386 and ntvdm64-386.
  Reconcile source directories, real producers, consumers and the current
  addon manifest at admission; the older add-on README alone is not a complete
  inventory. Shared headers are dependencies, not extra release binaries.
- Establish a dependency-ordered build/stage/verify/publish flow. Fail with a
  meaningful nonzero exit status at the failed step; do not mask failures or
  substitute unrelated historical outputs. Reuse unchanged outputs only when
  their declared inputs and toolchain identity are verified.
- Publish the complete validated host set and add-on set to `assets/release/`,
  retaining separate `release-manifest.json` and `addon-manifest.json` ownership.
  Record binary/source identity, architecture and existing acceptance scope.
  Successful compilation must not promote an unverified driver to verified.
- Prepare outputs in build-owned staging and validate completeness and hashes
  before replacing the release set. Preserve a recoverable previous release;
  publication failure must not leave a mixed package reported as successful.
- Consolidate redundant or obsolete project-owned build/publication wrappers
  only after tracing callers and migrating them. Remove proven dead entrypoints
  and update build instructions. Preserve required component producers and
  original-source build metadata; do not delete scripts merely by extension.

Original Microsoft guest media is not rebuilt or patched. This task is build
orchestration and release hygiene, not a runtime architecture rewrite. The
requested default destination is `assets/release/`; installed-runtime deployment
remains an explicit separately selected operation under the existing policy.

## Proposed execution stages

1. Inventory all host/add-on outputs, architectures, producers and consumers;
   establish the dependency order and disposition of existing build wrappers.
2. Implement `tools/build.cmd` and consolidate host build orchestration, with
   explicit prerequisite checks, bounded owned execution, logs and exit codes.
3. Integrate every add-on producer and reconcile the complete release manifests;
   retire superseded wrappers and update their callers and documentation.
4. Complete staged publication and failure recovery. Verify a clean build,
   incremental rebuild, changed-input invalidation, invocation outside the repo,
   missing prerequisites, component failure and publication failure. Check all
   eleven host outputs and every add-on against the published manifests, and run
   the applicable existing component/regression gates.

These stages guide the eventual S breakdown; they do not admit active S tasks.

## Completion criteria

One documented invocation builds the full dependency closure and publishes
both complete verified sets to `assets/release/`. No output is silently skipped,
no test/probe binary enters the release, and failures preserve truthful status
and a recoverable release. Record exact commands, toolchain and input identity,
coverage, output hashes and remaining limitations. Deliver the implementation,
focused regression checks, updated documentation and coherent release artifacts
under the existing verification and delivery rules.
