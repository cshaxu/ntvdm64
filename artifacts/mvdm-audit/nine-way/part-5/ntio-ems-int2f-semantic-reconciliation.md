# Part 5: NTIO INT2F and EMS resident-stub semantics

The three owned `softpc.new/bios` files here match both frozen sources in the part-5 BIOS identity report. Original `msbio.lnk:1-2` and makefile:46–77 select `msint13`, `spcemm` and `sysconf` objects for the historical NTIO recipe; identical packaged/installed NTIO bytes do not prove the source recipe produced them.

`msint13.asm:1-13,60-72` explicitly says its INT 13 support was removed and the file implements INT 2F handling; its 13h/08h multiplex branches return without handling external block device / INT13 vector replacement. Other branches handle Win386 startup/exit and its idle call through BOP 5A. Thus classifying this source as a hard-disk INT13 provider from its filename would be incorrect. Preserve original return/chain conditions; source comments are not proof of live guest behavior for all multiplex requests.

`spcemm.asm` declares an `EMMXXXX0` resident device header and explicitly says it is an identification stub, only loaded when NTVDM EMS is available. `sysinit1.asm:1300-1335` tests availability then copies and links its resident range. EMS allocation/mapping implementation belongs to the separately selected EMS provider, not this header; a device-name presence does not prove LIM page mapping or release. `sysconf.asm` is original NTIO system configuration input; exact queried path and output are not established here.

| Owned path | Bounded classification |
| --- | --- |
| `softpc.new/bios/msint13.asm` | Original NTIO INT2F/Win386 multiplex handler; name is not evidence of an INT13 disk implementation. |
| `softpc.new/bios/spcemm.asm` | Original EMS identification device stub, gated by availability and resident-copy setup; not the EMS implementation. |
| `softpc.new/bios/sysconf.asm` | Original system-configuration source included by the historical recipe; exact current output and runtime profile remain pending. |

No live runtime test was performed here. T406's default MEM case explicitly reported EMS configuration size zero (`m0-t406-s1-physical-mapping-design.md:491`); it cannot prove active LIM allocation. S12 should test actual immutable guest INT2F supported/unsupported return and Win386 idle/start/exit, EMS absent/present identification, allocation/map/unmap and process cleanup with an enabled controlled profile. S13 owns device resources; S19 binds NTIO and host worker identities. Do not manufacture a successful EMS device or alter guest BIOS to pass a presence test.

Replay `rg -n "msint13|spcemm|sysconf" src/mvdm/softpc.new/bios/makefile src/mvdm/softpc.new/bios/msbio.lnk`, read msint13.asm/spcemm.asm and `sysinit1.asm:1300-1335`, then replay part-5 BIOS observer. No guest, product, or shared-ledger change.
