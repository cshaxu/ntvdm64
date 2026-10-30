# DPMI task ownership and XMS allocator: combined failure witness

This controlled host fixture links the three current cached x86 production
objects `xmem.obj`, `dpmimemr.obj` and `suballoc.obj` together. It supplies
CPU register seams, a private XMS pool and test callbacks; the callbacks
reserve, commit and decommit actual Windows virtual memory. The injected
callback returns failure before calling `VirtualFree` once. It is not the
actual VDM callback, does not run a guest, and does not establish that a real
`VirtualFree` failure has occurred. Linker `.drectve` warnings occurred.

The test fills a 128 KiB pool with 32 distinct 4 KiB DPMI records. Owner A
owns block 16 and owner B the rest. It then requests owner-A cleanup, frees
owner B's first block, allocates two B blocks, writes a byte to the second,
and requests owner-A cleanup again. `VirtualQuery` reports the second block's
state after that final request.

* Normal control, no injected failure: both allocations eventually reuse A's
  properly freed address; the second A cleanup has no stale record, and the
  second B block remains `MEM_COMMIT` (`0x1000`). Exit 0.
* Injected failure: A's first cleanup leaves its XMEM record, while the
  allocator reports 4096 bytes free. Once B frees a lower address, subsequent
  allocations reuse A's still-tracked address. The second A cleanup decommits
  B's current page; `VirtualQuery` reports `MEM_RESERVE` (`0x2000`). Exit 0
  means the fixture recognized the expected counterexample, not that failure
  behavior passed.

This establishes the combined selected-object failure, including actual
Windows memory states, not merely two independent mock observations. It does
not prove the same owner is retried while another real task remains alive.
The original guest-source trace in [owner review](dpmi-xmem-owner-review.md)
shows one per-child cleanup and a later all-client cleanup, rather than the
fixture's explicit second owner-A call; those paths must not be equated.
Actual VDM callback effects, error incidence, task sequencing and allocator
rollback remain S12 obligations. In particular, do not add a blind retry:
this witness shows why a stale XMEM node alone is insufficient to establish
continued ownership. Guest and product source/binaries were not modified.

Inputs SHA-256:

* Fixture `03E8D369AD08B7F1FB0A41850E4539F25EA7621CD25802FFFA701B9C2712DC1D`.
* `xmem.obj` `E2A12D8FB9485616AAF4170FEBA72DE5DC79C79B2537540F24EF7E20B5DF84CB`.
* `dpmimemr.obj` `D3F07F4BD83EBEDD830593EF979E3CD5059B336B5A468AAD3F8B9923734E7FF4`.
* `suballoc.obj` `F5CF62A66083A173274455CB4714BA7E8C3C9FA27D56A2F548F7AF908169C373`.
* Test executable `945AB39EF1915C45DDE04397860256A728946093F1850E6B626AE0C890B79E10`.

The fixture source is adjacent. Build with x86 MinGW GCC, the repository
`src/mvdm/inc` include path, `-Wl,--gc-sections`, and the three cached objects
above; run with no arguments, with `--inject`, and with `--explicit-inject`. This is isolated
host evidence, not a product regression or a new release.

## Original PSP-selector lifetime follow-up

In the retained `dpmi/dxfunc.asm`, protected-mode client setup calls
`AllocateSelector` in a loop and assigns one of those descriptors to
`selPSPChild`; the guest stores the previous `selPSPChild` in its host-data
record. The `dxstrt.asm` child termination path sends that value in DX to
`TerminateApp`, and restores the parent's saved value afterward. The
`dxutil.asm` allocator takes the next descriptor from a linked free list and
`FreeSelector` puts a descriptor back at its head. Thus the owner key is a
reusable selector in principle, not a unique allocation generation.

This does **not** establish that the terminated child's PSP selector is
freed while a different DPMI client remains active: the observed
`dxfunc.asm` free-selector loop is explicitly the client-setup failure path,
and the `dxstrt.asm` `ReInitGdt` call occurs only when the last DPMI client
leaves. Nor does source alone prove which exact immutable DOSX image is in a
particular runtime. A valid guest witness needs to identify two live client
lifetimes, their actual PSP selectors and the real release/reallocation
point, without modifying or rebuilding the guest. Do not describe the
fixture's second owner-A cleanup as an established production retry.

Reviewed source SHA-256: `dxfunc.asm`
`EA8857F9497CEA63ECD9746AF3E16F222869F0C5C396CF78309CFC506546FBDC`;
`dxstrt.asm` `E67463A6CA5BBE18447AC51123AB3DEC7F3DCCDB1606F10DBE611EE10580F295`;
`dxutil.asm` `C87933E2DA13CD0AA87E9ACF74DEA8254AF83672D4447624C3A775FB03730016`.

## Explicit DPMI free failure without task retry

A third mode tests the actual `DpmiFreeXmem` entry rather than task cleanup.
On the same full pool, the injected decommit failure makes the entry return
CF=1 and retain the original XMEM handle. After the other owner frees an
earlier block and allocates twice, it receives the failed-free address. A
write through the new allocation changes the byte read through the original
address from `73` to `5a`. The fixture reports
`explicit_free_failure_cf=1 same_address=1 original_data=5a` and exits zero.
No second cleanup or selector reuse is needed for this controlled
counterexample. The normal and task-cleanup controls were rerun and retained
the expected states.

This strengthens the actual-object ownership finding: a failed explicit free
can leave a tracked allocation sharing an address with a later allocation.
The decommit failure is still injected at a test callback, not observed from
`VdmDeCommitVirtualMemory` or a real guest. Determine real error reachability,
the original contract, partial multi-page effects and the minimal repair at
the S12 allocator/provider boundary; this test does not by itself authorize a
guest modification or establish that all memory failures can be rolled back.
