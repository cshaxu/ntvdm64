# T169 S1 generic STOP native exit observation 001

## Inputs and procedure

One fresh bundle used the T168 derivative executable plus the retained T160 ROM
and T164 v4 BYOB inputs. T165's full-CLI observer ran
`ntdos64-run -> engine -> native Bochs` once with a 60-second watchdog.

## Observation

The process completed in 4215 ms without timeout and returned exit code 0.
The retained stderr records the existing guest sequence through `50:36`, the
exact `FE` at `0D00:0100`, `adapter controlled stop accepted`, and
`quit_sim called with exit code 0`. Input manifest, both streams and outcome
JSON are in `artifacts/analysis/t169-full-cli-native-controlled-stop-001`.

`cpu loop quit, shutting down simulator` is deliberately absent: T168 calls
`SIM->quit_sim(0)` directly in the already accepted generic STOP branch, so
the old `kill_bochs_request` CPU-loop-return route is not taken.

## Interpretation

The first profile has a complete, noninvasive CLI process completion contract
for its exact source-proven controlled-stop target. This does not claim a
generic BOP success facility or arbitrary DOS-program completion.
