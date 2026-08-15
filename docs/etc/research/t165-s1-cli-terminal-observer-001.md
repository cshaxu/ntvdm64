# T165 S1 CLI terminal observer 001

## Question

Can a future single native CLI observation preserve terminal evidence without
changing Bochs, adapter, guest, BOP, or BYOB behavior?

## Inputs and procedure

`tools/probe/Invoke-CliNativeControlledStopObservation.ps1` records immutable hashes
for the full CLI chain (`ntdos64-run`, engine, profile, root, Bochs bundle and
target) before launch. It redirects stdout/stderr, closes only the outer
runner's stdin, enforces a bounded watchdog, and writes outcome JSON in all
normal/timeout/launch-error cases.

The first two fake attempts correctly exposed test-setup mistakes: direct
engine launch lacks the runner's adapter environment, and an arbitrary v3
profile name is rejected. The third fixture used the real T124 runner and
engine, a valid fixed v3 profile, and the existing fake Bochs parser. It
returned 47 with `watchdogTerminated=false`; `inputs.json`, `stdout.log`,
`stderr.log`, and `observation.json` were retained. Reusing that evidence root
was rejected.

## Interpretation

The observer covers the actual CLI ownership chain, unlike T164's direct shim
launch. It is process evidence only: markers are recorded without interpreting
guest success. No native Bochs process was launched in T165.

## Follow-up

A new packet may pin real inputs and admit one native use of this observer.
