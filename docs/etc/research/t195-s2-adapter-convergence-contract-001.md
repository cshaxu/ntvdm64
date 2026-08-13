# M0 T195 S2: Adapter Convergence Contract

## Purpose

This record freezes the interface that repairs the two live T195 S1 targets:
the BOP catalogue/listener in `src/bochs/cpu/exception.cc` and the direct
machine-composition probe from that same file.  It is a target contract, not
a source move or an assertion that any BOP provider now executes.

The architecture authority assigns service and historical handler meaning to
`src/bx-ntvdm-adapter`.  Bochs remains the owner of CPU exceptions, copied
state capture, checked native mechanics, result application, RAM, port I/O,
and machine lifecycle.

## Existing Facts

`bx_ntvdm_exception_event_v1`, `bx_ntvdm_cpu_state_v1`, and
`bx_ntvdm_instruction_window_v1` already form a fixed-width copied exception
boundary.  The first has the exception vector and fault RIP; the second has
only copied register values; the third has at most fifteen copied bytes.  None
contains a raw CPU pointer, descriptor cache, guest pointer, or retained
native object.

`bx_ntvdm_exception_result_v1` and `bx_ntvdm_cpu_result_v2` already express
generic pass-through, resume, and controlled-stop outcomes.  An absolute,
typed `resume_rip` is the semantic result of the adapter decision; Bochs may
validate and mechanically apply it, but must not derive it from a BOP length
or selector.

The current `bx_ntvdm_machine_composition_v1_probe` is not compliant with the
target boundary: it reads `C4 C4`, extracts selector `02h`/`06h`, chooses the
historical handler, and returns a handler-specific outcome while its caller is
Bochs.  The existing mechanics context is useful only as an ephemeral native
capability, but its `machine_bop` names, classification, and selector fields
are not a cross-boundary contract to preserve.

## Target A: Generic Event Delivery And Adapter Observer

Bochs delivers exactly one copied exception event, copied CPU state, and
copied instruction window to one adapter dispatch entrypoint.  It neither
tests `C4 C4` nor calls a BOP catalogue/listener nor formats BOP identity.

The adapter is the sole owner of all of the following:

- deciding whether the copied window is a BOP;
- decoding selector and optional service;
- choosing a host-service plane, a historical machine-composition provider,
  an unavailable/deferred disposition, or pass-through;
- emitting BOP observation records, including selector/family/service; and
- choosing the generic typed CPU result, including any resume RIP.

The observer is observational only.  It may receive the adapter's copied
classification record and report it, but it cannot alter provider precedence,
guest state, CPU state, or result.  This replaces the current Bochs-side
catalogue listener family (BX-ABI-040 and BX-TRACE-043 through BX-TRACE-046,
BX-TRACE-059 through BX-TRACE-061).

For a non-BOP, malformed, unknown, deferred, or unavailable identity, the
adapter returns the source-proven result for the selected profile, or generic
pass-through when no provider is admitted.  The observer itself never turns a
failure into success.

## Target B: Adapter-Owned Machine-Composition Request

When adapter classification selects a historical machine handler, the adapter
may ask for a generic *native mechanics transaction*.  This request is opaque
to Bochs: it has a version, byte size, and an adapter-defined operation token,
but no selector, service, BOP family, OpenNT, DOS, BIOS, or SoftPC field.

Bochs may answer such a request only by constructing a synchronous,
non-retainable mechanics capability from its native owners:

- checked byte port read and write;
- checked guest-RAM byte load and store;
- copied real-mode stack facts where an original handler contract requires
  them; and
- an error-report callback.

The capability is valid only during the one adapter call.  It crosses neither
the persisted adapter ABI nor a device boundary; adapter code may not retain
the opaque pointer or use it after return.  Bochs does not select a handler,
decode an instruction, or interpret a provider-specific result while making
that callback.

The adapter machine-composition plane owns the operation-token-to-original-
handler mapping, including the current `unexpected_int` and `illegal_op_int`
islands.  It converts the original handler's success/failure into the same
generic result channel used by any other adapter provider.  Thus `02h`/`06h`
are recognised only after the copied window has reached adapter ingress; they
are never conditions in a Bochs source file.

## Result And Rejection Rules

Only three engine-visible dispositions remain:

| Disposition | Owner of decision | Bochs action |
| --- | --- | --- |
| pass-through | adapter or no interceptor | continue normal exception path |
| resume | adapter | validate/apply copied delta and typed resume RIP |
| controlled stop | adapter | stop through the existing typed stop path |

If the generic event or mechanics request is malformed, unsupported, used
outside its synchronous call, or fails a checked native operation, the adapter
returns a non-success result.  It may not fabricate a successful handler
result.  Bochs then performs only the corresponding generic pass-through or
controlled-stop action; it does not retry by inspecting BOP bytes.

No raw CPU pointer, Bochs class, device pointer, SAS/CCPU object, host handle,
or mutable guest pointer is admitted to either direction.  No contract grants
the adapter ownership of IRQ, PIC, RAM, port dispatch, CPU exception delivery,
or BIOS/device implementation.

## S3/S4 Migration Prerequisites

S3 moves catalogue classification and observation into an adapter observer
behind Target A, retaining the current copied event/state/window ABI.  It must
delete the corresponding selector/family/service recognition from Bochs.

S4 relocates the `machine_bop` header/component into the adapter machine-
composition plane, renames its externally visible request/result contract to
the generic form above, and changes the Bochs call site to an opaque mechanics
callback.  It must delete both the `02h`/`06h` constants and their selection
branches from Bochs.  The original handler objects and their three native
wrapper mechanics may remain as adapter-owned composition inputs; this does
not make those mechanics adapter-owned devices.

S5 removes obsolete build switches/hooks and leaves any remaining diagnostic
default-off.  S6 must prove, from a generated closure and final map, that
Bochs has no BOP catalogue symbol, selector/service formatting, or historical
handler-selection symbol.

## Compatibility Limits

This contract deliberately does not change BOP routing, provider priority,
guest-visible failure semantics, original-handler behavior, device coverage,
or the active runtime profile.  Existing v1 names may remain temporarily only
inside the migration objects until S4 replaces them; they are not an approved
new public boundary.  Any need for an additional Bochs semantic field,
provider behavior change, or an asynchronous mechanics capability stops the
repair and requires a newly admitted task.
