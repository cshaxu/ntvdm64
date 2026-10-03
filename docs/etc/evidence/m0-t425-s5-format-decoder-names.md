# T425 S5 shared format-decoder naming

## Question, inputs and disposition

Owner asks whether NTCON's TEXT_FRAME, TEXT_CONFIGURATION and DIB dispatch
is format handling rather than worker-type divergence, and authorizes a new
S to repair misleading names if confirmed. Baseline: S4 d1588a08f and the
published S3 ec0cf2b02 eight-file protocol/RPC36, I/O24 package.

| Source and current role | Finding and selected disposition |
| --- | --- |
| src/ntcon-exe/console_video.c, run16_console_video_begin | Project-owned copied-payload validation selects message kind. Keep text pairs/triples, configuration-only metadata and DIB depth/stride checks unchanged. |
| src/ntcon-exe/window_frame.c, dos_text_frame | Project-owned shared text decoder: rename decode_text_frame; preserve cells, banks, palette, optional styles and cursor. |
| src/ntcon-exe/window_frame.c/h, frontend_window_dos_frame | Project-owned shared TEXT_FRAME/DIB decoder: rename frontend_window_decode_frame, including production caller and linked tests. Configuration-only payload is not an independently rendered frame. |
| src/ntcon-exe/native_console_frontend.c | The one production caller is shared by both worker channels; update identifier only. No new path or wrapper. |
| window_frame.h, FRONTEND_NATIVE_CELL_WIDTH/HEIGHT | Repository search finds only the declaration, no consumer. Remove these unused historical constants, not the text capacity or worker fonts. |

Original mirror logic, worker interpretation, imported libraries, common
protocol and worker-base are unchanged. No wire change or version bump.
Historical S4 evidence is retained; it correctly classifies this dispatch
as format handling, but the owner-requested naming cleanup follows in S5.

## Procedure and current results

Declared run root: build/M0-T425/S5/r001. Reuse the dependency-driven MSVC
Win32/x86 /MT CCPU40 cache build/M0-T424/S2/r001.

- [x] Admission mechanically passes documentation governance before source edits.
- [x] Incremental Ninja product-programs, frontend-window-library-test.exe and
  console-frame-failure-test.exe compile/link successfully (eight build steps).
  Existing compiler warnings are retained; no unrelated warning cleanup.
- [x] Compare S4 d1588a08f and current source after substituting only the two renamed
  identifiers: window_frame.c, native_console_frontend.c and the decoder test
  are exactly equal after line-ending normalization. Header changes additionally
  remove unused constants and document configuration-only semantics.
- [x] Repository search finds no old decoder identifiers or unused constants
  under src/tests. No mirror/library/common/worker-base source diff.
- [x] frontend-window-library-test.exe via observer.exe on an unswitched private
  desktop: result=exited, exit=0. Exercises the selected production decoder,
  text/font/palette/cursor/styles and DIB validation, not a renamed test stub.
- [x] console-frame-failure-test.exe --private-desktop-full: exit=0, final
  fixture PASS; rollback/publication/channel/FIFO/cancellation/peer death and
  stable pending handle count 437. Mocked broker attachment is not guest proof.
- [x] tests/observation/verify-worker-neutral-input.ps1: all seven cases pass.
- [x] tests/component-integration/verify-frontend-link-ownership.ps1 passes,
  including deliberate dependency leakage negatives.
- [x] Fresh Console17/Window17, retained lifecycle/EDIT/isolation, RPC11/GUI5,
  version negatives/WOW frontiers and strict DIR finish against selected set.
- [x] Coherent recoverable eight-file publication and postpublication smoke.
- [x] Diff/governance review complete; the containing S5 P commits this verified
  delivery. Other-session Queue/proposal changes remain uncommitted and preserved,
  not part of this P or a claim that the entire shared worktree is clean.

Preparation attempt focused.txt failed before tests because it assumed WOW32.DLL
was at the cache root. The corrected script reads the prior manifest's actual
wow32/wow32.dll path; focused-r002.txt retains passing observations. No product
change was made for this test-script error. The first incomplete preparation
was not published or presented as passing.

Raw evidence and scripts remain under the declared run root. The candidate
manifest and 265 current source/generated input hashes are frozen there.
O:/winnt now matches all eight S5 candidate hashes after publication and smoke.
S3 recovery is retained in accepted-s3-recovery with its complete hash manifest.
The containing reviewed P delivers S5; no subsequent S is automatically admitted.
T425 remains open; physical RDP acceptance and new WOW functionality are not
claimed by these automated fixtures.

## Final release verdict

All five final phases terminate zero: matrices, retained, RPC, versions-wow
and strict-dir. Console/Window summaries each contain 17 matching expected/
actual results. Strict DIR records entered-dos=1, dirty-prompt=0, final=0.
WINMINE's observed visible-window frontier and SOL/WRITE's existing respective
error-dialog frontiers match both S3 and historical S2; these are not new WOW
functionality passes. Postpublication rapid native/DOS pairs (12), interactive
CMD relaunches (12), cooked outer CMD exit 19 and GUI startup/wait (0/37) pass;
all eight published hashes remain unchanged. Temporary Z: is removed.

The only changed product is ntcon.exe. Current source/generated inputs (265),
47 imported-library inputs and 57 copied non-product runtime files are checked;
no original mirror, guest/configuration, common/worker-base or protocol delta.
Build/test scripts and raw results reside under build/M0-T425/S5/r001:
focused-r002.txt, frame-failure.txt, decoder.observer, final-phases-r001.txt,
wow-frontier-review-r001.txt, identity-r001.txt, publication-r001.txt and
postpublication-r001.txt. Candidate, published and recovery manifests are
separate and retained.

| Published file | SHA256 |
| --- | --- |
| run16.exe | 9BF75F5B634550503417242AB2A515292E03E60FC5C3AF860F0C755DEDB8647B |
| ntsrv.exe | 8ACA4D82B3D9CAE1E04BE7CF937ADE263DB0B2BF5B293C0D7BA16EE88DC1DB7E |
| ntvdm.exe | F29B70759B94843C78AF83C4F277BAD72FFF13FF77C40BEFFDF1CEC575E8E0B7 |
| ntvwm.exe | 9AD76C4D824B774D8C0F0B026BF00788586CF8FB06123AF4270602A748B646A1 |
| ntcon.exe | CCA29825256F4CC83D67DEFEF3C57BF9A2D3253E029928444F6AAC54C7C58C5C |
| ntmon.exe | 9B861F15171C13785A1E9274C354062B073AA5B24DD7570BCDF20318B1B0E6B5 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.DLL | 19228039788CFD099553A090B30E463B564101468160EFC31948C8A1651C307A |

## Reproduction entrypoints

Initialize the recorded Visual Studio 2022 BuildTools x86 environment, then
run Ninja against build/M0-T424/S2/r001 with targets product-programs,
frontend-window-library-test.exe and console-frame-failure-test.exe. The
existing dependency files select the changed caller/decoder/test closure.

The linked decoder test is tests/component-integration/frontend_window_library_test.c;
it invokes frontend_window_decode_frame, including no-data/invalid lifecycle,
monochrome top-down/padding, complete text, banks/styles and dual cursor cases.
Its observer command uses --observation-timeout-ms 45000 and the private-desktop
environment. The error/lifetime fixture entrypoint is
tests/app/console_frame_failure_test.c --private-desktop-full <report>.

Retained product entrypoints are tools/audit/Verify-CommandExitStatus.ps1
-OrdinaryFrontend (once per Console and Window), verify-command-native-edit-return,
verify-frontend-relaunch, verify-ntvwm-management -TwoSessions,
verify-broker-retirement, verify-s7-rpc-fixtures and verify-native-gui-routing
under tests/observation, tools/audit/Verify-ProductVersions.mjs and
tests/observation/observe-wow-frontiers.ps1 -WaitTarget. Run scripts and exact
arguments/report locations are retained in run-final.ps1/run-regressions.ps1
under the declared S5 root. Only Z: is used temporarily and removed in finally.
