# M0 T473 S1 — unselected host-source disposition

## Scope and rule

This ledger disposes of the 122 current `*.c`/`*.asm` paths that are neither
selected by the `ntvdm.exe`, `VDMREDIR.dll`, or `wow32.dll` product edge nor
immutable guest/media inputs. It is derived from
`build/t473-s1/mvdm-union-node-r25/mvdm-union.csv`. A disposition is not a
claim that an excluded feature never existed; it says whether the current
CPU40 x86 product may silently add the path as a host provider. It may not.

The 18 unselected module definitions and 25 unselected non-code build inputs
follow the same family disposition. `vdmredir/vdmredir.def` is the one
exception: it is an actual `VDMREDIR.dll` linker argument (the selector reports
compiled source objects only), and is therefore retained as an active module
definition rather than a recovery candidate.

## Evidence-backed exclusions

| Count | Paths | Disposition and evidence |
| ---: | --- | --- |
| 8 | `bde/*.c` | Historical debugger-engine profile, not a CCPU40 product provider. The prior package audit places `bde` outside the current product profile; it requires a separate debugger admission. |
| 3 | `sim16/{sim16,siminit}.asm`, `sim32/sim32.c` | Retired simulator/profile tooling, not an alternative CCPU40 executor. The selected CPU40 carrier is a hard architecture boundary. |
| 9 | `vdd/samples/{adlibvdd,hpscan,mscdex,sample1,sample2,vsbd}/**/*.{c,asm}` | Original example VDDs. They demonstrate external device contracts but are not a product device selection. |
| 5 | `fax/wowfax/wowfax.c`, `fax/wowfaxui/{wfhelper,wfsheets,wfupgrad,wowfaxui}.c` | Optional Fax product profile. No selected caller, DLL boundary, or release component exists. |
| 14 | `ieuvddex/{alpha,i386,mips,ppc}/*.c`, `ieuvddex/{data,help,pmode,procdep,system,trace}.c` | Optional Internet Explorer VDD extension profile. It cannot be selected without a separately bounded external-client contract. |
| 19 | `vdmexts/**`, `vdmdbg/vdmdbg.c` | Debugger extension/profile. The prior runtime-package audit marks these outside the product closure; they are not a substitute for the selected worker debugger bindings. |
| 6 | `vdmutils/{forcedos,graftabl,pifedit,win}/*.c` | Standalone original utilities, each requiring its own executable/admission rather than linkage into NTVDM. |
| 19 | `softpc.new/{bios,roms}/**/*.asm`, `softpc.new/{convert,dat2obj,debugger,obj.vdm,rename}/**/*.c` | Firmware, generator/converter, debugger, or object-generation inputs. They are not host runtime bodies under the immutable-media and CPU40 build profile. |
| 9 | `softpc.new/host/src/{nt_aorc,nt_ertbl,nt_hfx,nt_hunt,nt_mess,nt_smenu,nt_term,nt_vflop,stf_conf}.c` | The original host manifest has no i386 selection for these bodies (`nt_aorc` is PPC/MIPS only); no current product edge may add them by inference. |
| 8 | `wow32/{debug,wctlm32,wcurcash,wddeml32,wkglobal,wklocal}.c`, `wow32/i386/{callpr32,fastwow}.asm` | Dormant/diagnostic/profile-only WOW32 bodies: the original manifest places the generated table path behind `!IF 0`. They cannot be mixed with the selected table/object composition without a complete provider audit. |
| 1 | `oemuni/toemuni.c` | Original `UMTEST=toemuni` test application, not a member of the selected `oemuni.lib` (`file.c`, `process.c`). |
| 2 | `suballoc/{debug,tsa}.c` | Diagnostic body and `UMTEST=tsa`; the selected original library member is `suballoc.c`. |

## CPU40/profile alternatives retained as explicit boundaries

| Count | Paths | Why whole-file import is rejected |
| ---: | --- | --- |
| 2 | `softpc.new/base/ccpu386/{ntstubs,vglob}.c` | `ntstubs.c` duplicates CPU/FPU/SAS state against selected `fpu.c` and `ccpusas4.c`; `vglob.c` is an alternate GDP profile. T420 CCPU package evidence records the selected one-leaf `sas_overwrite_memory` subset and forbids linking the competing whole state provider. |
| 12 | `softpc.new/base/{bios/bootstra.c,cpu/src/evid/{ev_glue,ev_stubs}.c,cvidc/{ev_stubs,sascdef,vglob}.c,dasm386/{build,dasm386,decode}.c,system/{cmos,quick_ev}.c,video/ga_glue.c}` | Either absent/commented from the original i386 manifest, generated/event alternate carriers, or PPC-only disassembler sources. The current original C-VID, CMOSNT and CPU40 selection is the authority; an alternative may not be introduced one object at a time. |
| 2 | `dpmi32/i386/{dpmimem,dpmimisc}.c` | Native kernel-VDM i386 implementation. It expects passive `CONTEXT`/`VDM_TIB` monitor state unavailable to CPU40. T420 DPMI package recovery retains original portable `dpmimemr.c`/`modesw.c` as the correct standalone owner. |
| 1 | `xms.486/i386/xmsmem86.c` | Native monitor callback carrier, not the selected CPU40 XMS body. It remains outside the standalone product boundary. |

## Product-facing boundary still requiring package proof

| Count | Paths | S1/S2 disposition |
| ---: | --- | --- |
| 1 | `dbg/dbg.c` | Built only by the debugger fixture, not linked into the product. `dbg` remains a partial host boundary: S2 must either establish a complete original debugger package and callers or retain the current fixture-only role with an explicit no-product-provider decision. |

No row in this ledger is a silent “optional” exclusion. The excluded families
are retained inventory evidence with their original role and re-admission
condition. The only remaining unselected product-facing source owner is the
single `dbg` package boundary; the separate 195 semantic mirror differences
remain governed by the semantic-delta ledger and become the next finite S2
reconciliation group.
