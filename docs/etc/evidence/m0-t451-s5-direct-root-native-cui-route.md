# M0 T451 S5 — direct-root native CUI route

## Scope and source finding

This delivery repairs the common direct-root native-CUI delivery failure behind
`run16 command -> VER`, NTMON and the installed native `EDIT.EXE`.  It does
not special-case a shell token, alter COMMAND.COM/BOP/BaseCheckVDM, or modify
guest media.

The original `BaseCheckVDM` path gives a direct DOS root a distinct,
broker-authenticated execution Console.  NTSRV already records the relation
between that context and the visible frontend root in
`service_root_console_matches`.  `service_take_native_command`, however,
discarded that proof and required raw `frontend->console == caller->console`
at delivery time.  The NTVWM-CUI worker consequently received no queued native
command.  It is a delivery-time adapter defect, not a short-lived-command or
final-render defect.

The repair keeps the existing authenticated relation: delivery admits the
visible root Console itself or a broker-issued context belonging to that root;
an unissued/foreign locator remains rejected.  Host Console membership is
still required and `BindNativeTarget` retains its machine/subsystem check.

## Final-frame candidate

The pre-existing final native-CUI release candidate is retained as a separate
ordering improvement.  It adds a versioned `CONSOLE_IO_PRESENT_BARRIER` used
only after NTVWM's final capture.  NTCON's presentation thread, not the
producer, advances the exact `(video pointer, serial)` commit frontier.  A
replacement frame, stop or thread loss fails the barrier; ordinary publication
remains asynchronous.  This does not substitute for the NTSRV delivery repair
above.

## Build and focused evidence

| Check | Result |
| --- | --- |
| x64 service/frontend/worker/launcher/monitor/hook and x86 worker/WOW32 Ninja graph | Passed; the x86 worker safely rebuilt its invalidated Ninja log and linked current `ntvdm.exe`; x64 `ntsrv.exe` relinked from the repaired source. |
| `basesrv-service-reservation-test.exe --native-command-distinct-context` | Passed: an issued distinct context delivers through the actual `service_take_native_command` routine, while a foreign locator returns `ERROR_PIPE_NOT_CONNECTED`. |
| `console-channel-lifetime-test.exe --present-barrier` | Passed: the complete final native frame reaches the production presentation commit frontier before the peer receives the reply. |
| coherent stage | Passed: `build/t451-s5/package/product-system32-manifest.json` verifies all ten explicitly selected images against the immutable extracted `assets/winnt.zip` baseline. |

## Superseded runtime limitation

An earlier restricted agent-token invocation exposed no interactive desktop.
Both the new direct-root observer and the existing private-desktop RPC observer
therefore failed before a visible NTCOn window or Console snapshot existed.
These are retained as historical failed attempts under
`build/t451-s5/direct-ver.txt` and `build/t451-s5/control-r2/`; they are not
claimed as successful guest acceptance.  The product-verification driver also
contained stale removed fixture labels (`startup-timeout`,
`workerless-grace/cancel`); S5 removes them so the current fixture's declared
contract is the single authority.  Its subsequent desktop-unavailable failure
is distinct from that repaired invocation defect.

That limitation does not apply to the documented modern PowerShell
desktop-capable token.  The rule is now explicit in
[Execution](../../rules/EXECUTION.md#private-desktop-test-token): a
`result=not-started` record from a restricted executor is a launch error, not a
desktop or product verdict.

## Terminal-root retirement correction

The direct-route work exposed an adjacent lifecycle regression after a native
and DOS carrier were used in one Console root.  The first terminal-root draft
let *any* retained child launcher start the root's terminal retirement.  Thus
an inner `MEM` completion could mark the root terminal while its outer
`cmd.exe`/NTVWM carrier was still alive; later `exit` could not finish the
normal NTSRV-coordinated handoff.

NTSRV now admits this transaction only from the root creator.  A nested Run16
still retains the root for authenticated routing, but its completion has no
authority to retire it.  Once the creator completes, NTSRV waits for active
carrier work to finish, sends the normal worker-close instruction to an idle
NTVWM-CUI carrier, and advances to the existing frontend return only from that
carrier's actual worker-watch exit.  Run16, NTCON and workers acquire no new
direct lifecycle link; guest files remain unchanged.

| Check | Result |
| --- | --- |
| x64 actual service fixture, desktop-capable modern `pwsh`, 40 cases | Passed in 6975 ms. The new shared-carrier cases prove an inner completion leaves the root admissible, and both native-root/DOS-root terminal completion wait for real idle-carrier exit. |
| Short physical-stage `COMMAND → installed native EDIT → CMD → MEM → exit` | Passed. It proves real native-to-DOS return, visible EDIT/CMD/MEM markers and root launcher completion. |
| Short physical-stage `CMD → nested COMMAND/DOS → parent return → exit` | Passed. It proves the inverse route's actual input/output and direct exit 23. |

The private-desktop cases use the checked runner under the configured modern
`pwsh.exe`; the short stage is the checked `New-PhysicalRuntimeStage` helper,
not a drive alias.  No desktop automation was used.

## Publication

The final cached full-component run at
`build/t451-s5/terminal-retire-r13/full-component-build/result.json` passed:
all ten selected images built from the stable Ninja trees, all 1,488 checked
intermediates remained unchanged on the second build, and the final
`ntsrv.exe` SHA-256 is
`46E153D87EC815A9F5609B5BED79458F46DED3669AAAA0A5515F2692E3A7D61F`.
The final private-desktop product probes at
`build/t451-s5/terminal-retire-r13/final-probes.result.json` passed against
that package before publication.

The staged ten images were copied, after recoverable prior copies were made,
to both `assets/release` and `O:\winnt\system32`.  Candidate, release and
installed SHA-256 values were compared entry by entry in
`build/t451-s5/terminal-retire-r13/publication/published-manifest.json`.
`assets/winnt.zip` and all guest files remain unchanged.

The final product probes prove both cross-carrier directions through real
Console input/output; remaining owner acceptance is visual/usability evidence,
not a substitute for those lifecycle assertions.

## Root Run16 exit regression

The former root-only hang was not a COMMAND, `VER`, `MEM`, or shell-token
failure.  It was an NTSRV retirement ordering error.  Once the creator's root
transaction completed, `service_advance_terminal_root_retire` treated only an
idle NTVWM-CUI carrier as closeable.  A live DOS carrier was therefore left
open.  In the inverse shape, it final-closed the frontend before the creator's
already-defined `WaitFrontendConsoleRestored` exchange.  NTCON then took its
final-close path rather than PARKing and acknowledging restored Console mode,
leaving the root launcher waiting indefinitely.

The service now owns both actions consistently:

1. only the root creator can begin terminal retirement;
2. the service sends `WORKER_CLOSE_WORKER` to either textual carrier kind
   (`DOS` or `NATIVE`/NTVWM-CUI); WOW and GUI-launcher observations are not
   Console carriers;
3. after their observed exit, it clears the root retirement transaction but
   preserves the frontend lease; and
4. the creator's existing restoration RPC lets NTCON PARK and acknowledge
   Console return before Run16 exits.

No component gains a new direct lifecycle link; no guest file, BOP, COMMAND
or BaseCheckVDM logic changes.

`tests/observation/verify-root-carrier-exit.ps1` is the fixed executable
regression.  It uses a physical isolated package, a clean deployment-like
environment, and result-gated input—not typeahead.  Against
`build/t451-s5/root-exit-r12/package`, all of the following passed:

| Root form | Command | Result required |
| --- | --- | --- |
| `run16 command` | `VER`, then `EXIT` | visible host `VER`; direct launcher exits 0 |
| `run16 command` | `MEM`, then `EXIT` | visible MEM result; direct launcher exits 1 (guest MEM's documented result) |
| `run16 cmd` | `VER`, then `EXIT` | visible host `VER`; direct launcher exits 0 |
| `run16 cmd` | `MEM`, then `EXIT` | visible MEM result; direct launcher exits 0 |
| `cmd → run16 command` | `VER`/`MEM`, then inner and outer `EXIT` | visible result; outer launcher exits 23 |
| `cmd → run16 cmd` | `VER`/`MEM`, then inner and outer `EXIT` | visible result; outer launcher exits 23 |

The service fixture also passed after rebuilding the affected x64 service
target.  It now asserts the mixed DOS/NTVWM-CUI case directly and verifies
that terminal rundown preserves—not prematurely closes—the frontend lease.

The tested ten-image package was published after recoverable copies to both
required destinations.  `build/t451-s5/root-exit-r12/publication/
published-manifest.json` records entry-by-entry equality.  The new `ntsrv.exe`
hash is `3FE8AE82825DB4EC747063E4975FAF326FA5D31A397676B3E60C40B652C87872`.
`ntvdm.exe` and `ntvwm.exe` were refreshed as part of the required coherent
package, not because this repair changes their source.  `assets/winnt.zip`
and every guest file remain unchanged.
