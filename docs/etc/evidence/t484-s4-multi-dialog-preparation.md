# T484 S4 P32: multi-dialog preparation

## Scope

The retained multi-dialog probe exercises six original application paths:
Notepad, Program Manager, WINMINE, Calculator dialogs, Calculator arithmetic,
and Write. This batch completes their preparation, not a product repair.
Existing native CMD compilation, process-bound messages, menu/dialog recovery,
arithmetic and launcher-exit assertions remain unchanged.

prepare-test-inputs.cmd owns the host-probe/application evidence copies;
physical-stage TestFiles owns the tests/GUEST directory. Original PROGMAN
stays away from the Windows-root native-shell redirect. A bounded CMD helper
copies the four source groups and generates the same isolated PROGMAN.INI.
It requires an owned immediate O:/tmp directory and preflights every group
before writing. The source installation and original guest binaries are not
modified. No additional native helper or interpreter was introduced.

## Evidence and corrections

Root: build/t484-s4-multi-dialog.

- PID21276: cached full host build and11 raw/22 dual-publication hashes pass;
  first launch fails because the caller used GUEST rather than tests/GUEST.
  The observer records the missing path. Corrected the caller, not the shared
  stage layout.
- PID17812,2026-11-24T20:39:16Z-20:41:57Z,exit0: all six application paths
  pass actual menu/dialog recovery and launcher completion using native Userenv.
- contracts.ps1 checks exact four group hashes, exact CRLF INI content and
  rejection of incomplete source/installed destination. The first negative
  run exposed exit0 from EXIT /B inside the FOR body, although no target write
  occurred. The helper now records failure inside the loop and returns outside
  it. negative-result.json proves exit2 with identical before/after INI hashes;
  contracts-result.txt records the complete pass.
- verified-process.json and verified-result.json identify the final PROGMAN
  repeat after that CMD failure-propagation correction. Its retained probe
  requires Program Group Properties, parent recovery and normal launcher exit.

The host product has no changes after its cached build; final test preparation
is exercised directly. Both publication locations retain their verified images,
so no fresh product copy is claimed. No Full runtime pass is attributed here.

## Accounting

Final PROGMAN repeat PID26064 passed at 2026-11-24T20:43:04Z,exit0. This caller is migrated-delivered:
348/355 disposed,7 pending. Five overlapping supplemental preparation chains
remain. S4 remains active and S5 final clean acceptance is still required.
