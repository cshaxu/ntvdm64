# Proposal — Windows XP NTVDM baseline upgrade

## Owner objective and status

Upgrade the project's NTVDM baseline to Windows XP, support audio and long file
names (LFN), update all guest components, and implement necessary modern-host
compatibility repairs. This is an unnumbered candidate; its position is governed
by [Queue](../states/QUEUE.md), and it does not change the active task.

## Scope

1. Inventory the present host and guest baseline against an identified Windows
   XP NTVDM edition, including all guest components, build and runtime bindings,
   interfaces, source and binary provenance, and compatibility differences.
   Determine availability and rights under the [source policy](../etc/operations/policy/source-policy.md)
   before acquiring or integrating any source or guest components.
2. Plan and implement the baseline transition across the runtime and all guest
   components as a coherent package. Record each component's selected XP
   counterpart, compatibility, deployment identity and any unavailable or
   excluded component; do not silently retain an incompatible mixed baseline.
3. Recover end-to-end audio capability and LFN behavior through their actual
   guest, worker and host owners. Identify supported interfaces and applications,
   preserve error and lifecycle semantics, and test real guest behavior.
4. Identify and repair modern-host incompatibilities from reproducible evidence,
   retaining original behavior and using the smallest justified host adaptation.
   Avoid application-name or path-specific workarounds.
5. Preserve existing DOS, Win16/WOW, graphics, input, worker, service and launch
   capabilities, and explicitly resolve migration, installation and rollback.

## Acceptance

Bind all tests to the selected coherent package and guest identities. Cover
real DOS and Win16 applications, audio output and lifecycle, LFN creation,
enumeration, rename and error handling, long/non-ASCII paths, concurrency,
startup and shutdown, and regressions against the pre-upgrade baseline. Record
unsupported or unavailable components and unresolved rights or source gaps
without claiming the upgrade complete. Existing guest-immutability and source
rules apply unless the owner separately authorizes a specific change.


## Accepted transfer: Win95 restart into MS-DOS

The owner assigns the unresolved restart-to-MS-DOS work to this package and
closes its predecessor as investigation/handoff, not a repaired feature. See
[closure](../history/m0-t482-closure.md),
[actual evidence](../etc/evidence/m0-t482-dos-restart-investigation.md) and
[bounded design](../etc/operations/m0-t482-win95-dos-restart-plan.md).

Carry both obligations without dropping either:

1. Trace and supply the filesystem capability required by the real Win95 Shell
   to create/open its localized Exit-to-DOS PIF. Current guest volume reports
   maximum component 12, flags 0; long paths fail before DEM CreateFileOem,
   short-name controls succeed. A host-precreated long-name PIF is insufficient.
2. Trace and repair the downstream transition independently. A short-name test
   PIF reaches QUERYENDSESSION/ENDSESSION but does not prove usable DOS return,
   with both companions on, NTVDM64 off or both off. The exact downstream cause
   remains unknown; neither timeout nor a single CPU address proves deadlock.

Evaluate XP guest/host changes against the actual Win95 provider contract;
XP-era LFN support alone is not evidence that Win95 IFS is connected. Preserve
original guest binaries, error semantics, NTSRV lifecycle authority and one
justified implementation. No Shell resource patch, filename special case,
false LFN advertising, forced worker exit or synthetic successful return.

Acceptance must use the real Explorer menu as well as diagnostic controls:
localized/ASCII long-name access, normal shutdown control, real DOS prompt and
command/input interaction, intended Windows return/restart, actual process
cleanup and applicable DOS/WOW/Win3.x regressions. Bind receipts to exact guest,
package and driver settings. Keep code 1's existing approved boundary separate.
Do not reuse predecessor diagnostic job exit 0 as runtime success; those jobs
only prove owned cleanup, except separately identified normal-shutdown controls.
