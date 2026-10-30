# Optional CCPU diagnostic translation recovery

The pre-change project reporter translated a linear CALLF source address and
passed the resulting physical address to `c_sas_loads`, whose original
contract is linear. A controlled test linked the actual pre-change reporter:
with a synthetic nonidentity mapping it observed linear `0x18` translated to
`0x9018` and passed `0x9018` to the linear reader. Disabled tracing and a
nonmatching selector made neither call. This demonstrates a diagnostic
address mismatch, not an observed real guest exception or a failure with
diagnostics disabled. The pre-change source and fixture identities are retained.

The candidate removes diagnostic-only descriptor/code sampling and translation
from the far-call, DPMI-fault and descriptor-store reporters. These records
retain their existing filter and already supplied scalar metadata, but no
longer include instruction bytes or sampled descriptor contents. The change is
project-owned, has no mirror-source or guest diff, and reduces code. It does
not change original instruction execution or introduce a substitute reader.

The actual changed reporter passed a controlled test with translator and SAS
boundaries that abort if called: disabled trace, unmatched selector and all
three enabled reports completed, produced expected scalar records and
preserved last-error. The selected CCPU far-destination suite passed its four
fault-order controls, and the diagnostic last-error suite passed. A cached
x86 worker build passed. The sealed ten-component package then passed all 16
Full gates and a separate Winmine gameplay probe; publication verified all 20
destination hashes. These tests do not prove every enabled diagnostic or live
guest failure path; no SOL 1400 attribution is implied.

A later [selected CCPU CALLF witness](../ccpu-callf-trace/review.md) executes
and returns through the original real-mode instruction path with the trace
off, unmatched and matched. It complements the separate fault-order fixture;
it is not a protected-mode guest acceptance.
