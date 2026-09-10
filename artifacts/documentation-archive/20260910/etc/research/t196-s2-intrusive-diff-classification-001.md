# M0 T196 S2 Bochs 2.6 Intrusive-Diff Classification

## Method And Correction

The S1 command was repeated against the same pinned local Bochs 2.6 tree.
Counting the Git zero-context `@@` records gives 31 changed paths and **60**
hunks. The earlier reported total of 54 omitted the one hunk represented by
each of six project-added files. S1 has been corrected; this table is the
complete 60-hunk crosswalk.

`Keep` is a proposed repair disposition, not approval of the current source.
It means S4 must prove the cited exception remains selector-blind and minimal.
`Remove` means S3 must delete the historical diagnostic and reconcile its
register row. No row authorizes BOP, DOS, OpenNT or SoftPC semantics in Bochs.

## Complete Hunk Disposition

| Hunk | Path and zero-context location | Proposed disposition | Register relationship / S group |
| --- | --- | --- | --- |
| H01 | `Makefile.in:93` bcrypt link addition | Keep only if current MSVC closure proves need | Unregistered build compatibility; S4. |
| H02 | `Makefile.in:179` observation-fixture target | Remove | Historical fixture target; S3. |
| H03 | `bx_ntvdm_minimal_machine.cc` added file | Keep/minimize | BX-MACH-026; S4. |
| H04 | `bx_ntvdm_minimal_machine.h` added file | Keep/minimize | BX-MACH-026; S4. |
| H05 | `bx_ntvdm_minimal_sim.cc` added file | Keep/minimize | BX-MACH-023; S4. |
| H06 | `bx_ntvdm_minimal_sim.h` added file | Keep/minimize | BX-MACH-023; S4. |
| H07 | `config.cc:3261` null-pointer lexical form | Keep only with current compiler proof | BX-BUILD-002; S4. |
| H08 | `cpu/bx_ntvdm_exception_intercept.h` added file | Keep/minimize | BX-UD-001, ABI-004/005/006/015/018/019/021; S4. |
| H09 | `cpu/cpu.cc:27` prefetch diagnostic macro | Remove | BX-TRACE-056; S3. |
| H10 | `cpu/cpu.cc:584` prefetch diagnostic log | Remove | BX-TRACE-056; S3. |
| H11 | `cpu/cpu.h:893` typed real-mode state/entry methods | Keep/minimize | BX-EXEC-016 and opaque startup mechanics; S4. |
| H12 | `cpu/ctrl_xfer16.cc:568` IRET-TF diagnostic capture | Remove | BX-TRACE-050; S3. |
| H13 | `cpu/ctrl_xfer16.cc:577` IRET-TF diagnostic log | Remove | BX-TRACE-050; S3. |
| H14 | `cpu/ctrl_xfer32.cc:529` far-jump diagnostic | Remove | BX-TRACE-051; S3. |
| H15 | `cpu/data_xfer16.cc:63` MOV-SP diagnostic capture | Remove | BX-TRACE-048; S3. |
| H16 | `cpu/data_xfer16.cc:68` MOV-SP diagnostic log | Remove | BX-TRACE-048; S3. |
| H17 | `cpu/data_xfer16.cc:135` MOV-SS diagnostic capture | Remove | BX-TRACE-048; S3. |
| H18 | `cpu/data_xfer16.cc:141` MOV-SS diagnostic log | Remove | BX-TRACE-048; S3. |
| H19 | `cpu/event.cc:28` IRQ13 diagnostic macro | Remove | BX-TRACE-037; S3. |
| H20 | `cpu/event.cc:135` IRQ13 pre-transfer log | Remove | BX-TRACE-037; S3. |
| H21 | `cpu/event.cc:142` IRQ13 post-transfer log | Remove | BX-TRACE-037; S3. |
| H22 | `cpu/exception.cc:28` generic seam declarations/helpers | Keep/minimize | Generic UD/result/mechanics exceptions; S4. |
| H23 | `cpu/exception.cc:1289` real-mode INT diagnostic capture | Remove | BX-TRACE-049; S3. |
| H24 | `cpu/exception.cc:1312` real-mode INT diagnostic log | Remove | BX-TRACE-049; S3. |
| H25 | `cpu/exception.cc:1465` unmatched-UD diagnostic | Remove | BX-TRACE-039; S3. |
| H26 | `cpu/exception.cc:1473` generic #UD/result/stop/mechanics path | Keep/minimize | BX-UD-001, ABI-004/005/006/008/015/018/019/021/036/037/053/057; S4. |
| H27 | `cpu/init.cc:56` real-mode state application | Keep/minimize | BX-EXEC-016; S4. |
| H28 | `fpu/fpu.cc:30` x87 diagnostic macro | Remove | BX-TRACE-035/038; S3. |
| H29 | `fpu/fpu.cc:75` x87 snapshot log | Remove | BX-TRACE-035/038; S3. |
| H30 | `gui/paramtree.cc:296` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H31 | `gui/paramtree.cc:315` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H32 | `gui/siminterface.cc:1290` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H33 | `gui/siminterface.cc:1302` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H34 | `iodev/Makefile:59` minimal port object membership | Keep/minimize | BX-BUILD-033; S4. |
| H35 | `iodev/Makefile.in:59` minimal port object membership | Keep/minimize | BX-BUILD-033; S4. |
| H36 | `iodev/devices.cc:272` extracted empty-port initialization | Keep/minimize | BX-IO-025/029; S4. |
| H37 | `iodev/devices.cc:422` extracted default read/write handlers | Keep/minimize | BX-IO-025/029; S4. |
| H38 | `iodev/devices.cc:715` extracted default-handler registration | Keep/minimize | BX-IO-030; S4. |
| H39 | `iodev/harddrv.cc:2562` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H40 | `iodev/hdimage/hdimage.cc:223` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H41 | `iodev/iodev.h:32` machine forward declaration | Keep/minimize | BX-MACH-026; S4. |
| H42 | `iodev/iodev.h:544` friendship and empty-port declarations | Keep/minimize | BX-MACH-026/BX-IO-025; S4. |
| H43 | `iodev/minimal_port_space.cc` added file | Keep/minimize | BX-IO-025/029/030; S4. |
| H44 | `iodev/pit.cc:143` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H45 | `iodev/pit.cc:202` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H46 | `iodev/pit.cc:339` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H47 | `iodev/usb/scsi_device.cc:685` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H48 | `iodev/usb/scsi_device.cc:694` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H49 | `iodev/usb/scsi_device.cc:702` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H50 | `iodev/virt_timer.cc:486` FMT lexical fix | Keep | BX-ABI-028; S4. |
| H51 | `main.cc:30` opt-in adapter include | Keep/minimize | BX-EXEC-014/032/034/042; S4. |
| H52 | `main.cc:79` opaque execution-plan consumer | Keep/minimize | BX-EXEC-014/016/032/034/042; S4. |
| H53 | `main.cc:138` logging macro movement | Keep with H52 only | Incidental to execution-plan block; S4. |
| H54 | `main.cc:1036` pre-CPU-loop opt-in calls | Keep/minimize | BX-EXEC-014/032/034/042; S4. |
| H55 | `memory/memory.h:38` machine forward declaration | Keep/minimize | BX-MACH-026; S4. |
| H56 | `memory/memory.h:84` machine friendship | Keep/minimize | BX-MACH-026; S4. |
| H57 | `memory/memory.h:110` no-SIM initializer declaration | Keep/minimize | BX-MEM-024; S4. |
| H58 | `memory/memory.h:129` checked RAM method declarations | Keep/minimize | BX-MEM-007/010/020; S4. |
| H59 | `memory/misc_mem.cc:155` no-SIM initializer | Keep/minimize | BX-MEM-024; S4. |
| H60 | `memory/misc_mem.cc:857` checked RAM primitives | Keep/minimize | BX-MEM-007/010/020; S4. |

## Repair Order

S3 removes H02, H09--H10, H12--H21, H23--H25, and H28--H29: 17 stale,
default-off Bochs diagnostics plus the obsolete observation-fixture target.
Their source
questions are closed historical evidence; future diagnostics require a new
exception and admission.

S4 then handles the remaining classified mechanical groups as four coupled
areas. The S3 removal later proves that classification rows and Git `@@`
hunks are not always one-to-one: an removed declaration can share a hunk with
retained generic mechanics. Post-repair comparison counts, rather than this
arithmetic, are authoritative.

1. `H03--H06`, `H34--H38`, `H41--H43`, and `H55--H60`: minimal reset/RAM/
   default-port machine embedding.
2. `H08`, `H11`, `H22`, `H26--H27`, `H51--H54`: opaque typed exception,
   result, stop and startup mechanics; verify selector-blind source scans.
3. `H01` and `H07`: isolated build compatibility, each retained only with a
   current minimal MSVC closure proof.
4. `H30--H33`, `H39--H40`, `H44--H50`: thirteen lexical FMT compatibility
   edits, retained only as exactly scoped compiler lexical changes.

S5 repeats the full 60-hunk comparison and reconciles every register row with
the post-repair source/build surface. S6 builds the admitted closure and runs
the focused boundary regressions. Neither group may begin BOP runtime work.
