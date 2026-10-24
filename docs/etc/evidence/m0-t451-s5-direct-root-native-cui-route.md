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

## Runtime limitation

The automated execution context exposed no interactive desktop (`cua` reported
no native app surface).  Both the new direct-root observer and the existing
private-desktop RPC observer therefore failed before a visible NTCOn window or
Console snapshot existed.  These are retained as failures under
`build/t451-s5/direct-ver.txt` and `build/t451-s5/control-r2/`; they are not
claimed as successful guest acceptance.  The product-verification driver also
contained stale removed fixture labels (`startup-timeout`,
`workerless-grace/cancel`); S5 removes them so the current fixture's declared
contract is the single authority.  Its subsequent desktop-unavailable failure
is distinct from that repaired invocation defect.

## Publication

The staged ten images were copied, after recoverable prior copies were made,
to both `assets/release` and `O:\winnt\system32`.  Candidate, release and
installed SHA-256 values were compared entry by entry.  `assets/winnt.zip` and
all guest files remain unchanged.

The owner should exercise the direct new-Console matrix on the normal desktop:
COMMAND `VER`, NTMON and installed native EDIT; DOS MEM/EDIT.COM and inherited
CMD remain controls.  This is an acceptance limitation, not a claim that the
runtime window matrix passed in the unavailable automation desktop.
