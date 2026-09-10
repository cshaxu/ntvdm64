# T225 S13 Primary-Image First-BOP Reach Ladder Result

## Question

Does the immutable source-built primary image reach a BOP-shaped `C4 C4`
event under a bounded, internal execution budget; and does Direct differ from
Readonly before that point?

## Inputs

- S12 Ninja native CLI:
  `build/M0-T225-S12/ninja-r1/bin/ntdos64-native.exe`, SHA-256
  `A30F2D2481F89C3C7D20AF6675CA5C591BEB00728627EE79CF57E8DABD2D6589`.
- S12 manifest configuration SHA-256:
  `0e06777d39567aa518b2ea5c967ef74e14ab481e1ce53726e41a602c6deb2eee`.
- Immutable S10 primary profile:
  `build/M0-T225-S10/primary-bootstrap-inputs-r2/profile.json`, SHA-256
  `C41D200BD3FDAE40D445CF3F8CA6256C0E94DB3988A9DF250C7C3450D52C3D36`.
- The same staged `fixture-config.nt`, `fixture-autoexec.nt`, and
  `TARGET.EXE` used by S10.  The process environment selected the first two
  paths through the existing startup contract.

## Procedure

Each run used the existing `--observe-bop-sequence` listener and an internal
`--instruction-tick-budget`, with no external watchdog and no source, guest,
provider, adapter, mantle, or Bochs change.  Raw diagnostics are disposable
at `build/M0-T225-S13/ladder-r1/` and are not a governed evidence input.

S12 had already recorded Direct at 10,000 ticks.  This packet then ran the
next admitted levels independently:

| Mode | Budget | Elapsed | Exit / terminal | Observer |
| --- | ---: | ---: | --- | --- |
| Direct | 50,000 | 177 ms | 4 / budget (4) | count=0, overflow=0 |
| Readonly | 50,000 | 171 ms | 4 / budget (4) | count=0, overflow=0 |
| Direct | 100,000 | 135 ms | 4 / budget (4) | count=0, overflow=0 |
| Readonly | 100,000 | 147 ms | 4 / budget (4) | count=0, overflow=0 |

Each terminal line retained lifecycle=5, presentation=5, cancellation=0 and
its requested budget.  No run hit a natural stop, machine failure, observer
overflow, non-return, or mode discrepancy.

## Observation and interpretation

The primary image has not reached any observed `C4 C4` BOP ingress through
100,000 internal ticks in either selected product mode.  Thus there is no
first selector/service to classify, and this evidence neither proves a BOP
absence nor attributes the result to a missing provider.

The shared pre-BOP boundary is instead native execution continuity.  The
reported CPU diagnostics (including rejected `LOCK` prefix and unrecognised
`SLDT`) are observations only; this packet does not assign them a cause or
authorize an emulator change.  The next package must map the reached guest
control flow and these instruction classes to OpenNT source/artifact context,
then assign any demonstrated prerequisite to its original owner.

## Confidence and follow-up

Confidence is high for the bounded result because all four newly-run rows
returned through the normal internal terminal with the same default-off
observer enabled.  It is intentionally bounded at 100,000 ticks.  S14 owns
the source/ABI continuity map; it must not implement a BOP, enable a device,
or treat this zero-count result as a license for a Bochs patch.