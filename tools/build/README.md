# Build Tools

This directory contains tracked build entry points.  Each tool has one
declared responsibility and may create or consume only disposable output below
`build/<task-id>/<run-id>/`; it never publishes compiler output to `tools/` or
`artifacts/`.

Root-level compiler/linker products are prohibited and deliberately are not
hidden by `.gitignore`.  CMake configuration and the T260 formal Ninja graph
reject a root containing `.obj`, `.exe`, `.pdb`, `.ilk`, `.map`, `.lib`, `.a`,
`stdout.txt`, or `stderr.txt`.  Reusable caches remain under their declared
`build/<task-id>/<run-id>/` roots.

`New-T260S8FullNinjaGraph.ps1` normally creates an immutable formal graph in a
new build root. Its explicit `-Refresh` switch is the controlled incremental
path for an existing formal root below `build/`: it regenerates only the graph,
manifest, response files and projected config, retaining object files for
Ninja's normal source/header dependency checks. It is appropriate after an
owner-preserving source move; it never imports cache output as a source or
release input.

An admitted fixture may declare `supportSources`, each constrained to a
`tests/` source. Ninja compiles those objects immediately before the fixture
and its libraries. This keeps a test-only replacement of a generic mechanical
callback out of every production library; it is not a second production build
variant or a component input.

`Invoke-DosBoxBatchWithCompletion.ps1` is the T225 S8 host-process wrapper for
an already staged DOS batch.  It neither interprets guest/build semantics nor
supplies tool arguments: the batch owns those.  The wrapper only mounts the
declared disposable stage in DOSBox and waits for its declared completion
marker.

Build-T225S8PrimaryNtio.ps1 reproduces the original NTIO tools16 closure in a caller-selected fresh stage: OpenNT BUILDIDX/NOSRVBLD, MASM, LINK16 and RELOC. It verifies the generated message classes and locked NTIO.SYS identity before writing the stage manifest.

Stage-T225S10PrimaryBootstrapInputs.ps1 creates a separate, disposable S10 native-observation input root from the four S8 primary source-built stages. It verifies every stage manifest and artifact identity, preserves all image bytes, and records the profile-only SHARE.EXE to TARGET.EXE alias required by the current CLI target-admission contract. Its emitted manifest limits use to the bounded Direct/Readonly bootstrap observation.

Invoke-T225S10PrimaryBootstrapObservation.ps1 is the bounded host watchdog for the S10 native observation. It starts only the declared native CLI twice, redirects its output into the caller-selected disposable run root, and terminates only that child process if its wall-clock budget expires. It does not inspect guest memory or interpret BOPs.

`New-T235S1RunnerNinjaGraph.ps1` emits the isolated MSVC x64 `/MT` Ninja graph
for the modern runner and its external-engine probe. It has no Bochs, OpenNT
runtime-object, guest-media, or artifact-build input and is used only to build
the two focused CLI executables in a caller-selected disposable root.
