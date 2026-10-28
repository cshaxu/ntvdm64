# Shard 1: pending guest DEBUG binary and SYSVER alternatives

The observer checks the **current** `shard-1/file-audit.csv`: the three paths below are `待核验`. All three current files match both frozen OpenNT editions byte-for-byte. It also checks the original makefile/link response, selected include and four actual x86 worker/WOW32 and x64 frontend/service graph/dependency logs. Exact source/ledger hashes and catalog IDs appear in `guest-debug-variant-observations.json`.

| File | Original role and current evidence |
| --- | --- |
| `dos/v86/cmd/debug/DEBUG.EXE` | Original guest DOS DEBUG image; original `makefile` sets `dest=debug.exe`, assembles object inputs and runs `link16 ... @debug.lnk`, whose response names `DEBUG.EXE`. This source path is a retained image; the makefile is not proof it generated these specific bytes. |
| `dos/v86/cmd/debug/sysver.fal` | Historical variant with `SYSVER EQU FALSE`, describing the default DOS-I/O side. The retained `sysver.inc` selects FALSE and is included by `debug.inc` and `debmes.asm`. That content agreement is not evidence that the `.fal` file was copied during the build. |
| `dos/v86/cmd/debug/sysver.tru` | Historical alternative with `SYSVER EQU TRUE` and source comment saying direct BIOS I/O enables DOS debugging. The makefile does not explicitly select this file in the inspected recipe; a different historical selection process or build is not ruled out. |

No exact path appears in the four checked host build graphs or their dependency logs. The two SYSVER files are candidates for historical guest build-variant input roles **only for these checked host targets**; do not treat either as a proved selected input or remove DEBUG behavior. The retained DEBUG.EXE is a guest binary role and must not be classified as an irrelevant intermediate just because it is absent from a host link graph. At this observation, `assets/winnt.zip` had no member ending `/debug.exe`, and the exact `O:/winnt/system32/DEBUG.EXE` path was absent. Neither observation searches other guest media or future installations.

Replay `python artifacts/mvdm-audit/shard-1/guest-debug-variant-observer.py`. No guest, product, source, build tree or shared ledger changed, and no runtime or guest compiler was executed. DEBUG executable lineage, actual variant, instruction-level behavior and other package availability remain unverified.
