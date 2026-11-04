# T475 S1 — NXVM and Win32 libraries, RPC

Owner directs a distinct `src/lib-nxvm` for the existing NXVM closure and
`src/lib-win32` for project platform mechanisms, starting with RPC. Existing
`io` moved alongside RPC to keep its Win32 ownership explicit. NTVDM64
protocol clients and authorization/lifecycle policy remain in common or NTSRV.

Local binding and async completion moved to lib-win32/rpc. The service's local
peer process matching was separated from its authentication and authorization:
NTSRV still performs those checks first, then calls the extracted Windows RPC
locality, PID, and process-liveness mechanism. Errors and output clearing are
unchanged. `tests/app/win32_rpc_peer_test.c` covers eight success and refusal
cases; `tests/app/common_rpc_binding_test.c` covers 42 checks and cleanup.

The NXVM import manifest records 52 files, including 44 mechanical include
prefix adjustments. The previous upstream SHA-256 is retained per changed
file, and comparison against the previous commit proves those are the only
byte changes. The original NXVM and libvterm licenses and archive are retained.

The fresh x86 and x64 product builds and ownership negative controls passed;
the x86 frontend-window-library test passed. The staged ten-image package was
checked against all ten fresh input hashes and output hashes; the 63 immutable
guest files and 274 NTVDM export ordinals passed. Build and test command, PID,
time, exit status and output logs are retained under `build/t475-s1`.

Control passed all 12 selected gates on the same ten-image package, including
real DOS/native interaction, rejection, failure and nested-window scenarios.
An earlier run was interrupted by the session switch and is not counted. The
optional external DOS4GW/NESticle fixture was absent and skipped; Full was not
run. The original guest long-PATH limitation remains. Package
`b08813cb-cf4a-48d5-8ec8-cd0590a35f47` is published only to this worktree's
`assets/release`; all ten release hashes matched.

## Process attribute-list extraction P

The owner admitted a local P for `lib-win32/process`. Its generic attribute
list helper owns allocation, initialization, one attribute update and cleanup.
NTSRV Worker creation retains Job policy, admission, resume and failure
rollback; frontend creation retains its inherited handle list, admission and
retirement decisions. Attribute values remain borrowed until list destruction.

The x64 NTSRV and x86 focused fixture were rebuilt. The production library
composition and negative ownership checks passed. The 36-check unit fixture
covers success, allocation, initialization and update failures, duplicate
initialization, resource release and repeat destruction. The package uses the
new NTSRV and nine verified baseline build products. All ten input and staged
output hashes matched; 63 immutable guest files and 274 export ordinals passed.
Build, fixture, stage and Control command/PID/output records reside in
`build/t475-process-p`.

Control passed all 12 selected gates against the identical ten-image package;
the optional DOS4GW/NESticle external fixture was absent and skipped. Full was
not run and the known original guest long-PATH limitation remains. Package
`f3161950-6ff1-4558-9080-9686bd37cb98` was published to this worktree's
`assets/release`, with all ten release hashes verified.

## Shared Win32 handle P

The owner admitted a further P to share same-access handle duplication between
NTSRV's RPC output export and NTHOOK's child attachment. `lib-win32/handle`
accepts explicit source and target process handles; its remote cleanup duplicates
the target-owned handle back with `DUPLICATE_CLOSE_SOURCE`, then closes the
local duplicate. NTSRV retains MIDL allocation, output cleanup and RPC transfer
ownership; NTHOOK retains payload, adoption and rollback decisions.

The fresh ten-image build passed frontend and BaseSrv ownership checks. The
new handle fixture passed 13 checks with an actual child process, including
local duplication, remote duplication, remote release and stale-handle
rejection. The x64 Hook installer regression passed 118 assertions. An initial
link graph generation inserted an input before a multi-output separator; it was
corrected. A later build script combined test executable names; separately
invoked fixtures passed. Build, fixture, staging and Control commands, PIDs,
times and outputs are retained under `build/t475-handle-p`. Ten fresh package
inputs and outputs matched their hashes, 63 immutable guest files and 274 NTVDM
export ordinals passed verification.

Control passed all 12 selected gates on this identical package. The optional
external DOS4GW/NESticle fixture was unavailable and skipped; Full was not
run, and the original guest long-PATH limitation remains. Package
`9baf506d-436a-411a-aeda-cf70fcfea4a0` was published to this worktree's
`assets/release`, with all ten release hashes verified.

## Follow-up P: process-hook

The owner directs generic suspended-child hook propagation into
`src/lib-win32/process-hook`. Its provider owns payload copy and selection of
same-width Detours update versus bounded opposite-width helper installation.
The caller supplies the child, architecture, payload identity and bytes, and
optional DLL; it retains child lifetime and rollback. NTHOOK keeps packet
serialization, target classification, launcher exception, capability delivery
and whether injection is required. Failure still closes duplicated remote
handles; callers must terminate an uncommitted partially modified child.

The registered Detours 4.0.1 source, bounded helper adaptation and MIT license
are relocated under process-hook without byte changes; all eight files match
the previous commit. `lib-process-hook.lib` is independently selected;
`nthook-context.lib` no longer embeds Detours. The x64 hook graph selects
the same source closure. Product builds and dependency negative controls pass.
`tests/app/win32_process_hook_test.cpp` passed seven payload-only, native-width,
cross-width, error and zero-error cases. The x86 and x64
`tests/component-integration/nthook_install_test.cpp` fixtures each passed 118
assertions including nested propagation and rollback. Fresh x86/x64 product
builds succeeded; all ten staged inputs/outputs matched, the 63 guest files
and 274 NTVDM exports passed. Background command, PID, timing, exit and logs
are recorded under `build/t475-hook-p`. Initial graph generation failed because
the dependency insertion encountered a multi-output edge; corrected generation
and all subsequent builds passed.

The first Control run failed the nested-window handoff observer's input-delivery
marker even though the final command appeared on screen and the observed direct
exit was 23. The observer waits for a fresh prompt after Enter, which can race
the final command's process exit; this is a diagnostic inference, not a
product pass for that run. The same package passed a focused handoff repeat
and then all 12 selected Control gates in a complete repeat. The optional
DOS4GW/NESticle external fixture was absent and skipped; Full was not run.
Package `4d73890b-33b8-4edf-8ee7-0856c8b61761` was published only to this
worktree's `assets/release`; all ten release hashes matched.

## Follow-up P: process observation

The owner directs `process-attributes`, `process-observation` and
`process-hook` as separate siblings. The former `process` attribute provider
was renamed and its archive and consumers updated. `process-observation` owns
retaining a query/synchronize handle, obtaining process ID, creation time and
image path, and one-shot exit-wait registration and synchronous callback
drain. NTSRV retains task identity, parent/admission policy, insertion order,
callback meaning, fail-fast decision and handle close. Its production
observation path selects the library. Generic code never depends on common or
private components.

The initial real lifecycle fixture passed an invalid handle into Windows
thread-pool wait registration and terminated with 0xC000070A. That is a test
input/runner failure, not product evidence. The fixture was corrected to use
valid owned wait objects; failure injection now tests registration and drain
errors without submitting an invalid kernel wait to the thread pool.
`tests/app/win32_process_observation_test.c` passed 16 checks including blocked
callback drain; the injected-failure fixture passed three; the renamed process
attributes fixture passed 36 and process-hook seven. The x86 and x64 production
`native-observation-fixture.exe` and `dos-observation-fixture.exe` each passed
identity, parent/replay, exit, rejection and cleanup cases. Build graph
ownership and negative controls passed. Build/test commands, PID, time, exit
status and output are retained under `build/t475-observation-p`.

The x86/x64 product builds succeeded. Ten fresh input hashes and package hashes,
63 immutable guest files and 274 NTVDM export ordinals passed. All 12 selected
Control gates passed with identical ten-image package. The optional external
DOS4GW/NESticle fixture was absent and skipped; Full was not run. Package
`9e98af44-2d1d-4862-8795-5cabfff112c3` was published only in this worktree's
`assets/release`, and all ten release hashes matched.

## Follow-up P: Win32 object handle comparison

The owner admitted four sequential library P deliveries. P1 centralizes the
Windows CompareObjectHandles provider lookup in lib-win32/handle. NTSRV,
NTVWM and NTVDM retain their existing provider-unavailable and mismatch
policies. No new source import or guest behavior change is intended.

The x86 and x64 production builds and ownership checks passed. The handle
fixture passed 16 checks including identical duplicated objects, distinct
objects and remote cleanup. Stage verified 63 immutable guest files and 274
NTVDM exports. Control passed all 12 selected gates for package
c7cf5920-8f98-48c6-badd-24ba9e1a35e8; ten local release hashes matched.
Full was not run; optional DOS4GW/NESticle was absent.

Initial background launches exited before completion and one had a malformed
PowerShell argument. A supervised Windows PowerShell build then failed to
resolve Get-FileHash; the previously recorded pwsh build path succeeded.
The first Control attempt used a staged binary-only service path and its
version-negative helper could not find generated service.h. The full repeat
used OPENNT_BROKER_PRODUCT_BUILD pointing at the actual service build and
passed. Commands, owned PIDs, times, exit codes and logs are in
build/t475-handle-compare-p. No main-runtime publication or push was made.

## Follow-up P: local overlapped pipe pair

P2 moves local byte-mode overlapped duplex named-pipe pair creation and
failure cleanup to lib-win32/io. NTCON retains naming, client PID validation,
authorization, protocol, activation and endpoint ownership. The real-pipe
fixture exercises creation, same-name rejection, validation and data transfer.
It passed 13 checks. An initial fixture incorrectly supplied a pseudo process
handle rejected by the transfer interface; it was changed to an owned process
handle and the complete build and fixture repeated successfully.

The x86/x64 builds and dependency checks passed. Stage checked 63 immutable
guest files and 274 NTVDM exports. All 12 selected Control gates passed for
package 5b5a0256-66cf-43af-9f8f-ec2e9a6d0040 and all ten local release
hashes matched. Full was not run; optional DOS4GW/NESticle was absent.
Commands, PIDs, results and logs are under build/t475-pipe-pair-p; no main
runtime publication or push occurred.

## Follow-up P: parent process observation

P3 moves the native ProcessBasicInformation parent identity query into
lib-win32/process-observation. NTSRV retains parent/reporter authorization,
observation records and their lifetime. The x86/x64 builds, 19 observation
checks including an actual suspended child and invalid inputs, and native and
DOS production observation fixtures on both architectures passed. Stage
verified 63 immutable guest files and 274 NTVDM exports. All 12 selected
Control gates passed for package f8ee2662-b573-41c1-955e-ed7ca0555884;
ten local release hashes matched. Full was not run; optional DOS4GW/NESticle
was absent. Commands, PIDs, results and logs are in
build/t475-parent-observation-p. No main runtime publication or push occurred.

## Follow-up P: Win32 file identity

P4 creates lib-win32/file and moves the existing volume-serial and file-index
comparison from NTHook into it. File-open attributes, share mode, identity
fields, cleanup and errors are preserved; NTHook retains launcher selection.
The x86/x64 builds and ownership checks passed. The file fixture passed 14
checks including same path, hard link, different files and invalid inputs.
The x86 and x64 NTHook installer fixtures each passed 118 assertions. Stage
verified 63 immutable guest files and 274 NTVDM exports. All 12 selected
Control gates passed for package 2b1325bd-17fb-4435-b2be-55995de325f6;
ten local release hashes matched. Full was not run; optional DOS4GW/NESticle
was absent. Commands, PIDs, results and logs are in build/t475-file-identity-p.
No main-runtime publication or push occurred.
