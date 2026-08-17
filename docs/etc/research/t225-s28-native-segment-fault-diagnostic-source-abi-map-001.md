# T225 S28 Native Segment-Fault Diagnostic Source, ABI And Failure Map

## Question

Can the repeated native segment-limit failure recorded by T225 S27 P5 be
observed once and terminated through existing finite mechanics, without
routing a non-UD exception through the BOP bridge or changing unarmed Bochs
exception behavior?

## Inputs

- S27's [native prerequisite map](t225-s27-native-parent-return-gate-prerequisite-map-001.md)
  and [first native attempt](../evidence/t225-s27-native-parent-return-attempt-result-001.md).
- Adopted Bochs 2.6 `src/bx-core/cpu/access.cc`, `access32.cc`, and
  `exception.cc`.
- `src/bx-mantle/bx_ntvdm_generic_ud_bridge.*` and the existing finite-run
  controlled-stop path.
- The formal MSVC x64 `/MT`, CPU5/Pentium-MMX graph in
  `tools/build/t225-s7-full-module-manifest.json`.

## Observed Original Path

S27 P5's bounded Direct run first emits `read_virtual_word_32(): segment limit
violation`, followed by `read_virtual_checks(): read beyond limit`. In adopted
Bochs, `read_virtual_word_32` invokes the original `exception(int_number(s),
0)` for the failed two-byte access (`access32.cc`), and `exception` performs
normal vector classification and delivery (`exception.cc`). The log does not
identify the access segment, pre-delivery register state, or the terminal
exception vector. It therefore cannot distinguish an invalid guest-state
precondition from incorrect exception delivery or later repeat behavior.

The existing mantle generic bridge is deliberately `#UD`-only: its `exception`
call site admits only `vector == BX_UD_EXCEPTION`, and its event includes a
copied instruction window. Reusing it for a non-UD diagnostic would conflate
mechanical fault observation with the adapter's BOP ingress and would violate
the retained boundary. It is rejected.

Bochs' normal logging/debugger route is also rejected: it supplies text rather
than a versioned copied ABI, can repeat indefinitely, and does not return
control to the finite owner at the first event.

## Admitted Mechanical Boundary

`BX-TRACE-084` is entered in the external-intrusion register before code
changes. If implementation remains necessary after source inspection, it is
limited to a default-off core-to-mantle event containing only:

- ABI magic/version/size and CPU id;
- exception vector and error code;
- fault RIP, EIP/EFLAGS, eight general registers and six selector values;
- no instruction bytes, guest memory, descriptor cache, host pointer, service
  identity, profile or provider field.

A mantle-private observer may be armed only by a formal diagnostic fixture or
an explicitly diagnostic native invocation. On its first accepted non-UD
record it uses the existing controlled-stop mechanism. An unarmed observer,
an invalid response, a duplicate event, or `#UD` must decline; native Bochs
then follows its original exception path exactly.

The record is intentionally insufficient to diagnose a segment descriptor by
itself. It establishes the smallest next branch: whether the first fault is a
segment/access violation at the expected state, or an exception-delivery /
control-flow failure before normal native continuation.

## Failure And Profile Disposition

The observer is profile-neutral: Direct and Readonly differ only in host
mutation capability, not CPU exception mechanics. Overlay remains deferred.
No profile can cause a diagnostic observation to succeed, fail, read guest
memory, or alter the copied fact.

A primary observation that does not reach a first non-UD exception remains an
explicit absence; it cannot be relabeled as parent-return evidence. A record
that reaches an exception does not authorize a repair. The next owner is
selected only after the Direct/Readonly facts are compared.

## Procedure And Follow-up

1. Add the default-off typed observer only under `BX-TRACE-084`.
2. Prove disabled pass-through and one test-owned non-UD capture/controlled
   stop through the formal graph.
3. Rebuild from a fresh `build/M0-T225-S28/<run>/` tree.
4. Run the immutable primary inputs once in Direct and once in Readonly with
   the diagnostic explicitly armed; retain hashes and concise facts in an
   evidence record.
5. If the first fault is captured, assign the resulting missing prerequisite to
   its native owner. If it is not, retain the ambiguity and do not widen this
   task into a repair.

## Confidence

High that a separate non-UD mantle diagnostic is required: the current source
makes the #UD restriction explicit, and S27's raw run shows the repeated
access/exception symptom. Medium on the eventual fault classification until
one bounded typed observation completes.