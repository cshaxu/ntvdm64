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
  reconstruction/         archived/trace-only historical experiments
tests/
  runner/                 CLI integration tests (historical test-path name)
  bx-vdm/                 bridge, memory, stop, and negative boundary tests
tools/                    isolated inventory, audit, and probe utilities
artifacts/                generated and retained evidence only
```

`src/bx-core/` is adopted third-party Bochs material. Preserve upstream layout,
notices, source identity and local patch series. `src/bx-mantle/` is the
project-owned, Bochs-internal lifecycle assembly layer; it may use native Bochs
structures but contains no VDM or OpenNT meaning. Put adapters in `src/bx-vdm/`
and OpenNT seams only under the matching `src/opennt/` ownership path. This is
an internal research repository: distribution/license review is deferred until
a release is considered.

`src/bx-core` is the manifest-verified local Bochs 2.6 adoption from
`O:\repos.external\bochs-2.6-compat\bochs-2.6`. The prior Bochs 3.0 snapshot
is retained only under `src/archived/` and is not a runtime input. Any source
exception below the imported tree is registered in
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
