# T427 S5 — system32 host co-location

## Scope and source

Owner requests the five added EXEs beside NTVDM.EXE in system32. Baseline is
d0c2fc1d0, S4/r050/runtime and S4/r052's retained gates. Original TXTSETUP.SIF
evidence in the [S4 record](m0-t427-s4-guest-root-layout.md) puts NTVDM.EXE,
COMMAND.COM, WOW32.DLL and VDMREDIR.DLL in target 2/system32, SYSTEM.INI in
target 1/root. This corrects deployment, not guest execution. No MVDM/OpenNT
mirror source or immutable guest binary changes in S5.

## Implementation

- common/system_root removes the actual loaded EXE filename and its system32
  directory. Windows/product root is its parent, independent of argv/CWD/PATH
  and inherited text. Existing ANSI capacity and failure contracts remain.
- run16 broker startup, NTSRV frontend/worker startup, native shell-out run16
  and exact provider DLLs use system32-relative paths. Internal COMMAND stays
  system32/COMMAND.COM; no double system32 composition or user-search fallback.
- NTVDM.REG stays beside its worker, now system32/NTVDM.REG. Migration retains
  bytes and recovery, refusing conflicting overlays. Neither overlay existed
  in the live package; authored fixture proof covers move/conflict behavior,
  not a live user-overlay test.
- Native directory APIs still mean host Windows. Guest Windows root remains
  product root, original Win16 System remains root/system, kernel/module path
  remains root/system32. No new unused Win16 wrapper or guest patch.
- Staging and selected test entrypoints use system32 host paths. Test-only
  Get-PackageBinaryRoot supports sealed flat baselines/build caches and rejects
  mixed layouts. Production has no flat fallback. Alias cleanup keeps the
  full relative path and exact-image/creation-identity validation.

APP0.0.427/RPC38/I/O25 and existing acceptance remain unchanged. No parser,
frontend, scheduler or execution/lifecycle change. User search remains CWD
then explicit PATH; bare commands require visibility there or an explicit path.

## Verification

All runs below are build/M0-T427/S5. Retained S2/r001 MSVC14.43/SDK22621/x86
/MT CCPU40 Ninja closure rebuilt affected EXEs/VDMREDIR dependency-first.
Unchanged WOW32 provider and NTMON retain S4 hashes; parent import ABI unchanged.
Formal VdmTib storage check passes.

| Entry / run | Observed assertions |
| --- | --- |
| [common_system_root_test](../../../tests/app/common_system_root_test.c), r006/root fixture/system32 with expected-root argument | Own-image parent root despite changed CWD/inherited root; joins, capacity/error clearing and invalid relatives pass. |
| Cache guest-environment-test/application-search-test, fresh r006/search fixture | Bounded projection/host data; directory-first suffix/explicit/drive/empty/Unicode/capacity cases pass. |
| [original_runtime_layout_test](../../../tests/observation/original_runtime_layout_test.mjs), r002/runtime | NTVDM in system32, no flat copy; unique original destinations and source/destination hashes. |
| New-T310FirmwareResourceNinja r003/resources all-verify | Firmware/media, missing local DLL with CWD impostor, native fallback and WOW boot-environment ownership pass. |
| [verify-native-host-root](../../../tests/observation/verify-native-host-root.ps1), r007, S4/r001/HROOT.EXE | Direct/DOS-native/nested DOS-native retain real host root/APIs; wrong inherited product root ignored. |
| [verify-win16-directory-probe](../../../tests/observation/verify-win16-directory-probe.ps1), r008, S4/r001/DIRP.EXE | Actual guest Windows/System/kernel calls identify Z:/, Z:/SYSTEM and Z:/SYSTEM32/KRNL386.EXE. Original drive-root double separator normalized only by assertion. |
| [verify-search-internal-handoff](../../../tests/observation/verify-search-internal-handoff.ps1), r009 | Internal COMMAND/native streams outside package CWD/PATH; DOS picks actual CWD native image with package absent from PATH. |
| [verify-application-search](../../../tests/observation/verify-application-search.ps1), r010 | All 17 actual image/exit cases pass; no implicit EXE-directory priority. |
| [verify-system32-package-staging](../../../tests/observation/verify-system32-package-staging.ps1), r014/r015 | Eight destinations; byte-exact overlay move/source preservation; conflicting overlays/mixed test layouts reject. r015 verifies the final stronger test-only mixed-copy guard and both live system32/flat-cache destination selection. |
| Invoke-ProductVerification final r012 | Console17/Window17 and three independent retained WOW frontiers pass; final eight hashes unchanged. |
| r011/publish.ps1, r013/published-smoke.ps1 | Recoverable migration; eight published Console/Window COMMAND/MEM/EDIT.COM/native VER smoke cases pass; eight hashes and 46 other root/system32 media/config/user files unchanged. |

Full gate command:

```powershell
& tools/audit/Invoke-ProductVerification.ps1 -RuntimeRoot build/M0-T427/S5/r001/runtime -BuildCache build/M0-T427/S2/r001 -Observer build/M0-T427/S4/r049/console-startup-observer.exe -WindowObserver build/M0-T427/S4/r049/worker-window-snapshot.exe -LogRoot build/M0-T427/S5/r012 -Suite Product -GuestFixture build/M0-T425/S9/r008/G7.COM -WowBaselineRoots build/M0-T427/S4/r052
```

r012/timings.json: WOW67726 ms, Console17 66457 ms, Window17 77048 ms, total
217386 ms. Same assertions/profiles/timeouts; no new SOL/WRITE usability or
gameplay acceptance. Physical desktop/RDP not newly tested. Global-endpoint
cases remain serial; every Z: mapping is removed finally.

Retained attempts: initial Ninja nonexistent target spellings were corrected.
r005 used a preceding fixture binary; rebuilt r006 supersedes it with an
expected-root assertion. r004 passed runtime groups but failed its remaining
flat-path final hash check, so is not an overall pass. Corrected runner r012
repeated all groups and final identity successfully.

## Publication and handoff

O:/winnt/system32 now contains run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe,
ntvwm.exe, ntmon.exe, WOW32.DLL and VDMREDIR.DLL. r011/published-manifest.json
pins exact tested r001 bytes. r011/recovery retains the coherent prior flat
eight-file set and preexisting system32 copies; its manifest records both
existing and absent paths for rollback. Flat duplicates were removed only
after backup/hash validation. No other file was removed.

Root SYSTEM.INI, original guest media and user programs including root edit.exe
are unchanged. Published logs are O:/winnt/Logs2/t427-s5-published-*.
From root use `system32\run16 system32\command.com`, or enter system32 and
use `run16 command`. No root wrapper or implicit search exception.

Final source/diff review, governance and relative-link gates pass. r016 retains
the reviewed 28-file source/hash manifest and diff. Code delivery 62a71fd90 is
committed and pushed to main; publication still equals its verified set.
Other-session queue/proposal edits remain outside it. T427 remains open for
owner validation; no next task is admitted by this delivery.
