# T139 S1 reproducible post-handoff observation baseline 001

## Retained baseline

The only comparable late-fault baseline is T130:

- binary SHA-256:
  `2E591BDC8FA846544B1EA9AEFBE2EE54643C0EDCB382F3D1C1063953CDDA2404`;
- fixed deferred profile SHA-256:
  `3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982`;
- evidence: the corrected T130 trace with native POST, the `0BC1:FFFF`
  prefetch/INT 0Dh route, and original BOP `02h` return behavior.

## Excluded observation shape

T136's direct `cpu.cc` replacement is excluded as a comparison baseline. Its
map necessarily replaces `cpu_loop` and other CPU functions together with
`prefetch`, while the retained archive supplies those functions in T130. The
single T137 result may be retained as negative evidence but must not be used
to attribute the T130 fault or to modify host/guest behavior.

## Future admission gate

A future runtime observation must first demonstrate one of the following:

1. an observation point already resident in the T130 binary; or
2. a reproducible full CPU-object/archive build contract whose behavior is
   independently compared before the diagnostic is enabled.

Neither condition is currently admitted. This branch is deferred without a
repair. The next evidence path is static guest-image/source correlation of
the already observed physical address, which requires neither condition.
