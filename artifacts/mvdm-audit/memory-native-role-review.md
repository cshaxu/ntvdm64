# S10 CPU40 host memory owner reconciliation

Base `4d0f2d2c6`; exactly three pending source roles are resolved here. This
changes no original source, product image or guest. It is not complete memory
capability acceptance.

## Original owner and selected profile

The [observer](memory-image-observer.py) joins all three files to the DOS16
catalog and both frozen WOW32 editions, compares primary original bytes,
and checks the unchanged original NT recipe, actual archive/compile edges and
valid retained dependencies. [Observations](memory-image-observations.json)
retain exact catalog identities, source/object hashes and selected flags.

The sole supported backend is CCPU40 under [Goal](../../docs/design/GOAL.md)
and [source policy](../../docs/etc/operations/policy/source-policy.md).
The production flags select CPU_40_STYLE/CCPU/C_VID, without MONITOR, X86GFX,
HOST_SAS or SUN_VA. The image's sas_init, sas_term and c_sas_connect_memory
come from original ccpusas4, not an application memory engine. Their three
bodies are image-bound independently of the seven nt_umb bodies. Two original
public VDD callers and their actual helper call targets are also image-bound.

| File | Disposition | Reason and remaining capability |
| --- | --- | --- |
| nt_umb.c | Partial | Original list initialization, split, reservation, ownership transitions, SAS requests and XMS notification are present; seven image-bound functions and bounded native cases are proved. Allocation failures, public hook/list identity, nesting, guest faults, reset and retirement remain S13. |
| nt_sas.c | Excluded physical alternative profile file | Its host_sas_funcs and old address helpers require HOST_SAS/SUN_VA; none is linked in this CPU40 image. Actual original ccpusas4 supplies the selected SAS behavior. The file remains unchanged and archived in the original recipe. This does not exclude SAS/address/A20/video-memory capability at its partial owners. |
| nt_sec.c | Excluded physical NT4 hardware mapping helper | Its fixed 0xa0000 virtual-memory aliases and X86GFX kernel ROM/PIC setup are not extracted into the non-MONITOR CPU40 image. The selected emulator uses original SAS memory and host-owned DIB storage, not guest numeric addresses interpreted as modern host addresses. This does not exclude VGA memory, emulated ROM, mode changes or their partial lifecycle contracts. |

All three retain 100% of primary original source bytes. An executable-code
restoration estimate for the UMB file is unmeasurable from these bounded
witnesses: seven linked functions do not measure every guest/failure/lifetime
path. The two physical alternative profiles have no applicable restoration
percentage; their selected equivalent capabilities remain independently open.

## Actual-object contract and a narrow invalid-input finding

The [fixture](../../tests/mvdm-host/umb_object_contract_fixture.c) links the
actual cached nt_umb.obj. It owns its lists, uses real CRT allocation and real
SetLastError/GetLastError, and records SAS mapping/XMS notification requests.
It never maps low memory or changes a guest. The original UMBNODE x86 layout
is checked at 20 bytes; a wrong-width compile must fail at C2118.

**4,240 normal assertions pass**, with six SAS calls and two XMS notifications.
They cover every page-aligned subrange of a 64-KiB block for EMM/VDD/ROM
splitting, exact coverage/adjacency/owner, initial ROM/free list construction,
EMM release and XMS claim, VDD reserve/commit/decommit/release, invalid owner,
wrong-owner release, unavailable EMM space, unmapped range rejection and all
256 original UMBNotify no-op codes. An existing IOS control still passes
70 assertions and its own wrong-width negative after the shared runner change.

A **separate strict diagnostic exits 1**, retaining 12 failed assertions:
the direct VDDCommitUMB/VDDDeCommitUMB helpers accept six bad ranges, including
start-before-block, end-exactly-at-block-start, and arithmetic wrap. Their
predicate compares block start with request end, rather than request start.
The recording seam proves the numeric bad SAS request, not a real invalid
mapping or guest crash. No success marker is substituted for this failure.

This does **not** by itself establish a product defect. Actual VDDAllocMem and
VDDFreeMem additionally check nonzero address/count, count <= 0x20000, matching
hVDD, and containment in a registered MemHookHead entry before calling the
helper. These source guards reject the six diagnostic inputs for a valid
matching hook. Their production bodies and call targets are image-bound, but
that is not a substitute for executing their hook registration/destruction
lifecycle. The direct diagnostic deliberately bypasses that public contract.
S13 must verify hook/UMB coherence and reachable inputs before deciding if any
helper hardening is necessary. No mirror patch is justified solely by a
private entry called outside its valid contract.

The non-MONITOR InitUMBList has no explicit success return in the original;
the selected config.c caller ignores its return. The fixture validates the
constructed list and does not invent a reliable Boolean result. Allocation
failure and repeated initialization still require separate lifecycle evidence.

## Verification and delivery limits

[Receipts](memory-object-runs/manifest.json) preserve normal/ABI and strict
diagnostic outcomes. Initial fixture attempts corrected the expected x86 node
layout and missing CPU40 generated include path; those were fixture setup
errors, not runtime failures. No production object was recompiled.
The first role merge encountered a write error on an unchanged shard; the
merger now skips shards containing no selected rows. The successful exact-
delta gate still verifies every unrelated row and frozen inventory.

The [delivery gate](memory-native-delivery-verification.ps1) verifies exact
ledger/shard changes, both catalogs and image bindings, normal and retained
non-pass receipts, all input hashes, documentation and unchanged accepted/
candidate packages. Other source rows and other-session edits remain intact.
The held SOL Options regression is still unresolved. This is a local audit
checkpoint, not candidate release qualification, S10 closure or T acceptance.
