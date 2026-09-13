# M0 T409 S2 — original DEM/VDD handle restoration

## Result

The original OpenNT DEM VDD lifecycle is again the selected production
implementation. `GetFreeSftEntry`, `VDDAllocateDosHandle`,
`VDDAssociateNtHandle`, `VDDReleaseDosHandle`, and `VDDRetrieveNtHandle` now
execute from `src/mvdm-host/dos/dem/demfile.c`. The superseded SFT/JFT shadow
provider, its public header, its dedicated fixture/generator, the six WOW32
commit/discard hooks, and its two formal WOW32 imports were removed.

The reviewed source delta is 33 added and 724 removed lines, net **-691**.
Of the additions, 15 executable/source-comment lines implement the sole
retained adapter: resolve the durable 16:16 SFT-head value recorded during
DEM setup to a synchronous CCPU40 `Sim32GetVDMPointer` only when an original
VDD call begins. The rest is the source-boundary regression check and a
watchdog option that distinguishes inherited-console observations from
redirected-standard-handle observations.

## Source-first ledger

| Rung | Disposition | Evidence |
| --- | --- | --- |
| Original source | Selected | The five original bodies were already present in `demfile.c` under `#if 0`; S2 restores them verbatim except for the SFT-head acquisition seam. |
| Smallest same-shaped facade | Selected only for head acquisition | CCPU40 `Sim32pGetVDMPointer` resolves the original far address synchronously through `c_GetLinAdd`; `demSftHeadPointer` supplies that original `PDOSSF` input from the existing 16:16 location record. |
| External-code intrusion | Not used | No CCPU, DOS guest, or external source was changed. |
| New lifecycle behavior | Not used | The shadow registry, copied JFT/SFT images, commit/discard protocol, and replacement traversal/mutation algorithm are gone. |

The S1 hypothesis that only a bounded lease could compose was corrected by
the selected CCPU40 source: `Sim32pGetVDMPointer` directly returns the host
pointer for the translated guest linear address, including the already-selected
physical mapping provider. The restored pointers are used only inside the
single worker process and the original synchronous VDD/WOW call chain; no
pointer crosses the machine/broker boundary or enters guest state.

## Preserved observable contract

- Optional current-PDB lookup, JFT free-slot search, SFT free-slot search,
  reservation, zeroing, association field writes, retrieval, invalid-handle
  failure, release, and reference-count decrement retain original ordering.
- WOW32 open/create failure rollback continues to set the JFT byte and
  decrement the SFT count at its original source location; open/create/close
  no longer perform a second shadow commit or introduce a new
  `ERROR_INVALID_ADDRESS` result after those writes.
- The checked `sft_head_location.valid` gate turns an unavailable DEM setup
  record into the same public allocation/retrieval failure direction, without
  retaining a native head pointer across setup.

## Verification

| Check | Result |
| --- | --- |
| `tests/mvdm-host/dos/Verify-T409OriginalVddLifecycle.ps1` | Pass: all five original owner functions selected; original source ordering present; only synchronous SFT-head binding retained; no selected shadow caller/build member. |
| Formal graph generation | Pass with Git Bash Node `v22.22.1`, root `build/M0-T409/S2/r002-original-vdd-clean`. |
| Formal Win32/x86 link | Pass: `original-softpc-process.exe`, 3,238,400 bytes. `demfile.c` and `wkfileio.c` compile; link completes without the shadow provider. Historical warning families remain warnings only. |
| Staging | Pass: `build/output/ntvdm32.exe` and `O:\ntvdm64\ntvdm32.exe`, both 3,238,400 bytes, SHA-256 `837f3cff9681406e8952453d153d5afe2b70d34c584b27835944f07679a658cf`. |
| Runtime A/B | Limited, not a pass/fail VDD proof: both the staged candidate and retained pre-T409 `ntvdm32-o.exe` exceeded the 20-second inherited-console `MEM.EXE` watchdog and were test-owned process-tree terminations. Candidate record: `O:\ntvdm64\logs\m0-t409-s2-mem-inherited.json`; baseline record: `O:\ntvdm64\logs\m0-t409-s2-mem-old-inherited.json`. The same redirected test also timed out. This is an existing automated Console observation limitation, not evidence of a new VDD regression. |

## Limitation and handoff

This S proves source selection, compilation, linking, staging, and the
non-regression A/B boundary. It does not claim that a noninteractive watchdog
has exercised an actual VDD file-open sequence, nor that COMMAND/EDIT have
passed an interactive file-operation regression. Those runtime observations
remain required during T409's fast-read/ordinary-file workload S3; they are
not silently converted into a VDD semantic claim here.
