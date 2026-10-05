# Source Layout

## Own-image root mechanics — T427 S2

`common/system_root.[ch]` owns only checked actual-EXE directory derivation,
bounded relative joins and loss-checked ANSI conversion. Its separately
selected `common-root.lib` is usable by all native EXEs without depending on
worker-base or EXE-private implementations. A consumer with no product-file
lookup does not acquire an artificial lookup merely to use the library.
Product root is not a global environment override or RPC identity check.

Guest directory facades use the explicit NtvdmGetWindowsDirectoryA/W and
NtvdmGetSystemDirectoryA/W names. Host Windows directory APIs retain their
native meanings; no global macro redirects them. Original default DOS media
and configuration bind to root/system32, while SYSTEM.INI remains at root.

Common owns `guest_environment.[ch]`, the bounded ANSI guest projection used
by run16's Win16 task record and NTVDM's WOW kernel boot PDB. It does not
modify the Unicode host worker environment or its saved native environment.
The original GetWowKernelCmdLine owns transformation/copy/free ordering;
its registered adapter projects only the completed CRT-owned guest block.
NTVDM's firmware
adapter owns the exact internal DLL-name binding behind the registered
SafeLoadLibrary hook; arbitrary VDD loading and its original FPU save/restore
remain unchanged. NTSRV retains startup/admission/rollback policy while using
common root mechanics for its selected sibling paths. See the
[S2 record](../etc/evidence/m0-t427-s2-own-image-root-bindings.md).

## Delivered T424 common library and service separation

S7 delivers common/two-protocol organization; S8 delivers the separately
admitted service-provenance audit and NTSRV-private physical split. Their
indexed ledgers retain actual source classification and production tests.

The owner admits src/common as the successor to src/interface after
S6 naming delivery. The delivered organization has moved copied declarations
and IDL into common/protocol, bounded packet codecs into common/codec and
neutral transfer/snapshot mechanisms into common/transport and common/console.
Neutral I/O-client instance/API declarations and implementation now belong to
common/console/client; worker-only connection adaptation and launcher-private
client declarations retain their owners.
The [S7 ledger](../etc/evidence/m0-t424-s7-common-service-separation.md) records delivery;
the transitional interface/native_request_protocol.h and native control pipe
have been removed. In S7, keep protocol declarations/version identity/service IDL in a
declaration-only submodule; separately select audited project-added shared
transport, codec, authenticated-client and Console-snapshot implementations.
This bounded approval supersedes the generic-common prohibition below only
for these cross-component mechanisms. It creates no executable/helper or
generic compatibility framework. Common cannot depend on EXE-private code or
worker-base; worker-base remains worker-only and may use neutral common modules.
Resource ownership, authentication policy, service records/retirement and
frontend rendering remain with their existing owners.

S8 provenance-classifies and splits project-added NTSRV base_service.c
implementation into bounded service-private modules. Original OpenNT/MVDM
source stays in its upstream-relative mirror; preserve original algorithms,
ordering and minimal registered hooks, with pinned-source diff accounting.
Do not extract original execution/completion into common/worker-base.
See the [S7 checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s7-common-library-and-service-source-separation).

T423 S12's owner-approved common text ABI extension adds an optional per-cell style
byte to the original glyph/attribute pair. Both workers use the same interface
definition and frontend decoder; DOS keeps pairs. This supersedes the exact
pair-only wording below, not the ban on native-specific frontend renderers.

## Production roots

```text
src/
  mvdm/
  opennt-host/
  opennt-abi/host-compat/
  common/{protocol,codec,transport,console}/
  worker-base/
  run16-exe/
  ntsrv-exe/
  ntvdm-exe/
  ntcon-exe/
  ntvwm-exe/
  ntmon-exe/
  wow32-dll/
  vdmredir-dll/
  nthook32-dll/
```

`mvdm/` is the one canonical physical selected-OpenNT `base/mvdm` tree: its
host, guest, tool and firmware slices retain their upstream-relative paths and
their manifest-declared build roles. It has no private implementation partner:
strict mirror policy permits source only in its original OpenNT owner file,
including audited original files newly imported at their upstream paths.
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

Owner-admitted T431 S2 nthook32-dll is a specialist x86 /MT native launch hook.
Its installer/context static slice is selected by NTVWM and the DLL; the
context-only consumer is selected by run16. Pinned MIT Detours sources remain
under this component with notices/provenance. GUI/CUI propagation is separate
from character frontend authority. No helper or x64 island is admitted.

## Executable-owned runtime

T424 S4 delivers the following ownership migration of project-added code,
without changing original mirror logic: frontend/worker CreateProcess and
authenticated launch orchestration move into ntsrv-exe; run16 keeps CLI,
broker discovery and submission/direct-result/Console-handoff RPC clients.
NTSRV coordinates takeover/return acknowledgements; NTCON performs actual
Console operations. The run16/NTCON direct bootstrap channel is removed.
Broker typed copied contracts reside in common/protocol. NTVWM owns native target
CreateProcess/wait/result reporting; NTCON owns Console/Window/I/O service;
worker-base remains worker-only shared client/mechanism code. S5 removes
displaced ownership paths; S7 selects shared mechanisms under the bounded
common approval. No duplicate startup owner is retained.
No generic spawn/remote-handle-duplication service, additional common root, helper,
mirror change or NTCON-owned worker policy is admitted.

`run16-exe/bootstrap_client.c` and `native_request_client.c` own the finite
`frontend-client.lib`. Packet validation is selected from common/codec;
`native_launch.c` is the separately selected resource/CreateProcess primitive
used for broker bootstrap and by NTSRV/NTVWM, not local run16 GUI execution.
NTCON never links target creation. Local startup/client declarations remain
with their implementation owners; copied wire declarations live in common/protocol.
No private NTCON/NTVWM implementation root is a shared startup source owner.

Cross-component wire declarations and service IDL have one owner: common/protocol.
Control RPC and direct worker I/O pipes remain separate protocols; no obsolete
native/initial-frontend control pipe remains. Endpoint implementations and
process-private types stay in their executable components. Generated RPC
outputs remain below build/; actual wire changes synchronize app/IDL versions.

The owner-admitted worker-base static library holds project-added mechanisms
shared by NTVDM and NTVWM with matching full contracts: lifecycle, worker
clients, ordered transport, validation/cancellation, frame/input codecs and
handoff acknowledgments. Audit source provenance, not only file placement.
Original mirror execution, scheduling, completion, blocking/resume and cleanup
remain in place; no reverse-call extraction or new scheduler is permitted.
Broker discovery/submission belongs to run16; worker/frontend creation and
management to NTSRV; presentation
to NTCON; monitoring to NTMON. These consumers retain their common kind-aware
handling internally. Backend state operations remain explicit in each worker.
BaseSrv client implementation remains NTSRV-owned; IDL belongs to common/protocol.

Latest owner directive selects an ordinary hidden Console owned/attached by
ntvwm-exe, replacing the ConPTY/VT plan below. No private helper is permitted.
Console state, input and membership operations stay in NTVWM; NTSRV only
coordinates authenticated lifecycle and NTCON only presents copied frames.

The admitted NTVWM replacement supersedes the historical S9/S11 owner
description below. src/ntvwm-exe owns its ordinary hidden Console, native Console
state, input binding, member observation and text-frame production, and produces
ntvwm.exe. NTCON keeps only visible Console/Window, display and input/frame
routing; run16 keeps launcher duties. NTSRV implements authenticated
backend registration/client transport and NTMON consumes that management view.
No original DOS/WOW record or generic compatibility root is introduced.

NTVWM text frames use the existing NTVDM console_video.h layout exactly,
including font banks, glyph/attribute pairs, palette and cursor. NTVWM never
publishes graphics frames. Its bitmap glyph mapping must agree with NTVDM;
NTCON has one backend-neutral text-frame renderer, not a native VT/font engine.
T424 S12 delivers unified frontend logical_surface and consistent publication
with exact current-state handoff. Font/palette/style remain worker metadata;
the frontend owns storage and projection, not native Unicode/VT interpretation.

`src/ntcon-exe/ -> ntcon.exe` owns visible Console, Window/display, logical
storage and the I/O service. The former frontend-owned ConPTY/helper prototypes
belong to indexed T423 history, not the current source graph. NTVWM owns its
hidden execution Console. Run16 links only the finite frontend client, never
the renderer/input pump. No helper, ConPTY backend, duplicate renderer or
second runtime owner is retained. CURRENT and evidence govern publication;
source layout alone is not acceptance.

T418 has moved the original three-program runtime to `src/run16-exe/`,
`src/ntsrv-exe/` and `src/ntvdm-exe/`; T419 adds the product-owned native Console
manager, now located at `src/ntmon-exe/` and producing `ntmon.exe`. Retained
`app`/adapter directory READMEs are archival move markers, never production
source roots or destinations. `session` is worker-local implementation inside
`ntvdm`; broker service transport is inside `ntsrv`.

Do not create a shared Win32 helper root. Place a Win32 binding in the one
executable that owns its process-local resource. Original code remains in its
mirror even when several EXEs link it; BaseSrv protocol/client code remains
`ntsrv`-owned even when both clients link it. Only a named, same-shaped
historical host ABI whose finite public-Win32/NTDLL binding is recorded may
live in `opennt-abi/host-compat`. The stateless shared product surface is
limited to common's specifically admitted protocol/codec/transport/Console
mechanisms; worker package/media configuration lives in
`ntvdm-exe/package_layout.[ch]`. Common/protocol owns service IDL and copied
versioned records; NTSRV owns service policy. `interface` and `product-abi`
are retired roots, not production destinations. No generic `win32api` root.

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
  assembly. `ntvdm` owns its worker-local session, machine bindings, copied Console endpoint,
  redirector, VDD, WOW and debugger bindings. `monitor` owns only its native
  Console presentation and client selection state; task records and task
  termination remain BaseSrv-owned. Their code is not made shared by naming it
  Win32 compatibility.

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
acceptance row, producing `run16.exe`, `ntsrv.exe` and `ntvdm.exe`, with
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
README. An actual original OpenNT file may be newly imported at its original
relative path after provenance and package-boundary admission, as byte-exact
source, a registered true subset or a minimal same-shaped adaptation. A
project-invented file or material new mechanism belongs in its named adapter
or ABI family; it may not be placed in a mirror or private overlay.

Every retained mirror diff, overlay, or non-mirror autonomous seam has a
review record naming the original owner, source/ABI shape, required resource
lifetime and failure behavior, why the original body cannot directly compose,
and its removed-versus-retained line accounting. A build warning fix,
temporary diagnostic, or convenient helper is not a sufficient reason to
retain behavior. If an original package can now own the logic, remove the
duplicate implementation rather than keeping parallel paths.

## Capability-test and external-boundary practice

Each selected original capability is tested from its real guest caller through
its provider boundary, with normal operation, representative failure, and
task/handle/worker cleanup. Successful compilation, a loaded DLL, a symbol
map, a host-only call, or the generic COMMAND/MEM/EDIT regression is supporting
evidence only; none alone proves the capability.

When a required modern service, protocol, device, or peer is absent, first
test every non-invasive source-shaped binding that the current host can offer
and retain the original unavailable result. If a protocol-accurate mock,
controlled peer, fake device endpoint, or fault-injection harness can be
written without changing product behavior, it is mandatory test-only work.
It must exercise the original guest caller, original provider, I/O or request
boundary, failure, and cleanup. It belongs below `tests/` and its emitted
artifacts follow the normal `build/`/`O:\winnt\tests` rules; it must never be
linked, copied, or selected as a product provider.

Mock results are labelled as mock/unit evidence and state both the contract
they prove and the real-host property they cannot prove. They maximize future
retest coverage but never replace a real device/service end-to-end result.
Where that result remains impossible, add a concrete row to
`docs/states/TODO.md` naming the original caller/provider, attempted binding,
missing prerequisite, retained diff/seam, evidence record, and future retest
condition. Do not reclassify a reachable capability as a profile exclusion
solely because this machine lacks its external medium.


## Build layout

The NTSRV-private service units share only `service_internal.h`, the same
explicit service instance and finite internal helper declarations. Do not
include a service `.c` file in another translation unit, export private state
as a protocol, or copy the implementation into tests. Private fixture hooks
link the production archive. Preserve original DOS/WOW lock, callback, receipt
and cleanup ordering when reorganizing project adaptation code.

Ninja is generated from the source-owner and package-selection manifests.
Disposable objects, libraries, generated files, fixture executables and build
results belong under `build/<task-id>/<run-id>/`; selected formal product
executables may be published at `build/output/`. A real-package run copies only
original guest media/configuration and the coherent eight-file runtime set
(run16, ntsrv, ntvdm, ntvwm, ntcon, ntmon and two runtime DLLs) to the
`O:\winnt` package root. Disposable test harnesses, probes, manifests and
evidence belong below the admitted `build/` run. Production diagnostic logs
may use `O:\winnt\logs\` or the owner-approved `logs2`, not package-root files.
`O:\winnt\builds\` is reserved only for deliberately retained versioned
runtime-package history, never fixtures or disposable output.
Guest objects and libraries are
packaging/loading inputs only and never enter the host link. Formal verification
currently covers accepted x86 CCPU40 compilation plus architecture-neutral token
behavior; `CPU_30_STYLE` is retired and historical-only; x64 compatibility
verification is not an admitted product profile.
