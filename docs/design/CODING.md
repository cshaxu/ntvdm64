# Source Layout

## Production roots

```text
src/
  mvdm/
  opennt-host/
  mvdm-platform-abi/
  opennt-host/base/win32/winnls/fontsup/system/
  adapter-mvdm-host-in/
  adapter-mvdm-host-out/
  adapter-opennt-host/
  session/
  broker/
  app/
```

`mvdm/` is the one canonical physical selected-OpenNT `base/mvdm` tree: its
host, guest, tool and firmware slices retain their upstream-relative paths and
their manifest-declared build roles. It has no private implementation partner:
strict mirror policy permits a change only in an existing original owner file.
The remaining
`opennt-host/base/win32/winnls/fontsup/system/` carries the exact selected
OpenNT font files, which are not under `base/mvdm`. The extracted printer
carrier is named monitor-adapter material, not a second original root.
One physical tree may generate several libraries; a path move does not create
a link edge. Production roots contain production inputs only; tests and
examples stay under `tests/`; historical source comparison stays in the
explicitly read-only external reference roots under `O:\repos.external`.
`src.old/` is quarantined comparison material and never a source, build, link
or runtime input.

## Machine-profile selection

Every product build manifest defines `CPU_40_STYLE` and selects the original
CCPU40 source contract. `CPU_30_STYLE` is retired: it must not appear in a
project-owned compile definition, generated Ninja graph, link input, runtime
selection, fixture, or acceptance row. An occurrence inside an unmodified
OpenNT mirror is retained only as source identity; an occurrence in a
historical record is evidence, not a selectable configuration.

## Owner placement

- `mvdm` is the canonical physical selected-OpenNT `base/mvdm` mirror. Its
  executable host packages, load-only DOS/V86/Bin86/WOW16 carries, independent
  `vdmutils` tool and immutable `softpc.new/{bios,roms,data}` inputs preserve
  original relative paths. Build manifests, rather than parallel source roots,
  declare their host/guest/tool/firmware role.
- `opennt-host` contains every complete selected original OpenNT host-service
  package outside MVDM, plus separately admitted load-only OpenNT inputs such
  as the exact WinNLS font directory; each retains upstream-relative paths and
  filenames.
  Base VDM is its first accepted slice, not a limit on future admitted owner
  packages. Each package is admitted only with rows in the shared
  file/interface/dependency/build trackers and the external package-boundary
  ledger. The audit must show a direct `mvdm-host` caller, retained original
  service value and a finite outward modern-binding closure; a standalone
  convenience helper, a symbol hit or a recursively required NT4 product-shell
  package never qualifies.
- `mvdm` retains selected shared MVDM build/header carriers and original
  support libraries at their original relative paths (`inc`, `dirs`,
  `makefil0`, `oemuni`, and `suballoc`); no separate support component exists.
- `mvdm/vdmutils` is an independent original tool and never a main-program
  library. `mvdm/softpc.new/{bios,roms,data}` remains immutable firmware input,
  not a host-runtime library or a second machine. Executable
  `mvdm/softpc.new/base/{bios,keymouse}` remains host-selected; `app` selects
  immutable inputs through its admitted composition binding.
- `mvdm-platform-abi` contains exact declarations outside MVDM required by
  those packages; it has no implementation.
- NTVDMx64-derived declarations or missing-provider fallbacks are provenance-
  registered adapter/ABI inputs, never a mirror component. They may not add a
  file below `mvdm` or `opennt-host`; a mirror change is permitted only in an
  existing upstream-mirror file and only when that file owns the logic.
- `mvdm/dos/v86`, `mvdm/bin86` and `mvdm/wow16` are load-only selected guest
  carries; `opennt-host/base/win32/winnls/fontsup/system` is the separately sourced load-only Win16 font
  carry. None implies an unselected external source-universe import.
- `adapter-mvdm-host-in` contains only its declared selector-blind typed
  machine-event boundary. `adapter-mvdm-host-out` contains the named same-shaped historical
  interface families `win32`, `softpc`, `monitor`, `redir`, `wow`, `vdd` and
  `debugger`; a missing historical product interface is assigned to one of
  those families before a mirror source is changed to avoid it.
- `adapter-opennt-host` contains only the same-shaped package-private host
  interface family required by each owning `opennt-host` package. Its source
  subdirectories identify that original owner package. It cannot be used by
  `mvdm-host` directly or become a generic compatibility root.
- `session` contains neutral per-instance lifetime, mappings, resources,
  events and teardown.
- `broker` contains the versioned IPC client/server contract and per-user
  cross-process coordination state.
- `app` contains CLI admission and final wiring.

Historical MVDM build tools such as `tools16`, `bin86`, `convert` and
`dat2obj` belong under `tools/opennt`, not `src/`.

Before authoring or reimplementing a project-owned mechanic, audit existing
current and quarantined same-owner `adapter-*`, `app`, `session`
and related component code as a recovery/reference source. Reuse only the
individually selected, provenance-recorded portion that already satisfies final
ownership, dependency and mirror rules; never bulk-import a component tree or
revive a rejected semantic path.

For the source-function BFS, zero-degree consists of all original definitions
in `mvdm-host` and their transitive resolved call closure while each callee's
physical original definition remains in the selected OpenNT `mvdm` tree and is
already mirrored under a project `mvdm-*` component. Do not sweep every
tool, firmware, or guest definition into zero merely because it
exists locally: it must be reachable from that closure. This classification
does not change final build ownership. Resolve by selected physical definition
identity, never by a bare same-spelled function name.

## Host-width coding model

The current recovery build has one MSVC `/MT` Win32/x86 compilation and
acceptance row, producing `run16.exe`, `basesrv.exe` and `ntvdm.exe`, with
the original CCPU40 executor in the worker.
Native x64 compile/link output is outside the product target and must not
drive a source change. `CPU_30_STYLE` is an NT4 kernel-VDM
V86-monitor contract. It is retired and prohibited from every project-owned
compile, link, runtime, fixture and acceptance input. Cross-component and
broker wire records use fixed-width
integer fields.
Native process-local implementation uses `uintptr_t`, `size_t`, `HANDLE` and
other pointer-sized platform types.

Directly composable original Win32/x86 process-local pointer/HANDLE carriers
retain their 32-bit value. A fixed-width original ABI that cannot carry a
pointer owns a source-shaped narrow table; it must not be generalized into a
session identity service. Guest 16:16 and linear32 addresses use a bounded
lease rather than a durable pointer.
A synchronous historical pointer API may receive a native pointer only through
a checked address/span/access/epoch lease. The pointer is not serialized,
retained by asynchronous work or passed across a component ABI.

Numeric length, offset, flag, time, error, register and guest-address fields
keep their original meaning. Arithmetic is validated in a wider temporary type
before narrowing. Native structures that contain pointers or HANDLEs are
materialized and translated in the owning adapter.

## Session and broker code

Every project-owned stateful API takes or is bound to an explicit session
instance. Thread entry to imported MVDM code binds the current monitor context
and unbinds it on exit. No project-global current machine or generic resource
table is allowed; a source-shaped process-local table is permitted only where
its fixed-width original ABI requires it.

Broker command records contain only versioned copied fields and broker-owned
IDs, never local surrogate IDs, native/guest pointers, C++ objects or callbacks.
Separate authenticated OS-managed resource attachments may convey required
file, pipe, event and process capabilities; ordinary fields never treat a
sender-local handle number as identity or authority. The receiver materializes
and closes its own local references under the original lifetime contract.
Process discovery is cooperative registration with leases, not arbitrary
process enumeration. Console handles use a separate local binding.

## Strict mirror practice

An imported production file is exact upstream, a true subset, or a
same-shaped minimal modification in an existing upstream-relative file. Each
crop or changed expression is marked `DIVERGENCE:` and indexed in the component
README. A proposed new mirror file, material added mechanism, or body that
would cease to be a small owner-local correction belongs in its named adapter
or ABI family; it may not be placed in a private overlay.


## Build layout

Ninja is generated from the source-owner and package-selection manifests.
Disposable objects, libraries, generated files, fixture executables and build
results belong under `build/<task-id>/<run-id>/`; selected formal product
executables may be published at `build/output/`. A real-package run copies only
those selected executables to the `O:\winnt` package root; its observations and all
runtime logs belong in `O:\winnt\logs\`, never in the repository or beside
the package-root executable. Guest objects and libraries are
packaging/loading inputs only and never enter the host link. Formal verification
currently covers accepted x86 CCPU40 compilation plus architecture-neutral token
behavior; `CPU_30_STYLE` is retired and historical-only; x64 compatibility
verification is deferred until the SoftPC/MVDM execution path is connected.
