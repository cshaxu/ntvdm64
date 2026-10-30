# WOW16 provisional C/H source-role census

All 240 `wow16/` entries in the provisional `role-pending` list were compared
individually with their recorded selected OpenNT originals and the recorded
x86 worker and WOW32 Ninja build/dependency data. Every current file matches
its recorded original hash; no exact one of these paths appears as a direct
source or recorded dependency in those two graphs. [The per-file census](wow16-role-pending-census.json)
contains each path, hash, prior provisional disposition and graph result.
This is selected-build evidence only: it is not a complete audit of other
configurations, source roles, or functional restoration.

The directory name alone is demonstrably insufficient for host exclusion.
For example, `wow16/ddeml/tests/ddestrs/sources` builds `ddestrs` as a Windows
test application with `WIN32` defined, and `ddestrs.c` has both WIN16 and
WIN32 branches. These files are test-tool inputs in the inspected product
graphs, but may be relevant as test sources; they are not evidence that DDE
behavior is unnecessary. Their possible test role must be reviewed separately
from the S14/S15 runtime contract.

Likewise `wow16/inc/print.h` is included by the shared `inc/wowfax.h` only
under `_WOWFAX16_`; `wow32/wow32fax.c` includes that shared header on the
32-bit side, while `wow16/test/shell/wowexfax.c` defines the 16-bit branch.
The current WOW32 compiler dependency lists `inc/wowfax.h`, not the 16-bit
`print.h` path. This is a cross-boundary provider relationship, not grounds
to discard fax behavior or claim the 16-bit header itself is compiled in the
host. S16 already retains fax provider, mapping, callback and teardown work.

The 240 provisional labels therefore remain unchanged pending per-component
source ownership and capability evidence. Do not label them all excluded, or
present a source hash match as a behavioral pass. No guest or mirror was
modified in this census.
