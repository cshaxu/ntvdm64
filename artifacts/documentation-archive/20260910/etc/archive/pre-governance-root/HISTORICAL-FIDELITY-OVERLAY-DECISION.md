# Historical Fidelity And Overlay Decision

Status: M39 binding research decision, 2026-08-08.

## Decision

The project has two distinct execution lanes. They may share source evidence,
the generic backend, and an external adapter process, but their claims and
admission criteria must not be mixed.

### Lane H: Historical Host Reference

Lane H preserves the original historical host-function algorithms and their
data flow. It permits only a **host-operation bridge**: a replacement for one
unavailable host facility at a named original call boundary, with the original
caller continuing to make all protocol, record, error, and lifecycle decisions.

Lane H can claim a restored historical link only when the trace identifies the
original caller and its original downstream historical ownership. It may not
replace a selector function's profile policy, shorten a dispatcher table, or
turn an excluded historical product path into a successful result.

### Lane P: Contained Profile Adapter

Lane P is a separately declared external adapter profile for a non-invasive,
one-shot contained CLI session. It may use a **service-body overlay** only when
M38 proves that the original body irreducibly embeds excluded process/CSR,
PIF, temporary-file, ambient-environment, registry, raw-handle, or console
product policy.

Lane P retains the original dispatch table, selector consumption, function
signature, and downstream DOS ownership. It may replace a named internal body
region with profile data plus neutral mantle capabilities. It must never claim
that the resulting host behavior is an unmodified historical host stack.

## Overlay Classes

| Class | Permitted in Lane H | Permitted in Lane P | Example shape |
| --- | --- | --- | --- |
| H1 direct host-operation bridge | yes | yes | original file, time, event, or process-context call binds to a modern contained capability |
| H2 memory transport bridge | yes, only if byte-for-byte data flow and original error path remain | yes | raw guest pointer borrow becomes copied checked transaction |
| P1 profile service-body overlay | no | only by explicit admission | immutable command/environment/configuration profile replaces irreducible old product-state region |
| P2 selector/dispatcher replacement | no | no | new selector table, direct handler invocation, or bypassed dispatcher |
| P3 DOS/PC-AT semantic replacement | no | no | synthetic BOP/SVC success, DOS file/process state, BIOS/device response |

## P1 Admission Threshold

A P1 overlay is not a casual compatibility patch. Before implementation it
needs all of the following:

1. M38-style source proof that no H1/H2 bridge can satisfy the required call
   without enabling prohibited host-global policy.
2. A fixed profile input schema that supplies every replacement fact before
   guest mutation; no ambient host discovery is allowed.
3. An explicit input/output/failure table for the original function entry,
   including register effects, guest writes, termination/return disposition,
   and retry behavior.
4. An adapter-local transaction/capability implementation backed by an
   adopted generic backend contract; no raw pointer or serial guest writes.
5. A differential bounded fixture: the overlay must be distinguishable from
   Lane H and report itself as Lane P in redacted trace metadata.
6. Default-disabled build/link isolation from generic runner and historical
   reference targets.
7. No claim of historical equivalence, original-host restoration, or general
   NTVDM compatibility beyond the exact fixed profile/fixture.

## First-Cut Consequences

M38 classifies console acknowledgement and session stop as H1 candidates.
Address transport for set-info, COMSPEC, image load and file reads is H2 once
the core adoption gate passes. Configuration, initial environment, keyboard
layout, first command assembly, current-directory projection, and completion
are P1 candidates because their original command bodies embed excluded product
policy. They remain unimplemented and cannot advance a runtime now.

## Non-Negotiable Boundaries

- Neither lane may add a new command selector, BOP table, DOS service handler,
  CPU, firmware, or PC/AT device model.
- Lane P profile data belongs outside core and mantle. Core and mantle contain
  no file-role, drive, path, command, selector, or console-product vocabulary.
- A Lane P stop, fault, or unsupported-service result must be explicit; it
  cannot be transformed into a DOS success merely to advance the smoke test.
- Lane H evidence is the only evidence that may support claims about historical
  host responsibility or source behavior.

## Result

Service-body overlays are compatible with the broader research objective only
as explicitly labeled Lane P adapter work. They are not compatible with the
claim that the original historical host stack itself has been restored. M40
will define the required Lane P per-service input/output/failure dossier before
any overlay source is admitted.
