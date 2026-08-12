# Adapter BOP Selector Probe v1

## Scope

This is the first adapter-side consumer of the mechanical instruction-window
record. It accepts only three available bytes with the exact historical
`C4 C4` prefix, then resolves the third byte through the already admitted
one-entry selector map.

At present the only positive result is:

| Input bytes | Result |
| --- | --- |
| `C4 C4 50` | owner identity `OPENNT_MS_BOP_0` |

All other prefixes, short windows, invalid records, unknown selectors, and a
null result destination fail closed with `NONE`.

## Deliberate omissions

The probe does not read the following service byte; it has no guest-memory
access. It does not invoke an owner, call `DemDispatch`, move IP, write CPU
state, choose a resume address, or emulate a DOS operation. It is therefore a
boundary classifier, not a BOP dispatcher.

The `C4 C4` comparison appears only below `src/bx-ntvdm-adapter`; the Bochs
source remains generic observation transport and the OpenNT source remains
untouched.

## Evidence

The UCRT C11 fixture proves the one admitted input and rejects selector `51`,
UD2-shaped input, a two-byte window, and a null output destination.
