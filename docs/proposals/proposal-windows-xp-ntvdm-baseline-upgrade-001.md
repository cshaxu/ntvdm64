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
