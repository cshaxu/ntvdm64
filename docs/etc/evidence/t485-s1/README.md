# T485 S1 existing-component migration ledger

Local baseline 449c938a4cc190cc88df6200e381274e1a2e8f4f. Local XP candidate catalogue O:/repos.external/winxpscodes at a2f6d7c93aa4f11efc51147566d5961c59a8ef89, Source/XPSP1/NT. Compared Git tree metadata only. Exact XP build, SKU, provenance, integration rights and corresponding immutable media still require qualification.

| Component | Leaf batches | Tracked files | Identical blob | Different blob | Unmatched path |
| --- | ---: | ---: | ---: | ---: | ---: |
| mvdm-host | 36 | 1018 | 11 | 913 | 94 |
| non-mvdm | 26 | 228 | 42 | 141 | 45 |
| mvdm-guest | 97 | 2740 | 110 | 1524 | 1106 |

The [file ledger](files.csv) records every path and its batch exactly once, including historical tools and intermediates. Enumerate the pinned Git tree with git ls-tree -r. Walk tracked directories depth-first in Windows DIR /AD /B /O:N sibling order. Assign files directly in a non-leaf directory to its first descendant batch as ancestor-direct. Such files keep their original paths and may serve several siblings. For example, common dpmi32 sources remain common when booked with i386.

Match current MVDM relative paths below base/mvdm and non-MVDM paths below the XP NT root, case-insensitively. Blob equality is byte identity in Git. Different blobs include possible formatting and local adaptations. Unmatched paths require owner/rename or retirement research. Path matches establish neither production selection nor ABI compatibility.

XP-only components belong to a later T. Admit a minimum dependency into the requesting batch only with a concrete caller, required interface and evidence that this batch cannot work without it. Record its exact XP identity, provenance and validation. For a companion already in the ledger, record the lead batch and its member files at both entries; count each file once. Guest replacement requires separately authorized immutable media.

| Batch | Leaf | Leaf files | Ancestor files | XP paths found | Unmatched |
| --- | --- | ---: | ---: | ---: | ---: |
| H001 | src/mvdm-host/dbg | 3 | 6 | 2 | 7 |
| H002 | src/mvdm-host/dos/command | 15 | 1 | 14 | 2 |
| H003 | src/mvdm-host/dos/dem | 22 | 0 | 20 | 2 |
| H004 | src/mvdm-host/dos/test | 1 | 0 | 1 | 0 |
| H005 | src/mvdm-host/dpmi32/i386 | 1 | 22 | 19 | 4 |
| H006 | src/mvdm-host/fax | 3 | 0 | 3 | 0 |
| H007 | src/mvdm-host/inc | 89 | 0 | 53 | 36 |
| H008 | src/mvdm-host/oemuni | 5 | 0 | 3 | 2 |
| H009 | src/mvdm-host/softpc.new/base/bios | 16 | 2 | 16 | 2 |
| H010 | src/mvdm-host/softpc.new/base/ccpu386 | 270 | 0 | 270 | 0 |
| H011 | src/mvdm-host/softpc.new/base/comms | 7 | 0 | 5 | 2 |
| H012 | src/mvdm-host/softpc.new/base/cpu/src/evid | 1 | 0 | 1 | 0 |
| H013 | src/mvdm-host/softpc.new/base/cvidc | 55 | 0 | 55 | 0 |
| H014 | src/mvdm-host/softpc.new/base/debug | 11 | 0 | 9 | 2 |
| H015 | src/mvdm-host/softpc.new/base/disks | 12 | 0 | 10 | 2 |
| H016 | src/mvdm-host/softpc.new/base/dos | 3 | 0 | 1 | 2 |
| H017 | src/mvdm-host/softpc.new/base/inc | 80 | 0 | 80 | 0 |
| H018 | src/mvdm-host/softpc.new/base/keymouse | 7 | 0 | 5 | 2 |
| H019 | src/mvdm-host/softpc.new/base/support | 7 | 0 | 5 | 2 |
| H020 | src/mvdm-host/softpc.new/base/system | 14 | 0 | 11 | 3 |
| H021 | src/mvdm-host/softpc.new/base/unix | 2 | 0 | 2 | 0 |
| H022 | src/mvdm-host/softpc.new/base/video | 21 | 0 | 19 | 2 |
| H023 | src/mvdm-host/softpc.new/host/inc | 40 | 1 | 41 | 0 |
| H024 | src/mvdm-host/softpc.new/host/link | 7 | 0 | 7 | 0 |
| H025 | src/mvdm-host/softpc.new/host/src | 48 | 0 | 46 | 2 |
| H026 | src/mvdm-host/softpc.new/obj.vdm | 8 | 0 | 6 | 2 |
| H027 | src/mvdm-host/suballoc | 6 | 0 | 6 | 0 |
| H028 | src/mvdm-host/vdd/samples/mscdex/vdd | 7 | 6 | 4 | 9 |
| H029 | src/mvdm-host/vdd/samples/sample1 | 1 | 0 | 1 | 0 |
| H030 | src/mvdm-host/vdd/samples/sample2 | 1 | 0 | 1 | 0 |
| H031 | src/mvdm-host/vdmdbg | 1 | 0 | 1 | 0 |
| H032 | src/mvdm-host/vdmredir | 23 | 0 | 21 | 2 |
| H033 | src/mvdm-host/wow32/i386 | 3 | 177 | 178 | 2 |
| H034 | src/mvdm-host/xms.486/alpha | 1 | 10 | 8 | 3 |
| H035 | src/mvdm-host/xms.486/mips | 1 | 0 | 0 | 1 |
| H036 | src/mvdm-host/xms.486/ppc | 1 | 0 | 0 | 1 |
| N001 | src/non-mvdm/base/ntdll | 1 | 1 | 1 | 1 |
| N002 | src/non-mvdm/base/ntos/ex | 1 | 0 | 1 | 0 |
| N003 | src/non-mvdm/base/ntos/rtl/x86 | 2 | 3 | 2 | 3 |
| N004 | src/non-mvdm/base/win32/client | 1 | 0 | 1 | 0 |
| N005 | src/non-mvdm/base/win32/inc | 3 | 0 | 3 | 0 |
| N006 | src/non-mvdm/base/win32/server | 3 | 0 | 3 | 0 |
| N007 | src/non-mvdm/base/win32/winnls/fontsup/system | 42 | 0 | 42 | 0 |
| N008 | src/non-mvdm/com/ole32/olethunk/olethk32 | 1 | 0 | 1 | 0 |
| N009 | src/non-mvdm/ds/netapi/netlib | 10 | 0 | 10 | 0 |
| N010 | src/non-mvdm/ds/netapi/rap | 7 | 0 | 7 | 0 |
| N011 | src/non-mvdm/ds/netapi/rpcxlate | 1 | 0 | 0 | 1 |
| N012 | src/non-mvdm/ds/netapi/xactsrv | 15 | 0 | 14 | 1 |
| N013 | src/non-mvdm/private/inc | 1 | 0 | 0 | 1 |
| N014 | src/non-mvdm/private/windows/inc | 1 | 0 | 0 | 1 |
| N015 | src/non-mvdm/public/ddk/inc | 5 | 0 | 4 | 1 |
| N016 | src/non-mvdm/public/internal/base/inc | 21 | 0 | 7 | 14 |
| N017 | src/non-mvdm/public/internal/ds/inc | 19 | 0 | 4 | 15 |
| N018 | src/non-mvdm/public/internal/net/inc | 1 | 0 | 1 | 0 |
| N019 | src/non-mvdm/public/internal/shell/inc | 2 | 0 | 0 | 2 |
| N020 | src/non-mvdm/public/internal/windows/inc | 7 | 0 | 4 | 3 |
| N021 | src/non-mvdm/public/sdk/inc | 17 | 0 | 17 | 0 |
| N022 | src/non-mvdm/windows/core/ntuser/client | 39 | 0 | 39 | 0 |
| N023 | src/non-mvdm/windows/core/ntuser/inc | 4 | 0 | 2 | 2 |
| N024 | src/non-mvdm/windows/core/ntuser/kernel | 9 | 0 | 9 | 0 |
| N025 | src/non-mvdm/windows/core/ntuser/rtl | 10 | 0 | 10 | 0 |
| N026 | src/non-mvdm/windows/core/ntuser/server | 1 | 0 | 1 | 0 |
| G001 | src/mvdm-guest/bin86/i386 | 1 | 18 | 18 | 1 |
| G002 | src/mvdm-guest/dos/v86/cmd/append | 13 | 3 | 5 | 11 |
| G003 | src/mvdm-guest/dos/v86/cmd/backup | 2 | 0 | 0 | 2 |
| G004 | src/mvdm-guest/dos/v86/cmd/command/usa-ms | 1 | 93 | 45 | 49 |
| G005 | src/mvdm-guest/dos/v86/cmd/debug/dosdebug | 1 | 39 | 17 | 23 |
| G006 | src/mvdm-guest/dos/v86/cmd/edlin | 23 | 0 | 7 | 16 |
| G007 | src/mvdm-guest/dos/v86/cmd/exe2bin | 20 | 0 | 8 | 12 |
| G008 | src/mvdm-guest/dos/v86/cmd/fastopen | 6 | 0 | 2 | 4 |
| G009 | src/mvdm-guest/dos/v86/cmd/graphics | 58 | 0 | 34 | 24 |
| G010 | src/mvdm-guest/dos/v86/cmd/keyb | 43 | 0 | 25 | 18 |
| G011 | src/mvdm-guest/dos/v86/cmd/loadfix | 6 | 0 | 1 | 5 |
| G012 | src/mvdm-guest/dos/v86/cmd/mem | 31 | 0 | 14 | 17 |
| G013 | src/mvdm-guest/dos/v86/cmd/nlsfunc | 15 | 0 | 6 | 9 |
| G014 | src/mvdm-guest/dos/v86/cmd/qbasic | 10 | 0 | 9 | 1 |
| G015 | src/mvdm-guest/dos/v86/cmd/setver/usa-ms | 1 | 15 | 7 | 9 |
| G016 | src/mvdm-guest/dos/v86/cmd/share | 6 | 0 | 2 | 4 |
| G017 | src/mvdm-guest/dos/v86/dev/ansi | 21 | 1 | 8 | 14 |
| G018 | src/mvdm-guest/dos/v86/dev/country | 24 | 0 | 19 | 5 |
| G019 | src/mvdm-guest/dos/v86/dev/himem/usa-ms | 1 | 18 | 6 | 13 |
| G020 | src/mvdm-guest/dos/v86/dev/keyboard | 95 | 0 | 59 | 36 |
| G021 | src/mvdm-guest/dos/v86/doskrnl/bios | 42 | 1 | 18 | 25 |
| G022 | src/mvdm-guest/dos/v86/doskrnl/dos | 100 | 0 | 51 | 49 |
| G023 | src/mvdm-guest/dos/v86/h | 33 | 0 | 33 | 0 |
| G024 | src/mvdm-guest/dos/v86/inc | 76 | 0 | 76 | 0 |
| G025 | src/mvdm-guest/dos/v86/lib/xmm | 4 | 3 | 2 | 5 |
| G026 | src/mvdm-guest/dos/v86/messages | 20 | 0 | 0 | 20 |
| G027 | src/mvdm-guest/dos/v86/redir/obj | 11 | 20 | 18 | 13 |
| G028 | src/mvdm-guest/dos/v86/redir/usa | 1 | 0 | 1 | 0 |
| G029 | src/mvdm-guest/dos/v86/tools/src/buildidx | 4 | 0 | 4 | 0 |
| G030 | src/mvdm-guest/dos/v86/tools/src/getsize | 4 | 0 | 4 | 0 |
| G031 | src/mvdm-guest/dos/v86/tools/src/h2inc | 4 | 0 | 4 | 0 |
| G032 | src/mvdm-guest/dos/v86/tools/src/stripdd | 4 | 0 | 4 | 0 |
| G033 | src/mvdm-guest/dos/v86/tools/src/stripz | 4 | 0 | 4 | 0 |
| G034 | src/mvdm-guest/dpmi/486 | 32 | 48 | 44 | 36 |
| G035 | src/mvdm-guest/dpmi/usa | 1 | 0 | 1 | 0 |
| G036 | src/mvdm-guest/sim16 | 8 | 0 | 8 | 0 |
| G037 | src/mvdm-guest/softpc.new/bios | 27 | 0 | 27 | 0 |
| G038 | src/mvdm-guest/softpc.new/data | 6 | 0 | 6 | 0 |
| G039 | src/mvdm-guest/softpc.new/roms | 8 | 0 | 8 | 0 |
| G040 | src/mvdm-guest/v86/util | 2 | 0 | 2 | 0 |
| G041 | src/mvdm-guest/vdd/samples/hpscan | 6 | 0 | 0 | 6 |
| G042 | src/mvdm-guest/vdd/samples/mscdex/tsr/messages/usa | 1 | 6 | 0 | 7 |
| G043 | src/mvdm-guest/vdd/samples/sample1/fax16 | 2 | 0 | 2 | 0 |
| G044 | src/mvdm-guest/vdd/samples/sample2/16bits | 3 | 0 | 3 | 0 |
| G045 | src/mvdm-guest/vdmutils/forcedos/obj/i386 | 1 | 7 | 5 | 3 |
| G046 | src/mvdm-guest/vdmutils/graftabl/obj/i386 | 5 | 6 | 3 | 8 |
| G047 | src/mvdm-guest/vdmutils/pifedit | 12 | 0 | 12 | 0 |
| G048 | src/mvdm-guest/vdmutils/win/obj/i386 | 1 | 4 | 4 | 1 |
| G049 | src/mvdm-guest/wow16/bin | 2 | 1 | 2 | 1 |
| G050 | src/mvdm-guest/wow16/commdlg/messages/usa | 8 | 60 | 47 | 21 |
| G051 | src/mvdm-guest/wow16/ctl3dv2 | 17 | 0 | 17 | 0 |
| G052 | src/mvdm-guest/wow16/ddeml/debug | 18 | 26 | 19 | 25 |
| G053 | src/mvdm-guest/wow16/ddeml/messages/usa | 3 | 0 | 3 | 0 |
| G054 | src/mvdm-guest/wow16/ddeml/tests/ddestrs | 20 | 1 | 21 | 0 |
| G055 | src/mvdm-guest/wow16/ddeml/tests/src/client | 23 | 3 | 26 | 0 |
| G056 | src/mvdm-guest/wow16/ddeml/tests/src/ddemo | 6 | 0 | 6 | 0 |
| G057 | src/mvdm-guest/wow16/ddeml/tests/src/server | 15 | 0 | 15 | 0 |
| G058 | src/mvdm-guest/wow16/drivers/comm | 29 | 0 | 17 | 12 |
| G059 | src/mvdm-guest/wow16/drivers/display | 20 | 0 | 5 | 15 |
| G060 | src/mvdm-guest/wow16/drivers/keyboard | 14 | 0 | 6 | 8 |
| G061 | src/mvdm-guest/wow16/drivers/mouse | 13 | 0 | 4 | 9 |
| G062 | src/mvdm-guest/wow16/drivers/sound | 11 | 0 | 4 | 7 |
| G063 | src/mvdm-guest/wow16/gdi | 27 | 0 | 16 | 11 |
| G064 | src/mvdm-guest/wow16/inc | 70 | 0 | 69 | 1 |
| G065 | src/mvdm-guest/wow16/kernel31/debug3 | 77 | 117 | 113 | 81 |
| G066 | src/mvdm-guest/wow16/kernel31/messages/usa | 1 | 0 | 1 | 0 |
| G067 | src/mvdm-guest/wow16/kernel31/retail3 | 77 | 0 | 0 | 77 |
| G068 | src/mvdm-guest/wow16/kernel31/swappro | 3 | 0 | 3 | 0 |
| G069 | src/mvdm-guest/wow16/killwow | 12 | 0 | 4 | 8 |
| G070 | src/mvdm-guest/wow16/lib | 25 | 0 | 13 | 12 |
| G071 | src/mvdm-guest/wow16/mciole | 16 | 0 | 9 | 7 |
| G072 | src/mvdm-guest/wow16/mmsystem/debug | 31 | 63 | 52 | 42 |
| G073 | src/mvdm-guest/wow16/mmsystem/messages/usa | 5 | 0 | 5 | 0 |
| G074 | src/mvdm-guest/wow16/mmsystem/mmtask | 11 | 0 | 4 | 7 |
| G075 | src/mvdm-guest/wow16/mmsystem/rinc | 1 | 0 | 1 | 0 |
| G076 | src/mvdm-guest/wow16/ole/client | 28 | 0 | 28 | 0 |
| G077 | src/mvdm-guest/wow16/ole/server/debug | 12 | 15 | 13 | 14 |
| G078 | src/mvdm-guest/wow16/ole/server/retail | 4 | 0 | 0 | 4 |
| G079 | src/mvdm-guest/wow16/rasapi16 | 13 | 0 | 6 | 7 |
| G080 | src/mvdm-guest/wow16/regedit/debug | 17 | 26 | 24 | 19 |
| G081 | src/mvdm-guest/wow16/regedit/messages/usa | 4 | 0 | 4 | 0 |
| G082 | src/mvdm-guest/wow16/shell/library | 1 | 13 | 6 | 8 |
| G083 | src/mvdm-guest/wow16/sherlock/messages/usa | 3 | 25 | 17 | 11 |
| G084 | src/mvdm-guest/wow16/system | 26 | 0 | 18 | 8 |
| G085 | src/mvdm-guest/wow16/test/hello | 4 | 1 | 4 | 1 |
| G086 | src/mvdm-guest/wow16/test/shell | 17 | 0 | 7 | 10 |
| G087 | src/mvdm-guest/wow16/timer/debug | 6 | 17 | 10 | 13 |
| G088 | src/mvdm-guest/wow16/timer/messages/usa | 2 | 0 | 2 | 0 |
| G089 | src/mvdm-guest/wow16/toolhelp | 66 | 0 | 34 | 32 |
| G090 | src/mvdm-guest/wow16/user/k/daytona | 45 | 69 | 67 | 47 |
| G091 | src/mvdm-guest/wow16/wfwnet | 25 | 0 | 12 | 13 |
| G092 | src/mvdm-guest/wow16/win87em | 62 | 0 | 62 | 0 |
| G093 | src/mvdm-guest/wow16/winoldap | 11 | 0 | 3 | 8 |
| G094 | src/mvdm-guest/wow16/winsock | 11 | 0 | 4 | 7 |
| G095 | src/mvdm-guest/wow16/winspool | 12 | 0 | 4 | 8 |
| G096 | src/mvdm-guest/wow16/wowdeb | 11 | 0 | 3 | 8 |
| G097 | src/mvdm-guest/wow16/write/messages/usa | 4 | 282 | 188 | 98 |

## Cross-batch interfaces identified in the current tree

| Lead | Companion | Evidence to compare when qualifying the XP pair |
| --- | --- | --- |
| H002 DOS COMMAND services | G004 COMMAND | Guest tcode.asm invokes CMDSVC SVC_CMDEXEC. Host cmddisp.c dispatches it to cmdExec. Check service number, register contract and return to COMMAND together. |
| H005 DPMI32 | G034 DOSX | Guest 486/dxstrt.asm issues BOP_DPMI InitDosx. Host dpmi32.c contains DpmiInitDosx in the dispatch table. Check entry number, initialization data and mode handoff. |
| H028 VCDEX | G042 MSCDEXNT | Guest mscdexnt.asm explicitly loads VCDEX.DLL. XP catalogues the corresponding TSR source and VDD definition under base/mvdm/vdd/mscdex instead of vdd/samples/mscdex. Verify DLL exports, registration and a guest CD request together. |

These are confirmed current interfaces and future joint-check candidates. Move a companion early only when qualification shows the current batch needs it. Each batch remains unstarted. Record its exact input blobs, affected output or historical-only disposition, companion files, compatibility test and rollback package before integration.

For 66 unmatched current paths the file ledger also lists a relocation lead. The search retains a unique candidate matching a trailing directory plus filename (or a longer trailing path) across the XP NT catalogue. Verify the original owner and interface before promoting any lead. The exact-path counts above exclude these hints.
