# Shard 2: DOS host-platform declarations

Five pending headers were checked against frozen originals and all captured **VALID** worker Ninja dependency records, resolving relative paths from the worker build directory. The observer and JSON record exact hashes and object names; the result does not cover stale dependency records or other build profiles.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `softpc.new/host/inc/host_inc.h` | Four captured worker objects include this exact file, including C-VID `accessfn.obj` and CCPU `localfm.obj`. Candidate **partial** declaration; guest memory or CPU behavior is not proved by inclusion. |
| `softpc.new/host/inc/host.h` | No exact captured VALID dependency hit; original source contains direct basename includes that may resolve to another include root. Keep **pending** until the actual selected include paths and other profiles are reconciled. |
| `softpc.new/base/inc/winfiles.h` | Original BIOS C source includes it under `SWIN_HFX`; no exact captured VALID hit. Keep **pending** for alternate profile and consumer analysis. |
| `softpc.new/base/inc/host_msw.h`, `ms_windo.h` | Historical Windows-driver related declarations; no exact captured VALID hit. Keep **pending** until exact consumer and platform/profile applicability are established. |

No matching dependency in this snapshot is not a blanket proof of unused capability. Existing C-VID, CPU and host path audits independently cover actual implementations and their registered differences. Replay `python artifacts/mvdm-audit/shard-2/dos-host-platform-headers-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare the JSON. No guest, product, installation, shared ledger or build tree was modified.
