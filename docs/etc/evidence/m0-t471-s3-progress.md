# M0 T471 S3 — repeated/concurrent/abnormal acceptance progress

## Question

Can the S1-frozen `build/t467-s3/package-r2` complete the selected repeated,
concurrent, abnormal-cleanup, OEM and subsequent-DOS acceptance routes without
adding a provider, changing guest media, or weakening the deferred S2 WRITE
interaction boundary?

## Inputs and procedure

All runs use the S1 manifest package (including unchanged `run16.exe`
`91811CEF9FC34564827AE530D700F46CF06BA7763035FC3DF206D1674AB2A30D`,
`ntvdm.exe` `7F2312AF2CC8480264B4935E2F5C9910D1A99D38C78C8F8DB57156FF1168611B`
and `WOW32.DLL` `506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23`).
Each runtime run used the repository's marker-checked short physical stage;
no guest image was edited or rebuilt.

| Route | Evidence | Result |
| --- | --- | --- |
| Same WOW worker, two live tasks, independent retirement | `build/t471-s2/s3-idle-shared-r3/result.txt` | Pass: two distinct task identities overlapped, both retired normally. |
| Two distinct WOW workers, idle/retirement independence | `build/t471-s2/s3-idle-separate-r1/result.txt` | Pass: each worker reached its own idle callback; retirement of one did not stop its peer. |
| Selected OEM capacity/cleanup contract | `build/t471-s2/s3-oem-r1` | Pass: 15 assertions, including DBCS capacity, overlap/null rejection and session cleanup. |
| COMMAND → native `EDIT.EXE` → CMD → DOS `MEM` return | `build/t471-s2/s3-command-native-edit-r2.txt` and numbered Console snapshots | Pass: actual EDIT menu frontier, Ctrl+Q return, CMD echo, DOS MEM output and final launcher completion. |
| DOS `COMMAND` → WINMINE, identified WOW worker termination → DOS `MEM`/`EXIT` | `build/t471-s2/s3-dos-wow-r12/t471-s3-dos-wow-isolation-r12.txt` | Pass: the exact owned WINMINE WOW worker was terminated; the parent DOS worker returned, printed MEM output, and exited with the expected COMMAND result without a GUI-owned text frontend. |

## Abnormal DOS → WOW route: current result

`verify-dos-wow-isolation.ps1` was repaired before treating its observation as
runtime evidence:

1. It now resolves the standard structured `SYSTEM32` package layout and
   stages its exact package rather than assuming a flat release directory.
2. It supplies the staged `SYSTEM32` lookup path to the child, preventing a
   bare `COMMAND.COM` launch from falling through to the caller's native
   `COMSPEC` environment.
3. It uses an explicit first-line event gate, so `MEM` and `EXIT` cannot be
   typeahead submitted before the identified WOW worker has been deliberately
   terminated.
4. It selects the immutable package's real WINMINE frontier: the original
   `WFWNET.DRV` profile reaches the package's localized-or-English main
   window; the old `NETWORK.DRV` dialog applies only to the no-network
   profile.

The repaired script reaches a real DOS `COMMAND.COM` prompt, accepts the exact
`winmine` command, identifies the current package's real English
`Minesweeper` front window, and does not send `MEM`/`EXIT` until the selected
WOW worker has been terminated.  The final `r12` run passed its complete
abnormal route: it observed only the expected DOS carrier and one character
frontend before termination; after termination, `MEM` output and normal
COMMAND exit were observed.  Earlier `r9`--`r11` runs are retained only as
test-repair diagnostics, not as product failures.

## Scope boundary

The owner-deferred visible physical WRITE edit/save/dialog/resize check remains
an S2 non-pass exactly as recorded in
[S2 integration progress](m0-t471-s2-application-integration-progress.md).
No substitute input mechanism was used.

## Follow-up

Continue S3 with the remaining aggregate evidence and review. Preserve the
successful idle, OEM, native-return and abnormal-cleanup evidence; do not
regress them or replace immutable guest input with a test-only provider.
