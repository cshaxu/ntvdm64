# M0 T225 S53 P1 — Existing first-fault access-tuple observation

Using only the pre-existing `--observe-first-fault` option, the immutable
source-built four-image input ran once in Direct and once in Readonly. Both
runs are identical: exit 1, `BOUND_GdMa: fails bounds test`, then copied first
fault `vector=5 error=0 cs=0c41 eip=00001f97 eax=00002e5a eflags=00000006
mode=1`. The segment-access record is unavailable because its narrow ABI
covers only failed read-word limit branches; this BOUND access reached the
architectural bounds-test result instead.

The original-toolchain NTDOS map establishes a DOSCODE range `00000..07938`,
but current records do not provide the load-segment/relocation correspondence
needed to map runtime `0C41:1F97` uniquely to a source-map offset. Therefore
the result proves a guest `#BR` boundary condition and rejects a generic
segment-limit, VGA, PIC, FPU, BOP or adapter attribution; it does not name a
source function or authorize a repair.

## Correction

The first-fault option deliberately stops at the first delivered exception. Normal paired runs without that option continue to the established budget terminal. Therefore this record is a mechanical first-exception classification only; it does not establish BOUND as a runtime terminal, a guest-layout failure, or a repair candidate.
