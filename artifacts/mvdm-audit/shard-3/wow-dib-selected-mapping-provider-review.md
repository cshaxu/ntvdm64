# S10 DIB actual emulated-address provider correction

This supplements `wow-dib-provider-lifecycle-review.md` and corrects the branch attribution in `docs/etc/evidence/m0-t455-s3-dib-palette-boundary.md:13`. No guest, product, build, or shared ledger was modified; no test was run.

## Object and release binding

The current cached WOW32 `wdib.obj` SHA-256 `DC2BAE3CA643F70065F6CF05635A38B6FB22BE7041ACCD8BB1E1C551F1ECC813` has external COFF references to `VdmAddVirtualMemory`, `VdmAddDescriptorMapping`, `VdmRemoveVirtualMemory` (verify with `objdump -t`). These calls are inside `#ifndef i386` in `wdib.c:299-320,424,657-700,738,838`; this is direct object evidence that the observed product build compiles the emulated-address branch, **even though the host image is x86**. The cached WOW32 link map imports all three from NTVDM. `generic-thunk-package-observations.json` establishes complete `.text` equality between cached WOW32, published and installed WOW32; hence this call-site code matches the observed published DLL. The cached worker map attributes Add/Remove to `original-softpc-host-roots:nt_mem.obj` and descriptor mapping to `original-mvdm-dpmi32:dpmiselr.obj`; worker cached/installed whole-file hashes differed in the previous audit, so this alone does not bind those exact provider bodies to the installed worker.

`nt_mem.c::VdmAddVirtualMemory` reserves Intel virtual space, maps its physical records to the host buffer and flags the mapping for later removal. `dpmiselr.c::VdmAddDescriptorMapping` allocates a mapping record, prepends it and projects selector flat addresses. These are genuine resources and state; imported provider calls cannot be treated as omitted or as native x86 pointer identity. Existing T455 fixture proves a particular mapping round trip, not actual guest thunk failure and cleanup.

## Specific cleanup questions (static, not reproduced)

* In DIB.DRV path, after `VdmAddVirtualMemory` succeeds, descriptor mapping, `RET_SETDIBSEL`, or `W32AddDibInfo` may fail; `hdd_err` does not visibly call `VdmRemoveVirtualMemory`, remove a newly registered descriptor mapping, unmap the view, or reverse a completed selector replacement. Inspect provider ownership/callback guarantees and fault-inject each boundary before a correctness conclusion.
* In `WG32CreateDIBSection`, a successful virtual map is followed by a selector callback. Failure of that callback goes to `cds_err`, which deletes bitmap but has no visible `VdmRemoveVirtualMemory`; if descriptor mapping fails after selector allocation, same branch does not visibly free selector or virtual mapping. The distinct node-allocation failure branch *does* call the selector-free callback and `VdmRemoveVirtualMemory`, then deletes the bitmap. Whether each source condition is reachable and downstream ownership survives must be verified.
* `dpmiselr.c::VdmAddDescriptorMapping` inserts mapping records; trace descriptor removal on normal guest free, failed callback and task death rather than assuming `VdmRemoveVirtualMemory` frees descriptor records.

Retain DIB file and feature as pending. These observations do not prove a reproduced fault, nor justify indiscriminately switching to the `i386` branch or replacing original mappings.

Replay:

```powershell
Get-FileHash build/.ninja/msvc-x86-release/wow32/obj/wdib.obj -Algorithm SHA256
objdump -t build/.ninja/msvc-x86-release/wow32/obj/wdib.obj | Select-String 'VdmAdd|VdmRemove|WG32CreateDIBSection'
rg -n 'VdmAddVirtualMemory|VdmRemoveVirtualMemory|VdmAddDescriptorMapping' src/mvdm/wow32/wdib.c build/.ninja/msvc-x86-release/wow32/wow32.dll.map build/.ninja/msvc-x86-release/worker/ntvdm.exe.map
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/generic-thunk-package-observer.py
```
