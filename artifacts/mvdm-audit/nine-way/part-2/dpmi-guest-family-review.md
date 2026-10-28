# Part 2: DOSX protected-mode guest sources and interfaces

This report covers all 32 owned `dpmi/` keys; the companion JSON records each original/current SHA-256 and each literal reference with path, line and source snippet. Both original editions and the current mirrors match for these files. Captured ZIP, installed and source-tree 486 DOSX.EXE are byte-identical, but this does not prove the original recipe built the binary. The original x86 profile uses `dosxi.lnk`, conditional object inputs and the 486-specific source rules. Real/protected-mode transitions, fault handling, task lifecycle, memory mapping and NetBIOS behavior are not proven by source or byte identity.

| Owned key | Original source/recipe evidence | Suggested bounded status |
| --- | --- | --- |
| `dpmi/486/dxboot.asm` | dpmi/dxboot.asm:2, dpmi/dxboot.asm:8, dpmi/dxstrt.asm:15; 5 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/486/dxutil.asm` | dpmi/dxutil.asm:2, dpmi/dxutil.asm:8, dpmi/makefile:198; 3 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/cmacros.mas` | No literal filename reference in captured DPMI text scan; 0 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/dosx.def` | dpmi/makefile:78, dpmi/mk.cmd:43, dpmi/mk.cmd:53; 3 literal refs total | Recipe prerequisite, exact use by linker unresolved; pending. |
| `dpmi/dxboot.asm` | dpmi/dxstrt.asm:15, dpmi/makefile:109, dpmi/486/dxboot.asm:2; 5 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxbug.asm` | dpmi/makefile:114; 1 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/dxdisk.asm` | dpmi/makefile:118; 1 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/dxend.asm` | dpmi/dxendpm.asm:20, dpmi/makefile:133; 2 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/dxendpm.asm` | dpmi/dxend.asm:26, dpmi/makefile:137; 2 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/dxfunc.asm` | dpmi/makefile:143, dpmi/486/dxfunc.asm:4, dpmi/486/dxfunc.asm:10; 3 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxhpbios.asm` | dpmi/dxstrt.asm:1041, dpmi/makefile:148, dpmi/486/dxstrt.asm:1042; 3 literal refs total | Object rule exists but absent from observed x86 response; alternate profile pending. |
| `dpmi/dxint31.asm` | dpmi/makefile:157, dpmi/486/dxint31.asm:2, dpmi/486/dxint31.asm:8; 3 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxintr.asm` | dpmi/dxint31.asm:26, dpmi/dxmain.asm:726, dpmi/dxnetbio.asm:328; 8 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxmain.asm` | dpmi/makefile:167, dpmi/486/dxmain.asm:2, dpmi/486/dxmain.asm:8; 3 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxrom.asm` | dpmi/makefile:190; 1 literal refs total | Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven. |
| `dpmi/dxrom.inc` | dpmi/dxboot.asm:90, dpmi/dxdisk.asm:36, dpmi/dxemm.asm:40; 31 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/dxstrt.asm` | dpmi/makefile:194, dpmi/486/dxstrt.asm:2, dpmi/486/dxstrt.asm:8; 3 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxutil.asm` | dpmi/makefile:198, dpmi/486/dxutil.asm:2, dpmi/486/dxutil.asm:8; 3 literal refs total | Same-name root source differs from selected 486 rule; alternate profile and semantics pending. |
| `dpmi/dxvcpi.asm` | dpmi/dxmmgr.asm:229, dpmi/dxmmgr.asm:233, dpmi/dxvcpi.inc:47; 5 literal refs total | VCPI object group disabled by observed `!if 0`; alternate profile and VCPI capability pending. |
| `dpmi/dxvcpi.inc` | dpmi/dxboot.asm:87, dpmi/dxdma.asm:37, dpmi/dxend.asm:42; 23 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/dxvcpibt.asm` | dpmi/dxvcpi.asm:100, dpmi/dxvcpi.asm:288, dpmi/makefile:206; 3 literal refs total | VCPI object group disabled by observed `!if 0`; alternate profile and VCPI capability pending. |
| `dpmi/gendefs.inc` | dpmi/dxboot.asm:81, dpmi/dxbug.asm:39, dpmi/dxdisk.asm:32; 56 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/hostdata.inc` | dpmi/dxfunc.asm:74, dpmi/dxmain.asm:133, dpmi/dxstrt.asm:76; 6 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/netbios.inc` | dpmi/dxnetbio.asm:43, dpmi/makefile:182; 2 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/ntintr32.asm` | No literal filename reference in captured DPMI text scan; 0 literal refs total | Original source; selected consumer/profile and behavior pending. |
| `dpmi/program.sed` | dpmi/makefile:101; 1 literal refs total | Historical dependency-rewrite tool input; runtime is separate. |
| `dpmi/prot386.inc` | dpmi/dxvcpi.asm:264, dpmi/dxvcpibt.asm:405, dpmi/makefile:203; 4 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/segdefs.inc` | dpmi/dxboot.asm:80, dpmi/dxbug.asm:38, dpmi/dxdisk.asm:31; 57 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/smartdrv.inc` | dpmi/dxboot.asm:84, dpmi/dxstrt.asm:71, dpmi/makefile:110; 6 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/stackchk.inc` | dpmi/dxfunc.asm:77, dpmi/dxhpbios.asm:38, dpmi/dxint31.asm:56; 13 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/woaswapi.inc` | dpmi/dxfunc.asm:70, dpmi/dxmmgr.asm:97, dpmi/makefile:144; 5 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |
| `dpmi/wowmac.inc` | No literal filename reference in captured DPMI text scan; 0 literal refs total | Original declaration/macro; exact active conditional expansion and all consumers pending. |

The selected 486 input status is a file/build-role recommendation, not a license to delete guest code or remove DPMI functionality. Direct file references can appear in comments, inactive blocks and same-name paths; the cited source/recipe lines must be inspected, and absence of literal references cannot prove absence of consumers. `486/dxmain` is called by a recipe whose prerequisite names root dxmain: this mixed edge must not be flattened into an unsupported identity assumption. Unverified or alternate-path entries remain pending.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-2/dpmi-guest-family-observer.py` and compare JSON. No guest, build, product, shared ledger or external source was modified.
