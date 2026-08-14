# M0 T198 S81: Passive BOP 5F Observation

## Question

Does the current native NTIO/NTDOS composition reach the original BOP `5F`
keyboard/video initialization point before it transfers to the missing C000
native-INT10 vector?

## Inputs

- Existing source-built S74 and S76 runtime logs, both using
  `tests/bx-vdm/t198_s23_native_ntio_boundary_bridge.c`.
- That bridge's pre-existing passive `C4 C4 5F` observer and fixture report.
- S80's source map for the BOP-`5F` table/state chain.
- A fresh S81 r7 rebuild attempt. It rebuilt the full CPU5 core from source,
  but the tool-host terminated it while compiling adapter leaves; it did not
  produce an executable or run and is not used as a runtime result.

## Procedure

1. Review the observer ordering: canonical BOP detection records selector
   `5F` only after accepted common composition declines it; it performs no
   guest read/write and returns the same typed STOP used for other unhandled
   canonical BOPs.
2. Review source-built S74/S76 reports for its `spckbd observed` field.
3. Compare the first later noncanonical stop with S80's expected fullscreen
   branch.

## Observation

Both source-built full runs report:

```text
spckbd observed=0
fault=c000:014a
window=15:ffffffff
```

Their historical/instruction records put the predecessor at `073B:0939` and
the far jump bytes at `EA 4A 01 00 C0`. Therefore no canonical BOP `5F` was
observed before the runtime reaches the C000 no-image stop. The observer did
not modify the BOP outcome, write guest memory, set `use_host_int10`, add a
provider, or cross into Bochs.

## Interpretation

The original BOP-`5F` initialization contract is known, but the current
source-built start path has not yet shown that invocation. Consequently it is
not valid to select `STREAM_IO`, force `FULLSCREEN`, synthesize a resident
byte, or implement an S80-derived BOP-`5F` provider. The next question is
guest/control-flow provenance: which source-built NTIO/NTDOS initialization
path reaches `073B:0939` without a preceding observed `SpcKbd` BOP?

## Confidence And Limits

High confidence: S74/S76 contain a source-built passive observation and it is
not reached. High confidence: C000 occurs after that absent observation.
Medium confidence: the omitted call is due to the selected startup path rather
than a trace horizon, because the later C000 event is observed. The exact
reason remains unproven until the guest image/control-flow assembly is mapped.

## Follow-up

Audit the source-built NTIO/NTDOS image layout and initialization branches
between entry and `073B:0939`; do not add a BOP `5F` provider or mode policy.
