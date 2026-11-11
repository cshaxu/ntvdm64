# Project Status

## Current Work

**No active M/T/S packet.** T481 S16 is closed with owner acceptance.
T481 remains open; no next S is admitted by this closure.

## S16 accepted closure

The owner accepted the EDIT.COM mouse corruption and persistent residue fixes.
The optional atomic-transition change was declined after source review: cleanup
can be visible during switching, but the old block no longer persists or
accumulates. No new cross-layer presentation barrier was added.
See the [S16 evidence and closure](../etc/evidence/m0-t481-s16-win31-mouse-transitions.md#final-verification-and-closure).

Cached x86/x64 builds passed. Final Win3.1 generated-profile matrix: 14/14;
Full: 16/16 selected gates. DOS4GW-NESTICLE was explicitly skipped for a
missing external fixture, not reported as passed. Focused negative/fixed
mouse controls passed. The complete 11-image package
2fef3b20-3b5c-4466-b701-feb9ac9101c9 is hash-verified in assets/release and
O:/winnt/system32. O:/w31 addon hashes match assets; no guest binary was
changed and winnt.zip is untouched.

Win31-setup retains the accepted S15 dynamic launcher. A final proposed
VMD-key normalization was withdrawn: mouse=*vmd is a documented original
Win3.1 setting, not an obsolete spelling. The mouse repair does not depend
on forcing that key into device=. No unverified setup change is delivered.
S16 source, tests, assets and evidence form the closure P delivery.

## S15 accepted closure

The owner-approved launcher is implemented in tools/win31-setup/WIN.CMD.
Setup always installs both drivers and PIF.EXE; the installed WIN.CMD selects
mode/DOSONLY and toggles the two device entries, generating one WIN.PIF and
the NT files. The known S14 mouse issue remains deferred. See the
[S15 implementation and verification](../etc/evidence/m0-t481-s15-win31-dynamic-launch.md).
Cached x86/x64 builds reported no work. Script contract, package round trip,
exit-code and failure tests passed. Six scheduling contracts and all 14 runtime cases passed on the new generated
profiles using a static isolated source. Temporary guest copies were removed.
All 11 published host hashes remain unchanged at both release locations.
Task scratch: build/t481-s15 and short build/s15a through build/s15d,
owner S15, 8-hour/20-GiB budget and 30-minute no-progress review. The real
matrix owns and removes O:/tmp/q31; never remove the owner's O:/w31.

## S14 closure and handoff

S14 is closed by owner direction on 2026-11-10. The six scheduling cases passed
with independent Win16 progress measurements; the complete 14-case matrix
also passed. This is not a claim that the later reported EDIT mouse transition
is fixed. Its configuration, reproducer, captured state, unresolved hypotheses
and next steps are retained in the [closure and mouse handoff](../etc/evidence/m0-t481-s14-win95-launch.md#s14-final-scheduling-verification-and-owner-directed-closure).
The owner no longer needs to preserve the live window. S15 does not continue
that investigation. Subsequent scope allocation remains pending; the former
Win95 and mirror-review obligations are retained in the proposal.

## Evidence from earlier deliveries

The S14 Mouse31 source candidate passed the eight basic profile interactions
and both DOSONLY=1 guest pairs. The old release leaked desktop callback mask
001Fh into both DOS VMs; the candidate returns zero for both. See the
[paired control and test boundaries](../etc/evidence/m0-t481-s14-win95-launch.md).
All four DOSONLY=0 profiles returned a host CMD version receipt and exited
Windows normally in native-command-receipt. The enhanced two-addon profile
also completed two overlapping native commands with verified exit codes.
All four enhanced configurations passed windowed prompt -> Alt+Enter ->
Console text -> CAF -> window text -> DOS completion and Windows mouse/exit.
The 16 selected Full gates and the setup CMD media round trip passed.
The driver in assets/release and O:/w31/SYSTEM/MOUSE.DRV matches the tested
candidate. The final integrated release matrix passed all 14 runs, including focus and distinct post-switch input in both guest DOS windows.

S13 implementation, four-mode runtime checks and Full delivery are complete.
The post-Setup probe uses a Setup executable shim; a fresh original interactive
Setup run was not repeated. The unchanged original launch and failure-exit
contract was separately tested. See [S13 evidence](../etc/evidence/m0-t481-s13-win31-setup.md).

Earlier S14 work enhanced `tools/win95-launch` with the same DOSONLY=0 / DOSONLY=1
selection and matching launch configuration. For DOSONLY=0, install the
released `insignia.386` and `ntvdm64.386` in the installed Win95 `PATCH`
directory and configure the existing Win95 installation for multitask
payloads. Validate both choices, including launch, guest DOS execution and
the intended host-payload behavior, while preserving original guest binaries.

The earlier S14 implementation and complete host package were published for owner
acceptance. Both choices were exercised in real isolated Win95 copies; the
guest-only fixture terminates its own session after the DOS witness and does
not establish normal Win95 shutdown. The original interactive Win3.1 Setup
was not rerun. See [S14 evidence](../etc/evidence/m0-t481-s14-win95-launch.md).

Owner observed that a fresh `O:/w31` installation has an empty Program Manager
and unwanted multitask recovery records. S13 remains closed; these are S14
acceptance failures. An empty-file read defect was fixed and verified through
the real Win16 profile API and an original Program Manager DDE group-creation
baseline/fix comparison; the exact original Setup call chain has not been
proved by a completed fresh installation. The owner's existing four groups
were restored and the three verified obsolete records removed. The final
11-image host package is published for renewed acceptance.

Owner subsequently directed a project-wide mirror adaptation review: restore
original code where its behavior was unnecessarily diverted by local seams.
The first candidate restores the shared zero-length lease contract and removes
the added `demRead` bypass. A 1,060-file source-pin census is recorded in
`artifacts/mvdm-audit/t481-s14-mirror-restoration-inventory.json`; its entries
are pending review, not completed dispositions. The candidate has not yet
completed the project-wide audit. The zero-length checkpoint passed Full and
was published as package `4089b369-3dd9-419c-aee6-382052fee85e`.
The next candidate also removes the local zero-length write bypass; 20 real
DOS truncate/seek rounds and 20 guest VDMREDIR zero-byte pipe writes passed,
along with all 16 selected Full gates. Package
`b385aa0d-20bc-4de9-ad4e-3372a4e14bfc` was published to both host locations.
That package was subsequently rechecked through both real Win95 launch modes
and all four Win3.1 Program Manager DDE mode combinations. The Win95
guest-only witness still retires its own session. The owner subsequently
reported successful `tools/win31-setup` installation in a new Windows 3.1
directory with normal Program Manager groups; the automated original Setup
attempt remains incomplete and the reported mode was not specified.
For DOSONLY=0, the root Run16 and NTVDM both returned 1 after the guest
ExitWindows witness. A mapped call-site trace identifies the original
`cmdGetNextCmd` return path; why the guest supplied 1 remains unproved, so
this is not recorded as normal-shutdown acceptance.

The DEM open/create pathname lease now shares its identical copy and error
handling in one local helper. Cached x86/x64 builds and the x86 worker relink
passed, as did all 16 selected Full gates and the real Win3.1 empty-profile
probe. Package `092da729-caf9-4a89-8f2e-305e368faacf` was published to both
host locations. The owner subsequently reported successful fresh Setup with
normal program groups; this is manual acceptance, not an automated Setup pass.
The same published package then passed the setup-selection/runtime matrix:
both DOSONLY choices and Standard/386 startup and normal exit, with four
parseable groups in each copy. That matrix still uses a Setup shim.

Known baseline: the Win95 dual-editor test uses guest USER synthetic mouse
events and therefore does not prove the owner's physical mouse route. With
the current published package, its two prompt VMs both opened menus, saved
their distinct modified files and closed; the original installation was
unchanged. The
published O:/WINNT/edit.exe and C:/Windows/System32/edit.exe have identical
SHA-256 `5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`.
An independent real hidden-console probe observed Edit input mode 0x288:
native MOUSE_EVENT records were ignored while the selected NTVWM VT mouse
conversion opened its File menu. This does not establish end-to-end delivery
through Win95, focus, geometry or the physical RDP route.

[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md) records the input-chain investigation.

S14 disposable workspace: `build/t481-s14`, owner S14; budget 8 hours,
20 GiB, 30-minute no-progress review. Stop and join owned processes, record
results, and clean task-specific O:/tmp copies before retirement. The owner
diagnostic package remains available for the close investigation.

## Current Technical Baseline

The published 11-image host package was hash-verified. The installed Win3.1
addon selection depends on the selected profile. Win95 WIN.CMD points to the formal package. Original guest
binaries remain immutable. See [S9 evidence](../etc/evidence/m0-t481-s9-softpc.md)
for the 16 Full gates, skips, Win95 observer exit boundary and historical
mirror-guard test limitation.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for the current governance.
