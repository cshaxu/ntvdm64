# Evidence Model and Minimum-Diff Rules

## Categories

The audit uses A/B/C/D/U as bounded classifications, not quality grades.

| Class | Meaning in this audit |
| --- | --- |
| A | A compatible public or retained original route is source-qualified for the named local contract. |
| B | A bounded adaptation is required; preserve the original contract at its edge. |
| C | A source-qualified conversion or representation boundary is required. |
| D | A required compatibility provider or unavailable historical boundary is identified; it is not automatically impossible. |
| U | Evidence remains insufficient; retain the uncertainty and hand it to a finite later owner. |

No category itself proves an executable result. In particular, D does not mean
that a feature should be discarded, and A does not mean every behavior of an
API family is equivalent.

## Minimum-diff principles

Keep original source bodies, identifiers, layouts, widths, field order,
ordering, error mapping, partial side effects, and cancellation boundaries
where they carry observed semantics. Add only the smallest named connection at
an unavailable host boundary. Do not hide a missing provider by replacing a
subsystem with a new path/environment/handle manager, a new event queue, a
kernel/CSR shell, or a guest patch.

Guest code and binaries are immutable. The audit may use them to explain a
wire format or a consumer, but never treats guest modification as a solution.
Likewise, CPU/V86/BIOS/device-internal algorithms are stopping boundaries unless
one exact host-facing contract requires them.

## Required later evidence

A future implementation decision needs the specific receipts that apply:
selected provider, compiled ABI, object and ownership identity, conversion,
failure behavior, cancellation and late-delivery handling, and a focused
normal/negative/lifecycle runtime witness. A source report must never be
upgraded into such a receipt merely because its names are familiar on a modern
Windows system.
