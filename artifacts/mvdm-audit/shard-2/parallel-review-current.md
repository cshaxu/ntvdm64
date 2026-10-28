# S10 shard 2 current pending audit — preliminary handoff

This report covers the original manifest shard 2, not an additional partition. The
[observer](parallel-review-observer.py) freshly checked all 330 pending rows
against the current source and each available pinned original; the exact full
hashes and prior per-path evidence are in [receipt](parallel-review-evidence.json).
Four tools16 paths are absent from the current mirror and remain pending.
No guest was built, changed or executed.

The six candidate decisions below are proposals to the primary reviewer; the
shared ledger is unchanged. Matching source bytes only supports retained source
text, not a whole-behavior restoration estimate. No full classification is
proposed. All other 324 paths remain pending; their stored chain is context,
not a new assertion that it executes.

## Priority DOS-host contracts

The selected x86 production map and actual object/image code bind CmdDispatch,
demIOCTL, demLockOper, demAbsRead and demAbsWrite (relocation bytes excluded
from body comparison). Binding alone does not prove calls reach the right
relocation targets or that the whole function behaves correctly.

`nt_bop.c::MS_bop_4` reads a guest byte then calls CmdDispatch, whose original
index check is under `#if DBG`; the 17-entry table can be indexed beyond its
end in the selected production form. This is a source-level potential
out-of-bounds dispatch, not a demonstrated crash or proof that an ordinary
program reaches it. Analyze guest-accessible BOP routing and reproduce in an
isolated harness before deciding whether a compatibility repair is necessary.
`demIOCTL` similarly has a DBG-only bound check, but guest IOCTL dispatch
normalization must be traced before claiming out-of-range guest reachability.

The selected originals retain 100% source bytes. A defensible semantic
percentage cannot be inferred from linked bodies. `demdasd` media writes are
not to be tested against real disks; any later behavior test requires isolated
media and existing rules.

## Per-path results

`Pending` explicitly retains unresolved behavior/owner and does not approve
exclusion. Hashes below are SHA-256 prefixes; the receipt has full values and
both catalog counts. An absent current source is shown as `absent`, not 0%.

| Original path | Proposed status | Current source SHA | Existing chain or remaining boundary |
| --- | --- | --- | --- |
| `bde/bde.def` | pending | `CFB2DEEB3C42` | bde/sources:41,56 builds independent BDE debugger-extension DLL; definition describes its exports |
| `bde/data.c` | pending | `5DA2D4E3CD95` | bde/sources:83-90 selects debugger callback/global state for separate BDE extension |
| `bde/disasm.c` | pending | `8A59C16E5EDD` | bde/sources:83-90 selects disassembler called by bdecmds u command in separate BDE extension |
| `bin86/pifedit.hlp` | pending | `20BB0049ACBE` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `bin86/pmspl.dll` | pending | `432BB90629CE` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `build.log` | pending | `38C01C654934` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `dos/command/cmddisp.c` | partial candidate | `2A906E7100DA` | MS_bop_4 reads guest command byte and calls original CmdDispatch; 17-entry apfnSVCCmd. Production bound body has DBG-only upper-bound check; malformed-input reachability and result need proof. |
| `dos/command/cmdpif.h` | pending | `1C629826EBA5` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `dos/dem/demdasd.c` | partial candidate | `7438F5D4DF18` | DEM dispatch selects demAbsRead/demAbsWrite; both delegate to demAbsReadWrite; production bodies bound. Actual media, geometry, errors and safe teardown remain open. |
| `dos/dem/demioctl.c` | partial candidate | `6F480BF758D0` | DEM dispatch selects demIOCTL; original DOS ioctl.asm has HRDSVC calls and host subdispatch table; production body bound. Subfunction bounds, errors and lifecycle need proof. |
| `dos/dem/demlock.c` | partial candidate | `7716AB3BE08F` | DEM dispatch selects demLockOper; maps guest handle/offset/length to native LockFile/UnlockFile; production body bound. Multi-process locking, errors and release need proof. |
| `dos/dem/dosdef.h` | pending | `82522742F011` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `dos/v86/cmd/append/append.exe` | pending | `73D6928246F3` | Original guest group dos/v86/cmd/append; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/cmd/append/Makefile, src/mvdm/dos/v86/cmd/append/makefile. |
| `dos/v86/cmd/command/readme.txt` | pending | `041A97E02C22` | Original guest group dos/v86/cmd/command; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/cmd/command/Makefile, src/mvdm/dos/v86/cmd/command/makefile. |
| `dos/v86/cmd/exe2bin/exe2bin.tag` | pending | `D47D911CFA30` | Original guest group dos/v86/cmd/exe2bin; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/cmd/exe2bin/Makefile, src/mvdm/dos/v86/cmd/exe2bin/makefile. |
| `dos/v86/cmd/keyb/kb16.exc` | excluded candidate | `8DAB9416340D` | Original keyb/makefile links KB16.EXC as intermediate for KB16.COM. Exclude this guest build-artifact role only; keyboard and actual guest binary behavior stay in scope. |
| `dos/v86/cmd/keyb/keyb.tag` | pending | `7DE060FA90BF` | Original guest group dos/v86/cmd/keyb; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/cmd/keyb/Makefile, src/mvdm/dos/v86/cmd/keyb/makefile. |
| `dos/v86/cmd/loadfix/LOADFIX.COM` | pending | `8E62654A49BD` | Original guest group dos/v86/cmd/loadfix; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/cmd/loadfix/Makefile, src/mvdm/dos/v86/cmd/loadfix/makefile. |
| `dos/v86/cmd/mem/mem.exe` | pending | `C4582B1E0738` | Original guest group dos/v86/cmd/mem; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/cmd/mem/Makefile, src/mvdm/dos/v86/cmd/mem/makefile. |
| `dos/v86/cmd/mem/mem.skl` | excluded candidate | `71EBDE828ADE` | Original mem/makefile builds mem.ctl from mem.skl plus country messages. Exclude this guest message-build input role only; MEM correctness and runtime message behavior remain in scope. |
| `dos/v86/cmd/nlsfunc/nlsfunc.exe` | pending | `A49E8C6C392F` | Original guest group dos/v86/cmd/nlsfunc; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/cmd/nlsfunc/Makefile, src/mvdm/dos/v86/cmd/nlsfunc/makefile. |
| `dos/v86/cmd/qbasic/edit.com` | pending | `E0A0B24FEE40` | Original guest group dos/v86/cmd/qbasic; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/cmd/qbasic/Makefile, src/mvdm/dos/v86/cmd/qbasic/makefile. |
| `dos/v86/cmd/qbasic/edit.hlp` | pending | `DEB09A378B88` | Original guest group dos/v86/cmd/qbasic; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/cmd/qbasic/Makefile, src/mvdm/dos/v86/cmd/qbasic/makefile. |
| `dos/v86/cmd/qbasic/gorilla.bas` | pending | `9926FC1F50C4` | Original guest group dos/v86/cmd/qbasic; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/cmd/qbasic/Makefile, src/mvdm/dos/v86/cmd/qbasic/makefile. |
| `dos/v86/cmd/setver/setver.tag` | pending | `195938D160D0` | Original guest group dos/v86/cmd/setver; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/cmd/setver/Makefile, src/mvdm/dos/v86/cmd/setver/makefile. |
| `dos/v86/dev/ansi/ansi.skl` | pending | `069A2E18E48D` | Original guest group dos/v86/dev/ansi; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/dev/ansi/Makefile, src/mvdm/dos/v86/dev/ansi/makefile. |
| `dos/v86/dev/ansi/ANSI.SYS` | pending | `C929C0893BBA` | Original guest group dos/v86/dev/ansi; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/dev/ansi/Makefile, src/mvdm/dos/v86/dev/ansi/makefile. |
| `dos/v86/dev/ansi/ansivid.inc` | pending | `DE901DAA752D` | Original guest group dos/v86/dev/ansi; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/ansi/Makefile, src/mvdm/dos/v86/dev/ansi/makefile. |
| `dos/v86/dev/country/nls863.inc` | pending | `4DF45A508204` | Original guest group dos/v86/dev/country; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/country/Makefile, src/mvdm/dos/v86/dev/country/makefile. |
| `dos/v86/dev/himem/himem.exe` | pending | `8121862C21B7` | Original guest group dos/v86/dev/himem; guest executable/media image; adjacent build metadata: src/mvdm/dos/v86/dev/himem/Makefile, src/mvdm/dos/v86/dev/himem/makefile. |
| `dos/v86/dev/keyboard/kdfbr.asm` | pending | `29ECB6E0FF29` | Original guest group dos/v86/dev/keyboard; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/keyboard/Makefile, src/mvdm/dos/v86/dev/keyboard/makefile. |
| `dos/v86/dev/keyboard/kdfit.asm` | pending | `4AF550ECB3EA` | Original guest group dos/v86/dev/keyboard; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/keyboard/Makefile, src/mvdm/dos/v86/dev/keyboard/makefile. |
| `dos/v86/dev/keyboard/kdfru092.asm` | pending | `2FBC917CAFCD` | Original guest group dos/v86/dev/keyboard; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/keyboard/Makefile, src/mvdm/dos/v86/dev/keyboard/makefile. |
| `dos/v86/dev/keyboard/kdfyc116.asm` | pending | `28803A449E07` | Original guest group dos/v86/dev/keyboard; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/keyboard/Makefile, src/mvdm/dos/v86/dev/keyboard/makefile. |
| `dos/v86/dev/keyboard/kdfyc117.asm` | pending | `7D0286579D43` | Original guest group dos/v86/dev/keyboard; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/dev/keyboard/Makefile, src/mvdm/dos/v86/dev/keyboard/makefile. |
| `dos/v86/doskrnl/bios/msbio.skl` | pending | `2A2FB4D31B19` | Original guest group dos/v86/doskrnl/bios; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/doskrnl/bios/Makefile, src/mvdm/dos/v86/doskrnl/bios/makefile. |
| `dos/v86/doskrnl/dos/dos.tag` | pending | `914D68305895` | Original guest group dos/v86/doskrnl/dos; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/doskrnl/dos/Makefile, src/mvdm/dos/v86/doskrnl/dos/makefile. |
| `dos/v86/doskrnl/dos/msdos.skl` | pending | `A5605F538CCE` | Original guest group dos/v86/doskrnl/dos; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/doskrnl/dos/Makefile, src/mvdm/dos/v86/doskrnl/dos/makefile. |
| `dos/v86/h/bios.inc` | pending | `095016BB99A8` | Original guest group dos/v86/h; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/h/process.h` | pending | `0D005443E278` | Original guest group dos/v86/h; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/h/stddef.h` | pending | `EF005074DBC2` | Original guest group dos/v86/h; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/inc/msdata2.asm` | pending | `40CC58C96673` | Original guest group dos/v86/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/inc/oemnum.inc` | pending | `7FAC71D482C7` | Original guest group dos/v86/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/inc/pathmac.inc` | pending | `C4A595B9401E` | Original guest group dos/v86/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/inc/pcgequ.inc` | pending | `1F7557E0B253` | Original guest group dos/v86/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/inc/pcinput.inc` | pending | `C7AE852475F5` | Original guest group dos/v86/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/lib/oldnames.lib` | pending | `2DC0C5EE5790` | Original guest group dos/v86/lib; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: not established in retained inventory. |
| `dos/v86/messages/germany.msg` | pending | `EE003A8BE717` | Original guest group dos/v86/messages; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/messages/Makefile, src/mvdm/dos/v86/messages/makefile. |
| `dos/v86/messages/holland.msg` | pending | `D747A9FC011E` | Original guest group dos/v86/messages; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/messages/Makefile, src/mvdm/dos/v86/messages/makefile. |
| `dos/v86/messages/loadmsg.msg` | pending | `D13386A68AD2` | Original guest group dos/v86/messages; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/messages/Makefile, src/mvdm/dos/v86/messages/makefile. |
| `dos/v86/messages/usa-ms.idx` | pending | `FF460E0CFAE6` | Original guest group dos/v86/messages; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dos/v86/messages/Makefile, src/mvdm/dos/v86/messages/makefile. |
| `dos/v86/redir/fudgedef.inc` | pending | `02E1C6C267C2` | Original guest group dos/v86/redir; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/redir/Makefile, src/mvdm/dos/v86/redir/makefile. |
| `dos/v86/redir/obj/redir.exe` | pending | `F4BE4EA04769` | Original guest group dos/v86/redir; guest executable/media image; adjacent build metadata: not established in retained inventory. |
| `dos/v86/tools/src/getsize/getsize.def` | pending | `CCA1D14426EB` | Original guest group dos/v86/tools; guest export/resource input; adjacent build metadata: src/mvdm/dos/v86/tools/src/getsize/Makefile, src/mvdm/dos/v86/tools/src/getsize/makefile, src/mvdm/dos/v86/tools/src/getsize/sources. |
| `dos/v86/tools/src/h2inc/h2inc.c` | pending | `D41CCC8BB473` | Original guest group dos/v86/tools; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dos/v86/tools/src/stripdd/stripdd.c` | pending | `7A5E4536964D` | Original guest group dos/v86/tools; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/tools/src/stripdd/Makefile, src/mvdm/dos/v86/tools/src/stripdd/makefile, src/mvdm/dos/v86/tools/src/stripdd/sources. |
| `dos/v86/tools/src/stripz/stripz.c` | pending | `3B3DD85FBEF2` | Original guest group dos/v86/tools; guest source or declaration; adjacent build metadata: src/mvdm/dos/v86/tools/src/stripz/Makefile, src/mvdm/dos/v86/tools/src/stripz/makefile, src/mvdm/dos/v86/tools/src/stripz/sources. |
| `dpmi/486/dxintr.asm` | pending | `23B221750FD0` | Original guest group dpmi/486; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dpmi/dosx.def` | pending | `8ECE5C5CF5FA` | Original guest group dpmi/dosx.def; guest export/resource input; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/dxhpbios.asm` | pending | `7E421F0AEBAD` | Original guest group dpmi/dxhpbios.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/dxini.asm` | pending | `FC07E15E4E89` | Original guest group dpmi/dxini.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/dxmmgr.asm` | pending | `9D320F56ADB2` | Original guest group dpmi/dxmmgr.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/dxnetbio.asm` | pending | `C55A7F562564` | Original guest group dpmi/dxnetbio.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/dxoem.asm` | pending | `BF841C30A7EA` | Original guest group dpmi/dxoem.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/dxstrt.asm` | pending | `E67463A6CA5B` | Original guest group dpmi/dxstrt.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/migrate.bat` | pending | `3FB632F6A50F` | Original guest group dpmi/migrate.bat; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/mk.cmd` | pending | `53D3DE5DB2D4` | Original guest group dpmi/mk.cmd; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/ntnpxem.asm` | pending | `D1560824D54F` | Original guest group dpmi/ntnpxem.asm; guest source or declaration; adjacent build metadata: src/mvdm/dpmi/Makefile, src/mvdm/dpmi/makefile. |
| `dpmi/usa/dxmsg.asm` | pending | `2C4E34D55F74` | Original guest group dpmi/usa; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `dpmi32/debug.c` | pending | `88A0FB538D88` | Original source family dpmi32/debug.c; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `dpmi32/precomp.h` | pending | `F3111325CDFF` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `fax/dummy.ppd` | pending | `12F99F531442` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `fax/faxcomm.h` | pending | `3F0EBB100733` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `fax/wowfax/wowfax.def` | pending | `4C62075FC119` | fax/wowfax/sources:28-30 selects optional WOW fax GDI driver and its definition |
| `fax/wowfaxui/wfsheets.c` | pending | `C43B25CB761C` | fax/wowfaxui/sources:28-30,52-55 selects optional separate fax UI DLL sheets |
| `fax/wowfaxui/wfsheets.h` | pending | `262CCB34768E` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `fax/wowfaxui/wowfaxui.c` | pending | `D96BF03EBD44` | fax/wowfaxui/sources:28-30,52-55 selects optional separate fax UI DLL entry |
| `fax/wowfaxui/wowfaxui.h` | pending | `88D90B14CEC1` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `ieuvddex/help.c` | pending | `F27DDDEE55C2` | ieuvddex/sources:31-47 selects help in separate debugger-extension DLL |
| `ieuvddex/trace.c` | pending | `B1F435775188` | ieuvddex/sources:31-47 selects debugger-extension trace, called by procdep.c |
| `inc/BOP.INC` | pending | `A3705F225C47` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/cmdsvc.h` | pending | `C22D682422E2` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/curdir.h` | pending | `8FB302F40659` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/CURDIR.INC` | pending | `8490C9AA0702` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/dbginfo.h` | pending | `EB63D7148716` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/intapi.h` | pending | `4A4EA44F62A4` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/VDMTIB.INC.old` | pending | `675CEEC6E626` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/VINT.INC` | pending | `2BA06B3EEA9A` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/vrdefld.h` | pending | `E15089BF59F6` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/VRDEFLD.INC` | pending | `8A1A7D7BD9C7` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/WOW.INC` | pending | `87C5A5A1A3F3` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/WOWCMDLG.INC` | pending | `19CE7BB243B1` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/wowcomm.h` | pending | `4817961493D3` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/wowgdi.h` | pending | `474B263B337B` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/WOWGDI.INC` | pending | `5B6C2F8A7400` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/wowkbd.h` | pending | `ECFF4D47C62F` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/WOWMMCB.INC` | pending | `0BA724BB298B` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/wowmmed.h` | pending | `8416869D9F2A` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/WOWNT32.INC` | pending | `3F0DF9FE5BAE` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `inc/WOWTH.INC` | pending | `CE4A1BE01D64` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `sim16/sim16.mac` | pending | `16BC58DA7CC8` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/ccpu386/cmpxchg.c` | pending | `02318C7BD61E` | Compiled source functions are guarded by SPC486; original c_main.c decoder uses those handlers only under SPC486 and otherwise calls Int6 for 0F B0/B1 or 0F C0/C1. Current selected object has no map symbol; cmpxchg.obj dumpbin shows no executable symbol/section. |
| `softpc.new/base/ccpu386/evidgen.h` | pending | `7E429987C49E` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/ccpu386/pigger_c.h` | pending | `F8ABFAB1A700` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/ccpu386/univer_c.h` | pending | `72655450D96D` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/ccpu386/wbinvd.h` | pending | `806A12EB2D54` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/ccpu386/xadd.c` | pending | `80D97CE7D9B5` | Compiled source functions are guarded by SPC486; original c_main.c decoder uses those handlers only under SPC486 and otherwise calls Int6 for 0F B0/B1 or 0F C0/C1. Current selected object has no map symbol; cmpxchg.obj dumpbin shows no executable symbol/section. |
| `softpc.new/base/cpu/src/evid/vglob.c` | pending | `E695A9807285` | Original source family softpc.new/base/cpu/src/evid/vglob.c; getVideobit_prot_mask at src/mvdm/softpc.new/base/ccpu386/vglob.c:129; getVideochain at src/mvdm/softpc.new/base/ccpu386/vglob.c:437; getVideodata_and_mask at src/mvdm/softpc.new/base/ccpu386/vglob.c:96; getVideodata_xor_mask at src/mvdm/softpc.new/base/ccpu386/vglob.c:107 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/cvidc/accessfn.c` | pending | `326046C10F56` | Original source family softpc.new/base/cvidc/accessfn.c; PurgeLostIretHookLine at src/mvdm/softpc.new/host/inc/ppc/pig/c2cpusad.h:174; getAF at src/mvdm/inc/monregs.h:55; getAH at src/mvdm/dpmi32/int21map.c:190; getAL at src/mvdm/dpmi32/int21map.c:408 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/cvidc/bound_c.h` | pending | `8B0D1F2E0C5D` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/c2cpug_c.h` | pending | `6CD353C8CB05` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/conv_c.h` | pending | `3DB1FDFE05AD` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/cpnote_c.h` | pending | `311DACBCCEEC` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/dec_c.h` | pending | `7A4B90358D04` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/gdinit_c.h` | pending | `2D3923E7AFA1` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/hpphst_c.h` | pending | `FF55F89A97E6` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/inc_c.h` | pending | `2C9C5AE87D07` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/light_c.h` | pending | `491FBBAA1ACA` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/nano_c.h` | pending | `6F932E3D8E66` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/nanoa_c.h` | pending | `BB31A4C32DB6` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/page_c.h` | pending | `8A90AA33098F` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/pm_c.h` | pending | `B6BA5208F758` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/prot_c.h` | pending | `732094541E77` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/simple_c.h` | pending | `569A34BE63D1` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/sinit013.c` | pending | `9BA333037CAF` | Original source family softpc.new/base/cvidc/sinit013.c; S_0415_CiSetVideofwd_str_read_addr_00000034 at src/mvdm/softpc.new/base/cvidc/vglfunc.c:59; S_0416_CiSetVideobwd_str_read_addr_00000035 at src/mvdm/softpc.new/base/cvidc/vglfunc.c:60; S_0417_CiSetVideodirty_total_00000036 at src/mvdm/softpc.new/base/cvidc/vglfunc.c:61; S_0418_CiSetVideodirty_low_00000037 at src/mvdm/softpc.new/base/cvidc/vglfunc.c:62 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/cvidc/stub_c.h` | pending | `F2FB630C8FB5` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/cvidc/vglob_c.h` | pending | `CC795D6480E5` | Frozen file-reconciliation.csv product_link_selection; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md (build selection only, not per-function dispatch). |
| `softpc.new/base/debug/dasm.c` | pending | `6F90249D51EE` | Original source family softpc.new/base/debug/dasm.c; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/debug/ega_dump.c` | pending | `18C7205169A6` | Original source family softpc.new/base/debug/ega_dump.c; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/debug/markfrom` | pending | `D0734FC35CF6` | original debugger icon pixel table; non-runtime debugger resource |
| `softpc.new/base/debug/profile.c` | pending | `93A1891B0A27` | Original source family softpc.new/base/debug/profile.c; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/debug/trace.c` | pending | `B776BEC19F7C` | Original source family softpc.new/base/debug/trace.c; sub_io_verbose at src/mvdm/softpc.new/base/debug/yoda.c:2653; io_verbose at src/mvdm/softpc.new/host/inc/host_trc.h:22 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/disks/gfi_sflp.c` | pending | `396C621A6C6A` | Original source family softpc.new/base/disks/gfi_sflp.c; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/inc/build_id.h` | pending | `72C5484BF3EE` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/cdrom.h` | pending | `9DA2A1531AC1` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/dasm.h` | pending | `BB18CFBA18FC` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/decode.h` | pending | `D9ECC3CC6097` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/gisp_sas.h` | pending | `2413FD0F1B4A` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/haw.h` | pending | `E1EFD15347A6` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/host_hun.h` | pending | `B2EF5F35B082` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/host_msw.h` | pending | `CF1EC4EBDC5E` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/keycaps.h` | pending | `AEEAFE559EE2` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/loader.h` | pending | `C195AB5835BC` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/low_intp.h` | pending | `71F6BBC11D70` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/m_keybdp.h` | pending | `B8D73E0606DF` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/mark_ima.h` | pending | `9AF24AF31E9C` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/ms_windo.h` | pending | `F76510E61219` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/smeg.h` | pending | `2E0BCA83B80F` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/winfiles.h` | pending | `F307FC3F4039` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/inc/worm.h` | pending | `D49A3112A511` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/base/keymouse/mouse.c` | pending | `2D196D9F0C1E` | Original source family softpc.new/base/keymouse/mouse.c; mouse_inb at src/mvdm/softpc.new/base/inc/mouse.h:100; mouse_init at src/mvdm/softpc.new/base/bios/reset.c:1011; mouse_outb at src/mvdm/softpc.new/base/inc/mouse.h:101 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/base/keymouse/ppi.c` | pending | `367CCEAA8834` | Original source family softpc.new/base/keymouse/ppi.c; ppi_inb at src/mvdm/softpc.new/base/inc/ppi.h:31; ppi_init at src/mvdm/softpc.new/base/bios/reset.c:824; ppi_outb at src/mvdm/softpc.new/base/inc/ppi.h:32 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `softpc.new/bios/biostruc.inc` | pending | `63FB257501A0` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/bios/devmark.inc` | pending | `3250E13971FC` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/bios/msgroup.inc` | pending | `E07D8DE51424` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/bios/sysimes.asm` | pending | `C78FD2DDB0E2` | softpc.new/bios/makefile:55,78 selects guest BIOS build input; immutable-guest rule prohibits rebuild |
| `softpc.new/data/bios1.rom` | pending | `1CE33544249F` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/data/cmos.ram` | pending | `97FCA7ADB543` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/debugger/vdmdebug.c` | pending | `38A25FA9784C` | softpc.new/debugger/sources:35-46 selects independent debugger program and pipe server |
| `softpc.new/host/inc/host.h` | pending | `0192CAFB4E0E` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/host_bop.h` | pending | `3AB20D68C49D` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/host_emm.h` | pending | `312EC4DC5036` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/host_inc.h` | pending | `C78271903A7A` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/hostgrph.h` | pending | `DE614AFAF98A` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/hstsystm.h` | pending | `B6D4B4334123` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/monregs.h` | pending | `AF51BE97B692` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/nt_ega.h` | pending | `D60C53FEF1C7` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/nt_reset.h` | pending | `91E93365FC76` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/nt_thred.h` | pending | `9907EF3C8A40` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/host/inc/sim32.h` | pending | `EEB9A654E53B` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/obj.vdm/ntvdm.prf` | pending | `00D94378F996` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/roms/bios2.rom` | pending | `6BF8BDC71A77` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `softpc.new/roms/bios4.asm` | pending | `E8E03F141C87` | original BIOS ROM source; immutable guest firmware policy excludes rebuild and host linkage |
| `suballoc/foo.bar` | pending | `0350C8AD182E` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `suballoc/suballoc.c` | pending | `B06496E16D55` | Selected CPU40 non-i386 XMSInit reserves VdmAllocateVirtualMemory; on normal success SAInitialize uses VdmCommitVirtualMemory/VdmDeCommitVirtualMemory, while STATUS_NOT_IMPLEMENTED fallback uses xmsCommitBlock/xmsDecommitBlock -> CPU40 sas_manage_xms TRUE stub. xmsblock.c calls SAAllocate/SAFree/Reallocate. |
| `suballoc/suballoc.txt` | pending | `5F8EF01DA130` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `tools16/c23.err` | pending | `absent` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `tools16/fixexe.exe` | pending | `absent` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `tools16/lib16.exe` | pending | `absent` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `tools16/nosrvbld.exe` | pending | `absent` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdd/samples/adlibvdd/adlibvdd.def` | pending | `B9F65254A9CE` | original sample ADLIBVDD DLL export definition; samples/adlibvdd/sources:8-10 selects separate sample DLL |
| `vdd/samples/hpscan/hpscan16.exe` | pending | `6A8F336F374F` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdd/samples/hpscan/hpscan16.inc` | pending | `08E6F2F7D449` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdd/samples/mscdex/tsr/messages/usa/messages.inc` | pending | `0323803B2F55` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdd/samples/mscdex/vdd/mscdex.h` | pending | `ECCAF60CC04A` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdd/samples/mscdex/vdd/vcdex.rc` | pending | `1DE1253BAB13` | samples/mscdex/vdd/sources:6-8,19 selects version resource of separate sample VCDEX DLL |
| `vdmdbg/vdmdbg.def` | pending | `2C104E9E79BB` | vdmdbg/sources:16,31 builds separate debugger interface DLL; definition is its export contract |
| `vdmexts/i386/vdmtib.c` | pending | `F56CE99FA7D3` | vdmexts/sources:28,30,44 selects i386 debugger extension command implementation |
| `vdmexts/vdmexts.def` | pending | `A48B01B490F2` | vdmexts/sources:28,30 builds external debugger extension DLL; definition is its export contract |
| `vdmredir/dlstruct.h` | pending | `C98F6C2E38CA` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdmredir/vdmredir.rc` | pending | `92794973E7F1` | Original source family vdmredir/vdmredir.rc; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `vdmredir/vrdisp.c` | pending | `030A3A2F2BAA` | Original source family vdmredir/vrdisp.c; No candidate cross-file textual references located for the sampled map symbols. Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |
| `vdmredir/vrica.h` | pending | `B93042CE17AE` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdmredir/vrputil.h` | pending | `39F3E56F1F34` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdmutils/graftabl/graftabl.h` | pending | `55ED64F98B65` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdmutils/graftabl/obj/i386/graftabl.com` | pending | `BC241D49E73A` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `vdmutils/pifedit/pif.def` | pending | `79A710528DAF` | PIFEDIT independent utility module definition; pifedit/sources:4-12 selects utility library rather than worker |
| `vdmutils/pifedit/pifedit.c` | pending | `FE2330596F35` | pifedit/sources:4-12 selects separate utility library, not current worker PIF execution path |
| `vdmutils/win/obj/i386/win.com` | pending | `5EB085D36B58` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow16/bin/winstub.exe` | pending | `7FB9C1C6A87A` | Original guest group wow16/bin; guest executable/media image; adjacent build metadata: not established in retained inventory. |
| `wow16/commdlg/colordlg.h` | pending | `0034C2DFDBC6` | Original guest group wow16/commdlg; guest source or declaration; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/commdlg/COMMDLG.DLL` | pending | `54A7981C5667` | Original guest group wow16/commdlg; guest executable/media image; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/commdlg/parse.h` | pending | `61A0B104142D` | Original guest group wow16/commdlg; guest source or declaration; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/commdlg/privcomd.h` | pending | `2DCECD3D80CC` | Original guest group wow16/commdlg; guest source or declaration; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/commdlg/smfldr2.bmp` | pending | `8C380D1FBB4C` | Original guest group wow16/commdlg; guest export/resource input; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/commdlg/smfldr3.bmp` | pending | `FDFEFE7D685A` | Original guest group wow16/commdlg; guest export/resource input; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/commdlg/STRIPPED.DEF` | pending | `EAB482D6EE0D` | Original guest group wow16/commdlg; guest export/resource input; adjacent build metadata: src/mvdm/wow16/commdlg/Makefile, src/mvdm/wow16/commdlg/makefile. |
| `wow16/ctl3dv2/common.ver` | pending | `FC90D58A2E96` | Original guest group wow16/ctl3dv2; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: not established in retained inventory. |
| `wow16/ctl3dv2/ctl3d.c` | pending | `F712DE39B011` | Original guest group wow16/ctl3dv2; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `wow16/ctl3dv2/ctl3d.doc` | pending | `B9FAA5A532A7` | Original guest group wow16/ctl3dv2; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: not established in retained inventory. |
| `wow16/ctl3dv2/ctl3d.h` | pending | `DFAE25124E38` | Original guest group wow16/ctl3dv2; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `wow16/ctl3dv2/msdn.ico` | pending | `03E8F1A57F28` | Original guest group wow16/ctl3dv2; guest export/resource input; adjacent build metadata: not established in retained inventory. |
| `wow16/ctl3dv2/readme.txt` | pending | `7FCE264469F2` | Original guest group wow16/ctl3dv2; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: not established in retained inventory. |
| `wow16/ddeml/tests/ddestrs/ddestrs.txt` | pending | `6CB4DCA039C3` | stress-test usage/documentation for DDESTRS.EXE; original test only, distinct from runtime DDEML.DLL. |
| `wow16/ddeml/tests/ddestrs/runddes.bat` | pending | `4905460232FF` | stress-test execution wrapper for separate DDE test; original test only, distinct from runtime DDEML.DLL. |
| `wow16/ddeml/verddeml.h` | pending | `61A608E5BDFF` | Original guest group wow16/ddeml; guest source or declaration; adjacent build metadata: src/mvdm/wow16/ddeml/Makefile, src/mvdm/wow16/ddeml/makefile. |
| `wow16/drivers/comm/bimodint.inc` | pending | `FD0F31C168C8` | Original guest group wow16/drivers/comm; guest source or declaration; adjacent build metadata: src/mvdm/wow16/drivers/comm/Makefile, src/mvdm/wow16/drivers/comm/makefile. |
| `wow16/drivers/comm/comm.drv` | pending | `EE589C26791B` | Original guest group wow16/drivers/comm; guest executable/media image; adjacent build metadata: src/mvdm/wow16/drivers/comm/Makefile, src/mvdm/wow16/drivers/comm/makefile. |
| `wow16/drivers/comm/comm.rc` | pending | `001855B73658` | Original guest group wow16/drivers/comm; guest export/resource input; adjacent build metadata: src/mvdm/wow16/drivers/comm/Makefile, src/mvdm/wow16/drivers/comm/makefile. |
| `wow16/drivers/comm/ddkmake` | pending | `A8F8035C4C1E` | Original guest group wow16/drivers/comm; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/drivers/comm/Makefile, src/mvdm/wow16/drivers/comm/makefile. |
| `wow16/drivers/comm/ibmcom.inc` | pending | `A2E8E1FFD311` | Original guest group wow16/drivers/comm; guest source or declaration; adjacent build metadata: src/mvdm/wow16/drivers/comm/Makefile, src/mvdm/wow16/drivers/comm/makefile. |
| `wow16/drivers/display/config.LST` | pending | `46504B4494FF` | Original guest group wow16/drivers/display; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/drivers/display/Makefile, src/mvdm/wow16/drivers/display/makefile. |
| `wow16/drivers/display/display.rc` | pending | `34E98ADEA5F0` | Original guest group wow16/drivers/display; guest export/resource input; adjacent build metadata: src/mvdm/wow16/drivers/display/Makefile, src/mvdm/wow16/drivers/display/makefile. |
| `wow16/drivers/display/DISPLAY.RES` | pending | `C7B404B5F471` | Original guest group wow16/drivers/display; guest export/resource input; adjacent build metadata: src/mvdm/wow16/drivers/display/Makefile, src/mvdm/wow16/drivers/display/makefile. |
| `wow16/drivers/keyboard/keyboard.rcv` | pending | `85B7A4631DCE` | Original guest group wow16/drivers/keyboard; guest export/resource input; adjacent build metadata: src/mvdm/wow16/drivers/keyboard/Makefile, src/mvdm/wow16/drivers/keyboard/makefile. |
| `wow16/drivers/sound/sound.drv` | pending | `D5932CBD4E01` | Original guest group wow16/drivers/sound; guest executable/media image; adjacent build metadata: src/mvdm/wow16/drivers/sound/Makefile, src/mvdm/wow16/drivers/sound/makefile. |
| `wow16/gdi/gdi.rcv` | pending | `0984AC706475` | Original guest group wow16/gdi; guest export/resource input; adjacent build metadata: src/mvdm/wow16/gdi/Makefile, src/mvdm/wow16/gdi/makefile. |
| `wow16/inc/custcntl.h` | pending | `4117DA182C7C` | Original guest group wow16/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `wow16/inc/lzexpand.h` | pending | `93CA9A8AFC9C` | Original guest group wow16/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `wow16/inc/spool.h` | pending | `3F5CAA9323B5` | Original guest group wow16/inc; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `wow16/kernel31/KRNL386.RES` | pending | `18C74D3D6B8A` | Original guest group wow16/kernel31; guest export/resource input; adjacent build metadata: src/mvdm/wow16/kernel31/Makefile, src/mvdm/wow16/kernel31/makefile. |
| `wow16/kernel31/ksource.txt` | pending | `2FD7701FC69A` | Original guest group wow16/kernel31; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/kernel31/Makefile, src/mvdm/wow16/kernel31/makefile. |
| `wow16/kernel31/newexe.doc` | pending | `CC66AFB2D483` | Original guest group wow16/kernel31; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/kernel31/Makefile, src/mvdm/wow16/kernel31/makefile. |
| `wow16/kernel31/retail3/krnl386.exe` | pending | `BCED7650A5B5` | Original guest group wow16/kernel31; guest executable/media image; adjacent build metadata: not established in retained inventory. |
| `wow16/killwow/killwow.rc` | pending | `5DD9ACB70C93` | Original guest group wow16/killwow; guest export/resource input; adjacent build metadata: src/mvdm/wow16/killwow/Makefile, src/mvdm/wow16/killwow/makefile. |
| `wow16/lib/cdllcew.lib` | pending | `B167DF1530DD` | Original guest group wow16/lib; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/lib/Makefile, src/mvdm/wow16/lib/makefile. |
| `wow16/lib/snocrtdw.lib` | pending | `DBF6751B3E23` | Original guest group wow16/lib; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/lib/Makefile, src/mvdm/wow16/lib/makefile. |
| `wow16/lib/sound.def` | pending | `19DD3320243E` | Original guest group wow16/lib; guest export/resource input; adjacent build metadata: src/mvdm/wow16/lib/Makefile, src/mvdm/wow16/lib/makefile. |
| `wow16/mciole/libinit.asm` | pending | `436F6FED1D3E` | Original guest group wow16/mciole; guest source or declaration; adjacent build metadata: src/mvdm/wow16/mciole/Makefile, src/mvdm/wow16/mciole/makefile. |
| `wow16/mciole/MCIOLE16.DLL` | pending | `0EBA8DC565C5` | Original guest group wow16/mciole; guest executable/media image; adjacent build metadata: src/mvdm/wow16/mciole/Makefile, src/mvdm/wow16/mciole/makefile. |
| `wow16/mciole/mciole16.rcv` | pending | `878872EBAADB` | Original guest group wow16/mciole; guest export/resource input; adjacent build metadata: src/mvdm/wow16/mciole/Makefile, src/mvdm/wow16/mciole/makefile. |
| `wow16/mmsystem/messages/usa/mci.rc` | pending | `0DF6013A9080` | Original guest group wow16/mmsystem; guest export/resource input; adjacent build metadata: not established in retained inventory. |
| `wow16/mmsystem/messages/usa/mmtask.rc` | pending | `23FF18144C07` | Original guest group wow16/mmsystem; guest export/resource input; adjacent build metadata: not established in retained inventory. |
| `wow16/mmsystem/mmsystem.rc` | pending | `D6A328E813A3` | Original guest group wow16/mmsystem; guest export/resource input; adjacent build metadata: src/mvdm/wow16/mmsystem/Makefile, src/mvdm/wow16/mmsystem/makefile. |
| `wow16/mmsystem/mmtask/mmtask.asm` | pending | `AD9436F2B6F6` | Original guest group wow16/mmsystem; guest source or declaration; adjacent build metadata: src/mvdm/wow16/mmsystem/mmtask/Makefile, src/mvdm/wow16/mmsystem/mmtask/makefile. |
| `wow16/mmsystem/timea.asm` | pending | `6CC01772F84F` | Original guest group wow16/mmsystem; guest source or declaration; adjacent build metadata: src/mvdm/wow16/mmsystem/Makefile, src/mvdm/wow16/mmsystem/makefile. |
| `wow16/ole/client/funchead.c` | pending | `8C698B874AEC` | Original guest group wow16/ole/client; guest source or declaration; adjacent build metadata: src/mvdm/wow16/ole/client/Makefile, src/mvdm/wow16/ole/client/makefile. |
| `wow16/ole/client/mk.cmd` | pending | `B64EA57A825B` | Original guest group wow16/ole/client; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/ole/client/Makefile, src/mvdm/wow16/ole/client/makefile. |
| `wow16/ole/client/utils.c` | pending | `6ED48E621911` | Original guest group wow16/ole/client; guest source or declaration; adjacent build metadata: src/mvdm/wow16/ole/client/Makefile, src/mvdm/wow16/ole/client/makefile. |
| `wow16/regedit/debug/REGEDT16.EXE` | pending | `9AC95769EF42` | Original guest group wow16/regedit; guest executable/media image; adjacent build metadata: not established in retained inventory. |
| `wow16/regedit/reg1632.h` | pending | `1E56B04604D6` | Original guest group wow16/regedit; guest source or declaration; adjacent build metadata: src/mvdm/wow16/regedit/Makefile, src/mvdm/wow16/regedit/makefile. |
| `wow16/regedit/sdkreged.def` | pending | `129735B29CA5` | Original guest group wow16/regedit; guest export/resource input; adjacent build metadata: src/mvdm/wow16/regedit/Makefile, src/mvdm/wow16/regedit/makefile. |
| `wow16/sherlock/sherlock.ico` | pending | `087CD0589EDB` | Original guest group wow16/sherlock; guest export/resource input; adjacent build metadata: src/mvdm/wow16/sherlock/Makefile, src/mvdm/wow16/sherlock/makefile. |
| `wow16/system/romsys.def` | pending | `49B045478AD8` | Original guest group wow16/system; guest export/resource input; adjacent build metadata: src/mvdm/wow16/system/Makefile, src/mvdm/wow16/system/makefile. |
| `wow16/system/system` | pending | `440EA1B0A988` | Original guest group wow16/system; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/system/Makefile, src/mvdm/wow16/system/makefile. |
| `wow16/system/system.lrf` | pending | `CD6A2239FF01` | Original guest group wow16/system; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/system/Makefile, src/mvdm/wow16/system/makefile. |
| `wow16/system/system.rcv` | pending | `BD0A53FAF270` | Original guest group wow16/system; guest export/resource input; adjacent build metadata: src/mvdm/wow16/system/Makefile, src/mvdm/wow16/system/makefile. |
| `wow16/system/vecsys.inc` | pending | `11EFA3C60839` | Original guest group wow16/system; guest source or declaration; adjacent build metadata: src/mvdm/wow16/system/Makefile, src/mvdm/wow16/system/makefile. |
| `wow16/system/windefs.inc` | pending | `693D86727B3E` | Original guest group wow16/system; guest source or declaration; adjacent build metadata: src/mvdm/wow16/system/Makefile, src/mvdm/wow16/system/makefile. |
| `wow16/test/hello/hello.h` | pending | `E52F8AFC9854` | Original guest group wow16/test; guest source or declaration; adjacent build metadata: src/mvdm/wow16/test/hello/Makefile, src/mvdm/wow16/test/hello/makefile. |
| `wow16/timer/libinit.asm` | pending | `EFAEA505F8E2` | Original guest group wow16/timer; guest source or declaration; adjacent build metadata: src/mvdm/wow16/timer/Makefile, src/mvdm/wow16/timer/makefile. |
| `wow16/timer/local.asm` | pending | `30A1698384F8` | Original guest group wow16/timer; guest source or declaration; adjacent build metadata: src/mvdm/wow16/timer/Makefile, src/mvdm/wow16/timer/makefile. |
| `wow16/timer/math.asm` | pending | `7E6CA6CC7AD7` | Original guest group wow16/timer; guest source or declaration; adjacent build metadata: src/mvdm/wow16/timer/Makefile, src/mvdm/wow16/timer/makefile. |
| `wow16/timer/sysinfo.inc` | pending | `24A8BD23FDC0` | Original guest group wow16/timer; guest source or declaration; adjacent build metadata: src/mvdm/wow16/timer/Makefile, src/mvdm/wow16/timer/makefile. |
| `wow16/timer/TIMER.DRV` | pending | `4567FA8B0A10` | Original guest group wow16/timer; guest executable/media image; adjacent build metadata: src/mvdm/wow16/timer/Makefile, src/mvdm/wow16/timer/makefile. |
| `wow16/timer/timer.inc` | pending | `A5929F2DB4B3` | Original guest group wow16/timer; guest source or declaration; adjacent build metadata: src/mvdm/wow16/timer/Makefile, src/mvdm/wow16/timer/makefile. |
| `wow16/toolhelp/TOOLHELP.DLL` | pending | `DDEECCC103F7` | Original guest group wow16/toolhelp; guest executable/media image; adjacent build metadata: src/mvdm/wow16/toolhelp/Makefile, src/mvdm/wow16/toolhelp/makefile. |
| `wow16/user/edit.h` | pending | `B5E17F84747D` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/editec.c` | pending | `10975935F40E` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/iuser.inc` | pending | `8669936AD394` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/k/daytona/wowkr.asm` | pending | `EBF0F3014A58` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: not established in retained inventory. |
| `wow16/user/listing.inc` | pending | `C13C6D5FBEA6` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/winutil.asm` | pending | `7939DB7F692E` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/wowasm.cmd` | pending | `ECE2FF97CBA8` | Original guest group wow16/user; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/wowasmk.cmd` | pending | `30D5FF78380A` | Original guest group wow16/user; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/user/wsubcls.c` | pending | `BC4E9F995498` | Original guest group wow16/user; guest source or declaration; adjacent build metadata: src/mvdm/wow16/user/Makefile, src/mvdm/wow16/user/makefile. |
| `wow16/wfwnet/bseerr.h` | pending | `D2660A88EB8C` | Original guest group wow16/wfwnet; guest source or declaration; adjacent build metadata: src/mvdm/wow16/wfwnet/Makefile, src/mvdm/wow16/wfwnet/makefile. |
| `wow16/wfwnet/libentry.asm` | pending | `D227E572D9FC` | Original guest group wow16/wfwnet; guest source or declaration; adjacent build metadata: src/mvdm/wow16/wfwnet/Makefile, src/mvdm/wow16/wfwnet/makefile. |
| `wow16/wfwnet/wfwnet.c` | pending | `2E46BB1D5B9C` | Original guest group wow16/wfwnet; guest source or declaration; adjacent build metadata: src/mvdm/wow16/wfwnet/Makefile, src/mvdm/wow16/wfwnet/makefile. |
| `wow16/wfwnet/wfwnet.rcv` | pending | `76941EA02583` | Original guest group wow16/wfwnet; guest export/resource input; adjacent build metadata: src/mvdm/wow16/wfwnet/Makefile, src/mvdm/wow16/wfwnet/makefile. |
| `wow16/wfwnet/WFWNET.RES` | pending | `B464056F6A0E` | Original guest group wow16/wfwnet; guest export/resource input; adjacent build metadata: src/mvdm/wow16/wfwnet/Makefile, src/mvdm/wow16/wfwnet/makefile. |
| `wow16/win87em/emds.asm` | pending | `9A315A40BB12` | Original guest group wow16/win87em; guest source or declaration; adjacent build metadata: src/mvdm/wow16/win87em/Makefile, src/mvdm/wow16/win87em/makefile. |
| `wow16/win87em/emoemwin.t1` | pending | `1BFB4F8B04BC` | Original guest group wow16/win87em; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/win87em/Makefile, src/mvdm/wow16/win87em/makefile. |
| `wow16/winsock/WINSOCK.RES` | pending | `67CEC54BA7EE` | Original guest group wow16/winsock; guest export/resource input; adjacent build metadata: src/mvdm/wow16/winsock/Makefile, src/mvdm/wow16/winsock/makefile. |
| `wow16/wowdeb/wowdeb.def` | pending | `DEC650340062` | Original guest group wow16/wowdeb; guest export/resource input; adjacent build metadata: src/mvdm/wow16/wowdeb/Makefile, src/mvdm/wow16/wowdeb/makefile. |
| `wow16/write/dtabbtn.bms` | pending | `486C516D8F2E` | Original guest group wow16/write; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/dtabmark.bms` | pending | `C8AECC5A299D` | Original guest group wow16/write; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/editsect.c` | pending | `5E072173E2BD` | Original guest group wow16/write; guest source or declaration; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/form1.c` | pending | `8A6B41D5614B` | Original guest group wow16/write; guest source or declaration; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/help.c` | pending | `E252574FF828` | Original guest group wow16/write; guest source or declaration; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/just.bms` | pending | `850F8EF92396` | Original guest group wow16/write; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/kmake.bat` | pending | `8CF92ADA575B` | Original guest group wow16/write; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/looks.h` | pending | `1579AE9CC993` | Original guest group wow16/write; guest source or declaration; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/sp15.bms` | pending | `E6F776DA35A8` | Original guest group wow16/write; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/state.rst` | pending | `F26C74F0A15C` | Original guest group wow16/write; guest ancillary, generated data, library or test input; role requires owner-specific verification; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/WRITE.EXE` | pending | `08EE16597888` | Original guest group wow16/write; guest executable/media image; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/WRITE.RES` | pending | `39D3B445850E` | Original guest group wow16/write; guest export/resource input; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow16/write/write16.exe` | pending | `08EE16597888` | Original guest group wow16/write; guest executable/media image; adjacent build metadata: src/mvdm/wow16/write/Makefile, src/mvdm/wow16/write/makefile. |
| `wow32/fastwow.h` | pending | `0A59CCBB6B0B` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wcall32.c` | pending | `0F72AE39E691` | wow32.c pfnIn registers W32Local*, W32InitDlg, W32GlobalAlloc16/Free16 and W32EditNextWord; local instance identifies guest/native heap; callback paths use CallBack16 and call-owned guest allocations, original dialog init consumes projected WW. |
| `wow32/wddeml32.c` | pending | `13DB1E133CBB` | original DDEML public API/callback thunk provider unselected; actual DDE registration guest GPF and identity remain unresolved, requires bounded DDE package investigation |
| `wow32/wgfont.c` | pending | `383A2AECB42C` | Original GDI ordinal table wgtbl2.h maps EnumFonts to WG32EnumFonts and associated 16-bit enumeration callback wrappers. Link-map entrypoints, GDI table and independent public-host evidence: shard-2/wow-gdi-text-font-metafile-review.md. |
| `wow32/wgman.h` | pending | `191937F2EAE5` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wgmeta.c` | pending | `213B4AA2F1F7` | Original GDI ordinal table maps CloseMetaFile to WG32CloseMetaFile; source handles 16/32-bit metafile conversion and original aliases. Link-map entrypoints, GDI table and independent public-host evidence: shard-2/wow-gdi-text-font-metafile-review.md. |
| `wow32/wgtext.c` | pending | `863EFA2383AF` | Original GDI ordinal table maps ExtTextOut to WG32ExtTextOut; source owns text-output thunk into native GDI. Link-map entrypoints, GDI table and independent public-host evidence: shard-2/wow-gdi-text-font-metafile-review.md. |
| `wow32/winspldl.h` | pending | `1536AFD93C02` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/witbl.h` | pending | `F6C77E7764D7` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wkbtbl.h` | pending | `416AA19BD74B` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wkgthunk.h` | pending | `87E01C6D4E7D` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wmmedia.h` | pending | `7B2E07AC7DA3` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wmmtbl2.h` | pending | `C48E5FC47DF9` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wmsgsbm.c` | pending | `CF1EA6466933` | wmsg16.c WOWCLASS_SCROLLBAR table -> ThunkSBMMsg16/UnThunkSBMMsg16; wumsg.c message entrants use per-call MSGPARAMEX; reverse callback wmtbl32.c SBM entries -> wcntl32.c WM32SBMControl/GetRange/SetRange. |
| `wow32/wmtbl32.h` | pending | `C64CFBDD5B28` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wowtbl.h` | pending | `343409D8ADC9` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wres16.c` | pending | `24324D4B95B7` | Original ConvertDialog16 via WU32DialogBoxParam and common dialogs; ConvertMenu16 via WU32LoadMenu/Indirect; resource records via Find/Load/Lock/Unlock/Free and wkman task cleanup DestroyRes16. Exact upstream identity and direct product object link. |
| `wow32/wres16.h` | pending | `AF060CC040A5` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wshtbl2.h` | pending | `84F782A33328` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wsman.c` | pending | `3481808A0C62` | Original SOUND16 sound.asm emits SoundThunk DOBEEP, sound.def ordinal 17; selected WOW32 wstbl2.h MOD_SOUND maps DOBEEP to WS32DoBeep -> native MessageBeep(0) return. Other sound table ordinals map UNIMPLEMENTEDAPI. |
| `wow32/wsocktbl.h` | pending | `A16351B3392F` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wspool.h` | pending | `11E570DB8724` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wstbl2.h` | pending | `D3AEA894092D` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wthtbl.h` | pending | `8423DFBAB62C` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wuclip.h` | pending | `90600AF10DB9` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wucomm.c` | pending | `C9C8B11D65AC` | wutbl2.h USER COMM ordinals -> original wucomm.c PortTab and COM/LPT host handles; WU32CloseComm -> DeletePortTabEntry; wkman WowOriginalFreeTask -> FreeCommSupportResources(thread ID), which stops after first matching port. |
| `wow32/wucomm.h` | pending | `43D3F49FE9AB` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wudlg.h` | pending | `E92C13CC2AF2` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wuman.h` | pending | `DC0B3173C599` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `wow32/wumenu.c` | pending | `0FC60E98713F` | wutbl2.h USER ordinals -> original wumenu.c create/load/mutate/track/delete/destroy; menu load -> wres16 ConvertMenu16 -> pfnOut server/native create; selected public-USER owned HWND resolver projects only already-published current-worker full handles at host boundary. |
| `wow32/wuser31.h` | pending | `9CC837D93C61` | Frozen product_link_selection and S1 baseline; selection is not behavior proof. |
| `xms.486/xmsdisp.c` | pending | `24B0FA2FE32B` | Original source family xms.486/xmsdisp.c; XMSDispatch at src/mvdm/inc/xmsexp.h:15 Candidate reference names are not proof of execution; inspect the indicated registration/conditional source. |

## Limits and handoff

The identity receipt is not a reclassification of 324 pending rows. Prior-test
references, exact hashes and catalog row counts are preserved per path in the
receipt; the primary reviewer should check their scope and source-to-runtime
closure before any merge. The four native candidates still require failure,
reentry and lifetime tests. The two excluded candidates exclude only historical
guest build inputs, not the delivered guest functions. No product image, guest
file, shared ledger, build cache configuration or published package changed.
