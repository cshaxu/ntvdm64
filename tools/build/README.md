# Build Tools

This directory contains tracked build entry points. The
[execution policy](../../docs/rules/EXECUTION.md#build-cache-and-disposable-lifetime)
is the sole output/cache/lifetime authority. CMake presets select native
architecture/configuration trees; original source-owner manifest generators
retain their source lists and historical flags. Clear-BuildOutput.ps1 alone
owns exact-target disposal and final residual inspection.

Root-level compiler/linker products are prohibited and deliberately are not
hidden by `.gitignore`. Do not keep a reusable task-specific configure tree.

T484 is migrating the remaining interpreted build owners. `bootstrap.cmd`
builds the fixed native data transformers in `build/.ninja/build-tools` using
MSVC and native CMake/Ninja. The root CMake configuration calls it before
graph generation; direct legacy generator callers must run it first.
`NTVDM_VSDEVCMD` may select the Visual Studio environment script explicitly.
The font transformer verifies the pinned ROM and emits the existing two
bitmap arrays. The C-VID transformer reads the original slot/declaration and
offset metadata and preserves its count checks. Neither launches processes
nor interprets build recipes. Their unchanged outputs retain their timestamps.
They are build tools, not additional released product components. Remaining
PowerShell/Node/Python build edges have not yet been removed by this first step.

`ntvdm-tib product.map dpmiint.obj` replaces the interpreted post-link check.
It checks actual x86 COFF storage, linked ownership, overlap and startup
binding. Malformed input is a failure. It does not generate product code or
launch processes; both production and instrumented links use the same check.

The AMD64 Hook graph is now owned by `hook/CMakeLists.txt`, not a PowerShell
graph generator or a parsed x86 reference graph. Its original25 source entries
retain their order, individual include boundaries and selected MSVC options.
MIDL, DLL exports and the existing installer/image-machine fixtures are native
targets in the same stable `msvc-x64-release/hook` tree. The fixture launcher
comes from the actual native launcher producer, never published assets.

`tools/build.cmd` configures and builds all eleven host images with native
CMake/Ninja, building AMD64 producers before their x86 consumers. It works
from any current directory and accepts optional `x64` or `x86` selection;
x86 requires the current native producers. `worker/CMakeLists.txt` owns the
CCPU40 worker, VCDEX, VDMREDIR, Hook32 and existing worker fixtures. Original
source lists, selected flags, export ordinals and source hashes remain live
inputs. No released package is used to bootstrap host compilation.

The old T310 generator remains temporarily for test-producer callers awaiting
S3 migration; it is no longer called by the product configure/build entry.
Publication is still the separate S4 migration obligation.

The five AMD64 consumers are now owned by
`native-family/CMakeLists.txt` and their component source/link lists. They do not
consume an x86 reference graph or invoke an interpreted graph generator. Native
MIDL, original configuration extraction and manifest hashing remain live input
operations. The NXVM import manifest still selects and verifies its sources.
Frontend has
no worker-base or original RTL body dependency; source-specific C11 settings
and test entry points are retained. Import validates PE machine, EXE/subsystem
and copied hashes before replacing a build-cache product slot.

Monitor selects the existing management/RPC and broker transport plus its
native MIDL client, not machine/frontend font code or original VDM/RTL bodies.
Its layout/RPC/session fixtures use the same selected sources; actual package
verification can select an explicit native MonitorRpc fixture so x64-client/
x86-service behavior is exercised rather than only the old x86 test client.

`tools/build-fixtures.cmd <x86|x64> observation-tools` builds the native
observers. Observing private Window userdata requires an observer of
the frontend's native width; its NTVDM fault diagnostics retain the x86
context through WOW64 APIs when the observer is x64. This is test-only memory
observation, not a production protocol or extra product/helper.

`tools/build-fixtures.cmd x86 wow-controls` builds the explicit native WOW
control graph. The observation runner retains runtime assertions and research
selection; it delegates all binary production to that CMD entry.

`tools/build-fixtures.cmd x86 worker-diagnostics` builds the former environment,
COMMAND, thread-exit and DPMI trace variants under the worker cache's
`diagnostics/<boundary>/`. Each image writes `events.txt` in its own directory;
the lifecycle-only DPMI variant is named `dpmi-lifecycle`. They reuse current
product profiles, archive membership and link inputs. These diagnostic images
are not release images or self-contained runtime packages. Use the established
isolated-package staging and matching manifest before a guest diagnostic run.

`tools/build-fixtures.cmd x86 cpu-contracts` builds the NEG64, leaf declaration,
CPU-notification and native INT-prefix fixtures. The assertion entry is
`test/observation/verify-cpu-contracts.ps1 -EvidenceRoot <fresh-build-path>`;
it also requires the explicit negative ABI targets to fail with C4113.

For serial worker compilation, set `MVDM_BUILD_JOBS=1` and call the generated
`build/.ninja/msvc-x86-release/worker/run-ninja-parallel.cmd` with the target.
This uses the same native dependency graph and initializes its MSVC environment.
The former PowerShell command-replay fallback is retired; a Ninja dispatch
failure remains a failed build rather than triggering a second execution path.

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
