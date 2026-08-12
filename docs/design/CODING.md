# Source Layout

```text
src/
  cli/                    owned non-invasive CLI and BYOB admission boundary
  bochs/                  pinned, third-party Bochs 2.6 source import
  bx-ntvdm-adapter/       project-authored typed Bochs/NTVDM boundary only
  opennt/                 imported historical guest/service source
  opennt/overlay/         caller-proven OpenNT compatibility recovery
  opennt/local/compat/    reached modern-host seams for OpenNT owners
  reconstruction/         archived/trace-only historical experiments
tests/
  runner/                 CLI integration tests (historical test-path name)
  bx-ntvdm-adapter/       bridge, memory, stop, and negative boundary tests
tools/                    isolated inventory, audit, and probe utilities
artifacts/                generated and retained evidence only
```

`src/bochs/` is third-party material. Preserve its upstream layout, notices,
source identity and local patch series. Project-authored code must not be placed
inside that tree unless a patch record names the exact upstream file, rationale
and focused verification. Put adapters in `src/bx-ntvdm-adapter/`; put OpenNT
seams only under the matching `src/opennt/` ownership path. This is an internal
research repository: distribution/license review is deferred until a release is
considered.

`src/bochs` is the manifest-verified local Bochs 2.6 import from
`O:\repos.external\bochs-2.6-compat\bochs-2.6`. The prior Bochs 3.0 snapshot
is retained only under `src/archived/` and is not a runtime input. Any source
exception below the imported tree is registered in
`docs/etc/research/adapter-external-intrusion-exceptions.md`.

`src/cli` is a separate component, not a convenience wrapper inside the
adapter. It owns command-line parsing, immutable BYOB profile admission,
engine-child creation, cancellation, and user-facing diagnostics. It does not
own guest payload loading, service dispatch, BOP semantics, or machine
configuration; those concerns remain respectively in the adapter and Bochs.
