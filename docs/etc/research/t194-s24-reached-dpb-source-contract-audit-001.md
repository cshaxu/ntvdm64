# M0 T194 S24: Reached DPB Source-Contract Audit

Packet: M0 T194 S24  
Disposition: source-backed admission of one bounded follow-on integration slice.

## Question

Does the reached `C4 C4 50 46` BOP require a new DOS/filesystem/device implementation, or does existing adapter code already contain the exact OpenNT-shaped result producer and generic mechanical hand-off needed to consume the immutable CLI drive snapshot?

## Inputs

- OpenNT primary source: `src/opennt/base/mvdm/inc/dossvc.h`, `src/opennt/base/mvdm/dos/dem/demdisp.c`, `src/opennt/base/mvdm/dos/dem/demgset.c`, and `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`.
- The reached S23 bounded observation and the current S21 engine closure.
- Current adapter sources `bx_ntvdm_dem_dpb_service.c` and `bx_ntvdm_adapter_runtime.c`.
- Prior boundary records `r20-dem-getdpblist-boundary.md` and `t194-s18-v6-drive-policy-capability-contract-001.md`.

## Procedure

Read the original service definition, DEM dispatcher binding, caller continuation, and result-writing implementation. Then compare those facts to the existing source-derived DPB producer and the current runtime selection order. No source, profile, build, or runtime observation was changed or run.

## Source Facts

`dossvc.h` defines service `0x46` as `SVC_DEMGETDPBLIST`; `demdisp.c` binds it to `demGetDPBList`. `msinit.asm` invokes it after device initialization with ES:BP naming the output area. It treats an unchanged BP as an empty list; otherwise it retains the returned BP as the end of the list and continues initialization.

`demgset.c:demGetDPBList` iterates exactly the 26 drive letters. For only `DRIVE_REMOVABLE` and `DRIVE_FIXED`, it writes `DriveNum`, `Unit`, and `Next` in a DPB-sized destination slot, terminates the last `Next` with `-1`, and returns BP just past the final slot. It intentionally does not query volume information or populate the remaining DPB fields.

The existing `bx_ntvdm_dem_dpb_service_v1_prepare` mirrors that limited contract: a validated 26-slot type inventory produces checked ordinary-RAM multi-writes for exactly offsets 0, 1, and 25, advances BP, and requests the four-byte BOP resume. Its generic session queue carries only the opaque write/result transaction to Bochs; it contains no selector, drive, DPB, DOS, or device policy.

## Current Selection Gap

The normal CLI path already captures one immutable `bx_ntvdm_host_drive_snapshot_v1` before guest execution. S19 routes only the count service `50:0F` through this snapshot. After that helper declines, the runtime currently returns early when the obsolete static profile `host_drive_inventory` is absent. Consequently S23 reached `50:46` but could not select the already present DPB producer.

S18 deliberately limited its admission to `50:0F`; that was a scope boundary, not evidence that `50:46` needs a host namespace, DOS filesystem, mounted device, or new Bochs feature. The original `demGetDPBList` contract proves that the same immutable type snapshot is sufficient for this separate, now-reached service. Static profile inventory must remain a retained-fixture fallback and must neither override nor merge with the normal CLI snapshot.

## Interpretation

Confidence: high. The next action may be a narrow adapter-only integration: route the already admitted `50:46` source-derived provider through common ingress/registry/DEM-plane classification using the installed immutable CLI snapshot, then queue its existing generic multi-write result. This adds no Bochs source, configuration, device, BIOS, host-path, filesystem, or DPB device behavior.

The empty-qualified-drive case remains a known ABI limitation: the original returns unchanged BP, while the current generic multi-write queue intentionally does not accept an empty transaction. The follow-on test must cover the non-empty source path and must explicitly retain an empty-snapshot decline; it must not fabricate a DPB record or introduce a Bochs special case.

## Follow-up

Admit M0 T194 S25 only for the bounded snapshot-to-`50:46` route, a focused positive/negative runtime test, governance verification, and a source-only adapter derivative/bundle preparation if the code change requires it. A real engine observation, any empty-list ABI decision, filesystem access, or machine feature change requires a later S packet.
