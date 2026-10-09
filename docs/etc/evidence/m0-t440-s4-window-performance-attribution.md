# T440 S4 — Window performance attribution

## Scope and method

This is an attribution report, not a product repair.  It combines a fresh,
current-source renderer microbenchmark with the latest successful real Window
EDIT instrumentation that measures the same worker/NTCON transport boundary.
The latter was produced at T429 S1 `r017`; its timings are historical evidence,
not a claim that the current package has identical percentile values.  S3 did
not change the console-I/O wire protocol or the worker client chunking loop,
so it remains valid evidence about the cost class and protocol shape.

No production binary, guest media, protocol, publisher cadence, or input
policy was changed for this report.

## Measured costs

### Current source: Window renderer only

`build/M0-T440/S4/r002-window-render/results.txt` measures 240 full 640x480
Window renders from the current source:

| Stage | Samples | p50 | p95 | max | mean |
| --- | ---: | ---: | ---: | ---: | ---: |
| Window full-frame render | 240 | 0.382 ms | 0.640 ms | 0.702 ms | 0.425 ms |

This covers the renderer's full-surface damage scan and paint work, but not
guest execution, pipe transfer, Window-manager composition, or physical/RDP
display scan-out.

### Successful real Window EDIT run: transport and presentation

The following aggregates all 57 successful text frames in the three accepted
T429 S1 `r017` Window EDIT runs.  The raw reports are retained under
`build/M0-T429/S1/r017/worker-*.txt`.

| Stage | Samples | p50 | p95 | max | What it includes |
| --- | ---: | ---: | ---: | ---: | --- |
| Text assembly | 57 | 0.044 ms | 0.072 ms | 0.101 ms | Worker construction of a text frame |
| Worker→NTCON video transfer | 57 | 86.845 ms | 117.579 ms | 132.011 ms | Synchronous chunked pipe transactions and peer work |
| Frontend decode | 55 | 0.100 ms | 0.182 ms | 0.210 ms | NTCON frame decoding |
| Frontend present | 55 | 0.068 ms | 2.936 ms | 408.581 ms | Window present; one cold/setup outlier is retained |
| Worker input read | 182 | 0.060 ms | 0.348 ms | 0.883 ms | Worker read-side handling |
| Mouse enqueue→guest IRQ consumption | 146 | 6.678 ms | 97.534 ms | 97.950 ms | Queue residence/CPU consumer timing, not display latency |

The input queue was bounded (peak 17); all submitted entries were consumed.
That excludes runaway queue growth, but the 98 ms consumer tail can still
contribute occasional mouse lag.  It does not explain a persistent low frame
rate by itself.

## Source-backed protocol accounting

The current shared client (`src/common/console/client.c`) sends a
`VIDEO_BEGIN`, then calls the synchronous `ntcon_worker_call` once for every
`VIDEO_DATA` tile.  A tile is limited to `CONSOLE_IO_DATA_BYTES`, currently
16,384 bytes (`src/common/protocol/console_io.h`).  Each call writes a request
and waits for a reply before the next one is sent.

| Frame | Payload | Data tiles | Synchronous request/reply exchanges |
| --- | ---: | ---: | ---: |
| Historic EDIT text frame | 20,420–20,900 B | 2 | 3, including `VIDEO_BEGIN` |
| 640×480 8-bit VGA frame | 307,200 B | 19 | 20, including `VIDEO_BEGIN` |

On the receiving side, `src/ntcon-exe/console_channel.c` takes the frontend
session `io_lock` for each `VIDEO_BEGIN`/`VIDEO_DATA` dispatch.
`src/ntcon-exe/frontend_session.c` also holds that lock while executing the
decode/geometry/present sequence.  Therefore the protocol serializes every
16 KiB payload transfer with the same frontend critical section and a
cross-process acknowledgement.  This is a direct source-backed explanation
for why the small renderer measurement does not predict interactive
performance.

## Attribution

1. **Most likely current bottleneck: synchronous, per-tile worker→NTCON video
   transfer and contention with the frontend presentation lock.**  Historic
   real traffic measured 86.845 ms p50 for only three round trips per roughly
   20 KiB text frame.  A full 640×480 DIB needs 20 such exchanges today.  A
   linear projection would be speculative and is deliberately not reported as
   a measured graphics latency, but the exchange count makes this the only
   dominant candidate supported by both source and measurements.
2. **Secondary contributor: guest mouse-IRQ scheduling tail.**  The 6.678 ms
   median / 97.950 ms maximum measurement can make mouse movement occasionally
   late.  It is not a reason to change the bounded merge policy without a
   matched current graphical workload.
3. **Excluded as primary bottlenecks:** current renderer 0.382 ms p50;
   historical frontend decode 0.100 ms p50; text assembly 0.044 ms p50; and
   immediate queue operations previously measured at nanosecond scale.
4. **Publication cadence remains a cap, not this diagnosis.**  The accepted
   publisher has previously measured roughly 20–31 ms cadence.  Even its
   slower ordinary cadence is materially below the 86.845 ms historical
   transport median and cannot explain the transfer result.

The user's observation that the newer path retains a clean software cursor is
consistent with the present display path being functionally correct; this
report does not propose weakening it.

## Confidence and remaining measurement boundary

The current source renderer number is fresh.  The only complete semantic
worker-to-NTCON timing witness is the accepted T429 instrumented Window EDIT
run.  A current, manual Window-session capture could replace its historical
percentiles, but the private-desktop observer could not form a valid current
Window witness: it exits before the frontend route is established under the
current native-frontend package.  That limitation is a test harness failure,
not a performance result.

Two build-only current diagnostics were prepared and are not deployed:

* `build/M0-T440/S4/r003-worker-performance-retry/ntvdm-performance-observer.exe`
* `build/M0-T440/S4/r009-native-frontend/ntcon-performance-observer.exe`

They retain the existing phase markers and can be staged temporarily into an
ordinary interactive Window session, then restored by hash, to collect a
current manual trace without changing production source or release contents.

## Narrow next repair candidate (not implemented in S4)

After a current trace confirms the same distribution, replace the
request/reply-per-16 KiB **payload** exchange with a bounded streamed frame
payload and one frame-completion acknowledgement.  Preserve frame ownership,
backpressure, final-frame/handoff acknowledgement, error precedence, and the
existing worker-neutral I/O channel.  This is a protocol change, not an
optimization hidden behind deduplication or a timer change, and requires a
separate admitted task with focused failure/ordering tests.

No repair is made by S4.
