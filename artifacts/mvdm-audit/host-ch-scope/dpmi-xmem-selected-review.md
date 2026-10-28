# Selected DPMI XMEM object: resize and free failure controls

The [fixture](dpmi-xmem-selected-fixture.c) links the actual selected x86 `dpmi/xmem.obj` directly and supplies test-owned CPU-register and allocation-provider seams. [Runner](dpmi-xmem-selected-run.py) and [receipt](dpmi-xmem-selected-observation.json) record exact inputs, output and hashes; no product or guest source/object is modified. Selected object SHA-256 `E2A12D8FB9485616AAF4170FEBA72DE5DC79C79B2537540F24EF7E20B5DF84CB`; current source SHA `FC05182AA46C78DEE2A963D5D10A83DEB8B34D84114725D6F0D5D334B0FA29B0`; the observed test EXE SHA `9F313FA3AAFC4C45110C65D7609F7F26F49F7F8D2CA9BC29A4612004C268B64B`. The MinGW linker emitted its `.drectve` warning but succeeded. The current cached `ntvdm.exe.map` attributes the actual `DpmiAllocateXmem`, `DpmiFreeXmem`, `DpmiReallocateXmem` bodies to original-mvdm-dpmi32:xmem.obj; it is not independently bound here to installed release.

Original `dpmi32.c:57-59,66` dispatches allocate, free, reallocate and owner cleanup to this source family. `DpmiAllocateXmem` gets a provider allocation before creating its tracking node and publishing the node handle; reallocate uses the existing handle; free asks the provider before unlinking/freeing the node. Current `xmem.c` has registered mirror exception MVDM-HOST-DIV-272: the reallocation provider returns BOOL and must be checked as such. Original source identity, versioned guest lineage, the current provider bridge and remaining scope are documented in [DPMI allocator review](../nine-way/part-2/dpmi-i386-allocator-semantic-reconciliation.md).

The new process returned 0 after asserting three controlled flows:

* A fake successful allocation at `40000h` of `1000h`, followed by fake reallocation FALSE, gives CF=1, preserves the published node handle, and the later successful free receives the **original** address/size.
* Fake reallocation TRUE returns new base `50000h` and updates the size to `2000h`; CF=0 and BX:CX publishes the new base; the later free receives that new address and size with the same tracking handle.
* A fake free failure yields CF=1; a second free after the seam is changed to succeed invokes the provider again and returns CF=0. This shows the same fixture handle remains usable in this one failure/retry sequence.

These are actual selected-object branch observations with deliberately simulated callbacks and register storage. They are not native allocation/reallocation, a real failed pool or host API, installed-image proof, guest reachability, or proof of task cleanup, invalid handle robustness, simultaneous tasks, data-copy semantics, or every failure point. In particular do not read a fake retry as evidence every real failed decommit leaves coherent host backing. S11/S12 retain the real XMS/CPU40 pool ownership, actual failure/rollback, guest/PSP cleanup, descriptor lifetime and repeated/abnormal termination; S19 binds the package. The controlled result supports current DIV-272 BOOL branch and bounded node preservation, not full DPMI recovery.

Replay from the repository root with selected worker objects and bundled i686 MinGW:

```powershell
python artifacts/mvdm-audit/host-ch-scope/dpmi-xmem-selected-run.py
```

Temporary executable and logs go under `build/t473-s10/dpmi-xmem-probe`. No previously blocked probe, guest or production mutation is involved.
