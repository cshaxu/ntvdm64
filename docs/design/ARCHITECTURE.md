# System Architecture

> **Current authority (rebootstrap).** The twelve-component model below
> supersedes every following transition-era component list, diagram and package
> map. Those retained notes describe the prior layout only and must not be used
> for placement or dependency decisions.

## Current component model

`ntvdm.exe` is a non-invasive single-session CLI. It may use public Win32 APIs
and ordinary host resources, but never requires a rebuilt kernel, private NT
subsystem, replacement system files or installation-time registry mutation.
DOS/WOW16 code executes as loaded guest images, never as host-linked code.

The production roots are exactly:

- original mirrors: `bochs-core`, `opennt-mvdm-host`,
  `opennt-platform-abi`, `opennt-guest-dos`, and `opennt-guest-wow16`;
- mechanical adapters: `adapter-bochs`, `adapter-bop`, `adapter-softpc`,
  `adapter-win32`, and `adapter-vdm-monitor`;
- project components: `session` and `app`.

`opennt-mvdm-host` is one canonical merged host-side MVDM mirror, not separate
`opennt-bop`, `opennt-host`, `opennt-softpc` or utility roots. It owns selected
non-guest/non-tool MVDM packages: DEM, COMMAND, XMS, DPMI32, VDMREDIR, WOW32,
VDD/debugger, `softpc.new`, SIM/monitor, `suballoc`, `vdmutils` and OEM
support. It may produce many libraries. `opennt-platform-abi` owns exact
external OpenNT declarations required by MVDM, never a project host provider.

`adapter-bochs` alone calls `bochs-core`; it owns Bochs-only machine setup and
bounded execution. `adapter-bop` is selector-blind frame transport.
`adapter-softpc` preserves reached SoftPC/CCPU/SAS call shapes through bounded
`adapter-bochs` operations. `adapter-win32` preserves unavailable historical
Win32/NTDLL call shapes with public Win32 APIs. `adapter-vdm-monitor` owns the
complete same-shaped user-mode `NtVdmControl`/`VDM_TIB`/V86-event/interrupt and
fault-handler family using bounded one-session state plus an app-installed
opaque machine endpoint; unsupported operations fail deterministically and it
does not recreate NT kernel, CSRSS or a product shell.

`session` is neutral lifecycle/resource/completion/teardown state with no
service vocabulary. `app` is the sole composition and session-instance owner;
it selects guest-image manifests and loads immutable guest bytes through
`adapter-bochs`. Guest host interaction is only BOP, interrupts, ports and
guest-memory contracts. Guest object/library files never enter the host link.

```text
app -> session
app -> adapter-bochs -> bochs-core
app -> adapter-bop -> opennt-mvdm-host
app -> opennt-platform-abi                 (declaration selection)
app -> opennt-guest-dos / opennt-guest-wow16  (data/load only)
opennt-mvdm-host -> opennt-platform-abi
opennt-mvdm-host -> adapter-win32
opennt-mvdm-host -> adapter-softpc -> adapter-bochs
opennt-mvdm-host -> adapter-vdm-monitor
opennt-mvdm-host -> session                (neutral contract only)
adapter-bop -> adapter-softpc              (typed mechanics only)
```

The chosen source is a single package-scope union of
`OpenNT/base/mvdm` and `OpenNT-4.5/nt/private/mvdm`: each target-relative path
has one selection, identical files have dual provenance, and differing files
are selected with source/build/resource/artifact evidence at package scope.
No parallel edition root or arbitrary file-level hybrid is allowed. Guest
mirrors retain selected source, resources, build metadata, intermediates and
original products; historical build tools belong in `tools/opennt`.

Every mirror file is exact upstream, a registered true subset, or a registered
same-shaped minimal modification. A material new semantic body belongs in the
matching private `*-overlay`; only that mirror may call its overlay.

## Superseded transition-era architecture record

`ntvdm64` is a contained user-mode NTVDM reconstruction: an adopted guest
machine, complete OpenNT source-package mirrors, and a modern host.  It does
not require a modified Windows kernel, replacement system files, or
installation-time registry mutation. The product has fifteen named source
components; a source file has exactly one owner.  They form three strata:

- **Original code:** `bochs-core`, `opennt-abi`, `opennt-guest-dos`,
  `opennt-guest-wow16`, `opennt-bop`, `opennt-host`, `opennt-softpc`, and
  `opennt-utils` are source mirrors directly comparable to their pinned
  upstream package after re-rooting.  Every necessary modification is a
  registered exception in that component's `README.md` and carries a local
  `DIVERGENCE:` marker.
- **Mechanical adaptation:** `adapter-bochs`, `adapter-bop`, `adapter-softpc`,
  `adapter-win32`, and `adapter-vdm-monitor` bridge only named machine, host or
  historical VDM-monitor interface contracts; they do not become alternate
  OpenNT providers or a replacement NT kernel.
- **Project composition:** `app` performs CLI entry and final assembly;
  `session` is the dependency-free neutral lifecycle foundation shared by the
  components. It is foundational, not an upward product layer.

Each component root is production-only.  An imported original package may be
present before it enters the formal build: presence records source identity,
while the build manifest separately declares the selected translation units,
resources and artifacts.  A retained imported body stays in its original
owner-family grouping; every necessary source difference is marked
`DIVERGENCE:` and individually registered by its component README.

### Components And Ownership (superseded)

| Component | Owns | Does not own |
| --- | --- | --- |
| `bochs-core` | Adopted Bochs CPU/decode, memory, exceptions, and admitted no-device mechanics | VDM/guest-service interpretation, OpenNT/DOS/WOW semantics, host policy, or compatibility-provider selection |
| `adapter-bochs` | Minimal native Bochs lifecycle composition: SIM/logging/no-device time state and assembly of admitted core mechanics | VDM, BOP, OpenNT, DOS, host policy, GUI, plugins, or unadmitted PC devices |
| `opennt-abi` | Exact shared MVDM/VDM declarations: `base/mvdm/inc`, admitted `base/win32/inc`, and public/internal VDM contract headers | Project helpers, compatibility algorithms, BOP providers, or a generic common layer |
| `opennt-guest-dos` | Canonical OpenNT/OpenNT-4.5 DOS/V86 and DOSX/DPMI guest source, resources, intermediates and immutable guest-image products | Any host compilation/link input, host service dispatch, host Win32 capability, or machine mechanics |
| `opennt-guest-wow16` | Canonical OpenNT/OpenNT-4.5 WOW16/Win16 guest source, resources, intermediates and immutable guest-image products | Any host compilation/link input, WOW32 host thunks, BOP/host dispatch, or machine mechanics |
| `opennt-host` | Original user-mode host capability packages, WOW32, and admitted OpenNT Win32 VDM client/server source contracts | BOP routing, Bochs mechanics, guest algorithms, or unmarked project-authored helpers |
| `opennt-softpc` | Original OpenNT SoftPC firmware, ROM and machine-contract source/input packages after provenance admission | Bochs CPU/device operation, BOP routing, or an independently invented machine layer |
| `opennt-utils` | Selected reusable original OpenNT utility packages that are neither a guest image, BOP provider, host product package nor machine contract | Generic project helpers, BOP routing, host-product policy, or guest execution semantics |
| `adapter-bop` | Copied-frame, selector-blind transition between a typed Bochs machine event and an OpenNT BOP entry, including typed resume/pending/controlled-stop completion | BOP provider logic, selector-family meaning, DOS/WOW algorithms, or host-capability policy |
| `opennt-bop` | Complete minimal-change original BOP entries, dispatchers and provider packages: DEM, COMMAND, XMS, host DPMI, VDMREDIR and admitted BOP-facing SoftPC source | Bochs mechanics, modern Win32 reconstruction, or product entry composition |
| `adapter-softpc` | Source-shaped Bochs-backed implementation of reached historical SoftPC/CCPU interfaces: original spelling, parameters, ABI and observable mechanical semantics | BOP selector/service meaning, DOS/WOW algorithms, or host capability policy |
| `adapter-win32` | Source-shaped modern public-Win32 implementations of unavailable historical Win32 interfaces | OpenNT BOP/service policy or Bochs mechanics |
| `adapter-vdm-monitor` | Complete same-shaped user-mode facade for the historical `NtVdmControl`, `VDM_TIB`, V86 event and interrupt/fault-handler interface family, backed by bounded single-session state and app-installed machine capabilities | NT kernel/CSRSS emulation, BOP/provider meaning, SoftPC mechanics, or an unbounded NTVDM product shell |
| `session` | Dependency-free per-VDM lifecycle, neutral token/resource registries, capability registration, completion/event state and teardown | Component assembly, BOP/service/provider meaning, DOS/WOW/VDD/Redirector policy, Win32 facades, or Bochs mechanics |
| `app` | The `ntvdm64` executable entry point, CLI, and explicit composition/loading of the selected components; it creates, owns and drives one `session` | CPU/device mechanics, BOP providers, OpenNT host algorithms, compatibility facades, or durable session semantics |
| Research fixtures | Reproducible evidence for a bounded question | Product behavior or implicit runtime dependencies |

### Composition (superseded)

```text
app
  -> session                              (creates, owns and drives one instance)
  -> opennt-guest-dos / opennt-guest-wow16 (manifest-selected bytes loaded into the machine; never linked)
  -> opennt-softpc                        (selects admitted opaque firmware/ROM inputs)
  -> adapter-bochs -> bochs-core
  -> adapter-bop -> opennt-bop
  -> registers adapter-bop's generic ingress callback with adapter-bochs
opennt-bop
  -> opennt-abi
  -> opennt-host
  -> opennt-utils                         (only for an admitted original utility contract)
  -> adapter-win32
  -> adapter-softpc -> app-installed opaque machine endpoint
  -> adapter-vdm-monitor -> session-owned monitor state / app-installed machine endpoint
  -> session                              (declared neutral lifecycle/resource contract)
opennt-host -> opennt-abi
opennt-host -> adapter-win32
opennt-host -> adapter-softpc
opennt-host -> adapter-vdm-monitor
opennt-host -> opennt-utils               (only for an admitted original utility contract)
opennt-host -> session                    (declared neutral lifecycle/resource contract)
adapter-bop -> session                    (event/completion context only)
adapter-win32 -> session                  (registered session endpoint only)
adapter-vdm-monitor -> session            (bounded VDM state/event ownership only)
```

`adapter-bop` is the sole generic BOP ingress boundary. `app` registers its
generic ingress callback with the public `adapter-bochs` mechanical surface.
When a machine event occurs, `adapter-bochs` invokes that opaque, copied,
fixed-width callback and receives only a typed resume, pending, or
controlled-stop result. It does not include, link, name, decode, or otherwise
recognize `adapter-bop`, BOP, OpenNT, DOS, or VDM semantics. `adapter-bop`
forwards the finite event to `opennt-bop`; selector/service meaning, dispatch
order, provider choice, and documented failure behavior remain there.
`bochs-core` never calls or recognizes `opennt-bop`, BOP, DOS, or OpenNT
terminology.

The guest components have a data/load edge only. Their C, assembly, object,
archive and executable files never satisfy a host symbol. `app` resolves a
guest-image manifest, validates the selected immutable binary and asks
`adapter-bochs` to load its bytes into the declared guest address space.
Subsequent communication is machine-visible guest behavior—BOP, interrupt,
port and memory contracts—not a host-language call graph.

`adapter-softpc` preserves the reached historical SoftPC/CCPU/SAS interface
spelling, parameters, calling convention and observable mechanical result
while backing it with the declared selector-blind, fixed-width mechanical
facade of `adapter-bochs`. It receives and returns versioned, fixed-width
values and checked guest-memory ranges. It does not include a `bochs-core`
header, or pass C++ objects, host pointers, CRT-owned memory, implicit handle
ownership, or cross-architecture callbacks across that boundary. The runtime
event callback is registered by `app`, not a static dependency from machine
components back to OpenNT.

#### Guest-Pointer Mapping (superseded)

`session` owns the lifecycle and identity of separately scoped mapping-manager
instances. `adapter-softpc` owns the VDM-facing implementation of the
historical SoftPC/CCPU pointer-call facade that uses the applicable instance. It is the
single compatibility boundary for historical OpenNT pointer APIs such as
`GetVDMAddr`, `Sim32GetVDMPointer`, `Sim32FlushVDMPointer`, and
`Sim32FreeVDMPointer`.  For a synchronous, admitted OpenNT call it may acquire
a bounded, epoch-scoped direct mapping of stable Bochs guest RAM and return the
native process pointer required by the historical call shape.  That pointer is
valid only inside the active `adapter-softpc` call: it is never serialized into guest
memory, returned through the bx↔machine ABI, retained by an asynchronous
worker, or exposed as a Bochs object.

The same manager owns the corresponding mapping lease: guest real-mode
`16:16` and protected/linear `32-bit` addresses, access direction, maximum
span, mapping epoch, and teardown.  A call which can retain a pointer, execute
asynchronously, or cannot prove its access span uses a copied/bounce mapping
or an explicit unavailable result instead. The distinct managers may share
the same `session` lifecycle bookkeeping with host-handle and child-event tables,
but these remain distinct resource kinds: a guest address is never a Windows
`HANDLE`, and a native pointer is never a guest-visible token.

The first runtime process is MSVC x64 throughout: `app`, adapters, OpenNT host
composition, machine and adopted Bochs core share one static CRT. The
invocation shell is an outer product boundary, not a semantic architecture
layer. The guest CPU architecture is an
emulation property, not a host-process property; CPU5/Pentium-MMX guest code
therefore remains valid inside the x64 process. No MinGW object or a separate
CRT may enter this in-process composition.

### Boundary Invariants (superseded)

#### OpenNT Package Mirrors (superseded)

The OpenNT MVDM source family is imported by complete upstream package, never
by an individual trace hit. Each owner is a single canonical tree selected
from the pinned OpenNT and OpenNT-4.5 MVDM baselines. Identical target paths
collapse with dual provenance; one-sided paths extend the superset; a content
conflict is resolved once at complete-package scope using its source, build,
resource and artifact lineage. Parallel edition roots and arbitrary
file-by-file precedence are forbidden. Each owner preserves the selected
upstream-relative subtree below its component root; `opennt-abi` holds shared
declarations once rather than allowing copied compatibility headers to grow in
every consumer. The formal build selects host source files from these mirrors
by an audited manifest. A package not yet selected for linking remains an
owned source mirror, not a competing provider or a reason to modify it. Guest
packages are selected for loading rather than host linking.

The package map is:

- `opennt-guest-dos`: `base/mvdm/dos/v86` and `base/mvdm/dpmi` guest packages;
- `opennt-guest-wow16`: `base/mvdm/wow16` guest packages;
- `opennt-bop`: `base/mvdm/dos/{dem,command}`, `xms.486`, `dpmi32`,
  `vdmredir`, and admitted BOP-facing `softpc.new` units;
- `opennt-softpc`: the remaining `softpc.new`, `sim16`, `sim32`, `v86` and
  OEM machine-contract packages;
- `opennt-host`: user-mode MVDM host packages including `wow32`, VDD/debugger
  packages when admitted, plus selected `base/win32` VDM client/server source;
- `opennt-utils`: `suballoc`, `vdmutils` and named reusable utility packages;
- `opennt-abi`: `base/mvdm/inc`, admitted `base/win32/inc`, and required
  public/internal VDM declarations.

Historical build tools such as `tools16`, `bin86`, `convert` and `dat2obj`
belong under `tools/opennt`, not a runtime component.

The kernel VDM/V86 monitor, NT process-manager support and CSR/BaseSrv broker
are outside this user-mode source/build closure.  Their source can supply ABI
and behavior evidence where required, but a current one-session implementation
may only recover the reached public or same-shaped contract through
`adapter-win32`, `adapter-softpc`, `adapter-bochs` or `session`.  It may never
claim to recreate the NT4 kernel or CSRSS product shell.

The source-proven `NtVdmControl`, `VDM_TIB`, interrupt/fault-handler and V86
event calls form one complete `adapter-vdm-monitor` boundary from the start.
The adapter publishes the original interface family and owns deterministic
single-session state/failure dispositions for every operation. It reaches
machine mechanics only through an opaque endpoint installed by `app`; it does
not include Bochs types or implement BOP, DOS, VDD, WOW, kernel or CSRSS
semantics. Capabilities may be enabled in dependency order, but missing
capability never changes the component boundary or invites a family-private
shim.

- Machine mechanics stay in `bochs-core`. `adapter-softpc` may request bounded
  mechanical operations only through the declared selector-blind,
  fixed-width `adapter-bochs` facade; it does not include `bochs-core` or
  reproduce CPU, memory, firmware, interrupt, or device algorithms.
- `adapter-bochs` is Bochs-internal assembly only. It reuses native Bochs code and
  data structures, extracting only product-shell paths that prevent independent
  minimal operation. It has no VDM or guest meaning.
- Guest and host-service meaning stays outside Bochs. `opennt-bop` owns the
  OpenNT-derived interpretation and routing; `opennt-host` owns independently
  composable host capability; the adapters only translate bounded contracts.
- The guest owns DOS and WOW behavior. Neither the invocation shell nor the
  adapter becomes a replacement DOS kernel or filesystem implementation.
- Modern host capability is explicit and policy-governed. It may deeply
  integrate with the actual host through existing user-mode Win32 APIs,
  including filesystems, devices, paths, disks, ports and registry state when
  the caller's permissions and selected profile allow it. It never requires
  rebuilding, replacing or modifying Windows system files, installing kernel
  components, or making installation-style registry changes in order to
  install, start, or sustain NTVDM itself. When a guest DOS workload explicitly
  requests such a host operation, the selected capability may faithfully
  forward it only under the normal host permission boundary and an explicit
  CLI policy grant. That workload side effect is never an implicit NTVDM
  startup prerequisite. The CLI remains unpack-and-run: all state change is
  an ordinary, auditable process/API operation with the normal OS permission
  boundary.
- The product has no project-authored host-mutation profile. It recovers the
  applicable original OpenNT host-service semantics, with only the smallest
  necessary composition shim. Retained legacy profile, overlay, and virtual
  code is retained only in version history; it is not selectable or extensible
  product behavior.
- Historical code is normative source and ownership evidence. It is reused
  only through a bounded, independently auditable composition boundary; it is
  not a dependency on the unavailable historical product shell.
- A source-mirror component preserves upstream-relative path, filename, data
  layout, interface shape and control-flow structure wherever the original
  unit exists. Every necessary change is local, marked `DIVERGENCE:`, and
  recorded in that component's `README.md`; an invented helper must not
  silently pose as an imported mirror unit.
- Generic #UD/BOP ingress, copied CPU-frame ABI, neutral session lifecycle and
  resource infrastructure, and product-specific routing are self-authored
  concerns.
  They do not belong in `opennt-bop` merely because their result invokes a BOP.

### Dependency Direction (superseded)

Dependencies point inward through declared contracts:

```text
app -> opennt-guest-dos / opennt-guest-wow16
app -> opennt-softpc                      (opaque source/input selection only)
app -> adapter-bochs -> bochs-core
app -> session
app -> adapter-bop -> opennt-bop -> opennt-host
opennt-bop -> opennt-abi
opennt-bop -> opennt-utils                (declared original utility contract only)
opennt-bop -> adapter-win32
opennt-bop -> adapter-softpc -> session
adapter-softpc -> app-installed opaque machine endpoint
opennt-bop -> session                       (neutral declared contract)
opennt-host -> opennt-abi                      (declared original ABI only)
opennt-host -> adapter-win32                  (declared Win32 facade only)
opennt-host -> adapter-softpc                 (declared SoftPC/CCPU facade only)
opennt-host -> session                        (neutral declared contract)
opennt-host -> opennt-utils                  (declared original utility contract only)
adapter-bop -> session                        (neutral event/completion contract)
adapter-win32 -> session                      (neutral endpoint contract)
```

`session` has no dependency on any other product component and never calls a
component-specific provider. Components may use only its declared neutral
contract; they may not import `app` to obtain session state. `app` composition
is the sole instance owner and wiring point. No component may reverse these
directions by importing another component's private execution state. In
particular, `bochs-core` and `adapter-bochs` remain
reusable as a guest machine; `adapter-softpc` remains a mechanical boundary; and
the original OpenNT BOP and host ownership remains visible rather than being
absorbed by either adapter.

### Bochs And Overlay Privacy (superseded)

`app` is the only direct lifecycle caller of `adapter-bochs`.  It supplies a
typed opaque mechanical endpoint to `adapter-softpc` during composition;
`adapter-softpc` may not import an `adapter-bochs` implementation detail or a
`bochs-core` header, type, object or global. `adapter-bochs` is the only
production component that directly calls `bochs-core`.

Each `*-overlay` is private implementation detail of its identically rooted
native mirror. Only `bochs-core` may call `bochs-core-overlay`; only
`opennt-host` may call `opennt-host-overlay`; the same rule applies to every
future native-mirror/overlay pair. An overlay has no external public ABI and
may not be called, included, or linked by `app`, adapters, session, fixtures,
or another mirror.

### Adapter Admission (superseded)

The eight original-code components, four mechanical-adaptation components and
two project-composition components are the intended normal boundary set. Do
not introduce a generic `compat`, `common`, or catch-all adapter merely to
avoid an ownership decision.

An additional named adapter is admitted only after an audit establishes that
the upstream interface must be retained, that its dependency cannot belong to
`adapter-win32`, `adapter-softpc`, `adapter-bop`, `session`, or `app`, and that its
original evidence, boundary, divergence, and dependency direction have been
recorded. Do not create generic `adapter-common`, `adapter-host` or `compat`
roots. A VDD/debugger, Redirector, or WOW owner package first remains in its
original `opennt-host`/`opennt-bop` owner and uses the existing adapters plus
`session` where sufficient; a specialized adapter is not implied by this
architecture and must not be created preemptively.
