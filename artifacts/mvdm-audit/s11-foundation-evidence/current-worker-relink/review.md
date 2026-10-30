# Cached worker relink after the port fixture

Ninja requested a worker relink because the recorded `ntvdm.exe` timestamp
predated `opennt-base-bindings.lib`. The cached x86 worker was relinked and
the subsequent Ninja dry run reported no work. The build's VdmTib storage
check passed. The pre-link worker was retained separately under the task
build root for this comparison; no guest or release file was changed.

Both images are 2,097,152 bytes. A complete byte comparison found four
changed bytes: `0x130`, `0x131`, `0x19ce7c`, and `0x19ce7d`. Parsing the PE
header and debug data directory places those bytes within the COFF and debug
directory timestamps, respectively. Zeroing those two complete timestamp
fields makes the two images byte-identical, with normalized SHA-256
`27F1AF70EA78771531E1F9B8D9D95911F5CC1314EE6E7D8F6E26DFDDD0A97F9F`.
The unsealed pre-link image hash matches the published package's recorded
input hash. [Comparison](comparison.json) and [input hashes](before.json)
record the results.

This establishes identical file bytes outside those two timestamps, not a
fresh full product regression. The already published sealed image remains
the earlier tested artifact; the relinked image has not been published.
Future product changes still require a coherent package and Full gate.
