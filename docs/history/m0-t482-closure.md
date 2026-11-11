# M0 T482 closure: DOS-restart investigation transferred

## Owner decision

The owner closes this task and transfers the unresolved Win95 restart-to-MS-DOS
work to the queued [Windows XP baseline upgrade](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md).
This supersedes the prior pending scope question. Closure means investigation
and explicit handoff accepted; it does not mean the requested feature is fixed.

## Delivered evidence

- P1 3508c115e: admission, real shutdown-dialog witness, filename/PIF controls,
  independent guest diagnostic and original DEM call-boundary observation.
- P2 14967466b: companion-off downstream controls and bounded scope decision.
- [Investigation](../etc/evidence/m0-t482-dos-restart-investigation.md) preserves
  original identities, exact commands/receipts, positive and failing controls.
- [Plan and decision brief](../etc/operations/m0-t482-win95-dos-restart-plan.md)
  preserves the original goal and rejected special-case approaches.

The current Win95 volume reports 12-character components/no LFN. The original
localized Exit-to-DOS PIF and ASCII long-name controls fail before DEM file
creation, while short paths succeed. A diagnostic short PIF gets past preparation
and reaches session-end notifications, but usable DOS return remains unproved.
Disabling NTVDM64 or both companions does not demonstrate successful DOS return.
Different protected-mode terminal samples are not proof of a shared deadlock.

## Complete transfer

The receiving proposal owns both the long-name provider gap and the separate
post-session-end transition, including its unknown root cause. It must verify
the real Explorer menu, DOS input/commands, Windows return/restart, lifecycle
and relevant regressions. No task is dropped or silently treated as passing.
The XP baseline is not assumed to supply Win95 protected-mode IFS support.
Original guest immutability and minimal, unified implementation remain binding.

## Verification and release disposition

Independent probe builds and actual private-desktop controls ran. Final filename
controls preserve LastError; the normal-shutdown control exits naturally with
the previously approved guest code 1. All owned guest copies were cleaned and
the original installation hashes checked. Diagnostic timeouts remain failures
or inconclusive observations, never Full or feature acceptance passes.

No production source, original guest, installed configuration or release asset
changed in T482. Published 11-image package remains
7773a9b4-e360-4469-962a-df26bb16faf4. No new runtime build, Full pass or product
publication is claimed. This owner-directed closure/admission delivery changes
only documentation and uses documentation governance and diff review.

T483 is admitted separately through [Current](../states/CURRENT.md), following
the queue order. The transferred issue is owned by the XP proposal rather than
a duplicate TODO or another active task.
