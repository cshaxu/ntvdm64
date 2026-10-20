# M0 T447 S4 P4 — NTCON CAF management-close handoff

## Scope and ownership

This is a project-owned host lifecycle correction.  It changes NTSRV and
NTCON only; it does not alter or rebuild guest source/media, OpenNT mirror
files, guest configuration, or public RPC layout.  The existing scalar
workerless-frontend reply acquires two named values in the shared host header:
final close and pre-close return-to-Console.

## Root cause

The reported sequence is `run16 command` (or `run16 cmd`), Ctrl+Alt+F, NTMON
Console deletion, then return to the outer CMD.  A failing trace already
showed restored cooked mode (`0x1f7`), so a mode-bit or input-flush patch would
have addressed the wrong symptom.  The management path ordered DOS worker
shutdown before NTCON left Window mode.  Worker shutdown broke the owned
NTCON channel (`ERROR_PIPE_NOT_CONNECTED`); NTCON could not synchronously
select the borrowed Console before teardown.  The parent CMD's next read then
did not render one-character input until Enter.

Natural CAF return and CAF followed by normal Window close did not exhibit
this route, isolating it to management close while the Window route was live.

## Correction

1. NTSRV puts the selected frontend into its existing draining state and
   signals a pre-close return request while the authenticated channel lives.
2. NTCON's presentation thread selects the Console synchronously and reports
   the existing frontend-restored acknowledgement.
3. NTSRV waits, bounded at ten seconds, for that acknowledgement, then sends
   the unchanged authenticated worker-close request.  A failed acknowledgement
   still attempts requested worker close but is reported as failure.
4. Final frontend close is published only after that close route completes.
5. A second, unrelated lifecycle writer in `worker_registry.c` that made the
   final worker exit close an ordinary root immediately was removed.  The
   existing lifecycle deadline remains the single owner for ordinary
   workerless-root retirement.

This is not a delay/retry policy and does not change original DOS final
`ResetConsoleState` behavior.

## Verification

| Evidence | Result |
| --- | --- |
| `basesrv-service-reservation-test.exe` | Pass: reservation kinds, CUI/WOW worker reuse, authenticated close/lifecycle and original Check/Update/Get/ExitVDM path. |
| `Test-FullComponentBuild.ps1` final r3 | Pass: 13 images plus `wow_message_observer`; second build rebuilt no output or intermediate; 1,344 intermediate records unchanged. |
| Staging | Fresh `assets/winnt.zip` baseline; every selected image PE-machine and SHA-256 checked. |
| Publication | Exact 13 files in `assets/release` and `O:\winnt\system32` match the final manifest. |
| Runtime observer | Its command line quoting and hidden-window defects were corrected.  In this automation desktop the actual `LibKvmWindow` remains undiscoverable (`find-kvm`) after the outer CMD and inner command prompt reach readiness.  This is not counted as a product pass. |

The final manual acceptance should use the actual owner route: `run16 cmd` or
`run16 command`, Ctrl+Alt+F, NTMON Console delete, then type a character in
the original terminal before Enter.  It must echo immediately.

## Release identities

The final deployment's changed host images include:

- `ntcon.exe`: `0C7F46E000FEBF5B24F95391FD5B8220E97AF360A918C34A1A13AB040BB05845`
- `ntsrv.exe`: `FC191F97FB0EE1BEB4190B98B5BC37D197CC90E880DB48A6200EDFECD59E4D8E`
- `ntvdm.exe`: `691EE63CA50020E161403F035B6FD29D68CCC20117EE8356774F62AAE1BF3FFF`
- `nthook32.dll`: `6C48BBB2AEF67ECD2E066D3F214ACD111BB615A30D22192271C2104D4FA9C9BE`

All other selected image identities are listed in `assets/release/release-manifest.json`.
