# System Architecture

## Ownership

| Area | Responsibility | Hard boundary |
| --- | --- | --- |
| CLI (`src/cli`) | CLI invocation, BYOB identity admission, contained host policy, diagnostics, and engine launch | Does not interpret guest instructions, read/write guest RAM, or invent DOS behavior. |
| Bochs 2.6 backend | x86 execution, RAM/ROM, interrupt and PC device mechanics, CPU stop events | Does not implement or dispatch OpenNT DOS/WOW services. |
| Bochs adapter/shim | Sole integration layer: owns the versioned typed ABI, native-container lifecycle calls, validated guest-memory exchange, and conversion between a Bochs mechanical event and the adapter host-service plane | Does not recreate Bochs `SIM`, CPU, memory, exception, or device behavior. |
| Adapter NTVDM host-service plane | Coherent routing to independently composable original OpenNT BOP/DEM host providers, with source-derived rehosting only when the historical composition cannot be linked without CCPU/SAS | Does not implement DOS kernel/filesystem algorithms, WOW behavior, CPU, firmware, or PC devices. |
| OpenNT guest layer | NTDOS, DOS utilities, command programs and WOW payloads | Does not provide an x86 interpreter, firmware replacement, PC devices, or host service composition. |
| Host compatibility seam | Contained modern host facilities used by the host-service plane | Does not define CPU/device behavior or reach into Bochs internals. |
| Research fixture | Narrow evidence/probe | Default-disabled; never silently promoted to the product runtime. |

## Control Flow

```text
CLI / BYOB profile
  -> Bochs 2.6 CPU + PC machine
  -> typed adapter event
  -> adapter NTVDM host-service plane
  -> original OpenNT owner where linkable, otherwise source-derived host contract
  -> typed result
  -> Bochs CPU state / guest memory
```

The bridge is the sole Bochs/OpenNT integration point. It uses versioned, fixed-width values and validated guest-memory ranges; it passes no C++ object, host pointer, CRT ownership, or raw cross-architecture function pointer. A bridge result may update only fields expressly authorized by its record. `src/bx-ntvdm-adapter` is the only repository area allowed to include both the adapter ABI and Bochs-facing lifecycle glue. `src/cli` may validate and transmit immutable launch/profile metadata only; it must not include Bochs internals or adapter guest-memory APIs.

For `C4 C4 selector` BOPs, Bochs reports only the generic copied exception
event and instruction window. The adapter owns the source-derived BOP
catalogue: it may identify a selector and, for a catalogued service family,
its copied fourth-byte service ID; it observes every recognized invocation.
An entry cannot resume unless its exact historical owner, ABI and failure
behavior are separately admitted. Unimplemented entries are logged and pass
through to Bochs unchanged. This makes BOP identity visible without putting
DOS/SoftPC/CCPU dispatch semantics into Bochs or treating a catalogue as an
implementation. See `etc/research/opennt-bop-definition-inventory-001.md`.

## Native Bochs Preservation

`src/bochs` is an adopted runtime, not an implementation target. Native Bochs
initialization, `SIM`, CPU decoding/exceptions, memory, and device framework
remain internal to it. A Bochs source patch is exceptional and avoided by
default. It is allowed only when an adapter/build wrapper cannot solve the
identified problem, is smaller than the alternative, has owner approval plus a
source/patch/evidence record, and preserves or explicitly documents any
semantic effect. The adapter must be preferred whenever the connection can be
expressed at its typed boundary.

The active owner-approved exceptions are recorded exhaustively in
`etc/research/adapter-external-intrusion-exceptions.md`. Their authority is
exceptional and individually scoped; in particular, the default-off generic
and resumable `#UD` hand-off carries only fixed-width mechanical data, knows
no OpenNT service semantics, and falls through to the unchanged Bochs exception
path unless explicitly handled. A new Bochs or OpenNT source intrusion still
requires a prior register entry with evidence, negative behavior, and a review
or removal condition.

## Minimum Backend Profile

The backend is deny-by-default for optional capability. The initial runtime
profile excludes GUI interaction, debugger use, external plugin activation,
USB, networking, sound, attached storage, PCI/i440FX where original Bochs
configuration permits it, and every other optional device feature. Adding one
requires a reached first-profile OpenNT caller, a named Bochs owner, an
adapter impact assessment, an explicit admission record, and a focused
negative test. Upstream availability or a desire for compatibility is not
evidence.

The intact Bochs 2.6 lifecycle has a non-configurable **PC reset floor**:
CMOS, DMA, PIC, PIT, floppy controller, core VGA, keyboard, compiled I/O APIC,
and one empty ATA controller/hard-drive plugin required by the original
post-init drive-status call. They are retained as one native Bochs-owned unit,
not exposed as adapter or guest capability and not removed one by one. No
media, host path, PCI/i440FX, ACPI, PCI IDE or VBE is part of that floor. A
CPU/RAM/ROM-only product would require a replacement device composition and is
outside this adoption path; see
`etc/research/t95-s6-pc-reset-floor-profile-observation-002.md`.

## OpenNT Fidelity

Guest OpenNT/DOS behavior stays in guest code. Historical host-service
contracts retain their original calling conventions, layouts, dispatch order
and failure outcomes. The adapter's preferred role is parameter/result ABI
adaptation and routing to original OpenNT host code, not BOP rewriting. Each
endpoint selects exactly one provider disposition in this order:

1. Independently composable original OpenNT host implementation;
2. The original implementation with only declared, contained CLI capability
   backends replacing intrusive NT host facilities;
3. Minimal source-derived rehost of the recorded original contract when the
   original composition cannot be linked without CCPU/SAS or obsolete ABI
   dependencies; or
4. Original evidenced unavailable/failure behavior, or explicit deferral.

The third option is exceptional: it must name the failed composition edge and
must not broaden into a free rewrite. No option puts host-service semantics
into Bochs or claims the complete historical composition has been restored.
See `design/ADAPTER-HOST-SERVICE-PLANE.md`.

## Historical Evidence Role

The historical SoftPC/CCPU/BIOS source and existing trace fixtures remain behavior and ownership evidence. They are not an execution backend and must not be linked into the Bochs runtime merely to reproduce an old composition. `ntvdm64` is a downstream architecture consumer/comparison subject, not a build or runtime dependency of this repository.
