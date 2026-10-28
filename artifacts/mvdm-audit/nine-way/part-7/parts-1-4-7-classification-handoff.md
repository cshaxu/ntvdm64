# Parts 1, 4 and 7: limited evidence-backed classification suggestions

Coordinator note: this is a historical handoff, not the current ledger.
Some scoped guest exclusions and image-only partials have since been reopened;
look up the current per-file status before using any suggestion below.

This is a role/identity handoff, not behavioral completion or a proposal to delete code. Coordinator controls shared ledger. All paths below are relative to `src/mvdm` unless an external path is explicitly identified.

## Part 1

* `dos/v86/cmd/graphics/graphics.pro`: runtime printer-profile data; retained source, ZIP and exact installed system32 copy match SHA. `part-1/dos-graphics-review.md` / `dos-graphics-evidence.json`. Actual printer operation remains pending.
* `bde/{bde.def,bde.h,bdecmds.c,bdeheap.c,bdemisc.c,bdestack.c,bdesym.c,bdeutil.c,data.c,disasm.c,precomp.h}`: source-defined standalone NTSD debugger extension input/interface, not a worker CPU provider; original `bde/sources` builds separate DLL. See `part-1/bde-review.md`. The delivery/ABI policy and debugger behavior remain pending; absent graph edge is not grounds to discard capability.
* External key `SRC:0e87b2e6fc2615658f92` (`EGA.CPI`): shipped data identity verified in ZIP and installed; source-level conditional load and fallback only. See `part-1/external-font-review.md`; actual CPI access and codepage use pending.
* Other specific DOS keyboard, utility and bin86 classifications are individually recorded in `part-1/dos-keyboard-review.md`, `dos-utilities-review.md`, `bin86-review.md` and their per-key evidence, but runtime functionality and variants stay independently pending.

## Part 4

* `softpc.new/base/disks/gfi_test.c`: original non-PROD GFI trace shim; compiled graph edge does not imply active PROD function. `shard-3/gfi-debug-shim-audit.md`; actual floppy provider/runtime separate.
* `softpc.new/base/debug/dasm.c`, `ega_dump.c`: PROD objects with no executable section in captured x86 build; `profile.c` has fallback functions but no release map owner, while `trace.c` has retained map data symbols. `shard-2/base-debug-objects-review.md`. This disposition is bounded to captured configuration, not a blanket no-debug classification.
* `softpc.new/base/inc/j_c_lang.h`: normalized VALID dependency observed for a test fixture, not a demonstrated production dependency, per `part-4/mvdm-evidence.json`. Other graph hits alone remain pending binding.
* The 4,773 external NT4 `rasprint/mini` rows in `part-4/external-mini-evidence.json` have exact source identity as printer minidriver sources/data, including IFI/PFM build relationship; they remain pending per-provider applicability and no blanket product exclusion is justified.

## Part 7

* `wow16/drivers/keyboard/keyboard.drv`, `wow16/drivers/mouse/mouse.drv`, `wow16/mciole/MCIOLE16.DLL`: retained NE guest images byte-identical to named ZIP members (`part-7/wow16-evidence.json`); no runtime or source provenance conclusion.
* `wow16/mmsystem/mmtask.tsk`, `wow16/mmsystem/mmtask/MMTASK.TSK`: two byte-identical retained task stub guest images; `mmtask/mmtask.asm`, `mmtask/MMTASK.RES`, `messages/usa/mmtask.rcv` are original guest build inputs/resources. Precise image-to-source and runtime deployment remain pending. See `part-7/mmtask-review.md`, `mmtask-ne-search-review.md`, `mmtask-search-context-review.md`.
* `wow16/mmsystem/{drvr.c,drvrrare.c,drvrstr.c}`: original historical driver-table C source; original selected recipe lists drvr31.obj instead. Image behavior remains pending (`part-7/multimedia-driver-review.md`).
* `wow16/mmsystem/timea.asm`: retained alternate timing assembly; original examined recipe uses time.c/stack.asm instead. Binary selection/timer lifecycle still pending (`part-7/multimedia-timer-review.md`).
* `wow16/mmsystem/{mmiocf.c,mmiocf.h}`: original incomplete compound-file source and declarations, not listed by examined recipe. Ordinary RIFF uses other listed objects; both actual capabilities pending (`part-7/mmio-compound-review.md`).
* `wow16/gdi/{meta.c,metarec.c,metasup.c}`: retained historical C source not listed in original observed GDI recipe; source-level thunk path separately documented in `part-7/gdi-metafile-provider-review.md`; image and runtime semantics pending.
* `wow16/rasapi16/{bseerr.h,ras.h,rasapi16.c,rasapi16.rc,RASAPI16.RES}`: original guest thunk build inputs/resource (`part-7/ras-provider-review.md`); source calls six legacy host names absent in this host's observed export table; live invocation and other environments pending.
* 4,773 OpenNT raster-mini external rows remain specific provider-applicability pending (`part-7/external-mini-review.md`); source census is not functional classification.

No guest or product modifications, shared-ledger updates, live runtime or full integration tests were performed for these classifications.
