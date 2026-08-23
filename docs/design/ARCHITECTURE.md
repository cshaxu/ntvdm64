# System Architecture

## Architectural Goal

NTDOS64 is a layered composition of an adopted guest machine, an OpenNT guest
environment, and a modern contained host. Its design objective is to preserve
each layer's native responsibility rather than recreate one layer inside
another.

## Components And Ownership

| Component | Owns | Does not own |
| --- | --- | --- |
| OpenNT layer (`src/opennt`) | Original NTVDM host-service source, DOS/WOW guest source, and their source-derived contracts | CPU execution, firmware, PC-device emulation, or Bochs lifecycle |
| VDM adapter (`src/bx-vdm`) | Typed bx↔VDM contracts, BOP ingress/egress, compatibility adaptation, and explicit composition of OpenNT host/guest contracts | CPU execution, firmware, PIC, memory model, device model, DOS/WOW kernel algorithms, or ambient-host policy |
| Bochs mantle (`src/bx-mantle`) | Minimal native Bochs lifecycle composition: SIM/logging/no-device time state and assembly of admitted core mechanics | VDM, BOP, OpenNT, DOS, host policy, GUI, plugins, or unadmitted PC devices |
| Bochs core (`src/bx-core`) | Adopted Bochs CPU/decode, memory, exceptions, and admitted no-device mechanics | VDM/guest-service interpretation, OpenNT/DOS/WOW semantics, host policy, or compatibility-provider selection |
| Host-capability seam | Modern user-mode Win32, device, path, disk, port and registry facilities selected by CLI policy and invoked by the VDM adapter | Guest protocol meaning, CPU/device behavior, Bochs internals, host installation, or host system-file modification |
| Historical machine-handler islands | Individually admitted original machine-facing behavior under adapter selection | A replacement SoftPC/CCPU backend or general host-service plane |
| Research fixtures | Reproducible evidence for a bounded question | Product behavior or implicit runtime dependencies |

## Composition

```text
command-line invocation shell
  -> OpenNT host and guest layer
  -> bx-vdm adapter
      -> bx-mantle -> bx-core
      -> selected contained host capability
```

The VDM adapter is the composition boundary between machine events and host-side
meaning. It receives and returns versioned, fixed-width values and checked
guest-memory ranges. It does not pass C++ objects, host pointers, CRT-owned
memory, implicit handle ownership, or cross-architecture callbacks across that
boundary.

### Guest-Pointer Mapping

`bx-vdm` owns one session-scoped guest-pointer mapping manager.  It is the
single compatibility boundary for historical OpenNT pointer APIs such as
`GetVDMAddr`, `Sim32GetVDMPointer`, `Sim32FlushVDMPointer`, and
`Sim32FreeVDMPointer`.  For a synchronous, admitted OpenNT call it may acquire
a bounded, epoch-scoped direct mapping of stable Bochs guest RAM and return the
native process pointer required by the historical call shape.  That pointer is
valid only inside the active `bx-vdm` call: it is never serialized into guest
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

The first runtime process is MSVC x64 throughout: its command-line invocation
shell, VDM adapter, mantle and adopted Bochs core share one static CRT. The
invocation shell is an outer product boundary, not a semantic architecture
layer. The guest CPU architecture is an
emulation property, not a host-process property; CPU5/Pentium-MMX guest code
therefore remains valid inside the x64 process. No MinGW object or a separate
CRT may enter this in-process composition.

## Boundary Invariants

- Machine mechanics stay in the Bochs core. The VDM adapter may request bounded mechanical
  operations through typed contracts but does not reproduce CPU, memory,
  firmware, interrupt, or device algorithms.
- The mantle is Bochs-internal assembly only. It reuses native Bochs code and
  data structures, extracting only product-shell paths that prevent independent
  minimal operation. It has no VDM or guest meaning.
- Guest and host-service meaning stays outside Bochs. The VDM adapter owns the
  interpretation and routing needed to compose OpenNT-derived host behavior.
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

## Dependency Direction

Dependencies point inward through declared contracts:

```text
src/opennt -> bx-vdm -> bx-mantle -> bx-core
bx-vdm -> contained host-capability seam
bx-vdm -> src/opennt host/guest contract or historical machine-handler island
```

No component may reverse these directions by importing another component's
private execution state. In particular, the Bochs core and mantle remain
reusable as a guest machine, and the VDM adapter remains reusable as the
explicit NTVDM composition boundary for future hosts.
