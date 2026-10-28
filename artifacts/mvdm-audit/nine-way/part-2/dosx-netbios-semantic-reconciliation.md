# Part 2: DOSX protected-mode NetBIOS declarations and route

The owned `dpmi/netbios.inc` is byte-identical to both frozen originals (`dpmi-guest-family-observations.json`), and original `dpmi/dxnetbio.asm:43` includes it; `makefile:182` references the declaration and `dosxi.lnk` includes dxnetbio.obj. This is sufficient to classify the exact file as original guest interface/build input. It defines NCB fields and return/command codes, not a host network provider or proof that the released DOSX binary came from this source.

Original `dxnetbio.asm:970-1027` protected-mode INT5C path, when `PM_NCB_HANDLING` is active, detects presence requests and rejects their documented special case, otherwise WOW_x86 emits REDIR BOP `SVC_NETBIOS5C`. Original `src/mvdm/vdmredir/vrdisp.c:96-97` selects `VrNetbios5c` and asynchronous-completion dispatch; `vrnetb.c` queues completion and `vrinit.c` owns the completion queue. Guest NCB and post routine remain guest-owned; VDMREDIR owns translated host operation and queued completion; the modern host owns its Netbios endpoint. Conditional selection and an exact real protected-mode DOSX caller remain to be demonstrated; the DOS INT5C route alone does not prove this path.

| Owned path | Evidence-supported file role and boundary |
| --- | --- |
| `dpmi/netbios.inc` | Original DOSX NetBIOS command/status/NCB declarations; explicitly included in the historical dxnetbio source and build rule. Preserve it; runtime PM translation and binary provenance remain pending. |

The historical `m0-t420-s20-vdmredir-operation-matrix.md:43-65,239-251` records a real DOS asynchronous guest post on its identified formal-x86-013 package and a cancellation returning original NRC_CANCEL after request completion. That witness does not prove cancellation of an actually pending request, other host versions, or this DOSX protected-mode branch. The same record labels `VrCheckPmNetbiosAnr` as a protected-mode WOW helper with no DOS direct caller, rather than inventing one. Owner-approved DLC/RAP exclusions there do not eliminate NetBIOS.

S11/S14 should use an immutable protected-mode caller that actually reaches the DOSX branch to verify NCB translation, synchronous/async return, pending cancellation, guest callback after task loss and buffer/queue cleanup; S12/S17 handle DOS redirector/provider behavior, S19 binds exact binary and package. If no host LANA is available, report that distinctly; never fake a completion or claim all networking from a DOS witness. No runtime tests or guest/product/shared ledger changes.

Replay `rg -n "netbios.inc|dxnetbio.obj" src/mvdm/dpmi/makefile src/mvdm/dpmi/dosxi.lnk src/mvdm/dpmi/dxnetbio.asm`; inspect PMIntr5C and `vrdisp.c:96-97` and cited T420 evidence; rerun part-2 DPMI observer.
