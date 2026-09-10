# T198 S107 fast-I/O no-op native observation 001

One exact-input native run was retained at
`artifacts/build/t198-s107-fastio-noop-native-r1`; its provenance declares
`runs: 1`. The fast-read passive recorder is zero, proving that `50:42` was
handled by the new composition plane before that decline-only recorder.

Execution then reached a non-BOP `#UD` at `C000:014A`, with code/stack
provenance from `0070:025E`. The fixture exits `2`, a bounded observation—not
a DOS result. Existing snapshot evidence records IVT INT10 as `C000:014A`, so
S108 must determine the firmware/ROM lifecycle owner before adding anything.

- `run.log` SHA-256: `cb3b8be68650dd61e07f7fd4dd4d4e3d719fefff2a7cb437784ab74e182e04ba`.
