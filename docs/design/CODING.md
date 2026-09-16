# Source Layout

## Production roots

```text
src/
  mvdm/
  opennt-host/
  opennt-abi/host-compat/
  product-abi/
  product-package/
  run16/
  basesrv/
  ntvdm/
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

## Executable-owned transition

T418 has moved project-owned runtime code to `src/run16/`, `src/basesrv/` and
`src/ntvdm/`, each producing exactly its named executable.  Retained
`app`/adapter directory READMEs are archival move markers, never production
source roots or destinations. `session` is worker-local implementation inside
`ntvdm`; broker service transport is inside `basesrv`.

Do not create a shared Win32 helper root. Place a Win32 binding in the one
executable that owns its process-local resource. Original code remains in its
mirror even when several EXEs link it; BaseSrv protocol/client code remains
`basesrv`-owned even when both clients link it. Only a named, same-shaped
historical host ABI whose finite public-Win32/NTDLL binding is recorded may
live in `opennt-abi/host-compat`. The stateless shared product surface is
limited to `product-abi`/`product-package`; `basesrv` owns the service IDL and
copied, versioned broker protocol. There is no `common` or `win32api` root.

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
  `mvdm/softpc.new/base/{bios,keymouse}` remains host-selected; `ntvdm` selects
  immutable inputs through its admitted composition binding.
- `opennt-abi/host-compat` contains only the named, same-shaped historical
  host ABI declarations and finite public-Win32/NTDLL bindings that are used
  by more than one executable. It owns no broker wire policy, session state or
  guest state.
- NTVDMx64-derived declarations or missing-provider fallbacks are provenance-
  registered adapter/ABI inputs, never a mirror component. They may not add a
  file below `mvdm` or `opennt-host`; a mirror change is permitted only in an
  existing upstream-mirror file and only when that file owns the logic.
- `mvdm/dos/v86`, `mvdm/bin86` and `mvdm/wow16` are load-only selected guest
  carries; `opennt-host/base/win32/winnls/fontsup/system` is the separately sourced load-only Win16 font
  carry. None implies an unselected external source-universe import.
- `run16` owns discovery, CreateProcess-style admission and parent waiting.
  `basesrv` owns authenticated endpoint/transport and the original-record
  assembly. `ntvdm` owns its worker-local session, machine bindings, Console,
  redirector, VDD, WOW and debugger bindings. Their code is not made shared by
  naming it Win32 compatibility.

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
