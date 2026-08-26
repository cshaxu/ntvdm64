# Source Layout

> **Current rebootstrap layout.** This section supersedes the transition-era
> root list below. The production layout is exactly:

```text
src/bochs-core/
src/opennt-mvdm-host/
src/opennt-platform-abi/
src/opennt-guest-dos/
src/opennt-guest-wow16/
src/adapter-bochs/
src/adapter-bop/
src/adapter-softpc/
src/adapter-win32/
src/adapter-vdm-monitor/
src/session/
src/app/
```

The five mirror roots preserve selected upstream-relative paths after
re-rooting and each has a README exception register. `opennt-mvdm-host` is the
canonical merged non-guest/non-tool MVDM mirror. `opennt-platform-abi` is
declarations only. The guest roots retain selected source, resources, build
metadata, intermediates and original products but their objects/libraries
never enter the host link; `app` selects immutable guest bytes via a manifest
and loads them through `adapter-bochs`.

`adapter-bochs` is the sole production caller of `bochs-core`.
`adapter-softpc` reaches it only through `adapter-bochs`; `adapter-bop` is
selector-blind; `adapter-vdm-monitor` owns the same-shaped monitor interface
family. `session` stays neutral and `app` is the sole composition owner.
Historical tools live under `tools/opennt`; `src.old/` is comparison evidence,
not a build input. Mirror overlays are private to their matching mirror.

## Superseded transition-era layout record

The fourteen production components are classified before any file move or new
source is admitted:

- **Original code, with component README exception registers:** `bochs-core`,
  `opennt-abi`, `opennt-guest-dos`, `opennt-guest-wow16`, `opennt-bop`,
  `opennt-host`, `opennt-softpc`, `opennt-utils`.
- **Mechanical adaptation:** `adapter-bochs`, `adapter-bop`, `adapter-softpc`,
  `adapter-win32`.
- **Project composition:** `app`, `session`.

This classification is an ownership boundary, not an include shortcut:
`adapter-bop` transports opaque machine events to the declared OpenNT BOP
route, while `bochs-core` remains BOP/OpenNT-blind. `session` remains neutral and
dependency-free even though it is project-authored composition code.

```text
src/
  bochs-core/             adopted Bochs CPU, memory and exception mechanics
  adapter-bochs/          project-owned native Bochs lifecycle composition
  opennt-abi/             exact shared OpenNT MVDM/VDM declarations
  opennt-guest-dos/       imported OpenNT DOS/V86 and DOSX guest inputs
  opennt-guest-wow16/     imported OpenNT WOW16/Win16 guest inputs
  opennt-host/            imported OpenNT host-capability components
  opennt-bop/             minimal-change OpenNT BOP source mirrors
  opennt-softpc/          imported OpenNT SoftPC firmware/ROM/machine-contract inputs
  opennt-utils/           imported reusable OpenNT utility packages
  adapter-bop/            copied-frame BOP ingress and typed completion transfer
  adapter-softpc/         source-shaped Bochs-backed SoftPC/CCPU compatibility
  adapter-win32/          source-shaped modern Win32 compatibility facades
  session/                dependency-free per-VDM lifecycle/resource/event foundation
  app/                    ntvdm64 CLI and final component composition
src.old/                  quarantined pre-rebootstrap implementation evidence;
                          never a source, build, link or runtime input
tests/
  app/                    app-owned CLI fixture inputs
  adapter-bop/            copied-frame ingress boundary fixtures
  adapter-softpc/         same-shaped SoftPC/CCPU facade fixtures
  session/                neutral lifecycle/resource/event fixtures
  component-integration/  multi-component composition fixtures
  adapter-bochs/          Bochs-machine assembly fixtures
  legacy/opennt/          retained non-product historical fixture/archive inputs
tools/                    tracked tools, arranged by declared responsibility
  build/                  build/publish entry points
  governance/             inventory and governance verification
  import/                 admitted source/input acquisition helpers
  probe/                  diagnostic tools (their output never lives here)
  historical/             retained historical tool closures
  opennt/                 admitted original OpenNT build-tool closures
build/<task>/<run>/       ignored, disposable compiler/linker and debug output
docs/etc/                 Git-tracked evidence, operational records and research
artifacts/                only explicitly requested reports and formal releases
  build/<task>-<version>/ formal published executable plus manifest
  reports/<task>/         explicitly requested human-readable reports
```

`src/bochs-core/` is adopted third-party Bochs material. Preserve upstream layout,
notices and source identity. Its `README.md` is the complete local-intrusion
register: every modification records upstream identity, necessity, exception
identifier and focused verification. `src/adapter-bochs/` is project-owned,
Bochs-internal lifecycle assembly; it may use native Bochs structures but has
no OpenNT, DOS, VDM, WOW or Win32 meaning. It is a deliberately cropped Bochs
product assembly layer, not an adopted-source exception surface.

Only `app` may directly include, link, or call the declared selector-blind,
fixed-width mechanical facade of `adapter-bochs`; it supplies a typed opaque
endpoint to `adapter-softpc` during composition. `adapter-softpc` may not
include a `bochs-core` header or access a Bochs type, object or global. Only
`adapter-bochs` may directly include, link, or call `bochs-core`. A
component-specific `*-overlay` is private to the matching original mirror:
only that mirror may include, link, or call it. Overlays are not fixture,
adapter, session, or app inputs and expose no public component surface.

`src/opennt-guest-dos/` contains the canonical single-tree union of original
DOS/V86 and DOSX guest source and guest-image inputs selected from OpenNT and
OpenNT-4.5; `src/opennt-guest-wow16/` contains the corresponding canonical
WOW16/Win16 union. They preserve all selected original source, resources,
build descriptions, intermediates and product binaries. The guest components
are not host libraries: no guest object or archive enters the MSVC x64 host
compile/link graph. Original prebuilt images are the default packaging input;
source-built images are fallback/reproducibility inputs only where an original
artifact is unavailable. `app` consumes only a guest-image manifest and loads
the selected immutable bytes through `adapter-bochs`. Each component
`README.md` records every guest-source divergence and its exception identifier;
the expected normal count is zero.

`src/opennt-host/` contains independently composable original OpenNT
host-capability components. Its production files preserve their original
relative paths and filenames after re-rooting. A modern-host repair remains
local to its corresponding original unit, carries a `DIVERGENCE:` comment, and
is recorded in the component exception register; an invented helper cannot
pose as an upstream mirror file.

An OpenNT package may be imported intact before its translation units are
admitted to the formal build. All OpenNT mirror components use one canonical
target tree formed from the pinned OpenNT and OpenNT-4.5 baselines. Identical
paths collapse to one file with dual provenance; a one-sided path is included;
a content conflict is decided once at complete-package scope using its build,
resource and artifact lineage. Parallel edition directories and silent
file-by-file precedence are forbidden. The source import records original
identity and the union disposition; the build manifest separately lists
composed inputs. This permits low-diff whole-package mirrors without silently
reshaping unused source to satisfy a current local build.

`src/opennt-abi/` contains only exact shared MVDM/VDM declarations and has no
project-authored implementation. `src/opennt-softpc/` contains admitted original OpenNT SoftPC firmware, ROM
and machine-contract inputs. It is neither a second machine runtime nor a
dependency from `adapter-bochs`: `app` may select opaque admitted bytes for the
mechanical loader, while `adapter-bochs` remains OpenNT-blind. `src/opennt-utils/`
contains only selected original utility packages with a named original caller;
it is not a generic common-code root. Both components preserve upstream
relative paths after re-rooting and maintain a README exception register.

`src/opennt-bop/` contains OpenNT BOP mirror files. Preserve their original
function names, parameters, data layout, control flow and observable failure
semantics. A mirror may redirect an unavailable historical Win32, CCPU or
SoftPC reference only to its declared `opennt-host`, `adapter-win32`, or
`adapter-softpc` counterpart. Its `README.md` is the complete exception
register for an imported BOP mirror; every edited source expression carries a
`DIVERGENCE:` comment naming that replacement and why it is necessary.
Component roots never retain an uncompiled original, overlay, sample or
fixture merely for comparison: such material belongs in
`docs/etc/legacy_code/` (or `tests/` for test code). Live imported production
files are arranged by their original OpenNT owner family rather than by generic
staging labels such as `original`, `mirror` or `overlay`.
`src/adapter-bop/` is project-authored and owns only copied typed frame ingress
and typed resume/pending/controlled-stop transfer to the exposed OpenNT BOP
entry. It cannot implement a BOP provider, interpret a selector family, or
own host capability policy. Generic ingress, session composition and
project-specific routing must not be placed in `src/opennt-bop/`.
`src/session/` is project-authored, has no product-component dependency, and
owns only neutral per-VDM lifecycle, resource/token registry, capability
registration, completion/event state and teardown. It does not know BOP,
OpenNT, DOS, WOW, VDD, Redirector, Win32 or Bochs vocabulary. `src/app/`
owns composition and creates the session instance; no other component imports
`app` to acquire one.
`src/adapter-win32/` owns source-shaped implementations of unavailable Win32
interfaces using public modern Win32 APIs. `src/adapter-softpc/` owns only
same-shaped SoftPC/CCPU-to-Bochs mechanical adaptation; it never owns BOP or
DOS meaning. `src/app/` owns the CLI, loading and final composition, not the
semantics of the components it assembles. This is an internal research
repository: distribution/license review is deferred until a release is
considered.

`src/bochs-core` is the manifest-verified local Bochs 2.6 adoption from
`O:\repos.external\bochs-2.6-compat\bochs-2.6`; its pinned full imported tree
is under `refs/bochs/`. The unadmitted Bochs 3.0 snapshot was removed and is
not a runtime input. Any source exception below the imported tree is registered in
`docs/etc/research/adapter-external-intrusion-exceptions.md`.

No `src/cli/`, `src/bx-vdm/`, or generic `src/opennt/` source root exists in
the product layout. Historical OpenNT reference material remains under
`refs/opennt`; retained non-product local fixtures and adapters belong under
`tests/legacy/opennt/`. These roots are not product, formal-build or runtime
inputs.

The target layout has fourteen source components. The formal graph may select
only a subset of imported OpenNT packages. `opennt-guest-dos` and
`opennt-guest-wow16` are loaded data inputs and never host-side libraries;
`opennt-softpc` and `opennt-abi` do not require empty host libraries merely to
satisfy the diagram. Any composed modern runtime
source builds with MSVC x64 and the static `/MT` CRT. Generated build artifacts
record the compiler, target architecture and CRT. MinGW artifacts are retained
evidence only and cannot be linked into the x64 runtime process.

`build/<task>/<run>/` is the sole repository-local location for temporary
build trees, objects, libraries, generated project files and debug logs. It is
ignored and may be removed and recreated. Evidence, inventories, operational
records and research conclusions are Git-tracked below `docs/etc/`.
`artifacts/` is never ignored but is reserved for reports the owner explicitly
requested and for an explicitly approved executable release action to
`artifacts/build/<task>-<version>/`, accompanied by its manifest.
