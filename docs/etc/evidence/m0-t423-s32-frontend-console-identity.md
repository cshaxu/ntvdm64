# T423 S32 frontend-root Console identity

## Boundary and implementation

The S31 package let a short-lived run16 connection report the visible
Console's process list. NTSRV copied that launcher's sample into a resident
NTCON for later reuse. S32 makes the registered NTKVM root the sole sampler:
after `AttachConsole` and root registration, NTKVM reports one live,
self-containing member set. NTSRV authenticates the root, pins process objects
for the finite sample, and uses their live object identity rather than a raw
PID intersection. A launcher retains that root and submits a direct request;
it does not publish the outer Console identity. Its two local
`GetConsoleProcessList` uses remain for hidden-Console resume and temporary
WOW Console detach, not root registration.

Both resident worker kinds use the same NTSRV selection path and may outlive
the frontend which launched a task. NTSRV copies the authenticated root's
visible-Console sample to a worker on connection; NTCON refreshes it on direct
delivery. An NTVDM still attached to the old visible Console can additionally
be recognized by its live, pinned worker process object when it is present in
the new root's sample. That fallback is evidence for the same reuse rule, not
a separate DOS lifetime rule. NTSRV remains the sole selection/lifecycle
authority; `worker-base` remains the shared worker-side transport and command
client, not a second registry or Console-identity oracle. Original MVDM,
guest, NTCON execution and wire DTOs were not changed.

`frontend_bootstrap_start` remains in the already separate
`frontend-client.lib`: run16 invokes this client, but the library does not
own frontend identity. Moving its source file from the `ntkvm-exe` directory
would only rename a build input in this S, with no removed runtime mechanism.
NTKVM's service/presentation code was not moved into run16.

## Verification

- MSVC Win32/x86 `/MT` generated graph: all 588 selected commands for
  `run16.exe`, `ntsrv.exe`, `ntvdm.exe`, `ntcon.exe`, `ntkvm.exe`, `ntmon.exe`
  and `VDMREDIR.dll` compiled/linked under `build/M0-T423/S32/formal/`;
  `WOW32.DLL` is unchanged. The serial graph runner was used because this
  host's parallel Ninja output wait stalls. Log: `serial-full.log`.
- `basesrv-service-reservation-test.exe` default, `--console-identity`,
  `--native-worker` and `--worker-channel` passed. The identity negative
  rejects non-root publication, missing self, duplicates, wrong generation
  and repeat publication. Logs/results were taken from the S32 formal build.
- `Verify-ProductVersions.mjs` passed old application version, wrong
  application protocol, forged reply application/protocol and legacy MIDL
  interface for both launcher and worker: rejection was 1306 before task
  delivery. `APP_PROTOCOL_VERSION=25` and `service.idl version(25.0)` were
  already synchronized by S30; S32 changes no wire layout. The test used the
  temporary `Z:` short-path alias and then removed it.
- `Verify-CommandExitStatus.ps1` passed all 17 ordinary Console and all 17
  private-desktop Window cases on the isolated candidate, then repeated both
  complete matrices after the final DOS-worker object-identity correction.
  The verifier checked guest/native text, EDIT exit followed by MEM, nested
  COMMAND and real exit codes. Final logs: `final-console.log`,
  `final-window.log`. Additional isolated
  `native-root-frontend`, `native-cmd-dos-repeat` and `dos-native-dos` passed
  in `reuse.log`.
- The real RPC root capability/descendant/rundown fixture passed, as did
  `frontend-scope-lifetime-test.exe`. A GUI-separated chain yielded root
  identities `BEFORE=22808`, `INNER=28724`, `AFTER=22808`, three
  `GUI-NO-FRONTEND-CAPABILITY` witnesses and exit 37. The numbers are test
  process IDs; the equality/inequality and no-capability markers are the
  assertions. Two idle NTCON workers remained after the two distinct
  sessions, consistent with the admitted resident-worker policy, and were
  ended as exact test-owned processes.
- On the visible desktop, WINMINE reached its main window; SOL reached its
  original memory-error dialog; WRITE reached its original “Not enough memory
  for Write” dialog. The observer recorded resident WOW state and cleaned up
  exact test-owned processes. Logs: `s32-visible2-*-windows.txt` and
  `wow-frontiers2.log`. This preserves previous separate frontiers, not full
  SOL/WRITE acceptance or WINMINE gameplay proof.
- The published `O:/winnt` set passed `direct-mem`, `edit` (exit then MEM)
  and `native-cmd-dos` smoke cases. Its eight hashes match the isolated tested
  candidate. The prior set was saved in `build/M0-T423/S32/published-backup/`.
  No guest binary or configuration was changed.

| Published file | SHA-256 |
| --- | --- |
| `ntmon.exe` | `75EF39E596EE92790657F5217CBA0FFE5323A6C4D41854FA67C0CFB27BDEF1AD` |
| `run16.exe` | `1BC31E3144D6D5025284FE2E7B6E55724C9E394D26C5F7F91DBE017CF20AA3DD` |
| `ntsrv.exe` | `553BCC74C77191B7738E31BEAC41624AB1FA8459A6A2E68289F1AA907B2FE60F` |
| `ntvdm.exe` | `8498C3D3BE4184CA1FF3A6453348019F48094EE9F19721F4EC73A039E6CCE2C3` |
| `ntcon.exe` | `7ED98E0F0B0E89AB40E86DE14FCBC298E075AFD6296A432C77519B657A37B2EE` |
| `ntkvm.exe` | `C42B71B72EAA296DE2228418ACBF45958C37E944DAF1CCB27B7BC91867EEB732` |
| `WOW32.DLL` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| `VDMREDIR.DLL` | `0E6DA4CD00DF66AD280FFF1ED279809855F81D93C8D98DB5605D58F80B77DF24` |

## Non-passes and limits

The old `base-client-rpc-first-test.exe --ntcon-execution` fixture still
creates a nominal root without a functioning NTKVM presentation service.
The actual NTCON correctly waits for frontend I/O readiness and the fixture
times out with 1460 (`stage=begin-io`). S32 updated its obsolete
launcher-before-root member-report sequence but did not count that fixture
as a pass. The isolated product matrices and published smoke tests exercise
the real frontend and NTCON route instead. The old
`verify-native-root-console.ps1` cleanup assertion that every idle NTCON
must retire likewise fails under the owner-approved resident-worker model;
its useful GUI-separated root identity witness was executed directly,
without converting resident workers into a false failure.

The server pins live process identities supplied by an authenticated NTKVM
root. This prevents dead/reused PIDs from matching, but it does not claim to
independently reconstruct Windows' full Console membership from NTSRV's
address space. Root admission and channel capability remain the trust
boundary. No Job observer, descendant task graph or timed member polling was
added.
