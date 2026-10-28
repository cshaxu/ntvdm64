# Part 5: NTIO keyboard/mouse resident source and host input ownership

This supplements part-5 BIOS identities and the embedded ROM reconciliation. Original `softpc.new/bios/makefile:57-59,77` lists both `spckbd.obj` and `spcmse.obj` in NTIO build inputs. `sysinit1.asm:170-178,1271-1272,1359-1369` references their resident boundaries and copies keyboard/mouse sections to permanent guest memory. This is the original guest recipe and source flow, not a statement that current NTIO binary was rebuilt from it; previous observer shows packaged/installed NTIO image identity only.

`spckbd.asm` describes guest IVT/BIOS keyboard setup and uses the original BOP mechanism. `spcmse.asm` is the guest resident DOS mouse driver, with BOP BA–BE and a private stack/reentrancy design; host `base/keymouse/mouse_io.c` owns the underlying INT33 state and callbacks while host keyboard controller/PIC owns queued scan bytes, port consumption and interrupt acknowledgment. Guest resident ranges, host device state, and front-end input are distinct; do not create another scan queue or replace guest behavior with host window input.

| Owned file | Bounded classification |
| --- | --- |
| `softpc.new/bios/spckbd.asm` | Original NTIO guest resident keyboard/BIOS and vector code; preserve. Exact runtime BIOS/keyboard cases depend on immutable image and configuration. |
| `softpc.new/bios/spcmse.asm` | Original NTIO guest resident mouse entry/stack/vector code; preserve. Actual host callback and state handling require separate lifecycle evidence. |
| `softpc.new/bios/sysinit1.asm` | Original NTIO initialization and resident-copy owner; startup rollback/termination not established by source reading. |
| `softpc.new/bios/biosbop.inc` | Original guest BOP/printer constants; `mschar.asm:25` includes it, and it is not an independent host provider. |

T448 S11 baseline records a real INT33 save/restore checksum failure and a specific candidate DIV-357 fix with a passing isolated control; that dated passage explicitly says the candidate was not published *at that time*. It cannot be used alone as current status. T460 S3 later records PIC-reset and early-EOI host controller fixtures (24 and 46 assertions) against its separately identified candidate; T473 S5 describes intended current ownership. These do not prove current packaged guest execution, every input ordering, mouse state case or Win3.x pointer behavior. Inspect current source and release evidence before labeling any defect fixed or unfixed.

S13 should run current coherent release with immutable guest to cover keyboard scan data-read/EOI both orders and reset, INT33 callback/reentry/save/restore and corrupted state rejection, device teardown and repeated boot; S11 owns protected-mode callback return and S19 binds exact NTIO/worker hashes. Use the actual guest entry, not only a host fixture or screenshots. No guest, product, live device or shared ledger was modified and no test ran here.

Replay: `rg -n "SpcKbdBeg|SpcKbdEnd|SpcMseBeg|SpcMseEnd" src/mvdm/softpc.new/bios`, inspect makefile, `spckbd.asm`, `spcmse.asm`, and the T448/T460 records; rerun part-5 BIOS observer.
