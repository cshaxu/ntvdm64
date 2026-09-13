# M0 T407 S2 XMS/DPMI memory-contract restoration

## Scope and decision

S2 implements only D36 and records the D37 disposition identified by
[S1](m0-t407-s1-dpmi-xms-current-state-audit.md).  It restores
`xms.486/xmsblock.c::xmsMoveBlock` as the owner of the XMS move descriptor
layout, reverse field decoding, forward-copy direction and `setAX(1)`
success.  The standalone host retains one finite, synchronous
byte transport beneath that original call; it does not retain an XMS-specific
descriptor parser or an XMS-specific copy loop.

This is not a DOSX/WOW/WRITE acceptance claim.  D35, U03 and U04 remain out
of scope, and the current XMS commit/decommit fallback policy is retained
unchanged because the selected normal XMS path does not call it.

## Source-recovery ledger

| Rung | Result | Evidence and disposition |
| --- | --- | --- |
| 1. Original source reuse | Used | `O:\repos.external\OpenNT\base\mvdm\xms.486\xmsblock.c` provides `xmsMoveBlock`.  The local mirror now retains its descriptor offsets/order, forward `RtlCopyMemory` semantics, AX result and original cancellation branch. |
| 2. Smallest same-shaped facade | Used | CCPU40 has no durable `GetVDMAddr` host alias.  `mvdm_softpc_guest_memory_copy_forward(destination, source, count)` is a synchronous copied lease: numeric guest address/span only, forward ordered, no pointer crosses the ABI or survives the call. |
| 3. External-code intrusion | Rejected | No external source was changed.  The original mirror remains the owner; the facade is outside it. |
| 4. New behavior | Rejected | The former adapter-owned frame parsing and chunk policy were removed.  The facade supplies only the unavailable host-address transport, not XMS policy. |

`xmsMoveMemory` remains explicitly overlap-safe (`RtlMoveMemory` shape), while
`xmsMoveBlock` remains forward-copy (`RtlCopyMemory` shape).  The distinction
is deliberate and exercised by the focused fixture.

## D37 provider disposition

The selected CPU40 branch in
`mvdm-host/dpmi32/dpmimemr.c` continues to select the original suballocator
when `VdmAllocateVirtualMemory` is unavailable.  Direct source and the XMS
startup fixture show why this cannot be deleted as a duplicate allocator:
the normal XMS initialization reserves the extended range through the VDM
provider, then `ExtMemSA` owns suballocation within that already-reserved
range.  Selecting `SAAllocate` at the DPMI layer avoids a second VDM
reservation.  This S does not alter D37 because there is no proof that a
direct VDM provider has the same ownership/lifetime contract in the
standalone CPU40 host.

## Verification

Build roots are disposable and contained under `build/M0-T407/S2/`.

| Check | Command / input | Observed result |
| --- | --- | --- |
| Forward transport and overlap distinction | `xms-mapped-memory-fixture.exe`, built from `tests/adapter-mvdm-host-out/softpc/xms_mapped_memory_fixture.c` | Exit `0`: forward copy, overlap-safe callback move, lease-bound failure and teardown paths pass. |
| Original XMS initialization/provider branches | `node tools/build/Generate-T371XmsStartupNinja.mjs . build/M0-T407/S2/r003-xms-startup`; x86 Ninja fixture | Exit `0`: normal VDM allocation invokes VDM commit/decommit; `STATUS_NOT_IMPLEMENTED` uses original SA fallback without those callbacks. |
| Formal selected graph | `New-T310OriginalSoftpcNinja.ps1 -Architecture x86 ...`; `ninja ... original-softpc-process.exe` in VS x86 environment | Candidate graph compiled and `original-softpc-process.exe` linked successfully (3,236,352 bytes).  Existing source warnings only, including `xms.c` unused arguments, `suballoc.c` C4701 and `vdm.c` C4701/C4703. |
| Review | paired S1 original/current diff, `git diff --check`, focused source review | No XMS-specific descriptor parser or transfer loop remains in the adapter.  D37 is intentionally unchanged and recorded above. |

The formal graph's default target is `original-softpc-candidate` (libraries),
not the product executable; the explicit product target above is therefore
the relevant link check.

## Limitations and follow-up

- This fixture validates the restored XMS source contract and provider
  ownership boundaries, not a real guest program's XMS move BOP trace.
- `xmsCommitBlock`/`xmsDecommitBlock` keep their previous fallback behavior.
  They are reached only when the normal original VDM callbacks report
  `STATUS_NOT_IMPLEMENTED`; their zeroing/lifetime semantics need a separate
  source/consumer audit before modification.
- S3 owns D35 frame recovery.  S4 will only audit/design U03/U04 and will
  stop for owner review before implementation, as admitted.
