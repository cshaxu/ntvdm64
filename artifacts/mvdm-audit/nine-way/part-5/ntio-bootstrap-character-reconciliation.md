# Part 5: NTIO character, bootstrap and message source roles

All thirteen owned paths below have individual original/active identity records in `bios-resource-family-observations.json`. `softpc.new/bios/msbio.lnk` explicitly links msbio1, mschar, msinit, sysinit2, sysimes and msend into NTIO; `bios/makefile:29-78` supplies the named source and include dependencies. These are sufficient to classify the listed *historical source/build roles*, not to infer source-to-binary reproduction or completion of all device operations. Existing ZIP/installed NTIO hash equality identifies the guest only.

Original `msbio1.asm` defines guest BIOS data and device entry/exit; `msinit.asm` owns bootstrap setup and transfer to sysinit. `mschar.asm` dispatches console, auxiliary, printer and clock device calls: its dispatch-table contract returns carry/status, and `prn_writ` waits/retries via guest printer routines and returns errors. Original `base/bios/bios.c:164` binds BOP 17 to the host printer owner. Those layers must remain distinct; neither the build link nor a host printer fixture proves real guest device output. `msend.asm` defines segment end and DOS load boundary; `sysimes.asm` includes generated msbio.cl3, and msbio.skl holds source messages, including CONFIG.SYS errors.

| Owned path | Evidence-supported file-role disposition / remaining boundary |
| --- | --- |
| `softpc.new/bios/msbio1.asm` | Original NTIO BIOS data and device entry source; selected in historical link. Runtime initialization and binary lineage still need proof. |
| `softpc.new/bios/mschar.asm` | Original NTIO character/clock/printer dispatch source; selected historically, device-specific runtime remains separate. |
| `softpc.new/bios/msinit.asm` | Original BIOS startup source; selected historically, failure/reentry unverified. |
| `softpc.new/bios/msend.asm` | Original segment-end and DOS load boundary assembler input; link-selected, not a standalone service. |
| `softpc.new/bios/sysinit2.asm` | Original second initialization object in historical link; exact startup branches need runtime verification. |
| `softpc.new/bios/sysimes.asm` | Original guest initialization-message object, using generated msbio.cl3. |
| `softpc.new/bios/msbio.lnk` | Historical explicit NTIO linker response; build metadata, do not execute. |
| `softpc.new/bios/msbio.skl` | Original NTIO message skeleton; source data, not an independent runtime provider. |
| `softpc.new/bios/biostruc.inc` | Original BIOS/ROM call packet declarations used by historical recipe; active field/layout depends on guest profile. |
| `softpc.new/bios/devmark.inc` | Original DOS memory/device marker declarations used by recipe, including SoftPC resident-marker type; not a host allocator. |
| `softpc.new/bios/msequ.inc` | Original BIOS equates included by multiple linked sources; not standalone runtime code. |
| `softpc.new/bios/msgroup.inc` | Original BIOS data/segment declarations included by selected historical sources. |
| `softpc.new/bios/biosseg.inc` | Original BIOS segment structure included directly by `msbio1.asm` and `msinit.asm`; not established solely by missing literal makefile entry. |

S12 owns actual immutable guest startup, character/printer request returns and DOS load/cleanup including no device; S13 owns host printer/device behavior and failure/teardown; S19 binds guest/worker hashes. Build input role may be classified now, but all behavioral evidence remains separately pending. No device, guest, product, shared ledger, or runtime test was touched.

Replay: inspect the exact `msbio.lnk`, makefile dependencies, `mschar.asm` dispatch and error path, `sysimes.asm` and each include; rerun part-5 BIOS observer with bytecode disabled.
