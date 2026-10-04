# T427 S4 guest-root and original deployment integration

## Status and question

S4 implementation and publication gates are complete; T427 awaits owner
acceptance. Does own-image root isolation preserve the original guest-visible
Windows directories and original deployment-relative media locations?

## Original sources

- `O:/repos.external/OpenNT/base/mvdm/dos/command/cmdmisc.c`, SHA-256
  `1CA036932368A0D0BF1848B078CC80CD66E67A4CC46AD26A0C1B236FE40F4281`:
  GetWowKernelCmdLine creates the initial guest environment before first task
  delivery. The saved native environment is a separate owner.
- `O:/repos.external/OpenNT/base/mvdm/dos/dem/dem.c`, SHA-256
  `2573DA2DBF964C5E75102145B031766113CFC4E914DC00F0D80FDC6D98F5A2DE`:
  DemInit initializes its DOS directory with GetSystemDirectory; demLoadDos
  appends `ntdos.sys` using the original directory carrier.
- `O:/repos.external/OpenNT-4.5/setupext/i386/TXTSETUP.SIF`, SHA-256
  `86F9C2F24B58D2BF6ECD68564E7618FE77311252549A7177104A77EC2A6A8032`:
  WinntDirectories lines 34-39 define root, system32 and system. SourceDisksFiles
  target-directory field assigns the selected DOS utilities/configuration and
  WOW applications to system32; SYSTEM.INI has target 1, the Windows root.
  These are read-only provenance references, not new imported runtime inputs.

The `build/.../runtime` staging container represents the Windows root itself.
It is not an original installation subdirectory and adds no runtime layer.

## Changes and invariants

Common's existing bounded ANSI environment projection serves both run16 task
records and NTVDM's initial WOW guest PDB. MVDM-HOST-DIV-324 projects only the
completed guest block at GetWowKernelCmdLine, retaining original transformation,
copy/free ordering and the unmodified saved native environment. Guest
SYSTEMROOT's trailing separator is normalized for the original kernel's
literal `\SYSTEM` append; native own-image root remains an absolute root.

The existing system-media adapter resolves NTIO and default CONFIG.NT/AUTOEXEC.NT
under root/system32. SYSTEM.INI remains root-relative. No CWD/PATH or flat-copy
fallback is added. DIV-154 initializes DEM's original carrier from that same
system32 directory with the original MAX_PATH bound. DOS loading/execution
and PIF overrides remain in their original owners. Package capacity checks
include the actual system32 suffix.

Staging maps its selected DOS assets to original system32 paths, eliminating
the duplicate COMMAND destination. Test PATH explicitly includes system32 for
bare utility requests; this is test-owned ordinary search input, not hidden
production discovery. WOW test targets use explicit original locations.

No guest binary modification, new component/process, RPC validation, protocol
change or global host environment/Registry mutation is introduced.

## Evidence and failed attempts

- S4/r014 actual authored Win16 probe: guest Windows/module directories were
  local, but GetSystemDirectory still returned host C:/WINDOWS/SYSTEM. This
  proved submitted-task projection alone insufficient.
- S4/r016 failed placement in cmdCreateVDMEnvironment was removed. The final
  hook is at completed initial WOW boot-block ownership, not that transform.
- S4/r022 retained a WRITE OLECLI failure with a doubled drive-root separator;
  r024 non-drive-root comparison restored the earlier frontier. Guest-prefix
  normalization then yielded passing three-frontier comparison in r027.
- S4/r026 actual Win16 directory probe passed Windows Z:/, system Z:/SYSTEM
  and loaded kernel Z:/SYSTEM32/KRNL386.EXE. r027 complete Product gate passed
  Console17, Window17 and three WOW frontiers in 205090 ms. This package still
  had the old flat layout and is not proof of canonical media deployment.
- S4/r028 x86 MSVC /MT build and firmware/media/loader/WOW-boot fixtures pass.
  The flat-copy-only negative proves absent system32/NTIO cannot use root/NTIO.
  Stage-OriginalSoftpcRuntime emits original DOS destinations without duplicate
  root COMMAND. `fixtures.log`, `build.log` and `dem-directory-build.log` retain
  the build outputs; original linker warnings remain explicitly visible.
- S4/r029 actual DOS startup failed with 1067 after moving media: DEM still
  appended NTDOS to a project-selected flat root. That proved a second binding
  gap rather than justification for keeping duplicate guest files.
- After DIV-154 correction, S4/r031 passes actual direct native, DOS-to-native
  and nested DOS-to-native host-root assertions. Native SYSTEMROOT and Windows
  directory remain the real host; System directory equals host/system32.
- S4/r032 actual authored Win16 API probe passes on the reordered package.
- S4/r033 passes the serial retained Product gate for the canonical-layout
  candidate: WOW 68353 ms, Console17 66158 ms, Window17 74853 ms; total
  215458 ms. All eight candidate hashes are recorded by its manifest. This is
  pre-NtvdmGet naming evidence, not permission to deploy a later rebuilt set.

Owner subsequently requests the explicit `NtvdmGetSystemDirectory` family.
Existing guest-only directory adapters and callers are renamed consistently
to NtvdmGetSystemDirectoryA/W and NtvdmGetWindowsDirectoryA/W; actual host APIs
keep their Windows meanings. DEM now retains the original DWORD return-length
check with only the selected directory API differing. Provider export/import
lists are regenerated together; old and new symbol sets must not be mixed.

## Final naming and publication follow-up

- r039 passes actual direct/nested DOS-to-native host-directory assertions on
  the renamed candidate. r042 passes the authored Win16 API probe: Windows
  Z:/, System Z:/SYSTEM, kernel Z:/SYSTEM32/KRNL386.EXE. The preceding r040
  timed out; a concurrent published broker was observed, but contention is
  not proven its sole cause. r041 stopped before launch on endpoint ownership.
  These attempts remain failures, not discarded retries.
- r043 passes the renamed canonical package's retained Product gate: WOW
  67984 ms, Console17 74649 ms, Window17 89704 ms, total 239484 ms.
  r044 passes all 17 actual selected-image cases; r046 passes actual internal
  COMMAND/native-stream and DOS-to-native handoffs outside package CWD/PATH.
- r045 publishes all eight verified files and moves 23 exact guest/config
  files from root to original system32 destinations. Each removed flat file
  is hash-checked and retained under r045/recovery; SYSTEM.INI, NTVDM.REG and
  user applications are preserved. Its manifest records both published and
  recovery hashes. No recursive delete or guest-byte change occurs.
- r048 published smoke fails DOS EDIT interaction: bare `edit` actually
  starts the user's O:/winnt/edit.exe (modern Win32 EDIT), not EDIT.COM. The
  production resolver correctly honors CWD. The file is preserved; the DOS
  fixture now types `edit.com`, retaining menu/exit/MEM and completion
  assertions. This is explicit test-target selection, not product search
  injection or a weakened assertion.
- Owner admits the concurrent NTMON change. Frontend and WOW worker rows show
  `-` in TASK; actual DOS/native/WOW/GUI task rows retain their image. Kind,
  hotkeys, ordering and management policy are unchanged. r049's formal
  monitor-layout fixture and input-milestone negatives pass. The direct
  layout invocation without a real Console failed its buffer prerequisite;
  the documented private-Console wrapper passes.
- r051 fails before starting WOW because Windows PowerShell 5 has no
  ProcessStartInfo.ArgumentList. The unchanged supported PowerShell 7 runner
  is used for r052. Neither failure is product execution evidence.

Final candidate is r050/runtime (renamed S4 set plus rebuilt NTMON). r052
passes its exact retained Product gate: WOW 68870 ms, Console17 66565 ms,
Window17 75158 ms, total 216972 ms. r053 recoverably replaces the coherent
eight-file set; r054 passes all eight published Console/Window COMMAND, MEM,
EDIT.COM and native VER cases and rechecks every product/media hash. Reports
are O:/winnt/Logs2/t427-s4-published-final-*; package manifests and previous
coherent recovery remain under r053. r045 retains the earlier flat-media
recovery. The user O:/winnt/edit.exe is neither removed nor replaced.

Commands for the final retained gate (repository-relative paths):

```powershell
& tools/audit/Invoke-ProductVerification.ps1 -RuntimeRoot build/M0-T427/S4/r050/runtime -BuildCache build/M0-T427/S2/r001 -Observer build/M0-T427/S4/r049/console-startup-observer.exe -WindowObserver build/M0-T427/S4/r049/worker-window-snapshot.exe -LogRoot build/M0-T427/S4/r052 -Suite Product -GuestFixture build/M0-T425/S9/r008/G7.COM -WowBaselineRoots build/M0-T427/S3/r003
```

Publication and smoke entrypoints are retained as r053/publish.ps1 and
r054/published-smoke.ps1. Formal x86 NTMON and monitor-layout-test targets
use the selected S2/r001 Ninja graph; tests/observation/verify-monitor-layout.ps1
uses r049's observer and reports r049/monitor-layout.txt. Same graph's
guest-environment-test and common-system-root-test pass; application-search-test
passes with fresh r049/search-fixture. r049's input-milestone-test preserves
fresh echo, partial input, stale prompt, cursor/tail and Window PID negatives.
Documentation governance, relative links, source/diff and mirror review gate
the containing P. Concurrent queue/other-proposal edits are preserved and not
part of this implementation delivery.

## Whole-objective disposition

| Requirement | Production owner and concrete evidence |
| --- | --- |
| All six EXEs derive their own product root | common/system_root and S2 caller ledger/build closure; r049 common-system-root-test covers wrong inherited root, capacity and actual loaded image. No new RPC root check. |
| Separate user, host and guest roles | S1/S2 caller ledger; r039 actual host environment/API outputs and r042 original Win16 API outputs demonstrate different directory meanings. |
| Internal files do not fall back to user lookup | Selected media/resource/loader fixtures in r028, including renamed fixtures; missing system32/NTIO rejects flat-copy-only input. S2 provider negatives remain unchanged. |
| CWD/PATH and original arguments preserved | S3 resolver/CLI tests; r049 application-search-test and r044 real image/exit-37 witnesses across 17 cases. Product injects no package PATH. |
| Nested COMMAND uses explicit internal location | command_process_compat's system32/COMMAND.COM; r046 actual internal/native and DOS-to-native stream handoffs. |
| Original deployment, immutable media | TXTSETUP.SIF above; r034 original_runtime_layout_test asserts unique original paths and source/destination hash equality. r045 migration retains recovery and user configuration. |
| Embedded ROM and temp/PIF semantics retained | S1/S2 selected caller audit and resource tests; S4 adds no external ROM requirement or temp/PIF policy. Existing adapters and overrides retain ownership. |
| Mirror edits are minimal and registered | r047/mirror-comparison.json records bytewise and normalized comparisons for all eight selected modified mirror files. No format-only differences remain; DIV-324 is the boot-copy seam and DIV-154 retains original DEM capacity/order. |

Bare guest utility names require ordinary CWD/PATH visibility after relocation;
`run16 system32\command.com` is explicit from the root. The product must not
reintroduce implicit package priority to conceal that fact. Physical desktop/
RDP behavior is not newly exercised by this directory-only delivery. WOW
frontier comparison is not a claim that SOL/WRITE are fully usable. T427
remains open for owner acceptance.
