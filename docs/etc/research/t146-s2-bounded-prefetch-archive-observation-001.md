# T146 S2 bounded prefetch-archive observation

## Question

Does the source-pinned T146 CPU archive reach T130's recorded late prefetch
fault and emit the registered BX-TRACE-056 predecessor record?

## Inputs and procedure

The sole run used `ntdos64-t146-prefetch-archive.exe` SHA-256
`FCF8A5B7D67932B75B1D2730B15C4270971CF912B1411BF3B38247C1C519F1A8`,
the existing deferred-startup observer, a one-million benchmark, and a
15-second watchdog. Its materialized profile hash was
`3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982`,
which is the retained T122/T130 profile identity. Output is retained at
`artifacts/analysis/t146-s2-prefetch-archive-observation-001`.

## Observations

- The process exited with code 1 after 8.9 seconds; the watchdog did not
  terminate it.
- The observer recorded 31 BOP observations and four committed transactions.
- The final reached host boundary was `50:36` at `0032:5A70`, with the
  existing accepted resume to `0032:5A74`.
- The original CPU loop then quit at `0032:5A82`.
- There is no BX-TRACE-056 record, no `prefetch: EIP > CS.limit`, no INT 0Dh,
  no BOP `02h`, and no machine-composition outcome in this run.

## Interpretation and confidence

This is a valid bounded observation of the T146 artifact, but it is not a
late-fault observation and therefore yields no predecessor evidence. Its
profile/ROM identity is fixed, but reachability differs from T130. The
difference cannot be attributed to a BOP provider, adapter, guest input,
firmware, or device: none changed in T146 and this S expressly made no
semantic change. It is also not evidence that the registered log is wrong;
the log branch was not reached.

No retry is permitted under T146. A future owner-admitted static audit may
compare the rebuilt CPU archive's object/toolchain/link provenance with the
retained T130 archive before any further runtime observation is considered.
