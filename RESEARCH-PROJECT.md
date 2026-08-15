# Pre-Governance Independent Research Project Record

> **Superseded as the current goal authority on 2026-08-09.** Use
> [docs/design/GOAL.md](docs/design/GOAL.md). This detailed scope record is
> retained supporting evidence during the controlled documentation migration.

## Scope

This directory is an independent, non-Git research workspace. Its active
implementation objective is to repair and compose the OpenNT NT4
SoftPC/CCPU software-monitor path. It must not modify, link against, copy
from, or become a build dependency of `ntvdm64`, Bochs, PCjs, or another
external execution implementation.
No registry change, system-directory deployment, DLL injection, or automatic
host configuration is permitted here. Microsoft guest or system binaries may
be retained only as explicitly manifested, research-only artifacts; they may
not become a default build input, runtime dependency, or release asset.

The desired user-facing experiment is a CLI that classifies an executable:

* PE32 and PE32+ programs are launched normally by the host with
  `CreateProcessW`.
* DOS or 16-bit programs are offered only to an explicitly selected local
  runtime engine.
* Any Microsoft guest/runtime file used by the runner is BYOB: it is discovered
  at a user-selected path and is never copied into a normal output package.
  Fixed reference files retained for build-comparison evidence are segregated
  under `artifacts/`, hash-manifested, and are not runner inputs.

The historical runtime-restoration rule is binding for all NTDOS work:
`CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS`. Existing
self-authored CCPU adapters, BOP/DEM handlers, and scaffold IVT/BDA
initialization are trace instrumentation only; they cannot become the normal
DOS runtime or be linked into the default CLI target. See
`docs/HISTORICAL-RUNTIME-RESTORATION-DECISION.md` and
`docs/DEFAULT-BUILD-INSTRUMENTATION-ISOLATION.md`. The current source-role and
reconstruction rules are authoritative in
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`; an older backend/core/mantle work
order is retained only as archival evidence and cannot schedule, gate, or
replace SoftPC work.

## Source Snapshots

| Source | Local path | Revision | Intended use |
| --- | --- | --- | --- |
| OpenNT VDM subtree | `refs/opennt` and fixed copy in `src/opennt` | `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c` | Historical CCPU/BIOS/BOP/DEM research input, default-disabled. |
| NTVDMx64 patches/loader | `refs/ntvdmx64` and fixed copy in `refs/ntvdmx64-derived` | `84a13d2e7bb1a55d11148971e5b9c8ec99f670bf` | HAXM monitor reference input only, default-disabled. |

The fixed source roots are research-only and are never a redistributable or a
normal product dependency. The owner has authorized OpenNT as a
non-redistributable research build input for this workspace. `src/opennt` and
`refs/ntvdmx64-derived` preserve separate source universes and manifests; the upstream
checkouts remain unchanged. `toolchain-probe/` retains compiler/linker/trace
evidence only. No OpenNT/NTVDMx64-derived binary may enter a normal release
package. See `docs/HISTORICAL-SOURCE-INPUT-GATES.md`.

## Initial Feasibility Result

The requested properties cannot all be achieved by directly rebuilding the
historical implementation:

1. The NTVDMx64 build instructions require source archives not contained in
   the patch repository: a MinNT source package and an old NT source package.
2. Its documented x64 runtime model depends on host process injection, registry
   configuration, system-directory placement, console-host selection, and
   private Windows interfaces. That violates this project's non-invasive rule.
3. Its documented Win16 path depends on a USER32 function removed from recent
   Windows 11 releases. DOS support and Win16 support therefore have different
   feasibility boundaries.

The source study may still establish an isolated legacy-build reproduction in
a disposable test VM, but that outcome is not a non-invasive CLI product and
must never be installed or tested on the host by an automated script.

The 2026-08-06 read-only scan under `D:\home\repos` found no owner-supplied
MinNT or old-source archive. See `docs/BUILD-INPUT-DISCOVERY.md`.

## Work Streams

### A. Historical Build Reproduction (research only)

* Inventory the exact missing source archives, SDK/toolchain versions, patch
  order, and expected outputs.
* Do not acquire or redistribute unverified protected source archives as part
  of this workspace.
* If an owner supplies the required materials and authorizes their use, build
  only inside an isolated disposable VM and record commands, hashes, and
  non-content-bearing outcomes.
* The observed OpenNT artifact/source matrix is in
  `docs/OPENNT-ARTIFACT-MATRIX.md`; use
  `tools/governance/Test-HistoricalBuildInputs.ps1` before any extraction or build.
* The isolated CCPU source-closure and standalone-adapter assessment is in
  `docs/CCPU-STANDALONE-BACKEND-ASSESSMENT.md`. It is archived research
  evidence, not an alternative implementation route or an NTDOS runtime
  compatibility claim.

### B. Non-Invasive CLI Dispatcher

* Implement a new, owned CLI with PE/MZ/NE classification.
* Hand PE32/PE32+ directly to `CreateProcessW`, preserving arguments, working
  directory, inherited handles, exit code, and Ctrl-C semantics.
* Keep DOS/NE execution behind the reconstructed historical SoftPC path. Until
  that path is available, return a clear "engine unavailable" result; do not
  silently install a hook, inject a DLL, mutate the registry, or copy a user
  runtime.
* A future BYOB binding must declare every local runtime file it consumes and
  preserve the original historical function boundaries.
* `docs/CPU-EXECUTION-BACKEND-DECISION.md` is archival comparison evidence;
  it is not an execution-backend gate for this project.
* The source-derived bootstrap and BOP/mode-transition boundary is recorded in
  `docs/OPENNT-BOOTSTRAP-SERVICE-CONTRACT.md`.
* The complete OpenNT VDM/DOS artifact scope and historical build-input plan is
  recorded in `docs/OPENNT-VDM-ARTIFACT-PLAN.md`.

## Next Research Checkpoints

1. Recover the next caller-evidenced SoftPC/CCPU monitor boundary and its
   original data flow.
2. Trace the historical loader-to-NTVDM command contract and identify which
   fields can be provided by an ordinary CLI versus private console/server ABI.
3. Define the owned CLI executable classification and host-handoff contract.
4. Attempt the bounded BYOB smoke only after the historical monitor path
   reaches its documented bootstrap boundary.

## Verification Record

* Historical `ntdos64-run` evidence was configured and built in the archived
  `artifacts/build/legacy/referenced/build-root-pre-isolation/` tree with
  Visual Studio 2022 Build Tools 17.13.5, MSVC 19.43.34809, x64, and the
  bundled CMake.
* Host handoff test:

  ```text
  ntdos64-run.exe C:\Windows\System32\cmd.exe /c "exit 37"
  ```

  The dispatcher returned exit code `37` unchanged.
* The former owned-WHPX experiment is retained only as archived trace evidence.
  It is not part of the active build, runtime, validation route, or recovery
  plan, and it establishes neither NTDOS nor NE execution.
