# Pending OLE client and server C source roles

The mixed inventory contains 15 pending OLE client and six OLE server C
paths. The [per-file census](ole-pending-source-census.json) verifies all 21
current files against OpenNT originals; each is byte-identical. Every same-stem
object appears in the original client or server makefile. The original rules
use `cl16` and `link16` to build `olecli.dll` and `olesvr.dll`; their makefile
hashes are recorded. Conditional branches and guest binaries were not built.

None of these exact paths is present in the selected x86 worker or WOW32
Ninja commands or recorded dependencies; graph-query positive controls are
documented in the DOS crosswalk. The [bounded file dispositions](ole-pending-host-dispositions.csv)
exclude these historical guest-source roles from that host-input scope, not
OLE or DDE functionality. Guest interaction, thunking, host providers and
object/task lifetime remain separate runtime obligations in S14–S19; other
configurations and guest behavior are not established. No guest or mirror
file was changed and the frozen mixed-scope ledger was not rewritten.
