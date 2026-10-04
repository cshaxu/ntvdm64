# T427 S2 own-image root and internal binding delivery

## Question, inputs and bounded result

Can each product path owner select its own loaded EXE directory without
borrowing CWD/PATH/environment authority, while host paths and guest execution
remain intact? S2 answers this for the audited internal boundaries. User
application search remains the separately admitted S3 repair, not an S2 pass.
Owner explicitly rejects new RPC root/package-identity checks. No such check,
peer-directory field or wire/authentication change is delivered.

Source baseline: S1 admission at 401fbea70, accepted T426 production 700b3d862.
Previous published set: build/M0-T426/S3/r001/runtime. Candidate:
build/M0-T427/S2/r002/runtime, frozen before Product verification. Build row:
MSVC14.43, SDK22621, Win32/x86 /MT CPU_40_STYLE/CCPU40. APP0.0.427,
control/RPC38, I/O25. All disposable outputs remain below build/M0-T427/S2.
Original guest media is unchanged.

## Production owners and actual changes

| Owner/boundary | Implementation and failure contract |
| --- | --- |
| common/system_root.[ch] | GetModuleFileNameW(NULL) selects actual own EXE; derive its directory, retain drive-root slash and validate relative joins. Checked W/A APIs clear output on failure, report insufficient capacity and reject ANSI substitution. No cache, environment override, task state or RPC policy. |
| run16 main | Internal NTSRV sibling and WIN16DIR derive from common root; launcher user resolver is unchanged pending S3. |
| NTSRV frontend/worker registry | NTCON, NTVDM and NTVWM paths use common root; original reservation, BaseGetVdmConfig, creation and rollback ordering remain. KRNL386 uses declared system32 relative path. |
| NTVDM package_layout | Actual own-image process root and system32 media binding use common mechanics; explicit session/layout setters retain their contracts. Embedded BIOS/V7VGA and selected cmosnt remain selected; no external ROM directory is created. |
| NTVDM shadow Registry / native COMMAND launcher | NTVDM.REG and internal run16 derive from own root. Registry writes remain the package-local overlay. Internal bare COMMAND.COM /c is explicitly pending S3. |
| NTVDM SafeLoadLibrary | Exact WOW32/WOW32.DLL and VDMREDIR/VDMREDIR.DLL use local product paths. Missing providers fail locally; arbitrary VDD/native names and explicit paths retain LoadLibraryA. Original fsave/frstor sequence remains. |
| Win16 guest record | Only ANSI BaseCreateVDMEnvironment output receives bounded SYSTEMROOT projection; case-insensitive duplicate SYSTEMROOT entries are replaced, other entries retained. WIN16DIR and original loaded-kernel rules remain. Unicode host worker block and process/native SYSTEMROOT are unchanged. |
| COMMAND temporary-file fallback | Restore original GetWindowsDirectory host-scratch call; remove its unused firmware include. Original retry and failure ordering remain. No product directory becomes generic temporary storage. |
| NTCON / NTVWM / NTMON | Common root is available without extra runtime owner/state. Existing consumers link the selected closure; NTMON has no fabricated product-resource lookup. |

No production protocol file except APP version changes. Existing authentication
and acceptance tests remain unmodified. No helper, component, process, global
environment/Registry mutation, firmware import or guest binary change.

## Reproduction and verification

1. Generate production graph using tools/build/New-T310OriginalSoftpcNinja.ps1
   -Architecture x86 -BuildRoot build/M0-T427/S2/r001. Formal six EXEs and
   VDMREDIR link; WOW32 is rebuilt with New-T404S5Wow32ProviderNinja.ps1,
   matching r001/ntvdm.lib. Product build log and wow32-build.log retain results.
2. Run r001/common-system-root-test.exe and guest-environment-test.exe.
   Own-root assertions include wrong inherited NtvdmSystemRoot, changed CWD,
   invalid joins, cleared tiny outputs and W/A results. Copied root test at
   `r001/relocated fixture/root-test.exe` proves a relocated path containing
   spaces. Guest projection asserts bounded MULTI_SZ, duplicate/missing key,
   malformed input, unchanged source bytes and native host SYSTEMROOT.
3. New-T310PackageLayoutNinja.ps1 -BuildRoot r001/layout: selected fixture
   passes. New-T310FirmwareResourceNinja.ps1 -BuildRoot r001/resources:
   firmware/media fixtures pass against retained ROM/media inputs.
   `ninja -C r001/resources verify-loader` passes PRODUCT-LOADER-ROOT-PASS:
   absent local WOW32 fails ERROR_MOD_NOT_FOUND despite invalid CWD shadow;
   arbitrary kernel32 still loads. Fixture cleanup guards are separately
   rebuilt/retested after the full Product run, without production changes.
4. Invoke-ProductVerification.ps1 -Suite Product -RuntimeRoot
   build/M0-T427/S2/r002/runtime -BuildCache build/M0-T427/S2/r001 -Observer
   build/M0-T425/S9/r033/console-startup-observer.exe -WindowObserver
   build/M0-T425/S9/r033/worker-window-snapshot.exe -GuestFixture
   build/M0-T425/S9/r008/G7.COM -WowBaselineRoots build/M0-T426/S3/r003
   -LogRoot build/M0-T427/S2/r003. PASS all unchanged Console17/Window17
   assertions and three retained WOW frontiers. timings.json: WOW67464ms,
   Console63149ms, Window68361ms, total205403ms including preparation/cleanup.
5. verify-service-fixtures.ps1, same retained observer and r001 service
   fixture, LogRoot r004/service: PASS all22 at concurrency4, 2630ms. These
   are separate in-process services, not parallel global RPC matrices.
6. verify-s7-rpc-fixtures.ps1 -PackageRoot r001 -ReportPrefix r004/rpc/check:
   PASS client, bootstrap, startup-rejections, startup-timeout,
   native-worker-failure, native-completed-worker-loss and monitor-rpc. Real
   RPC cases run serially; no new root identity checks are introduced.
7. Verify-ProductVersions.mjs with explicit r001 build and r002 runtime:
   initial r005 long-path run fails before RPC with NTVDM161, retained as
   failed evidence. Approved subst Z: short-path rerun at r005-short passes
   all five peers (old-app, wrong-protocol, reply-app, reply-protocol,
   legacy-interface) for launcher and worker: exact1306 before delivery.
   APP/RPC major consistency is retained; Z: removed finally.

Retained WOW comparison: WINMINE reaches the accepted visible window;
SOL/WRITE retain their previous incompatibility/memory dialogs and execution
frontiers. Those existing limitations are not successful gameplay claims.
Full matrix proves startup and ordinary handoff, not a new guest API probe of
every Win16 directory function. The ANSI guest-record projection is unit
proved and production-used; exact Win16 API directory calls remain source-
attributed and subject to S4's final coverage audit. Legacy ANSI/short-root
and guest path-length limits are not removed by common root mechanics.

## Mirror provenance and review

Read-only pins: O:/repos.external/OpenNT/base/mvdm/dos/command/cmdredir.c
and softpc.new/host/src/nt_bop.c. r004/mirror-comparison.json records original
and local SHA256, byte comparison and newline-normalized comparison for both
changed mirror files. Both retain earlier registered semantic diffs, not
format-only differences. This S restores the original temp expression and
adds only MVDM-HOST-DIV-323's include/one-expression adapter call and local
marker. Source remains at original paths; original guest/task/FPU control
logic is not extracted or rewritten.

Review confirms checked capacity including NUL, caller-owned output/heap
ownership, no cached global root, unchanged service locks/lifetimes and ANSI
projection freed by the existing process-heap RtlFreeAnsiString binding.
Missing internal files do not gain user-search fallback. User-search and
internal interpreter separation remain explicit required S3 work.

## Publication and closure gates

r006/publish.ps1 verifies all eight candidate hashes against r003/runtime-
manifest.json, retains the previous eight-file package in r006/recovery,
stops only path/creation-checked owned package processes when needed, and
replaces O:/winnt coherently with rollback on error. PASS publication:
published-manifest.json has identical source/published hashes for all eight.
Existing guest files, configuration and user Registry overlay are untouched.
r007/published-smoke.ps1 verifies COMMAND, MEM, EDIT and native VER under
Console/Window and final eight-file identity; its outcome is recorded at S2
closure in CURRENT. Documentation governance, relative links, diff check and
reviewed commit/push complete the sequential delivery. S2 does not close T427.
