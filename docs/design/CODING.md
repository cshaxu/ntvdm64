# Source Layout

```text
src/
  cli/                    owned non-invasive CLI and BYOB admission boundary
  bx-core/                adopted Bochs CPU, memory and exception mechanics
  bx-mantle/              project-owned native Bochs lifecycle composition
  bx-vdm/                 project-authored typed Bochs/NTVDM boundary only
  opennt/                 imported historical guest/service source
  opennt/overlay/         caller-proven OpenNT compatibility recovery
  opennt/local/compat/    reached modern-host seams for OpenNT owners
refs/                     read-only comparison, historical and archival inputs
  bochs/                  pinned Bochs 2.6 imported source tree
  ms-dos-6/               recovered MS-DOS 6 source input
  opennt/                 external OpenNT comparison tree
  opennt-45/              OpenNT 4.5 comparison slice
  ntvdmx64/               external NTVDMx64 comparison tree
  ntvdmx64-derived/       fixed NTVDMx64-derived comparison slice
  archive/                retired probes, adapters and reconstruction fixtures
tests/
  runner/                 CLI integration tests (historical test-path name)
  bx-vdm/                 bridge, memory, stop, and negative boundary tests
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
notices, source identity and local patch series. `src/bx-mantle/` is the
project-owned, Bochs-internal lifecycle assembly layer; it may use native Bochs
structures but contains no VDM or OpenNT meaning. Put adapters in `src/bx-vdm/`
and OpenNT seams only under the matching `src/opennt/` ownership path. This is
an internal research repository: distribution/license review is deferred until
a release is considered.

`src/bx-core` is the manifest-verified local Bochs 2.6 adoption from
`O:\repos.external\bochs-2.6-compat\bochs-2.6`; its pinned full imported tree
is under `refs/bochs/`. The unadmitted Bochs 3.0 snapshot was removed and is
not a runtime input. Any source exception below the imported tree is registered in
`docs/etc/research/adapter-external-intrusion-exceptions.md`.

`src/cli` is the outer command-line invocation shell, not an architecture
component or a convenience wrapper inside the adapter.  Its retained sources
are transitional input-model evidence until their ownership moves under the
four-layer architecture.  It does not own guest payload loading, service
dispatch, BOP semantics, or machine configuration; those concerns remain
respectively in the OpenNT layer, adapter, and Bochs layers.

All current modern runtime sources below `src/cli/`, `src/bx-vdm/`,
`src/bx-mantle/`, and the admitted `src/bx-core/` closure build with MSVC x64
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
