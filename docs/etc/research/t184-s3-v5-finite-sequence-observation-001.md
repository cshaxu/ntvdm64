# T184 S3 v5 finite-sequence observation 001

Date: 2026-08-12  
Packet: M0 T184 S3  
Disposition: one complete bounded observation; it did not reach `54:01`, target or QUIT.

## Durable result

The sole controller invocation retained `artifacts/analysis/t184-s3-v5-finite-sequence-observation-001`.
Its `observation.json` records 30,431 ms elapsed, wrapper PID 21140, watchdog termination,
`taskkill /T` exit zero, and no launch error. The input manifest pins current runner, shim,
T182 engine, v5 profile and SHARE target identities. No runner, shim or native engine process
remained after the tree kill.

## Observed boundary

The retained native output has 264,064 COMMAND-selector observations.

| Marker | Count |
| --- | --- |
| `54:0C` CONFIG.SYS request | 264,063 |
| `54:01` command request | 0 |
| adapter transaction commits | 2 |
| controlled stop / selector `FE` / CPU loop shutdown | 0 / 0 / 0 |
| pass-through at `54:0C` and real-mode vector 06 | 264,063 each |

The loop is before the finite two-slot command provider. It neither tests nor disproves v5
`54:01` delivery, target execution, QUIT execution, normal DOS return or CLI result transport.

## Source classification and transfer

`54:0C` is original COMMAND `SVC_GETCONFIGSYS`, whose existing adapter provider is the
configuration portion of the fixed boot namespace. Current `byob_profile.c` validates v5
command placement and boot-file declarations, but its selection projection condition copies
those values only for v2/v3/v4. The v5 selection consequently lacks the runtime fields that
the existing configuration provider consumes. This is a profile-to-adapter compatibility
projection omission, not a Bochs/device/CPU/BOP semantic gap.

The next package must repair and test the complete v5 runtime selection projection for command
placement, boot files and slot-zero compatibility, then perform a separately admitted observation.
T184 cannot retry.
