# S11 CPU30 hardware monitor subset removal

Owner-approved mirror exception `MVDM-HOST-DIV-493`: remove retired CPU30/kernel-VDM monitor source branches from shared mirror files. Keep CPU40 software execution, its V86 emulation, existing DPMI real-mode reflection, selected software EOI and protected-mode handling. The old standalone `v86/monitor` and `v86/scaffold` directories are absent; DOS guest files are not the monitor and were not modified.

CPU30-only branches were resolved with CPU_30_STYLE absent, preserving other preprocessor conditions. The non-CPU40 block in `dpmi32/i386/dpmi386.c` was removed while retaining the selected debug-register provider. In the host files built without MONITOR, the kernel-only EOI, PIC IRET, COM IRET, hardware-monitor heartbeat/thread bookkeeping, VDD printer hooking, startup BIOS fixups and reset branches and `NtVdmControl` initialization and INT 21 registration were removed; the current directory-query adapter and unrelated graphics code remain. Removed legacy recipes no longer define CPU30.

Per-file working-tree line changes relative to the previous published commit (not a recovery percentage):

| Mirror source | Added / deleted |
| --- | ---: |
| `src/mvdm/dpmi32/dpmiint.c` | +0 / -18 |
| `src/mvdm/dpmi32/i386/dpmi386.c` | +0 / -313 |
| `src/mvdm/softpc.new/base/bios/bios.c` | +2 / -2 |
| `src/mvdm/softpc.new/base/bios/reset.c` | +4 / -7 |
| `src/mvdm/softpc.new/base/bios/tape_io.c` | +0 / -24 |
| `src/mvdm/softpc.new/base/ccpu386/ccpudefs.inc` | +1 / -1 |
| `src/mvdm/softpc.new/base/cvidc/ccpudefs.inc` | +1 / -1 |
| `src/mvdm/softpc.new/base/debug/dasm.c` | +0 / -23 |
| `src/mvdm/softpc.new/base/debug/yoda.c` | +0 / -17 |
| `src/mvdm/softpc.new/base/inc/cpu.h` | +0 / -930 |
| `src/mvdm/softpc.new/base/inc/ica.h` | +1 / -1 |
| `src/mvdm/softpc.new/base/inc/sas.h` | +0 / -2 |
| `src/mvdm/softpc.new/base/inc/xt.h` | +0 / -23 |
| `src/mvdm/softpc.new/base/keymouse/keyba.c` | +0 / -4 |
| `src/mvdm/softpc.new/base/keymouse/mouse_io.c` | +1 / -1 |
| `src/mvdm/softpc.new/base/support/terminat.c` | +0 / -3 |
| `src/mvdm/softpc.new/base/support/xt.c` | +4 / -186 |
| `src/mvdm/softpc.new/base/system/ica.c` | +0 / -48 |
| `src/mvdm/softpc.new/base/system/qevnt.c` | +0 / -23 |
| `src/mvdm/softpc.new/host/inc/host_cpu.h` | +0 / -97 |
| `src/mvdm/softpc.new/host/inc/nt_eoi.h` | +0 / -7 |
| `src/mvdm/softpc.new/host/inc/nt_fulsc.h` | +0 / -1 |
| `src/mvdm/softpc.new/host/src/config.c` | +0 / -48 |
| `src/mvdm/softpc.new/host/src/nt_bop.c` | +0 / -37 |
| `src/mvdm/softpc.new/host/src/nt_com.c` | +0 / -6 |
| `src/mvdm/softpc.new/host/src/nt_cprgs.c` | +3 / -3 |
| `src/mvdm/softpc.new/host/src/nt_eoi.c` | +0 / -108 |
| `src/mvdm/softpc.new/host/src/nt_fulsc.c` | +1 / -13 |
| `src/mvdm/softpc.new/host/src/nt_reset.c` | +0 / -21 |
| `src/mvdm/softpc.new/host/src/nt_sec.c` | +0 / -43 |
| `src/mvdm/softpc.new/host/src/nt_thred.c` | +0 / -6 |
| `src/mvdm/softpc.new/host/src/nt_timer.c` | +0 / -73 |
| `src/mvdm/softpc.new/host/src/nt_vdd.c` | +0 / -57 |

Runtime validation is recorded separately; this inventory alone does not prove functional equivalence.

`MONITOR` and `X86GFX` also select non-monitor software video and other host services, so they were not globally resolved. The removed branches were checked by their concrete providers and compilation flags. Guest media and software V86 instructions remain intact.

One Full Window-17 nested-MEM run timed out after MEM output and the first EXIT input. A selected identical nested-MEM rerun passed once on both the previously published package and this candidate; this does not establish the initial failure root cause. The failure and both controls are retained in [s11-cpu30-subset-evidence](s11-cpu30-subset-evidence/).

The x86 monitor immediate-dispatch quick-event branch was removed while preserving the selected software quick-event scheduler. A fresh relink produced identical `.text` bytes to the immediately preceding candidate; other PE sections changed, so the fresh binary receives its own complete verification.

The final package passed 134 CCPU cases, 24 PIC and 46 keyboard assertions, the Full product gates, all four Win3.1 startup/exit modes with 86 unchanged guest binaries, and Winmine, SOL, WRITE and SOL Options probes. It was published as a complete ten-component package to both destinations with 20 matching hashes. Physical Win386 mouse movement remains unverified: the prior hook probe failed with error 126.
