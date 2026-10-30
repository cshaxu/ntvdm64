# SOL retired-window scan diagnostic

An isolated ten-image package replaced only WOW32 with a diagnostic build of
the existing collector. It retains the per-slot locking and terminal behavior,
and samples every 64th call per thread using QPC; it writes sampled scan count,
slot count, pending count and duration after the collector returns. It restores
LastError around the instrumentation. The SOL probe records monotonic opening,
shown and close-complete ticks. All three modal interactions passed; the exact
staged manifest, diagnostic source, samples, analysis and hashes are retained.

For the sampled calls in each phase, the median scan traversed 3,329 slots;
none of these sampled scans found a pending item. Applying each sampled duration
to 64 calls estimates 448 and 448 scans on the second and third opens, and 384
scans on each of their closes. Estimated inclusive scan time was 53–56 ms for
those opens and 49–53 ms for those closes, versus 2.0–2.3 seconds wall time.
The first opening includes 1,920 estimated scans and 230 ms inclusive on this
same approximation, versus 2.5 seconds wall time. These are not exact counts:
phase edges can miss partial 64-call intervals, nested calls may double-count
inclusive duration, sampled calls may not represent unsampled calls, diagnostic
file writes and QPC perturb timings, and time-only matching can include other
threads. The diagnostic does not prove absence of expensive unsampled work.

This evidence establishes repeated scans of a sparse range as an actual cost,
but the sampled steady-state estimate is too small to explain the whole modal
delay. It is consistent with the inconclusive lock-batching comparison. Do not
promote that candidate or attribute all latency to this collector. No guest or
product source was changed for this diagnostic.
