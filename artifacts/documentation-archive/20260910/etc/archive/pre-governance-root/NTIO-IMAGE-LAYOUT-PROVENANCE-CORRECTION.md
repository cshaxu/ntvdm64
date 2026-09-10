# NTIO Image Layout Provenance Correction

Status: M17 evidence reconciliation, 2026-08-08.

## Fixed Profile Record

The currently selected fixed historical toolchain input and its fresh local
artifact agree exactly:

```text
NTIO.SYS length: 33792 bytes (0x8400)
NTIO.SYS SHA-256:
  CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937
NTIO conventional load physical: 0x700
linker-derived dos_load_seg: 0x08B0
derived NTDOS request physical: 0x8B00
```

The source is decisive about the relationship rather than a literal number:
`msinit.asm` declares a paragraph-aligned empty `dos_load_seg` segment and
executes `mov di,dos_load_seg`. Therefore any future profile computes or
records its selected layout from its accepted artifact/map pair.

## Disposition Of Earlier Values

Earlier isolated staging and frozen CCPU instrumentation records used a
different `0x83E0` image and consequently observed `DI=08AEh`. They retain
value as provenance for those specific experiments, but are not inputs to the
current fixed profile and cannot define the generic load-planning ABI.

| Location | Former statement | Correct status |
| --- | --- | --- |
| `OPENNT-BUILD-BOOTSTRAP-AUDIT.md` | `ntio-stage-v6`, `0x83E0` | historical staging record only |
| `OPENNT-STAGED-ARTIFACT-AUDIT.md` | `ntio-stage-v6`, `33,760` | historical staging record only |
| `OPENNT-NTIO-ENTRY-STATE-CARD.md` | `DI=08AEh` | frozen instrumentation observation only |
| `CCPU-STANDALONE-BACKEND-ASSESSMENT.md` | `DI=08AEh` | frozen instrumentation observation only |
| M6/M7 fixtures | `08AEh` / `0x83E0` | caller-selected synthetic test data, now neutralized or updated |

No prior document established `08AEh` as source ABI. The implementation of
`nt4_ntdos_load_plan_build` already takes `load_segment` from its caller; it
does not encode either value.

## Required Consequences

- `DI` is captured at the actual transition boundary and is never inferred
  from an NTIO file length by generic code.
- A profile admits NTDOS only after a matched NTIO artifact/layout record is
  known; it rejects an inconsistent pair before guest mutation.
- A transaction fixture may use arbitrary valid segments, but its prose must
  not call those values source-defined.
- Neither current nor former layout values authorize loading NTDOS before
  NTIO reaches `SVC_DEMLOADDOS`.

## Verification

The correction is complete when the owned M6/M7 fixtures pass with their
neutral/current test values and repository searches find no statement that
the former `0x83E0/08AEh` pair is a fixed-profile ABI. Historical staging
records may retain their literal observed values when explicitly scoped to
their own archived experiment.
