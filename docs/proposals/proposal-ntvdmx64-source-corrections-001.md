# Selective NTVDMx64 source-correction recovery

## Objective and priority

Audit and apply demonstrated OpenNT/CCPU40 correctness corrections supplied by
NTVDMx64, with the smallest source-shaped changes and no blanket patch import.
Owner requested this separate candidate while authorizing the current five
exception-hook scope corrections. Those five are excluded here.

Place after the existing x87 layout candidate and before WOW/debugger and
WOW16 acceptance: retain x87's already identified layout dependency, then
resolve shared CPU/device/DPMI risks before downstream application claims.
This proposal does not admit implementation or allocate a numeric task.

## Source and decision ledger

Reference: O:\repos.external\ntvdmx64, commit
84a13d2e7bb1a55d11148971e5b9c8ec99f670bf, ntvdmpatch/patches.
Freeze each selected file hash at admission; compare common and old-src/minnt
variants against both selected OpenNT provenance and the actual product graph.
These are candidate repairs, not proven current runtime failures.

| Family | Required decision and verification |
| --- | --- |
| common/spurint.patch | Current DIV-221 already rejects negative PIC acknowledgement before unsigned narrowing in c_main.c. Prove all callers; prefer no additional dispatcher guard when equivalent coverage exists. The earlier filename-based suggestion that this was simply missing is superseded. |
| common/8253.patch | Compare mode-zero first terminal count and subsequent 65536-clock wrap, other modes, zero/reload and elapsed-time boundaries; current timer.c retains the older division. Reproduce before changing. |
| minnt/callconv.patch and generated C-VID declarations | Audit actual selected caller/callee ABI, including thread and WOW callbacks. ErrorDialogBoxThread is already registered PATCH-003. Preserve correct current declarations; no bulk calling-convention edits. |
| minnt/ega_crash.patch | Verify units, zero/negative dimensions, clipping and all selected scaling modes before applying drawing bounds/loop corrections. |
| minnt/ega_align.patch and ega_panning.patch | Prove dirty/full refresh equivalence, non-four-byte start, wrap, split screen and horizontal pixel panning; distinguish missing capability from introduced rendering bugs. |
| common/dpmi.patch | Separate reusable client-teardown/selector/heap repairs from MONITOR rehosting, diagnostics and hacks. Test consecutive clients, freed FS/GS, selector gaps, low/high heap and interrupt state. Guest-source changes require source-policy admission and a matching guest build, never host-only claims over unchanged media. |
| minnt/mouse.patch | Verify coordinate-range scaling against our recovered absolute-coordinate path; do not reintroduce the fixed callback/button defect or duplicate current behavior. |
| common/pcibug.patch | Audit guest PCI BIOS unsupported-function result, carry and return-frame behavior; reject if unchanged media or unreachable owner prevents meaningful deployment. |
| old-src/fpu.patch | Existing x87 proposal is the sole implementation owner; consume its results, do not duplicate its layout work. |
| common/ccpu.patch remainder and patch.cmd | Review debug-register/accessor and generated C-VID call-to-goto changes for selected-build relevance; no automatic import of debug features or old vector layouts. |
| common/ccpu_segperm.patch | Explicit upstream HACK with unproved processor semantics: deny by default; require architectural and original-source evidence before any admission. |
| common/cvidcheap.patch, ccpu_optim_*, sas.patch | Do not import fixed scratch buffers or bulk optimization rewrites as correctness repairs. Isolate any genuinely demonstrated defect from these unrelated changes. |
| common/printerprt.patch | ACK oscillation is a compatibility policy, not established hardware equivalence. Retain as rejected/default-off candidate unless separately justified and admitted. |

## Proposed execution groups

1. Freeze the hunk-level adopted/equivalent/missing/unreachable/rejected ledger
   and complete caller/build/source-owner review. Name tests and original-source
   alternatives for every accepted group; do not count patch files as bugs.
2. Recover the accepted CPU/device/ABI corrections as coherent profiles; run
   negative controls and x86 /MT regression after each group.
3. Recover accepted rendering and DPMI lifecycle groups through original owners,
   with matching guest artifacts where required. Keep rejected hacks out.
4. Fresh formal three-program build, actual COMMAND/MEM/EDIT and appropriate
   protected-mode/graphics workloads; publish only tested products, measure
   mirror/overlay/new-runtime-code deltas, commit/push and clean closure.

## Boundaries and exit criteria

Follow source-policy recovery order and register all retained changes. No
injector, HAXM, driver, CSRSS, x64 product or CPU30 profile. No newly invented
provider where original source composes. Preserve upstream notices and exact
patch attribution. SoftPC patch evidence belongs in mvdm-softpc-patch; guest or
non-SoftPC changes need explicit applicable admission, not an expanded exception.

Every ledger row ends with evidence-backed adopt, already-equivalent, reject or
an explicit named receiver. A missing test is not a pass. Real integration,
negative controls, clean committed/pushed state and measured source deltas are
mandatory; neither patch presence nor COMMAND success alone closes this package.
