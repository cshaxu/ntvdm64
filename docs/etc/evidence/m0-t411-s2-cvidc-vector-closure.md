# M0 T411 S2 — C-VID vector-generation closure

## Question

Can the selected CCPU40 and C-VID views share the generated `Cpu` vector
without a profile-slot mismatch or a duplicate Cpu/Sas/Video owner; is any
part of the present binder/generator source-proven redundant?

## Inputs and method

The selected local OpenNT mirror, the S1 audit, the formal x86 graph and its
original `c2cpusad.h` metadata were re-read.  The new
`Verify-T411CvidcVectorContract.mjs` checks that metadata has 154 public and
55 private slots, that C-VID's `CalcQuickEventInstTime` uses the selected
CCPU provider, that CCPU's `effective_addr` is direct, and that the only
`Cpu.EffectiveAddr` indirection is under `CPU_30_STYLE` in `xt.c`.

The formal graph was generated in `build/M0-T411/S2/r004-cvid-vector-slim-run`.
It compiled the actual binder with the actual C-VID include ordering.  A
disposable x86 fixture linked that object with generated inert provider symbols
only; it does not replace a product provider or a renderer.  It checked the
published public/private Cpu, Sas and Video links, the CCPU quick-event
provider, original C-VID overwrite provider and repeat publication.  The
negative case asserts that `ClearHwInt`, intentionally selected as null by the
generator, is not fabricated into a cross-profile provider.

`ev_glue.c`, `main.c` and the binder were also compiled in the formal r001
graph.  The static contract check proves the early video-only bind before
`config()` and the complete bind at original `setup_vga_globals`; running the
entire renderer merely to test that call would add its unrelated 25-rule video
closure to this table-publication fixture.

## Observations

- The x86 fixture returned zero.  It verified `Cpu.CalcQuickEventInstTime`,
  `Cpu.Sas`, `Cpu.Video`, `Sas.Sas_overwrite_memory`, non-null private state,
  idempotent repeat binding, and the retained null `ClearHwInt` slot.
- The contract verifier returned:
  `public_slots=154`, `private_slots=55`, direct CCPU effective addressing,
  CPU30-only legacy indirection, early Video bind, setup bind, and null
  `ClearHwInt`.
- The apparent common-prefix naming conflict is intentional composition:
  C-VID's quick-event slot occupies the position CCPU calls directly through
  `c_effective_addr`; selected CCPU callers do not dispatch that slot.
- The initial `mvdm_cvidc_bind_video_vector()` and the later complete bind are
  not duplicates.  The former makes Video valid for `config()` before the
  original setup hook; the latter publishes Cpu/Sas/Video at the original
  C-VID setup point.

## Disposition

No binder/generator removal is source-proven in S2: **0 product lines removed**.
The 125-line C04 binder/generator carrier remains necessary pending recovery
of an original generated product unit.  Its source test and contract verifier
are added as regression evidence, not as production semantics.  C05's shared
calibration/restart state and E01's event-consumption policy remain explicitly
outside this result and transfer to S3/S4.

## Verification

```text
node v22.22.1 tools/audit/Verify-T411CvidcVectorContract.mjs .
=> {"public_slots":154,"private_slots":55,...}

build/M0-T411/S2/r004-cvid-vector-slim-run/cvidc-vector-binding-fixture.exe
=> exit=0

git diff --check
=> pass
```

Confidence is high for vector layout, provider selection and initialization
ordering.  This is deliberately not a C-VID rendering or quick-event timing
acceptance claim.
