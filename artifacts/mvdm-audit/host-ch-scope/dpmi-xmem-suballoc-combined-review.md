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

* Fixture `0FF982B208D857E0C0F2EA203802CDAECFEA902BEFDA6FD66251EBC064495927`.
* `xmem.obj` `E2A12D8FB9485616AAF4170FEBA72DE5DC79C79B2537540F24EF7E20B5DF84CB`.
* `dpmimemr.obj` `D3F07F4BD83EBEDD830593EF979E3CD5059B336B5A468AAD3F8B9923734E7FF4`.
* `suballoc.obj` `F5CF62A66083A173274455CB4714BA7E8C3C9FA27D56A2F548F7AF908169C373`.
* Test executable `3D5D798052C84420E958C4F49A50CEC0D5C13840349C2E60ACA08C20B02E5B54`.

The fixture source is adjacent. Build with x86 MinGW GCC, the repository
`src/mvdm/inc` include path, `-Wl,--gc-sections`, and the three cached objects
above; run once with no arguments and once with `--inject`. This is isolated
host evidence, not a product regression or a new release.
