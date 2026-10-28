# Part 8: Win16 guest families — identities, source leads and delivered-image samples

The 112 owned paths outside the separate eight-file EDIT comparison are grouped below. The companion JSON records each frozen source-edition/current SHA-256, family-local literal filename references with source line/snippet (up to 20; full match count retained), and ZIP name/content observations for EXE/DLL/DRV/MOD files. Basename references include comments, conditionals and collisions: they are leads rather than resolved, selected calls. Exact image equality proves content identity only, not recipe provenance, runtime loading or user-visible functionality. Current-only files are flagged individually and may not be called original.

## rasapi16

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/rasapi16/raserror.h` | both editions; 3 refs, e.g. wow16/rasapi16/makefile:71 | Pending exact selected profile, current owner and runtime behavior. |

## regedit

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/regedit/debug/REGEDIT.RES` | current-only; 8 refs, e.g. wow16/regedit/makefile:65 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/regedit/debug/REGEDT16.EXE` | current-only; 0 refs, e.g. no family-local literal hit | Current-only artifact; exact provenance and consumer pending. |
| `wow16/regedit/regedit.ico` | both editions; 1 refs, e.g. wow16/regedit/messages/usa/regedit.rc:7 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/regedit/sdkreged.def` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/regedit/sdkreged.h` | both editions; 9 refs, e.g. wow16/regedit/makefile:196 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/regedit/sdkreged.ico` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |

## shell

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/shell/library/shell.def` | both editions; 1 refs, e.g. wow16/shell/makefile:98 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/shell/shell.asm` | both editions; 1 refs, e.g. wow16/shell/makefile:85 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/shell/SHELL.DLL` | both editions; 6 refs, e.g. wow16/shell/makefile:70 | Exact ZIP image match: WINNT/SYSTEM32/SHELL.DLL; candidate partial image identity only, loading/behavior pending. |
| `wow16/shell/shell.rc` | both editions; 1 refs, e.g. wow16/shell/makefile:102 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/shell/SHELL.RES` | both editions; 3 refs, e.g. wow16/shell/makefile:98 | Pending exact selected profile, current owner and runtime behavior. |

## sherlock

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/sherlock/DRWATSON.EXE` | both editions; 7 refs, e.g. wow16/sherlock/makefile:42 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/sherlock/messages/usa/DRWATSON.RES` | both editions; 3 refs, e.g. wow16/sherlock/makefile:58 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/sherlock/sherlock.doc` | both editions; 0 refs, e.g. no family-local literal hit | Historical documentation role; features described remain separate and pending. |
| `wow16/sherlock/sherlock.ico` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/sherlock/sherlock.txt` | both editions; 0 refs, e.g. no family-local literal hit | Historical documentation role; features described remain separate and pending. |
| `wow16/sherlock/watson.ico` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |

## system

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/system/envect.asm` | both editions; 1 refs, e.g. wow16/system/hpsystem:19 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/equate.inc` | both editions; 1 refs, e.g. wow16/system/envect.asm:7 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/hpsystem` | both editions; 6 refs, e.g. wow16/system/envect.asm:68 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/hpsystem.def` | both editions; 2 refs, e.g. wow16/system/hpsystem:21 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/romstuff.inc` | both editions; 1 refs, e.g. wow16/system/system.inc:19 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/romsys` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/romsys.def` | both editions; 1 refs, e.g. wow16/system/romsys:18 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/system` | both editions; 31 refs, e.g. wow16/system/equate.inc:155 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/system.drv` | both editions; 13 refs, e.g. wow16/system/makefile:79 | Exact ZIP image match: WINNT/SYSTEM32/SYSTEM.DRV; candidate partial image identity only, loading/behavior pending. |
| `wow16/system/system.lrf` | current-only; 8 refs, e.g. wow16/system/makefile:93 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/system/system.rcv` | both editions; 1 refs, e.g. wow16/system/system.rc:5 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/SYSTEM.RES` | both editions; 3 refs, e.g. wow16/system/makefile:106 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/vecsys.inc` | both editions; 2 refs, e.g. wow16/system/envect.asm:9 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/system/windefs.inc` | both editions; 1 refs, e.g. wow16/system/system.inc:16 | Pending exact selected profile, current owner and runtime behavior. |

## test

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/test/hello/hello.c` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/test/hello/hello.h` | both editions; 3 refs, e.g. wow16/test/hello/hello.c:17 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/test/shell/pmdos.asm` | both editions; 1 refs, e.g. wow16/test/shell/wowexec.h:84 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/test/shell/wowexec.c` | both editions; 1 refs, e.g. wow16/test/shell/makefile:92 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/test/shell/wowexec.ico` | both editions; 1 refs, e.g. wow16/test/shell/wowexec.rc:18 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/test/shell/WOWEXEC.RES` | both editions; 2 refs, e.g. wow16/test/shell/makefile:122 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/test/shell/wowexfax.c` | both editions; 1 refs, e.g. wow16/test/shell/makefile:95 | Pending exact selected profile, current owner and runtime behavior. |

## timer

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/timer/api.asm` | both editions; 1 refs, e.g. wow16/timer/makefile:128 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/libinit.asm` | both editions; 2 refs, e.g. wow16/timer/makefile:130 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/local.asm` | both editions; 5 refs, e.g. wow16/timer/libinit.asm:300 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/math.asm` | both editions; 1 refs, e.g. wow16/timer/timer.asm:776 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/njumps.mac` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/startend.asm` | both editions; 1 refs, e.g. wow16/timer/makefile:134 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/sysinfo.inc` | both editions; 1 refs, e.g. wow16/timer/libinit.asm:15 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/timer.asm` | both editions; 3 refs, e.g. wow16/timer/local.asm:28 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/timer.inc` | both editions; 11 refs, e.g. wow16/timer/api.asm:28 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/timer.rcv` | both editions; 3 refs, e.g. wow16/timer/timer.rc:2 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/timer/TIMER.RES` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |

## toolhelp

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/toolhelp/class1.c` | both editions; 2 refs, e.g. wow16/toolhelp/makefile:115 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/class2.asm` | both editions; 2 refs, e.g. wow16/toolhelp/makefile:115 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/dllentry.asm` | both editions; 1 refs, e.g. wow16/toolhelp/makefile:120 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/module.c` | both editions; 1 refs, e.g. wow16/toolhelp/makefile:129 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/notify2.asm` | both editions; 1 refs, e.g. wow16/toolhelp/makefile:131 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/string.h` | both editions; 14 refs, e.g. wow16/toolhelp/global.c:10 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/task1.c` | both editions; 1 refs, e.g. wow16/toolhelp/makefile:135 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/ththunks.asm` | both editions; 2 refs, e.g. wow16/toolhelp/makefile:116 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/timer.asm` | both editions; 1 refs, e.g. wow16/toolhelp/makefile:139 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/TOOLHELP.DLL` | both editions; 20 refs, e.g. wow16/toolhelp/dllentry.asm:6 | Exact ZIP image match: WINNT/SYSTEM32/TOOLHELP.DLL; candidate partial image identity only, loading/behavior pending. |
| `wow16/toolhelp/toolhelp.h` | both editions; 12 refs, e.g. wow16/toolhelp/makefile:118 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/TOOLHELP.RES` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/toolhelp/usergdi2.asm` | both editions; 1 refs, e.g. wow16/toolhelp/makefile:142 | Pending exact selected profile, current owner and runtime behavior. |

## user

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/user/comdev.c` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/comdevi.h` | both editions; 1 refs, e.g. wow16/user/comdev.c:33 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/iuser.inc` | both editions; 1 refs, e.g. wow16/user/user.inc:220 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/k/daytona/user.res` | current-only; 5 refs, e.g. wow16/user/makefile:126 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/user/k/daytona/wowasmk.asm` | current-only; 1 refs, e.g. wow16/user/makefile:231 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/user/k/daytona/wowk.asm` | current-only; 2 refs, e.g. wow16/user/makefile:230 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/user/k/daytona/wowkr.asm` | current-only; 2 refs, e.g. wow16/user/makefile:246 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/user/k/daytona/wowkr1.asm` | current-only; 0 refs, e.g. no family-local literal hit | Current-only artifact; exact provenance and consumer pending. |
| `wow16/user/k/daytona/wowrtl.asm` | current-only; 4 refs, e.g. wow16/user/makefile:241 | Current-only artifact; exact provenance and consumer pending. |
| `wow16/user/listing.inc` | both editions; 5 refs, e.g. wow16/user/k/daytona/wowasmk.asm:3 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/msglayer.inc` | both editions; 2 refs, e.g. wow16/user/makefile:169 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/msutil.c` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/readme.txt` | both editions; 0 refs, e.g. no family-local literal hit | Historical documentation role; features described remain separate and pending. |
| `wow16/user/usermenu.inc` | both editions; 1 refs, e.g. wow16/user/layer.asm:9 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/winutil.asm` | both editions; 2 refs, e.g. wow16/user/user.inc:541 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/wmsyserr.c` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/user/wowasm.cmd` | both editions; 0 refs, e.g. no family-local literal hit | Historical build script/transform candidate; do not execute; profile and produced-file lineage pending. |
| `wow16/user/wowasm.sed` | both editions; 3 refs, e.g. wow16/user/wowasm.cmd:3 | Historical build script/transform candidate; do not execute; profile and produced-file lineage pending. |
| `wow16/user/wowasmc.cmd` | both editions; 0 refs, e.g. no family-local literal hit | Historical build script/transform candidate; do not execute; profile and produced-file lineage pending. |
| `wow16/user/wowasmk.cmd` | both editions; 3 refs, e.g. wow16/user/makefile:232 | Historical build script/transform candidate; do not execute; profile and produced-file lineage pending. |
| `wow16/user/wsubcls.c` | both editions; 7 refs, e.g. wow16/user/user.asm:349 | Pending exact selected profile, current owner and runtime behavior. |

## wfwnet

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/wfwnet/ints.asm` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/wfwnet/lfn.c` | both editions; 1 refs, e.g. wow16/wfwnet/makefile:117 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/wfwnet/misc.c` | both editions; 1 refs, e.g. wow16/wfwnet/makefile:109 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/wfwnet/print.c` | both editions; 1 refs, e.g. wow16/wfwnet/makefile:105 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/wfwnet/to32.c` | both editions; 1 refs, e.g. wow16/wfwnet/makefile:113 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/wfwnet/wfwnet.def` | both editions; 1 refs, e.g. wow16/wfwnet/makefile:136 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/wfwnet/wfwnet.rc` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |

## win87em

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/win87em/apisim.asm` | both editions; 1 refs, e.g. wow16/win87em/makefile:75 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/emds.asm` | both editions; 4 refs, e.g. wow16/win87em/emdos.asm:685 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/emoem.src` | both editions; 4 refs, e.g. wow16/win87em/makefile:25 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/emoemqb.asm` | both editions; 1 refs, e.g. wow16/win87em/makefile:107 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/emoemwin.std` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/emoemwin.t1` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/emoemwin.t2` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/key` | both editions; 2 refs, e.g. wow16/win87em/emdos.asm:353 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/switch.an` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/switch.n` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/switch.src` | both editions; 1 refs, e.g. wow16/win87em/makefile:720 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/wfpsig.asm` | both editions; 2 refs, e.g. wow16/win87em/makefile:676 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/win87em.h` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/win87em/win87hdr.txt` | both editions; 0 refs, e.g. no family-local literal hit | Historical documentation role; features described remain separate and pending. |
| `wow16/win87em/win87std.def` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |

## winoldap

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/winoldap/winoldap.c` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/winoldap/winoldap.def` | both editions; 10 refs, e.g. wow16/winoldap/makefile:95 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/winoldap/WINOLDAP.MOD` | both editions; 7 refs, e.g. wow16/winoldap/makefile:75 | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/winoldap/WINOLDAP.RES` | both editions; 3 refs, e.g. wow16/winoldap/makefile:107 | Pending exact selected profile, current owner and runtime behavior. |

## winsock

| Owned key | Original identity and captured lead | Bounded disposition |
| --- | --- | --- |
| `wow16/winsock/winsock.asm` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |
| `wow16/winsock/winsock.def` | both editions; 0 refs, e.g. no family-local literal hit | Pending exact selected profile, current owner and runtime behavior. |

Prior shard-2 family reports contain stronger contextual evidence for SYSTEM, TIMER, TOOLHELP, SHELL, WF WNET, WINOLDAP and guest-loader search; integrate only after matching each owned key and frozen identity. In particular `wow16/user` old guest C sources must not be conflated with the separate currently selected NT USER sources, and test/shell WOWEXEC inputs must not be equated with live runtime behavior without binary selection evidence. All timer, task, clipboard, shell, DDE, OLE, memory, callback, message, destruction and failure-handling contracts remain independently scoped.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-8/guest-family-inventory-observer.py` and compare JSON. No guest, product, build tree, external source, or shared ledger was modified.
