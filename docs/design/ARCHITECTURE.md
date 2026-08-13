# System Architecture

## Ownership

| Area | Responsibility | Hard boundary |
| --- | --- | --- |
| CLI (`src/cli`) | CLI invocation, BYOB identity admission, contained host policy, diagnostics, and engine launch | Does not interpret guest instructions, read/write guest RAM, or invent DOS behavior. |
| Bochs 2.6 backend | x86 execution, RAM/ROM, interrupt and PC device mechanics, CPU stop events, and the minimum native embedding lifecycle | Does not recognize BOPs or implement/dispatch OpenNT, DOS, WOW, DEM, COMMAND, SoftPC, or host-service semantics. |
| Bochs mechanical bridge | Default-off, fixed-width `#UD` event/state copy; checked ordinary-RAM read/write; typed result/resume/controlled-stop; and only the native embedding required to make those mechanics callable | Does not classify a selector/service, select a provider, inspect BOP meaning, or name an OpenNT/SoftPC contract. |
| Adapter NTVDM composition plane (`src/bx-ntvdm-adapter`) | Owns typed-bridge consumption, BOP ingress/catalogue, host-service routing, provider disposition, source-derived rehosts, CLI-capability adaptation, failure semantics, and machine-composition selection | Does not recreate Bochs CPU, exception, memory, firmware, PIC, or device behavior, or implement the DOS kernel/filesystem. |
| Adapter machine-composition plane | Selects individually admitted historical machine-handler islands, supplies an opaque typed mechanics request, and consumes a typed result | Does not move machine mechanics into the adapter, install `BIOS[]`, revive a SoftPC/CCPU backend, or become a general historical dispatcher. |
| OpenNT guest layer | NTDOS, DOS utilities, command programs and WOW payloads | Does not provide an x86 interpreter, firmware replacement, PC devices, or host service composition. |
| Host compatibility seam | Contained modern host facilities used by the host-service plane | Does not define CPU/device behavior or reach into Bochs internals. |
| Research fixture | Narrow evidence/probe | Default-disabled; never silently promoted to the product runtime. |

## Control Flow

```text
CLI / BYOB profile
  -> Bochs 2.6 CPU + PC machine
  -> opaque typed mechanical event
  -> adapter host-service plane, or adapter machine-composition plane
  -> original OpenNT host owner, source-derived host contract, or isolated machine-handler island
  -> typed result / bounded machine disposition
  -> Bochs CPU state / guest memory
```

The adapter bridge is the sole Bochs/**host-service** integration point. The
separate machine-composition plane is an adapter plane, limited to individually
admitted machine handlers. Both use versioned, fixed-width values and validated
guest-memory ranges; neither passes a C++ object, host pointer, CRT ownership,
or raw cross-architecture function pointer. A bridge result may update only
fields expressly authorized by its record. `src/bx-ntvdm-adapter` is the sole
repository area allowed to own BOP identity, OpenNT/SoftPC provider selection,
and Bochs-facing typed lifecycle glue. `src/cli` may validate and transmit
immutable launch/profile metadata only; it must not include Bochs internals or
adapter guest-memory APIs.

For `C4 C4 selector` instructions, Bochs exports only a generic copied
exception event and bounded instruction window. The adapter owns the
source-derived BOP catalogue for every family: it may identify a selector and,
for a catalogued service family, its copied fourth-byte service ID; it observes
every recognized invocation. The adapter machine-composition plane consumes the
separately pinned historical BIOS disposition catalogue and is the only
possible selector owner for an individually admitted machine-handler island.
An entry cannot resume unless its exact historical owner, ABI and failure
behavior are separately admitted. Unimplemented entries are logged and pass
through to Bochs unchanged. This makes BOP identity visible without putting
DOS/SoftPC/CCPU dispatch semantics into Bochs or treating a catalogue as an
implementation. See
`etc/research/opennt-bop-definition-inventory-001.md` and
`etc/research/t115-s1-machine-bop-composition-dispatcher-audit-001.md`.

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

## Bochs Intrusion Convergence

The exception register is an admission ledger, not a count of live patches:
it contains implemented, approved-but-pending, rejected, removed, and
diagnostic-only entries. No aggregate count may be used as the current source
surface without a record-by-record comparison against imported source,
generated build rules, and the final link map.

Every retained Bochs-side project intrusion must converge to one of four
selector-blind mechanical classes:

1. `#UD` event delivery and fixed-width CPU/instruction-state copying;
2. checked ordinary guest-RAM read/write;
3. typed CPU result, resume, or controlled stop; or
4. the minimum Bochs machine-embedding lifecycle needed to call those
   mechanics.

Selector/service recognition, BOP routing, OpenNT provider selection,
source-derived contract semantics, CLI capability, and failure disposition are
adapter responsibilities. The `02h`/`06h` historical handler route is an
adapter machine-composition-plane concern: Bochs may receive only an opaque
typed mechanics request and return a typed result. It must not identify a BOP,
or contain OpenNT, DOS, SoftPC, CCPU, or selector-specific branches.

Before any new Bochs semantic intrusion, a read-only convergence audit must
classify every register entry as **retain**, **move to adapter**,
**retain default-off**, or **remove**, and reconcile that decision with actual
source and build/link use. Until that audit is complete, no new Bochs semantic
intrusion is admitted.

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

The historical SoftPC/CCPU/BIOS source and existing trace fixtures remain
behavior and ownership evidence. They are not an execution backend and must
not be linked into the Bochs runtime merely to reproduce an old composition.
The sole exception is an individually admitted, independently compiled
machine-handler island through the machine-BOP composition boundary; it must
have exact source/ABI/mechanics proof and a pre-existing exception-register
entry. `ntvdm64` is a downstream architecture consumer/comparison subject, not
a build or runtime dependency of this repository.
