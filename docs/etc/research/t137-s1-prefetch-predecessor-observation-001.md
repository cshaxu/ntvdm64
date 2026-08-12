# T137 S1 prefetch-predecessor observation 001

## Inputs and procedure

One bounded run used the T136 r4 executable SHA-256
`0EF41EF1D1796BB79BFD99D14BD906D5365BE8D5850FA98B007FCBDC245831EE` and
the existing T122/T130 deferred profile. The newly materialized profile hash
is exactly the retained T122 profile hash:
`3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982`.
ROM and startup fixture materialization used the unchanged observation tool;
the run had a 15-second watchdog and a one-million-instruction benchmark.

## Observed result

The process exited after 8.5 seconds (`exitCode=1`; watchdog did not kill it),
with 31 BOP observations and four commits. It reached BOP `50:36` at
`0032:5A70` and then the native CPU loop shut down at `0032:5A82`; no
`prefetch-limit`, `EIP > CS.limit`, INT 0Dh, BOP `02h`, or machine-composition
outcome record occurred.

Therefore BX-TRACE-056 produced **zero records**. This is not evidence about
the T134 predecessor: unlike T130, this current-adapter derivative did not
reach the late-fault state at all.

## Disposition

The single permitted run is complete and will not be repeated. The unchanged
profile/ROM identity excludes an input-fixture explanation, but the T136
derivative sources current adapter objects while T130's observed engine used
an earlier composition. A source/build identity comparison is required before
another runtime observation could be meaningful. No BOP/provider/reset/device
change is inferred from the orderly earlier exit.
