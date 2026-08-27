# M0 T282 S14 COMMAND COMSPEC bounded-input audit

## Question

Does original `cmdComSpec` define a safe observable result for a missing-NUL,
oversized or unmappable `DS:DX` string, permitting the S12 bounded copy to
replace its raw pointer without inventing behavior?

## Observations

- `cmdmisc.c` calls `GetVDMAddr(DS, DX)`, then two unbounded `strcpy` calls,
  computes `cbComSpec`, and only then sets AL.
- The destination is `lpszComSpec[64+8]`; S9 therefore proves 64 bytes
  including the source NUL as the maximum safe input.
- No branch in `cmdComSpec`, `cmddisp.c`, or its reached source callers sets
  an error register, retry, controlled stop or diagnostic for an invalid
  source string. A historical invalid alias instead enters undefined host
  reads/writes before the service has an observable result.

## Interpretation

The original algorithm is reusable for a valid bounded string, but source
does not supply a service-level failure contract for the negative case.
S12 correctly rejects it; mapping that rejection to AL/CF, an empty COMSPEC,
or a session stop would be newly authored behavior. Accordingly this S does
not modify the mirror or enable `54:02`. The bounded-input shape remains
available only after an owner-approved integrated guest-fault policy provides
the missing observable result.

## Follow-up

`cmdGetInitEnvironment` is independent and has an original size-shortage
branch (`BX = required paragraphs`); it is the next eligible bounded
guest-memory recovery item.
