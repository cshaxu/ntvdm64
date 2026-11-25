# T484 S7 DOS error dialog mouse handoff

The original DOS error panel at `softpc.new/host/src/nt_error.c` asks for
Window input suspension before opening the existing dialog. The Console
endpoint carries that request to the NTCON window controller. A guest VM's
frontend uses the parent endpoint when it shares that window. The owning
Window thread drains its existing control mailbox and acknowledges the
freeze after the native capture and clip release. The synchronous Win32
message uses a ten-second timeout and the existing component failure path.

The existing ErrorPanel resource and dialog procedure keep the decision.
Ignore and other continuing decisions release the hold when the dialog
returns. Terminate keeps it until channel teardown. Closing the channel also
clears abandoned holds. Console presentation has no Window to suspend. WOW
still uses the original WOW decision path.

The mirror callsite changes one line, with one deletion and one insertion
relative to the S6 checkout. No condition structure or diagnostic line
directive changes. The shared Window library adds a bounded acknowledgement
to its existing freeze control and records both local and upstream hashes.
No original guest binary is rebuilt.

## Verification

The cached CMD host build succeeded and staged the eleven images as package
`0b82f7de-640f-43e1-962e-73f904c0bf60` in the build workspace.
The native Window tests passed 2/2, including the capture and confinement
contract with substituted Win32 calls. The existing private-desktop Console
frontend test passed with additional valid, invalid and failed modal
dispatch cases. These do not certify the live error-panel acceptance.

The initial Full attempt used stale test-cache host images and was rejected
before running. With matching images, an inherited-environment run reached an
illegal-instruction prompt before WinMine drew. A separate short-root
verification run uses the final package and a test-process environment
without agent-specific variables.

The final Full run passed 20 of 20 gates, including both 17-case shell
matrices and the real DOS4GW/NESticle menu and Escape checkpoints.
The reports and the 22 byte-identity checks for publication are in
[S7 evidence](../../../artifacts/build-migration/t484/s7/acceptance.json).

## Live DOS panel

Build the existing communications fixture with
`tools\build-fixtures.cmd x64 guest-comms` and the observer with
`tools\build-fixtures.cmd x64 observation-tools`. Use the existing
`New-PhysicalRuntimeStage -FreshPackageIdentity -TestFiles <C29.COM>`
helper to place the fixture at `tests\C29.COM` in a disposable package.
Run `console-startup-observer.exe` with the staged `run16.exe`, staged
root and a fresh report path, followed by `--wait`, the staged
`system32\COMMAND.COM`, and `--observation-timeout-ms 30000`.
Set `MVDM_OBSERVER_PRIVATE_DESKTOP=1`, `MVDM_OBSERVER_OWN_JOB=1`,
`MVDM_OBSERVER_WINDOW_INPUT=1` and select
`MVDM_OBSERVER_DOS_DIALOG_MOUSE` from the table.

| Profile | Required observation | Result |
| --- | --- | --- |
| `captured` | CAF enters Window. Win32 reports capture and changed clipping before the fixture. When the DOS panel is visible, capture is gone and the previous clipping rectangle is restored. A mouse click selects Ignore, the fixture completes, and MEM and exit still work. | Passed twice in independent sessions |
| `uncaptured` | A Window without capture presents the same panel, accepts Ignore, and continues through MEM and exit. | Passed |
| `terminate` | A captured Window presents the panel with capture and clipping released. A mouse click selects Terminate and the launcher exits within the deadline. | Passed |
| `console` | The Console presents its original panel, accepts Ignore, and continues through MEM and exit. | Passed |

The live probe reads the native capture owner and clipping rectangle. It
samples their state at the first observed visible panel. The call ordering is
established by the synchronous acknowledgement and the following dialog call
in source. The probe posts mouse button messages to the fixture's own
controls on the private desktop. The repeated live case uses fresh sessions
because the original COM provider reports a failed device open once per
adapter in each session. Earlier probe attempts with the wrong fixture path
never reached the panel.

The eleven tested images and matching manifest were published to
`assets/release` and `O:\WINNT\system32`. Both sets of hashes match the
candidate. A direct restaging attempt from the installed directory was rejected
by the test staging helper before launch. The recorded runtime passes were
obtained from the identical staged release package.
