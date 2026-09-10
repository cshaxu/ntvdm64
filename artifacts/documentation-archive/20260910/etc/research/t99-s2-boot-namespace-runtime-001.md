# T99 S2 boot-namespace runtime checkpoint 001

## Source/build result

The T98 narrow derivative was regenerated with the one new
`bx_ntvdm_boot_namespace_provider_v1` object and linked successfully as
`artifacts/build/bochs-2.6-t99-boot-namespace-msvc-r1/ntdos64-t98-current-adapter.exe`.
Its SHA-256 is `D6B0FF4ADF898F1EDCC9BA3A097F7A8A3B8AD0B90A8A76F43CF44B0D0717D135`.
No Bochs source, archive, device, firmware or configuration was rebuilt.

## Bounded v4 observation

The one admitted five-second v4 trace is recorded under
`artifacts/analysis/t99-s2-boot-namespace-v4-probe-001`.

| Marker | Result |
| --- | --- |
| Execution plan | Applied |
| `54:0C` | Observed at the common BOP ingress |
| CONFIG multi-write | One write, 14 bytes |
| BOP observations | 15 |
| Committed transactions | 3 |
| Watchdog | Not triggered |
| End condition | Configured benchmark budget, exit 1 |

This confirms that the v4 runtime reaches `54:0C` through the provider-owned
namespace state. It does not demonstrate successful subsequent DOS open/read
consumption, AUTOEXEC (`54:0D`), FCB behavior or continuous guest execution.

## Remaining S2 work

The runtime's normal DEM open/seek/read/close path remains deliberately
disabled pending migration through the same copied guest-read/session flow.
That is the remaining provider-consumer work; it must be migrated as one unit,
not enabled one service at a time.
