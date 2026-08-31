# Build Tools

This directory contains tracked build entry points.  Each tool has one
declared responsibility and may create or consume only disposable output below
`build/<task-id>/<run-id>/`; it never publishes compiler output to `tools/` or
`artifacts/`.

Root-level compiler/linker products are prohibited and deliberately are not
hidden by `.gitignore`. Reusable caches remain under their declared
`build/<task-id>/<run-id>/` roots.

`New-T310OriginalSoftpcNinja.ps1` creates the selected immutable Win32/x86
CCPU40 formal graph in a caller-selected new build root. It rejects retired
machine inputs and may refresh only graph metadata in an existing disposable
root; it never imports cache output as a source or release input.

The generated `run-ninja-parallel.cmd` is the formal execution entry point for
that graph. It initializes the matching MSVC environment once, then invokes
Ninja with eight translation-unit jobs. Build rules invoke `cl`, `rc`, `lib`,
and `link` directly from that inherited environment; they do not start a
per-translation-unit shell or environment script. `/MP` remains deliberately
absent because Ninja owns parallel scheduling. Pass a narrow library or
fixture target during iteration and use the complete candidate target only for
a packet gate.

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
