# Source Layout

```text
src/
  bx-core/                adopted Bochs CPU, memory and exception mechanics
  bx-mantle/              project-owned native Bochs lifecycle composition
  opennt-guest/           imported OpenNT DOS/WOW source and guest-image inputs
  opennt-host/            imported OpenNT host-capability components
  opennt-bop/             minimal-change OpenNT BOP source mirrors
  adapter-bop/            copied-frame BOP ingress and typed completion transfer
  adapter-softpc/         source-shaped Bochs-backed SoftPC/CCPU compatibility
  adapter-win32/          source-shaped modern Win32 compatibility facades
  app/                    ntvdm64 CLI and final component composition
refs/                     read-only comparison, historical and archival inputs
  bochs/                  pinned Bochs 2.6 imported source tree
  ms-dos-6/               recovered MS-DOS 6 source input
  opennt/                 external OpenNT comparison tree
  opennt-45/              OpenNT 4.5 comparison slice
  ntvdmx64/               external NTVDMx64 comparison tree
  ntvdmx64-derived/       fixed NTVDMx64-derived comparison slice
  archive/                retired probes, adapters and reconstruction fixtures
tests/
  app/                    app-owned CLI fixture inputs
  adapter-softpc/         same-shaped SoftPC/CCPU facade fixtures
  component-integration/  multi-component composition fixtures
  bx-mantle/              Bochs-machine assembly fixtures
  legacy/opennt/          retained non-product historical fixture/archive inputs
  adapter-softpc/         bridge, memory, stop, and negative boundary tests
tools/                    tracked tools, arranged by declared responsibility
  build/                  build/publish entry points
  governance/             inventory and governance verification
  import/                 admitted source/input acquisition helpers
  probe/                  diagnostic tools (their output never lives here)
  historical/             retained historical tool closures
build/<task>/<run>/       ignored, disposable compiler/linker and debug output
docs/etc/                 Git-tracked evidence, operational records and research
artifacts/                only explicitly requested reports and formal releases
  build/<task>-<version>/ formal published executable plus manifest
  reports/<task>/         explicitly requested human-readable reports
```

`src/bx-core/` is adopted third-party Bochs material. Preserve upstream layout,
notices and source identity. Its `README.md` is the complete local-intrusion
register: every modification records upstream identity, necessity, exception
identifier and focused verification. `src/bx-mantle/` is project-owned,
Bochs-internal lifecycle assembly; it may use native Bochs structures but has
no OpenNT, DOS, VDM, WOW or Win32 meaning. It is a deliberately cropped Bochs
product assembly layer, not an adopted-source exception surface.

`src/opennt-guest/` contains the original DOS/WOW guest sources and guest-image
inputs. Original prebuilt OpenNT images are the default packaging input;
source-built images are fallback/reproducibility inputs only where an original
artifact is unavailable. Its `README.md` records every guest-source divergence
and its exception identifier; the expected normal count is zero.

`src/opennt-host/` contains independently composable original OpenNT
host-capability components. Its production files preserve their original
relative paths and filenames after re-rooting. A modern-host repair remains
local to its corresponding original unit, carries a `DIVERGENCE:` comment, and
is recorded in the component exception register; an invented helper cannot
pose as an upstream mirror file.

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
`src/adapter-win32/` owns source-shaped implementations of unavailable Win32
interfaces using public modern Win32 APIs. `src/adapter-softpc/` owns only
same-shaped SoftPC/CCPU-to-Bochs mechanical adaptation; it never owns BOP or
DOS meaning. `src/app/` owns the CLI, loading and final composition, not the
semantics of the components it assembles. This is an internal research
repository: distribution/license review is deferred until a release is
considered.

`src/bx-core` is the manifest-verified local Bochs 2.6 adoption from
`O:\repos.external\bochs-2.6-compat\bochs-2.6`; its pinned full imported tree
is under `refs/bochs/`. The unadmitted Bochs 3.0 snapshot was removed and is
not a runtime input. Any source exception below the imported tree is registered in
`docs/etc/research/adapter-external-intrusion-exceptions.md`.

No `src/cli/`, `src/bx-vdm/`, or generic `src/opennt/` source root exists in
the product layout. Historical OpenNT reference material remains under
`refs/opennt`; retained non-product local fixtures and adapters belong under
`tests/legacy/opennt/`. These roots are not product, formal-build or runtime
inputs.

All current modern runtime sources below `src/app/`, `src/adapter-softpc/`,
`src/adapter-win32/`, `src/opennt-host/`, `src/opennt-bop/`,
`src/adapter-bop/`, `src/bx-mantle/`, and the admitted `src/bx-core/` closure build with MSVC x64
and the static `/MT` CRT. Generated build artifacts record the compiler,
target architecture and CRT. MinGW artifacts are retained evidence only and
cannot be linked into the x64 runtime process.

`build/<task>/<run>/` is the sole repository-local location for temporary
build trees, objects, libraries, generated project files and debug logs. It is
ignored and may be removed and recreated. Evidence, inventories, operational
records and research conclusions are Git-tracked below `docs/etc/`.
`artifacts/` is never ignored but is reserved for reports the owner explicitly
requested and for an explicitly approved executable release action to
`artifacts/build/<task>-<version>/`, accompanied by its manifest.
