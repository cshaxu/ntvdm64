# VDD uninstall and DMA channel boundary: S13 repair recheck

No product or guest was changed, and no test was rerun in this review. Existing actual-object results and immutable hashes are in `../vdd-native-role-review.md` and `../vdd-object-runs/manifest.json`.

## Actual entry and failure

`src/opennt-abi/source/public/sdk/inc/nt_vdd.h` declares these VDD APIs. Original sample VDD code demonstrates call shape: `vdd/samples/vsbd/vdd.c:529,554,708,754` installs/uninstalls port hooks and uses DMA; this is a source example, not proof a released guest loads that VDD. The current selected original `nt_vdd.c` implements the APIs and routes port registration/disconnection to original IOS and DMA access to `dmaGetAdaptor`. Existing controlled fixture links the actual selected VDD/IOS/CCPU I/O objects, not guest/device hardware.

`GetVddAdapter` reserves index 0–6 and returns `SPARE_ADAPTER1 + index` (36–42). `FreeVddAdapter` clears that slot but returns the raw index. `VDDDeInstallIOHook` interprets return zero as no owner and otherwise passes that raw number to `io_disconnect_port`. Thus slot zero exits before any disconnection and the other six disconnect using the wrong adapter. Existing strict actual-object witness reports 14/57 failed checks and retained callbacks for all seven; it does not exercise DLL unload or concurrent install/uninstall.

All three `VDDRequestDMA`, `VDDQueryDMA`, `VDDSetDMA` check `iChannel > 4*2`, then index `controller[channel/4]`; 8 passes and indexes controller 2 of the two-element array. The documented invalid-channel error is `ERROR_INVALID_ADDRESS`. Existing guarded actual-object strict fixture reports 7/8 failures and a guard modification for Set, while 0–7 controls passed. Query's returned data is deliberately replaced before the Set check to prevent a false unchanged-guard result.

## Minimal candidate and explicit boundaries

For the proven failures, the minimal mirror edit is **four source-line replacements in `nt_vdd.c`**: make `FreeVddAdapter` return `w + SPARE_ADAPTER1`; change each of the three DMA comparisons from `>` to `>=`. This preserves zero as not-found and rejects channel 8 before obtaining/indexing the controller. There are two callers of `FreeVddAdapter`: the uninstall consumes the return, while install's failed-connect rollback ignores it; test both. Record the new mirror divergence and exact baseline/line count per source policy. No guest, BOP identity, API signature, adapter capacity or other behavior need change for these four edits.

This minimal correction does **not** by itself prove concurrency-safe teardown: `FreeVddAdapter` currently releases the slot before disconnecting ports, so a concurrent reinstallation could reuse that number before old ports are retired. Verify actual permitted threading/serialization and callback retirement; if concurrent callers are admitted, implement the smallest existing-owner synchronization or ordered release needed, with separate exception evidence. Likewise, `WORD` inclusive port loops can wrap at `0xffff`; this is an independently unverified boundary, not automatically fixed by the four edits. The existing seven-slot calculation versus eight named adapter constants is an independent capacity contract and must not be changed without determining original intent.

## Required verification

1. Rebuild selected MSVC x86 original object and bind the test to its hash/map. Run normal `verify-softpc-object-contract.ps1 -Contract vdd`; retain all 305 original controls, ABI negative and no-device test boundaries.
2. Both existing strict modes must pass unmodified: all seven slots detach old read/write ports, wrong/nonowner and repeated uninstall are safe, and DMA channel 8 returns the correct failure/LastError before any controller access or guard write. Also cover channels 0, 7, 9, maximum WORD, Request/Query/Set independently.
3. Add success/reinstall, unrelated-owner preservation, partial connect rollback, callback retirement and deterministic permitted concurrency tests; test inclusive `0xffff` range without turning a hang into a timeout workaround. If real VDD module load/unload or device cases are not covered, say so explicitly.
4. Run required affected product regressions with original guest unchanged; separately report any environmental capability absent. Static four-line reasoning and the existing failed fixture do not establish a repaired runtime.

The original source hashes, image byte-attribution and controlled witnesses prove source defects under the existing fixture, not their incidence in a live DOS or Win16 program. Shared ledger was not changed.
