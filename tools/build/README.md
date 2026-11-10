# Build Tools

This directory contains tracked build entry points. The
[execution policy](../../docs/rules/EXECUTION.md#build-cache-and-disposable-lifetime)
is the sole output/cache/lifetime authority. CMake presets select native
architecture/configuration trees; original source-owner manifest generators
retain their source lists and historical flags. Clear-BuildOutput.ps1 alone
owns exact-target disposal and final residual inspection.

Root-level compiler/linker products are prohibited and deliberately are not
hidden by `.gitignore`. Do not keep a reusable task-specific configure tree.

`New-T310OriginalSoftpcNinja.ps1` creates the selected immutable Win32/x86
CCPU40 formal graph in a stable CMake-validated Ninja subtree. It rejects retired
machine inputs and may refresh only graph metadata in an existing
tree; it never imports cache output as a source or release input.

The formal product graph requires explicit `-NativeWorker` and
`-NativeFrontend` and `-NativeMonitor` AMD64 producer images; it cannot silently
link x86 NTVWM, NTCON or NTMON. Declarative source closures remain available to
`New-NativeWorkerNinja.ps1 -Component Worker|Frontend|Monitor`, which builds only the
selected native consumer and architecture-local dependencies. Frontend has
no worker-base or original RTL body dependency; source-specific C11 settings
and test entry points are retained. Import validates PE machine, EXE/subsystem
and copied hashes before replacing a build-cache product slot.

Monitor selects the existing management/RPC and broker transport plus its
native MIDL client, not machine/frontend font code or original VDM/RTL bodies.
Its layout/RPC/session fixtures use the same selected sources; actual package
verification can select an explicit native MonitorRpc fixture so x64-client/
x86-service behavior is exercised rather than only the old x86 test client.

`Generate-ObservationNinja.mjs <repo> <build> [x86|x64]` defaults to x86 for
existing callers. Observing private Window userdata requires an observer of
the frontend's native width; its NTVDM fault diagnostics retain the x86
context through WOW64 APIs when the observer is x64. This is test-only memory
observation, not a production protocol or extra product/helper.

`Invoke-T310OriginalSoftpcSerial.ps1` is the bounded fallback for a host where
Ninja itself cannot dispatch its first job. It reads the generated graph's
declared VS/Ninja paths, expands only the requested Ninja targets with
`ninja -t commands`, and executes that already-expanded dependency order in
one x86 toolchain environment. It stops at the first command failure; it is
not an alternate graph, compiler profile or source input.

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
