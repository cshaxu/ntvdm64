# T97 S5 Profile And Engine Checkpoint 001

## Fixture results

`byob-profile-test` builds and passes from the current source tree.  Its v4
case accepts the complete four-entry `guest_search_metadata` record and its
negative case renames that required member, which the profile parser rejects.
The runtime target's `--t97-path-search` mode independently accepts a v4
profile and proves `50:1B`, `50:09`, and `50:0B` through the installed runtime.

## Bounded live-engine probe

`tools/probe/Invoke-T95S7RuntimeTraceObservation.ps1` now emits the v4 profile
contract.  It was run against the retained engine artifact
`bochs-2.6-native-s7-runtime-trace-msvc-r5/ntdos64-s7-runtime-trace.exe` with
a five-second watchdog. The evidence root is
`artifacts/analysis/t97-s5-v4-engine-probe-001/`.

| Fact | Observed value |
| --- | --- |
| engine SHA-256 | `F4416F20F456366CC1AFC83D8B40748BF331D02E336BB9350EE8045329582DC2` |
| profile SHA-256 | `D76DCEABC8E087D59E753B52178505ACF979E0F34552B905044C921F22A9E33A` |
| BOP observations | 0 |
| committed transactions | 0 |
| stop | `requested execution plan rejected before CPU loop` |

The outcome is not a pathname-provider failure. It proves that the retained
engine binary has not been recomposed with the current v4 profile/adapter
closure, so it cannot execute the newly admitted runtime path. No guest BOP
was reached and no adapter transaction was attempted.

## T97 disposition

T97 is complete: its finite v4 capability, source-shaped `50:09/0B` provider,
positive/negative fixtures, runtime route, and live-engine limitation are all
classified. FCB remains deferred. The next required dependency is a minimal
adapter-to-engine composition closure; it must not be replaced by further DEM
service work or a full Bochs build loop.
