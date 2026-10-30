# Pending DOS guest-input crosswalk

The mixed source inventory contains 13 pending C/H paths under the original
MEM and SETVER command trees. The [per-file crosswalk](dos-pending-crosswalk.json)
compares each current file with its OpenNT counterpart: all 13 match exactly.
Ten also match an earlier individual source-role disposition in
`dos-command-input-role-dispositions.csv`. That earlier record traces the
original `cl16`/`link16` guest build inputs and excludes the historical file
role from the modern host-input denominator; its existing S12–S17 host
capability obligations remain open.

Three pending paths were not present in that earlier disposition CSV and are
therefore not treated as covered merely by its cohort totals. The original
MEM makefile names `memex.c` as the input to `memex.obj`, links that object
into MEM and lists `mem.h` for its C objects; the C sources also include the
header. The SETVER makefile copies `COUNTRY\message.h` and lists `message.h`
for `setver.obj`; the checked local header is byte-identical to the USA-MS
copy. These three have a supported guest-build-input role, subject to the
existing original build conditions and source policy.

The current selected x86 worker and WOW32 Ninja commands and dependency logs
contain none of the 13 exact paths. Each graph/tool combination has an observed
positive control: selected CCPU source and SAS header for the worker, WOW32
source and WOWFAX header for the provider. Graph-output hashes and per-path
results are in the crosswalk. This is bounded to recorded dependencies and
does not prove unbuilt configurations.

The [13 per-file host-source dispositions](dos-pending-host-dispositions.csv)
record these as excluded historical guest-build inputs for the selected host
scope. This is not exclusion of MEM, SETVER, DOS memory reporting or any host
service; S12–S17 retain those runtime contracts. The dispositions do not
modify the frozen mixed-scope ledger, establish all alternative build graphs,
or demonstrate guest execution. No guest or mirror file was changed.
