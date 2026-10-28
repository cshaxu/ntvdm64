# Part 5: SoftPC host inputs, historical build resources and suballocator declarations

All 39 owned paths match both frozen original editions. The observer resolves exact paths in VALID `ninja -t deps` records for captured worker/WOW32 graphs, recording up to all object hits for each, and checks full paths in their Ninja files. It also inspects code-section flags for the two selected C objects. Dependency presence is not proof a particular macro or function is active; dependency absence is bounded by cached records and does not prove all profiles or runtime irrelevance.

| Owned key | Captured graph/dependency evidence | Suggested bounded status |
| --- | --- | --- |
| `softpc.new/base/inc/tcpip.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/base/inc/tredltr.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/base/inc/vgastate.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/base/inc/winfiles.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/base/inc/winhelp.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/base/inc/worm.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/base/inc/yoda.h` | graphs none; 11 exact VALID object deps; e.g. worker/obj/ccpu/ccpusas4.obj | Exact current dependency; candidate partial input evidence, not proof every declaration is used. |
| `softpc.new/base/support/get_env.c` | graphs worker; 0 exact VALID object deps | Selected source and object has code sections; pending linked function/actual environment semantics. |
| `softpc.new/base/video/ega_trcr.c` | graphs worker; 0 exact VALID object deps | Selected/compiled object has no code section in captured profile; retain EGA_DUMP/other profile and video behavior separately. |
| `softpc.new/host/inc/dfa.gi` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/host.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/host_gen.h` | graphs none; 242 exact VALID object deps; e.g. worker/obj/ccpu/adc.obj | Exact current dependency; candidate partial input evidence, not proof every declaration is used. |
| `softpc.new/host/inc/host_hfx.h` | graphs none; 4 exact VALID object deps; e.g. worker/obj/bios/bios.obj | Exact current dependency; candidate partial input evidence, not proof every declaration is used. |
| `softpc.new/host/inc/hostgrph.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/hosttyps.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/hstsystm.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/kybdcpu.gi` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/monregs.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/nt_conf.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/nt_fulsc.h` | graphs none; 9 exact VALID object deps; e.g. worker/obj/host/nt_timer.obj | Exact current dependency; candidate partial input evidence, not proof every declaration is used. |
| `softpc.new/host/inc/nt_getxx.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/nt_mon.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/nt_paint.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/nt_smenu.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/sdos_ddi.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/winsigni.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/inc/xmsexp.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/link/nt_uis.h` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/link/resource.rc` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/link/softerr.ico` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/link/softpc.def` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/host/link/softpc.ico` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/obj.vdm/cdefine.inc` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/obj.vdm/ntvdm.prf` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/obj.vdm/resource.rc` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `softpc.new/rename/rename.c` | graphs none; 0 exact VALID object deps | Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency. |
| `suballoc/foo.bar` | graphs none; 0 exact VALID object deps | Historical private-header-like file, not byte-identical to suballcp.h; actual purpose/consumer pending. |
| `suballoc/suballcp.h` | graphs none; 1 exact VALID object deps; e.g. worker/obj/suballoc/suballoc.obj | Exact current dependency; candidate partial input evidence, not proof every declaration is used. |
| `suballoc/suballoc.txt` | graphs none; 0 exact VALID object deps | Historical suballocator documentation; actual allocator behavior stays separate. |

`yoda.h`, `host_gen.h`, `host_hfx.h`, `nt_fulsc.h`, and `suballcp.h` have concrete current-object dependency records; follow their actual usage and original conditionals before marking semantic restoration. Resource scripts and icon/definition inputs may be transformed/generated, so a missing literal graph path does not close their lifecycle. No guest, build tree, product or shared ledger was modified. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/host-input-family-observer.py` and compare JSON.
