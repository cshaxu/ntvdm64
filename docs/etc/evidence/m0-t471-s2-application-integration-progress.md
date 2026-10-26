# M0 T471 S2 — three-application integration progress

## Frozen input

Every run uses the S1-frozen `build/t467-s3/package-r2` artifact.  The staged
inputs recorded by each probe match the S1 manifest, including `run16.exe`
`91811CEF9FC34564827AE530D700F46CF06BA7763035FC3DF206D1674AB2A30D`,
`ntvdm.exe` `7F2312AF2CC8480264B4935E2F5C9910D1A99D38C78C8F8DB57156FF1168611B`,
and `WOW32.DLL` `506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23`.

## Executed integration evidence

| Application / entrypoint | Observed result | Disposition |
| --- | --- | --- |
| WRITE — `verify-wow-write-lifecycle.ps1`, `build/t471-s2/write-r6` | Two real `WRITE.EXE` rounds on one owned private desktop found `MSWRITE_MENU`, observed normal close, returned `run16 --wait` exit zero, then restarted and repeated cleanly. | Pass for startup, close and restart. |
| WINMINE — `verify-wow-mine-gameplay.ps1`, `build/t471-s2/mine-r2` | The real guest probe changed a cell and counter by flagging, used F2 to restore a covered cell, opened a cell by left click, closed the owned window, and observed zero launcher exit. | Pass for its selected interaction/close route. |
| SOL — `verify-wow-sol-gameplay.ps1`, `build/t471-s2/sol-r3` | The actual host desktop was 427x320 with 447x340 maximum tracking; SOL's 585x384 request was constrained to 431x281.  Probe verified the exact geometry witness, enabled Deal command, normal close and zero launcher exit. | Pass for the geometry-limited integration branch; deal/draw coordinates are not applicable on this host. |
| SOL Options — `verify-wow-sol-options.ps1`, `build/t471-s2/sol-options-r1` | Original command 1003 opened and closed the modal dialog, restored its parent and completed the launcher. | Pass. |

The SOL runner now accepts the geometry-limited branch only after reading the
probe's exact constrained-host marker.  It does not relabel that branch as
deal/draw gameplay; a host with enough tracking area continues into the
existing pixel-verified deal/draw path.

## Remaining finite S2 boundary

WRITE's ordinary `WM_CHAR` edit path and its Save As modal file-dialog submit
cannot be represented by cross-thread `WM_COMMAND` or queued Enter on an
inactive private desktop.  The prior source/physical analysis at
[T449 S4](m0-t449-s4-reconciliation-ledger.md#write-physical-interaction-witness-2026-10-23)
shows why: those messages do not make the original document queue or native
common-dialog loop consume the input.  Treating them as an interaction pass
would be a test-only semantic substitute.

The remaining S2 acceptance item is therefore a visible-desktop physical
WRITE edit/select/menu/dialog/save/reopen/failure/cancel/resize run against
the same S1 package.  This is a bounded test-driver limitation, not evidence
of a product defect or permission to change guest/host behavior.  S2 is not
closed by this progress record.
