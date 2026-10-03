# T423 S38: resident frontend reuse and direct completion

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

S37 was the reference package. The owner reproduced `run16 command` failing
on a later launch in the same Windows Terminal CMD tab (error 1460); killing
the retained NTCON made another launch work. This was not a COMMAND guest
failure: the previous borrowed frontend lease had drained its DOS channel,
NTSRV had removed the delivered route, and the resident NTVDM retained the old
channel. Reusing that worker left its next request without a viable frontend.

## Boundary and repair

- NTSRV remains the authority for worker/root associations and starts a
  cancellable ten-second workerless grace only after the root is idle with no
  worker or pending task. The active worker can remain READY after its direct
  task ends; a launcher exit alone does not kill it.
- A borrowed NTCON lease now parks Window/Console presentation, restores the
  outer caller's input mode, and keeps delivered worker channels. A later
  direct request reuses the same authenticated route. A dedicated root still
  drains channels when it actually retires. There is no second frontend
  process, new polling loop or DOS execution scheduler.
- NTW32 still creates the direct Win32 text target suspended and binds its
  actual PID before resuming it. NTW32 owns the process handle and reports the
  real Windows exit code after releasing native I/O. NTSRV writes the result
  to its direct Win32Record and signals the run16 receipt; run16 reads that
  result instead of treating a process handle as its own completion source.
  The result is recorded before the event is signalled, avoiding a wake-before-
  result race. Preflight and worker failure are not fabricated success.
- The original OpenNT DOS record, `GetVDMCommand`/exit path and WOW startup
  return remain at their original owners. This project-owned cross-process
  frontend and native completion code has no directly composable historical
  OpenNT translation unit: the original in-process Console ownership and DOS
  completion are retained, while the minimum copied/authenticated adapter
  carries only the new process boundary. No guest, MVDM mirror, shared KVM
  library, Win32 GUI completion policy or process-tree kill was changed.

Interface/application protocol and RPC interface revisions are both 28.
IDL was regenerated for x86 and affected host binaries were relinked. The
normal Ninja runner in this environment stalled without diagnostics; the final
NTSRV object/library/link were executed from the generated graph with the
same x86 MSVC toolchain. This is not claimed as a fresh cold full-graph build.

## Verification

- Focused fixtures passed: NTW32 next-command; NTW32 execution lifetime
  (448 checks, zero failures); frontend scope/request client (including real
  native exit 37); BaseSrv reservation default case; and private-desktop
  Console channel lifetime, including two park/resume cycles and input-mode
  restoration. The final NTSRV relink passed the reservation fixture again.
- In one persistent isolated CMD, `run16 command → exit` repeated twice with
  the same NTVDM PID; then `run16 cmd → exit` returned. Native
  `cmd /c exit 37` returned 37, and DOS `COMMAND.COM /c ver` printed
  `MS-DOS Version 5.00.500`.
- The final package passed 17/17 Console and 17/17 private-desktop Window
  matrix cases, including COMMAND/MEM/EDIT, nested MEM, native streams and
  guest exit status. Reports:
  `build/M0-T423/S38/r002/s38-race-console-summary.json` and
  `build/M0-T423/S38/r002/s38-race-window-summary.json`.
- Killing only the isolated NTVDM worker while a direct DOS launch waited
  returned 1067 to its run16. The workerless NTCON survived at seconds 0–9
  and retired at approximately second 10; NTSRV's own empty-service grace
  followed. This establishes the tested death edge, not a universal timing
  guarantee across arbitrary host scheduling.
- The published `O:/winnt` eight-file package matches the tested staged
  package byte-for-byte. A published noninteractive native smoke returned
  `native-exit=37` after the final NTSRV correction. The temporary `Z:`
  build-root alias was removed.

| File | Published SHA-256 |
| --- | --- |
| run16.exe | `879CFDF3169E17E9EDE6F6BD5FA405339CED4840AB50D06664B3C9C1744A2FBD` |
| ntsrv.exe | `5566B5C4B5FC86D97A54E6B0F66300AFEB33B6E7973D29DDFD67BA70D3BB76E5` |
| ntcon.exe | `3252111E651719B3A921FFB51A7AB79A66C991FCD28935AD83E6573CE65683AF` |
| ntvdm.exe | `60155D9B1E8DF83F17AC407B682EFF80033A3A9584CA0CE2314DAFCEE783C0C5` |
| ntw32.exe | `CBAB7B42FBDB2411126B3B1A5DC0CD8D80183DE94C2EE8CCFE5816EA04026966` |
| ntmon.exe | `3D86477B95A673B602540952805790DDFE7C4541496819030ADF16DDBD4FBA53` |
| wow32.dll | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| VDMREDIR.dll | `1A2418FE667348EF3C6764A85C375A53B40C00D3ECAF2FBC8F74B2D1DF4D881F` |

## Explicit limits and non-passes

The private-desktop matrix is not the owner's exact interactive Windows
Terminal sequence; the new published package is ready for that side-test.
The WOW frontier script exited zero for WINMINE/SOL/WRITE headless launches,
but its brief samples initially showed only a live NTVDM. A longer WINMINE
hold subsequently observed a visible 16-bit window and `WOWExecClass`;
SOL/WRITE did not yield equivalent window-depth proof in this S. Their
previously waived gameplay acceptance is not claimed here. The supplemental
`basesrv-service-reservation-test --frontend-delegated` assertion at line
995 still fails on the earlier S38 candidate and was not repaired or counted
as passing. An early matrix invocation used the wrong observer executable
and produced no valid report; it was rerun with the correct observer. No
historical guest media or system registry/Console settings were modified.

S38 closes only after reviewed governance, diff check, commit and push.
T423 remains open for owner acceptance; the transferred task-trace candidate
is not part of this S.
