# M0 T440 S1 — publication-timer baseline

## Scope

This is an attribution-only baseline.  It made no production source or
package change.  The current released `O:\winnt\system32\ntvdm.exe` hash was
`6339F3…`; temporary diagnostic replacement was restored and its matching
hash was rechecked.

## Measured project-owned stages

`tests/observation/measure-worker-primitives.*` compiles the unmodified shared
publisher and its DOS mouse queue with the x86 production flags.  Each
publisher sample measures `offer` through the configured send callback after
an idle publisher has been started.  The mouse result measures an immediate
queue push/take pair only; it does not claim to cover guest callback cost.

| Build-owned run | Publisher p50 | Publisher p95 | Publisher mean | Mouse queue pair |
| --- | ---: | ---: | ---: | ---: |
| `r010-worker-primitives` | 31.084 ms | 32.194 ms | 30.529 ms | 9 ns |
| `r011-worker-primitives-repeat` | 31.038 ms | 32.684 ms | 30.657 ms | 12 ns |
| `r012-worker-primitives-high-resolution` | 20.624 ms | 23.709 ms | 20.679 ms | 14 ns |
| `r012` repeat 1 | 20.618 ms | 24.766 ms | 20.962 ms | 13 ns |
| `r012` repeat 2 | 21.413 ms | 29.363 ms | 22.705 ms | 14 ns |
| `r012` repeat 3 | 20.585 ms | 26.437 ms | 21.347 ms | 11 ns |

The high-resolution rows use a test-only wrapper which requests
`CREATE_WAITABLE_TIMER_HIGH_RESOLUTION` and otherwise retains the exact shared
publisher, its 50 Hz interval, and its latest-state coalescing policy.

## Attribution

The ordinary `CreateWaitableTimerW` route turns the nominal 20 ms publication
cadence into roughly 31 ms on this host.  The high-resolution timer request
preserves the intended 20 ms cadence.  The measured mouse queue is orders of
magnitude below a visible stall, so it is not a repair candidate.

This does **not** prove a complete Win3.1 mouse end-to-end latency result:
ICA/IRQ dispatch, guest/DPMI callback work, frame construction, NTCON, and
Window presentation remain unmeasured.  The private desktop observer could
not acquire a valid frontend in either the released control or diagnostic
package, and a non-console `run16 command /c ver` invocation returned 87 with
no DOS witness.  Those observations are retained as unavailable, not treated
as product performance results.

## Disposition

The owner rejected this timer change after direct use confirmed that the 50 Hz
publisher is already visually smooth.  It remains an attribution fact, not a
product repair: the next measurement must target the actual Win3.1 input,
guest-callback, and Window-present path.  No production timer change is
admitted from this evidence.
