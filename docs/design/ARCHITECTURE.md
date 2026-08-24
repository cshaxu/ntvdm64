# System Architecture

## Architectural Goal

NTDOS64 is a composition of an adopted guest machine, OpenNT source mirrors,
and a modern contained host. Its design objective is to preserve each layer's
native responsibility rather than recreate one layer inside another. The
product has nine named source components; a source file has exactly one of
these owners. Four are source mirrors directly comparable to their upstream tree
after re-rooting: `bx-core`, `opennt-guest`, `opennt-host`, and `opennt-bop`.
The other five are self-authored composition or adaptation components:
`bx-mantle`, `adapter-bop`, `adapter-softpc`, `adapter-win32`, and `app`.

Each component root is production-only. Immutable upstream comparison copies,
uncompiled experimental mirrors, examples, fixtures and retained overlays are
evidence outside `src/`—under `refs/`, `docs/etc/legacy_code/` or `tests/`—not
an alternate product provider. A retained imported production body stays in
its original owner-family grouping; every necessary source difference is
marked `DIVERGENCE:` and individually registered by its component README.

## Components And Ownership

| Component | Owns | Does not own |
| --- | --- | --- |
| `bx-core` | Adopted Bochs CPU/decode, memory, exceptions, and admitted no-device mechanics | VDM/guest-service interpretation, OpenNT/DOS/WOW semantics, host policy, or compatibility-provider selection |
| Bochs mantle (`src/bx-mantle`) | Minimal native Bochs lifecycle composition: SIM/logging/no-device time state and assembly of admitted core mechanics | VDM, BOP, OpenNT, DOS, host policy, GUI, plugins, or unadmitted PC devices |
| `opennt-guest` | OpenNT DOS and WOW16 guest source and its immutable guest-image inputs | Host service dispatch, host Win32 capability, or machine mechanics |
| `opennt-host` | Re-rooted, independently composable original OpenNT host-capability components | BOP routing, machine mechanics, guest algorithms, or unmarked project-authored helpers |
| `adapter-bop` | Copied-frame, selector-blind transition between a typed Bochs machine event and an OpenNT BOP entry, including typed resume/pending/controlled-stop completion | BOP provider logic, selector-family meaning, DOS/WOW algorithms, or host-capability policy |
| `opennt-bop` | Minimal-change mirrors of the original OpenNT BOP providers and their original interface, parameter and failure contracts | Bochs mechanics, modern Win32 reconstruction, or product entry composition |
| `adapter-softpc` | Source-shaped Bochs-backed implementation of reached historical SoftPC/CCPU interfaces: original spelling, parameters, ABI and observable mechanical semantics | BOP selector/service meaning, DOS/WOW algorithms, or host capability policy |
| `adapter-win32` | Source-shaped modern public-Win32 implementations of unavailable historical Win32 interfaces | OpenNT BOP/service policy or Bochs mechanics |
| `app` | The `ntvdm64` executable entry point, CLI, and explicit composition/loading of the selected components | CPU/device mechanics, BOP providers, OpenNT host algorithms, or compatibility facades |
| Research fixtures | Reproducible evidence for a bounded question | Product behavior or implicit runtime dependencies |

## Composition

```text
app
  -> opennt-guest                         (guest-image input)
  -> bx-mantle -> bx-core
  -> adapter-bop -> opennt-bop
  -> installs adapter-softpc's opaque machine-event callback to adapter-bop
opennt-bop
  -> opennt-host
  -> adapter-win32
  -> adapter-softpc -> bx-mantle -> bx-core
opennt-host -> adapter-win32
opennt-host -> adapter-softpc
```

`adapter-bop` is the sole generic BOP ingress boundary. It receives a copied,
typed, finite machine event and a session-limited call context; invokes the
exposed OpenNT BOP entry; and returns a typed resume, pending, or
controlled-stop outcome. It does not implement a provider or interpret a
selector family. Selector/service meaning, dispatch order, provider choice,
and documented failure behavior remain in `opennt-bop`.

`adapter-softpc` preserves the reached historical SoftPC/CCPU/SAS interface
spelling, parameters, calling convention and observable mechanical result
while backing it with bounded Bochs operations. It receives and returns
versioned, fixed-width values and checked guest-memory ranges. It does not
pass C++ objects, host pointers, CRT-owned memory, implicit handle ownership,
or cross-architecture callbacks across that boundary. `app` installs its
opaque machine-event callback to `adapter-bop`; this runtime callback is not a
static dependency from the machine components back to OpenNT.

### Guest-Pointer Mapping

`adapter-softpc` owns the VDM-facing session-scoped guest-pointer mapping manager.
It is the
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
or an explicit unavailable result instead.  The manager may share session
ownership and lifecycle bookkeeping with host-handle and child-event tables,
but these remain distinct resource kinds: a guest address is never a Windows
`HANDLE`, and a native pointer is never a guest-visible token.

The first runtime process is MSVC x64 throughout: `app`, adapters, OpenNT host
composition, mantle and adopted Bochs core share one static CRT. The
invocation shell is an outer product boundary, not a semantic architecture
layer. The guest CPU architecture is an
emulation property, not a host-process property; CPU5/Pentium-MMX guest code
therefore remains valid inside the x64 process. No MinGW object or a separate
CRT may enter this in-process composition.

## Boundary Invariants

- Machine mechanics stay in the Bochs core. `adapter-softpc` may request bounded mechanical
  operations through typed contracts but does not reproduce CPU, memory,
  firmware, interrupt, or device algorithms.
- The mantle is Bochs-internal assembly only. It reuses native Bochs code and
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
- Generic #UD/BOP ingress, copied CPU-frame ABI, session composition, mapping
  infrastructure and product-specific routing are self-authored concerns.
  They do not belong in `opennt-bop` merely because their result invokes a BOP.

## Dependency Direction

Dependencies point inward through declared contracts:

```text
app -> opennt-guest
app -> bx-mantle -> bx-core
app -> adapter-bop -> opennt-bop -> opennt-host
opennt-bop -> adapter-win32
opennt-bop -> adapter-softpc -> bx-mantle -> bx-core
opennt-host -> adapter-win32                  (declared Win32 facade only)
opennt-host -> adapter-softpc                 (declared SoftPC/CCPU facade only)
```

No component may reverse these directions by importing another component's
private execution state. In particular, the Bochs core and mantle remain
reusable as a guest machine; `adapter-softpc` remains a mechanical boundary; and
the original OpenNT BOP and host ownership remains visible rather than being
absorbed by either adapter.

## Adapter Admission

The five self-authored components are the intended normal boundary set. Do not
introduce a generic `compat`, `common`, or catch-all adapter merely to avoid an
ownership decision.

An additional named adapter is admitted only after an audit establishes that
the upstream interface must be retained, that its dependency cannot belong to
`adapter-win32`, `adapter-softpc`, `adapter-bop`, or `app`, and that its
original evidence, boundary, divergence, and dependency direction have been
recorded. A possible future VDD, redirector, or WOW adapter is not implied by
this architecture and must not be created preemptively.
