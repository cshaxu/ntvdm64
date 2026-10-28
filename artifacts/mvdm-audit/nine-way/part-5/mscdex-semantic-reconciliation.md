# Part 5: MSCDEX guest/VDD request, failure and lifetime boundary

This refines the eleven owned MSCDEX sample rows in `vdd-samples-family-review.md`; it also uses `artifacts/mvdm-audit/shard-0/mscdex-package-review.md` for identity-bound package observations. It does not classify MSCDEX as irrelevant merely because host VDD source lives under samples.

Original `tsr/mscdexnt.asm` hooks INT 2F; on first supported request it attempts `RegisterModule` for `VCDEX.DLL` with VDDRegisterInit/VDDDispatch and caches its registration check (`fVDDChecked`). Success dispatches through the VDD; failed drive-count/check requests take specific zero/not-CD fallback paths, not synthetic success. `vdd/vcdex.c` discovers CD-ROM drives and owns its allocated DRIVE_INFO and native handles; UserBlock/Terminate hooks and DLL process detach close retained handles. No drive is an original initialization failure. The host module loader is `nt_bop.c::ISV_RegisterModule`; Windows owns actual optical handles. Guest TSR owns its hook and VDD handle, while a loaded provider owns native CD state.

A concrete original-source lifecycle concern needs evidence before import: after a drive opens, `vcdex.c:230-240` returns on `HeapAlloc` failure before the later `CloseHandle(hDriver)` at line 262; earlier allocated drive records may also already exist. This is a **source-level error-path risk**, not a demonstrated leak in the current product, because the selected worker does not build this sample VDD. Do not reproduce by modifying real guest or importing the historical binary. If a future selected provider uses this source, audit every partial-init rollback, task hook and detach before acceptance.

| Owned path | Role and bounded classification |
| --- | --- |
| `vdd/samples/mscdex/readme.txt` | Historical install and TSR/provider description; no claim that its ordering or all machines' config matches current profile. |
| `vdd/samples/mscdex/tsr/messages.inc` | Original TSR message input used by makefile; exact locale selection unproved. |
| `vdd/samples/mscdex/tsr/messages/usa/messages.inc` | Original USA message variant; do not collapse by basename. |
| `vdd/samples/mscdex/tsr/mscdexnt.asm` | Original guest INT 2F interception, lazy VDD registration and explicit fallback. Preserve immutable guest behavior. |
| `vdd/samples/mscdex/tsr/mscdexnt.exe` | Current-only source-tree binary with exact ZIP/installed match per independent observer; its build provenance is not proved. Shipped guest identity is meaningful even though named sample. |
| `vdd/samples/mscdex/tsr/mscdexnt.inc` | Original TSR interface/include input; exact conditional expansion pending. |
| `vdd/samples/mscdex/vdd/mscdex.h` | Original C packet/interface declarations; selected provider and ABI require independent proof. |
| `vdd/samples/mscdex/vdd/vcdex.c` | Original separately compiled provider implementation; current host graph does not select it. Future import must resolve partial initialization and lifecycle. |
| `vdd/samples/mscdex/vdd/vcdex.def` | Original provider exports; name equality alone does not prove ABI or a loaded provider. |
| `vdd/samples/mscdex/vdd/vcdex.h` | Original provider declarations; not a separate device owner. |
| `vdd/samples/mscdex/vdd/vcdex.rc` | Original VDD resource input; separate host image, not guest TSR code. |

The shard-0 observer found the named VCDEX DLL absent from standard release, system32 and ZIP at its observation; historical sample DLL exists and its 12 imported names occur in that worker's exports. This establishes a **candidate missing-provider edge**, not failure for every deployment: alternate search locations and runtime paths are not ruled out. Historical T420 debugger logs mention loading MSCDEXNT only, not successful CD reads. No runtime test occurred here.

S17 should follow the actual immutable TSR through first request and native loader against a fresh coherent package, verify absent-provider original fallback, and establish the exact VCDEX ABI/provider availability. If a suitable read-only optical fixture exists, verify initialization, read, no media, task block/termination and detach; inject allocation/open failure only through a controlled host fixture. A source-derived provider requires minimal audited rollback and project source-policy review; do not unconditionally ship a historical binary or claim hardware support from name-level export matches. S19 binds all results to specific release and guest hashes.

Replay source search `rg -n "RegisterVDD|fVDDChecked|VDDRegisterInit|UserBlockHook|DLL_PROCESS_DETACH|HeapAlloc|CloseHandle" src/mvdm/vdd/samples/mscdex`, and `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-0/mscdex-package-observer.py`. No products, guests, shared ledger or live devices changed.
