# M0 T282 S15 COMMAND initial-environment bounded-output audit

## Question

Can the original `cmdGetInitEnvironment` replace its early raw
`GetVDMAddr(ES, 0)` alias with a checked exact lease without creating a new
observable failure result?

## Observations

- The source first computes `cchInitEnvironment`, then sets
  `cchEnvBuffer = BX << 4` and returns the original required paragraph count
  in `BX` when capacity is insufficient.
- It obtains its raw `ES:0000` pointer before that capacity comparison,
  writes the COMSPEC and multisz in two steps, and frees the accumulated host
  environment only after the writes.
- The capacity-success path has no source branch for an invalid/unmappable
  guest address, an exact-lease refusal, or a staging/allocation failure.

## Interpretation

Capacity shortage has an original result and must remain unchanged. A checked
lease can be acquired only after that check, but failure at that point has no
source observable result. Mapping it to the capacity result, CF, an empty
environment or a controlled stop would be authored behavior. This S therefore
does not change `cmdenv.c` or expose `54:0F`; it records the exact missing
fault-policy contract for a later integrated session/monitor owner.

## Follow-up

The next packet audits all COMMAND service/helper dispositions against the
completed local source composition and marks only source-proven direct paths
as candidates for further recovery.
