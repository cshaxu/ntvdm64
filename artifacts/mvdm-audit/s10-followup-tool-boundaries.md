# S10 tool and debugger extension file audit

Scope: 57 pending `tool-or-scaffold-candidate` C/H rows in `host-ch-scope/inventory.csv`. Source and selected-build inspection only; no runtime test, no guest changes, no shared ledger changes. Original `sources` manifests specify each target; release Ninja graphs inspected: `build/.ninja/msvc-x86-release/build.ninja`, `msvc-x86-formal-release/build.ninja`, `msvc-x64-formal-release/build.ninja` contain no paths in these four directories. This proves only these captured build graphs, not all possible configs or installed binaries.

The original `bde/sources` is DYNLINK BDE and `bde.def` exports debugger commands; `vdmexts/sources` is DYNLINK VDMEXTS and its def exports DOS/WOW debugger commands; `ieuvddex/sources` is DYNLINK ntvdm with debugger-extension entrypoints in `ntvdm.def`. This `ntvdm.dll` is a debugger extension and is not the NTVDM worker executable. Prior T276 S18/S25 dispositions identify the same boundaries; that history is evidence, not proof the debugger capability was recovered. The separate original `vdmdbg` provider is outside this table and remains S17.

The original vdmutils manifests declare separate UMAPPL utilities: FORCEDOS, GRAFTABL, PIFEDIT and WIN. Excluding their individual implementation from this selected worker DLL/executable build does not establish that equivalent launching, codepage, PIF, or Windows-start functionality is implemented. Retain those product/behavior questions for S12/S17 as applicable; do not treat these utility images as guest or as runtime code.

Disposition except the explicitly pending PIF header: **bounded physical exclusion from the inspected worker build**, not permission to delete source or a claim feature recovered. For debugger outputs S17 must assess debugger-product reachability, diagnostics and lifecycle; alternate architecture IEUVDDEx rows are additionally excluded only from the x86 worker profile. Utility behavior has the S12 product-boundary audit. The actual MVDM host runtime, WOW and debugger API providers must still be assessed separately.

| File | Source role | Original target / evidence | Capability follow-up |
| --- | --- | --- | --- |
| `bde/bde.h` | extension declarations | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/bdecmds.c` | debugger commands | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/bdeheap.c` | heap debugging | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/bdemisc.c` | miscellaneous debugger commands | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/bdestack.c` | stack inspection | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/bdesym.c` | symbol inspection | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/bdeutil.c` | debugger helpers | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/data.c` | extension globals/data | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/disasm.c` | instruction disassembly | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `bde/precomp.h` | precompiled/debugger headers | BDE DYNLINK; bde/sources, bde/bde.def | S17 debugger product |
| `ieuvddex/alpha/helpp.c` | architecture-specific help (non-x86) | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; alpha/sources | S17 debugger product; alternate CPU profile |
| `ieuvddex/data.c` | extension globals/data | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `ieuvddex/help.c` | extension help | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `ieuvddex/i386/helpp.c` | architecture-specific help | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; i386/sources | S17 debugger product |
| `ieuvddex/i386/profile.c` | profile diagnostics | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; i386/sources | S17 debugger product |
| `ieuvddex/i386/reflect.c` | event reflection diagnostics | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; i386/sources | S17 debugger product |
| `ieuvddex/i386/regs.c` | register inspection | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; i386/sources | S17 debugger product |
| `ieuvddex/i386/vdmtib.c` | VDM TIB diagnostic inspection | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; i386/sources | S17 debugger product |
| `ieuvddex/ieuvddex.h` | extension declarations | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `ieuvddex/pmode.c` | protected-mode diagnostics | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `ieuvddex/ppc/helpp.c` | architecture-specific help (non-x86) | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def; ppc/sources | S17 debugger product; alternate CPU profile |
| `ieuvddex/procdep.c` | processor-dependent diagnostic dispatch | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `ieuvddex/system.c` | system-state diagnostics | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `ieuvddex/trace.c` | trace diagnostics | NTVDM debugger extension DYNLINK; ieuvddex/sources, ieuvddex/ntvdm.def | S17 debugger product |
| `vdmexts/data.c` | extension globals/data | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/debug.c` | debugger functions | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/dib.c` | bitmap diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/disasm.c` | instruction disassembly | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/heap.c` | heap diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/help.c` | extension help | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/i386/profile.c` | profile diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/i386/regs.c` | register inspection | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/i386/vdmtib.c` | VDM TIB diagnostic inspection | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/misc.c` | miscellaneous diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/platform.c` | platform detection | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/precomp.h` | precompiled/debugger headers | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/reflect.c` | event reflection diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/stack.c` | stack diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/sym.c` | symbol diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/system.c` | system-state diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/trace.c` | trace diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/util.c` | diagnostic utilities | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/vdmexts.h` | extension declarations | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/wow.c` | WOW diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmexts/wowprof.c` | WOW profiling diagnostics | VDMEXTS DYNLINK; vdmexts/sources, vdmexts/vdmexts.def | S17 DOS/WOW debugger product |
| `vdmutils/forcedos/forcedos.c` | FORCEDOS launcher utility | FORCEDOS standalone UMAPPL; vdmutils/forcedos/sources | S12 utility behavior / product scope |
| `vdmutils/forcedos/forcedos.h` | FORCEDOS declarations | FORCEDOS standalone UMAPPL; vdmutils/forcedos/sources | S12 utility behavior / product scope |
| `vdmutils/forcedos/pif.h` | PIF definitions; same-named `pif.h` is also included by `softpc.new/host/src/nt_pif.c` and `dos/command/cmdpif.c` | FORCEDOS standalone UMAPPL; `vdmutils/forcedos/sources`; include resolution in other profiles not yet proven | **Pending**: trace include paths before classifying this header; S12 PIF boundary |
| `vdmutils/graftabl/graftabl.c` | GRAFTABL console utility | GRAFTABL standalone UMAPPL; vdmutils/graftabl/sources | S12 utility behavior / product scope |
| `vdmutils/graftabl/graftabl.h` | GRAFTABL declarations | GRAFTABL standalone UMAPPL; vdmutils/graftabl/sources | S12 utility behavior / product scope |
| `vdmutils/pifedit/mods.c` | PIF editor settings | PIFEDIT standalone UMAPPL; vdmutils/pifedit/sources | S12 utility behavior / product scope |
| `vdmutils/pifedit/mods.h` | PIF editor declarations | PIFEDIT standalone UMAPPL; vdmutils/pifedit/sources | S12 utility behavior / product scope |
| `vdmutils/pifedit/pifedit.c` | PIF editor GUI | PIFEDIT standalone UMAPPL; vdmutils/pifedit/sources | S12 utility behavior / product scope |
| `vdmutils/pifedit/pifedit.h` | PIF editor declarations | PIFEDIT standalone UMAPPL; vdmutils/pifedit/sources | S12 utility behavior / product scope |
| `vdmutils/pifedit/pifhelp.h` | PIF editor help constants | PIFEDIT standalone UMAPPL; vdmutils/pifedit/sources | S12 utility behavior / product scope |
| `vdmutils/pifedit/pifsubs.c` | PIF editor support | PIFEDIT standalone UMAPPL; vdmutils/pifedit/sources | S12 utility behavior / product scope |
| `vdmutils/win/win.c` | standalone WIN launcher | WIN standalone UMAPPL; vdmutils/win/sources | S12 utility behavior / product scope |

The group classification is grounded in original build targets and current build graph, not directory names alone. The `sources` list identifies C members; header entries are supporting includes, not independently linked binaries. It does not assert that a header cannot be used by another component in an unobserved build; current graph absence is profile-bounded.

No linked-artifact comparison, end-to-end debugger test, standalone utility test or original behavior equivalence was performed here. Prior disposition scripts: `tools/governance/Audit-T276S18IeuvddexDisposition.ps1`, `Set-T276S25IeuvddexFinalDisposition.ps1`, and `Audit-T276S11W0W1Disposition.ps1`.

Header include audit: `rg` across `src/mvdm` finds the unique names `bde.h`, `ieuvddex.h`, `vdmexts.h`, `forcedos.h`, `graftabl.h`, `mods.h`, `pifedit.h`, and `pifhelp.h` referenced within their own product families. `pif.h` has cross-family includes, and `precomp.h` is ambiguous; do not infer their identity by basename alone. The PIF header remains pending until actual include resolution is verified.
