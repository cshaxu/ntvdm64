# Project Status

## Current Work

**Active: M0 T481 S14 — Win95 launch multitask selection.**
S12 was closed by owner decision after the reported close dialog did not recur
in bounded probes; its root cause remains unlocated. See
[S12 evidence](../etc/operations/m0-t481-multitask-completion-plan.md#s12-preliminary-source-observations).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S14, Ordinary Mode. |
| Admission And Approval | Owner approved both patch packages and directed the fresh Win3.1 installation failures to be investigated and repaired in S14 while keeping S13 closed. |
| Objective | Complete Win95 mode selection and repair Win3.1 setup's empty Program Manager and unwanted MULTITASK records. |
| Non-goals | Do not alter or rebuild original guest binaries; do not use Computer Use; do not change unrelated product behavior. |
| Reference Baseline | S13 production package c3bc700e-d623-4059-a47a-0f69641fc7dd; existing Win95 launch package and released driver pair. |
| Files And ABI Surface | tools/win95-launch, tools/win31-setup, shared multitask installer and host execution paths as necessary, applicable addon tools, integration tests, current task records. |
| Applicable Rules | Execution, source policy, guest immutability, source-first and minimal implementation; patch packages use CMD and existing addon tools only. |
| Verification | Validate both Win95 modes and real guest execution; trace and reproduce the Win3.1 Program Manager failure, validate fresh installation and group entries, clean PATCH records and both DOSONLY choices. |
| Expected Markers | Both profiles select the intended mode; DOSONLY=0 installs and configures both released drivers; original guest binaries remain unchanged except previously approved package adaptations. |
| Asset Needs | Released addon tools and drivers, isolated Win95 installation and test facilities; clean task-owned O:/tmp copies. |
| Reporting Requirements | Exact package changes, both mode results, hashes and unverified boundaries; retain integrated acceptance obligations. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Required changes and acceptance completed; coherent package built, tested, published, committed and pushed as applicable. |
| Original Owner Request | Offer Win95 DOSONLY choices; investigate and repair empty Win3.1 Program Manager and unwanted MULTITASK.HASH, MULTITASK.INI.BAK and MULTITASK.INI.NEW in S14. |
| Similar-Issue Sweep | Win31 setup, shared installer and generated Win95 profiles. |

S13 retains the other multitask gaps, normal Win95 shutdown evidence and
integrated acceptance. The direct-start large inherited-environment boundary
remains recorded in [S9 direct startup](../etc/evidence/m0-t481-s9-direct-startup.md).

S13 implementation, four-mode runtime checks and Full delivery are complete.
The post-Setup probe uses a Setup executable shim; a fresh original interactive
Setup run was not repeated. The unchanged original launch and failure-exit
contract was separately tested. See [S13 evidence](../etc/evidence/m0-t481-s13-win31-setup.md).

S14 enhances `tools/win95-launch` with the same DOSONLY=0 / DOSONLY=1
selection and matching launch configuration. For DOSONLY=0, install the
released `insignia.386` and `ntvdm64.386` in the installed Win95 `PATCH`
directory and configure the existing Win95 installation for multitask
payloads. Validate both choices, including launch, guest DOS execution and
the intended host-payload behavior, while preserving original guest binaries.

S14 implementation and complete host package are published for owner
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

The published 11-image host package and installed W31 addon pair are
hash-verified. Win95 WIN.CMD points to the formal package. Original guest
binaries remain immutable. See [S9 evidence](../etc/evidence/m0-t481-s9-softpc.md)
for the 16 Full gates, skips, Win95 observer exit boundary and historical
mirror-guard test limitation.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for the current governance.
