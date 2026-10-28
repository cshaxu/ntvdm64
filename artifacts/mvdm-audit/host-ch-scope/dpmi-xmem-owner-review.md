# DPMI XMEM owner cleanup: selected-object controlled failure

This independently executes current selected `xmem.obj::DpmiAllocateXmem`, `DpmiFreeAppXmem` and `DpmiFreeXmem` in an x86 host fixture, with test-owned register storage and allocator callbacks. The [fixture](dpmi-xmem-owner-fixture.c), [runner](dpmi-xmem-owner-run.py) and [receipt](dpmi-xmem-owner-observation.json) bind selected object, source and result hashes. The selected object SHA-256 is `E2A12D8FB9485616AAF4170FEBA72DE5DC79C79B2537540F24EF7E20B5DF84CB`; no product or guest object/source is rebuilt. The i686 linker emitted the known `.drectve` warning but produced the probe.

Three fake allocations produce actual XMEM tracker nodes for owner A (address 40000), owner B (50000), owner A (60000). A single controlled provider failure is injected for address 40000 during owner-A cleanup:

```text
OWNER first calls=2 a=00060000 b=00040000 cf=0
OWNER retry calls=3 c=00040000
OWNER other calls=4 d=00050000 cf=0
```

The first traversal frees the most-recent A block, then calls the provider for the older A block and returns when that callback fails; no other owner is freed. CF stays at its supplied zero because this original void cleanup routine does not set it. A subsequent explicit owner-A retry succeeds on the remaining block; the retained owner-B handle can then be freed separately. The test process exits 0 because these expected baseline observations were recognized. It does not establish that lack of a CF update is wrong for this internal cleanup service or that an actual teardown retries it.

`dpmi32.c:66` maps the original service slot to `DpmiFreeAppXmem`; `xmem.c` traverses task-owner nodes and returns on the first failed provider free. The current mapped image attributes XMEM methods to `original-mvdm-dpmi32:xmem.obj`; no deployed-image binding or live guest invocation is established. The callbacks here simulate addresses and failure; no native memory is actually allocated or decommitted. They cannot establish real XMS pool state, host failure incidence, guest-visible recovery, worker-death teardown, invalid handles or concurrent tasks. S11/S12 must trace the actual caller after a failed teardown and decide whether/when a retry or centralized cleanup is required before treating partial cleanup as a product defect or choosing a repair.

Replay with the current selected x86 worker object and i686 MinGW:

```powershell
python artifacts/mvdm-audit/host-ch-scope/dpmi-xmem-owner-run.py
```

The executable/logs are disposable under `build/t473-s10/dpmi-xmem-owner-probe`. No previously blocked probe, real device, user data, product or guest mutation is involved.
