# S10 nine-way remaining-work ownership

The owner requested nine disjoint audit parts. The execution environment currently permits four concurrent roles in total: the coordinator and three reviewers. Therefore these are **nine logical partitions, executed in parallel batches**, not a claim that nine reviewers run concurrently. The coordinator retains the independent integration/review role. The existing four physical source ledgers and their shard identifiers remain authoritative; this partition is an additional assignment layer and does not rewrite their ownership or change any disposition.

The [ownership table](nine-way-ownership.csv), [summary](nine-way-ownership-summary.json), and [generator](nine-way-partition.py) freeze the pending snapshot at commit `2bbd67135`: 1,079 MVDM file roles plus 42,954 pending external source-edition relevance records. For each kind, rows are sorted by case-folded path and unique key, then distributed into nine contiguous, near-equal rank ranges. Every `(kind, key)` occurs exactly once. External directory membership is still only a relevance obligation; it is neither blanket exclusion nor approval to import an NT4 subsystem. Later classifications or new evidence do not silently alter the frozen assignment. A later rebalance requires an explicit new version and handoff, with active work first reconciled.

| Logical partition | Reviewer | MVDM pending | External pending | MVDM path span in frozen assignment |
| --- | --- | ---: | ---: | --- |
| 1 | A | 120 | 4,773 | `bde/bde.def` – `dos/v86/doskrnl/dos/crit.asm` |
| 2 | B | 120 | 4,773 | `dos/v86/doskrnl/dos/dispatch.txt` – `ieuvddex/alpha/helpp.c` |
| 3 | C | 120 | 4,772 | `ieuvddex/data.c` – `softpc.new/base/cvidc/ccpudefs.inc` |
| 4 | A | 120 | 4,773 | `softpc.new/base/cvidc/chain4_c.h` – `softpc.new/base/inc/standard.h` |
| 5 | B | 120 | 4,773 | `softpc.new/base/inc/tcpip.h` – `vdmexts/precomp.h` |
| 6 | C | 120 | 4,772 | `vdmexts/reflect.c` – `wow16/drivers/keyboard/keyboard.asm` |
| 7 | A | 120 | 4,773 | `wow16/drivers/keyboard/keyboard.drv` – `wow16/rasapi16/RASAPI16.RES` |
| 8 | B | 120 | 4,773 | `wow16/rasapi16/raserror.h` – `wow16/winsock/winsock.def` |
| 9 | C | 119 | 4,772 | `wow16/winsock/WINSOCK.RES` – `xms.486/xms.h` |

Reviewer A is `/root/shard_one_resources`, B is `/root/shard_dos_continue`, and C is `/root/shard_drivers`. Each starts with its first partition and then proceeds through its other two. A partition owns only the keys assigned in the table, irrespective of its prior physical shard; reviewers write separate reports beneath `artifacts/mvdm-audit/nine-way/part-N/` and do not edit the shared ledgers. Relevant cross-partition dependencies may be read and cited, but only their assigned owner proposes that key's disposition. The coordinator checks source edition/hash, actual selected owner, exceptions and evidence, integrates all four physical ledgers sequentially, and preserves unrelated changes.

Within each part, group by actual functionality and dependency closure; do not mechanically declare an entire path range restored or excluded. Source presence, compilation and image identity are distinct from execution and lifecycle proof. Record concrete owner, evidence, code-retention estimate for mirror differences, and unresolved functionality against the existing S11–S19 owners. External rows need relevance/owner analysis before any restoration decision; their allocation does not make them MVDM mirror files. Prior uncommitted reports and probes are preserved as input evidence and reconciled by their newly assigned owners, with no duplicate work assumed complete.
