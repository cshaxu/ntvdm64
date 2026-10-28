# Win16 multimedia timer: source dispatch and published NE interface

This extends `win16-timer-guest-review.md` for six shard-2 pending files, all matching both original editions. Original MMSYSTEM source `TimeInit` calls `OpenDriver` with the alias `timer`; API paths call `timeMessage` for begin/end period and set/kill timer events. Original TIMER `api.asm` has corresponding message-table entries and handlers. The package TIMER.DRV is byte-identical to the source-tree image, and its NE name tables contain WEP ordinal 1 and DriverProc ordinal 2, matching the original timer.def. These are source-level relationships and published interface metadata, not proof that the alias resolves to this image, a particular guest actually loaded it, or a timer fired.

| File | Evidence and bounded recommendation |
| --- | --- |
| `libinit.asm` | Historical timer makefile links libinit.obj; it includes timer.inc and sysinfo.inc. Candidate **historical guest build source** only. |
| `local.asm` | Historical recipe links local.obj; includes timer.inc. Candidate **historical guest build source** only. |
| `math.asm` | Historical recipe links math.obj; includes timer.inc. Candidate **historical guest build source** only. |
| `sysinfo.inc` | Included by libinit.asm. Candidate **historical guest build input** only. |
| `timer.inc` | Included by all three preceding assembler files. Candidate **historical guest build input** only. |
| `TIMER.DRV` | Published ZIP bytes match both originals and current source-tree image; WEP/DriverProc exported ordinals parsed from NE. Candidate **partial package/interface evidence**; direct installed System32 presence is reported in JSON and may change independently. |

The original source explicitly treats failure to install the timer driver as nonfatal, so package presence cannot be promoted into a successful driver-load assertion. Timer resolution, concurrent events, cancellation, callback timing, task exit and installed-path selection remain untested. This does not conflate the separately audited SYSTEM.DRV with TIMER.DRV or remove any timer capability.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/timer-ne-family-observer.py` and compare JSON; the earlier `win16-timer-guest-observer.py` checks additional historical recipe/source edges. No historical build, guest, product, shared ledger or build tree was run or modified.
