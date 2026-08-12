# Neutral Core/Mantle Delta Synthesis

Status: M35 design synthesis, 2026-08-08. This document reduces historical
evidence into neutral requirements. It does not change `ntvdm64`, create a
local executor, or authorize Microsoft-specific public contracts.

## Evidence Inputs

| Record | Proven requirement |
| --- | --- |
| M29 | a synchronous host read stages then atomically publishes a checked guest span |
| M31 | a profile image is loaded at a live guest address, not an initial preload |
| M33 | command services need copied record reads and several all-or-none patches |
| M34 | the complete original dispatcher survives; obsolete operations occur below preserved historical functions |

## Already Present In Core

| Existing interface | Limit | M35 disposition |
| --- | --- | --- |
| `core_machine_memory_read/write/query` | checked physical ranges and `ORDINARY_RAM`/`PROVIDER` only at stopped or paused boundaries | retain; it is the sole mapping route for any later transaction |
| `core_machine_apply_entry_plan` | stopped-boundary real-mode state and ordinary-RAM preloads | retain for initial entry only; not a runtime image/service write API |
| immutable ROM/device mapping and frozen topology | topology cannot change while execution runs | retain; transactions cannot write a provider or alter mappings |
| `undefined_instruction_transition` | real-mode `#UD` only, copied GPRs/CS:EIP, finite outcomes | retain as a safe special case; it is not a general instruction boundary |
| copied trace and platform events | observation/events do not borrow RAM pointers | retain; redaction and product policy remain above core |

The `#UD` hook cannot observe an instruction that the decoder accepts. Its
input also omits `DS`, `ES`, `SS`, and address-mode description. An adapter may
not reinterpret it as arbitrary instruction override.

## Required Assignment

| Requirement | Core | Mantle/backend | External adapter | Profile/product |
| --- | --- | --- | --- | --- |
| physical copy and route classification | owns | consumes | none | none |
| atomic multi-span ordinary-RAM publication | candidate extension | opens transaction at paused epoch | supplies byte patches | none |
| copied logical-address resolution | exposes bounded context only | invokes normal translation | names register/record fields | none |
| initial image/entry state | validates generic real-mode entry/map | composes entry plan | no DOS identity to core | owns selected images and state policy |
| instruction/event transition | candidate generic decode-point trap | owns epoch/order | decodes adapter protocol | none |
| file/image/stream/time/input | opaque contracts only | binds session lifetime | adapts historical call boundary | containment, names, CLI, UI, exit policy |
| diagnostics | bounded factual events | redacts/correlates | service-class context | storage/export/display |

## Core Candidates

### Multi-Span Transaction

A generic memory transaction accepts translated physical spans and copied
source bytes. It must reject overflow, inaccessible/non-ordinary ranges, and
conflicting aliases before mutation; permit only agreeing overlap bytes; and
publish all spans or none via the frozen ordinary-RAM route. It exposes neither
RAM backing nor provider callbacks.

M29 and M33 are two demonstrated uses but both belong to one external adapter.
They do not alone meet the two-product consumer rule. Until an independent
consumer is admitted, mantle may specify this operation but must not emulate
atomicity by a sequence of public `memory_write` calls.

### General Trap Context

The current `#UD` contract establishes the right safety posture: copied state,
validated byte consumption, constrained GPR/FLAGS patch, and only
`unhandled`, `handled-resume`, `stop`, or `fault`. A future neutral extension
may add a configured decode-point trap with a bounded byte pattern, physical
fetch address, copied segment registers and opaque address-mode classification.
Core must validate IP consumption and commit one response atomically. It must
not permit CRx, descriptor cache, segment state, CPU mode, mapping, topology,
or arbitrary instruction replacement.

This is a core candidate only after a non-adapter consumer is established.

## Mantle Boundary

Mantle owns one execution-session composition over an adopted core backend. It
has no DOS selector table, file role, command record, or host-root policy. It:

- creates a session epoch around a paused transition;
- retains copied CPU/translation context only for that epoch;
- coordinates transaction begin/commit/abort with a finite disposition;
- binds opaque file/image/stream/clock/input capabilities; and
- maps core factual stops/faults to typed session results.

Mantle owns no instruction decoder, PIC policy, BIOS, SAS map, or hardware
device model.

## Adapter And Profile Boundary

The external adapter alone owns the historical chain, protocol identifiers,
packed records, original overlays, opaque file tokens, and original failure
routes. The profile owns BYOB identities, exact image roles, contained roots,
encoded environment, command line, keyboard choice, and native-executable
handoff policy.

`NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, `CMDINFO`, `SCSINFO`, BOP selector
numbers, service numbers, and `CmdDispatch` are forbidden in core or mantle
public contracts.

## Adoption Gate

Before an adapter may execute a private prefix, all conditions must hold:

1. a selected core backend reaches a paused boundary without a local CPU;
2. it exposes copied logical-address context or equivalent normal translation;
3. it supplies the M29/M33 transaction semantics through an admitted generic
   core feature with a second consumer, or an equivalent bounded primitive;
4. the adapter preserves historical dispatch/function ownership and binds only
   profile-provided capabilities; and
5. unavailable services use original-route failure/stop, never fabricated DOS
   success.

Until this gate passes, `ntdos64` remains source/adapter/contract research. It
must not acquire a CPU, SAS, firmware, interrupt, or device subsystem.

## Result

M35 identifies an adoption decision, not an immediate cross-repository move.
The next task audits the current core against this gate and records what is
adequate, missing, or still waiting for a second consumer.
