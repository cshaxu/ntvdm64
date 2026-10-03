# T425 S4 whole-objective audit

## Inputs and scope

Audit baseline: S3 ec0cf2b02, S4 admission 50d52b5bb, APP0.0.425,
protocol/RPC36 and I/O24. The approved four-stage plan, S1/S2/S3 ledgers,
current source and selected S3 r004 package are the inputs. New results belong
under build/M0-T425/S4/r001. No original mirror, guest or imported library
change is authorized. T closure remains owner-controlled.

This is the completed implementation audit, not owner T acceptance. Searches locate
decisions; actual callers, state, locks and test assertions determine whether
they satisfy the objective. A shared symbol or green fixture alone is not
proof that both production callers use it.

## Current source/caller disposition

| Requirement | Actual production path and decision | Evidence / remaining review |
| --- | --- | --- |
| Worker-neutral Window mouse | native_console_frontend window_input -> window_mouse dispatch -> window_records -> one input_write queue. Relative pixels, source geometry, buttons and ENTER/LEAVE are encoded identically. No worker type enters the converter. | S4 seven-fixture runner mouse passes; S1 decoder/device negative assertions retained. Physical RDP capture is not proved by this fixture. |
| Worker-neutral Console mouse | collect_console maps absolute canonical canvas cells into logical_window and rejects outside content. This predicate is geometry-based for any active channel. | Actual source reviewed; retained Console/Window matrices are selected S3 r004 evidence, not a fresh S4 product run. |
| Keyboard and returned input | window_input always calls frontend_keyboard_dispatch. Layout/dead-key/physical held-key translation has one sink. Common prepend encodes only KEY_EVENT batches; both worker clients use it with their own API result contracts. | S4 keyboard and input-return pass; returned-character/scan semantics remain original-worker local. |
| One owner/multiple pending | channel activation -> common bind/wait_ready. apply_binding serializes one owner and deduplicated FIFO pending nodes; predicate/event reset is under io_lock, wait outside it. No worker classification or task receipt is stored here. | S4 full fixture terminates zero: all operation-pair handoffs, multiple pending, cancellation, timeout, peer death, ordered input and exact handle count439. |
| Symmetric activation/geometry | Both clients call ntcon_worker_activate(client,BOOL). Only NTVDM then requests explicit VGA dimensions, selected in its project-owned console_geometry.h. NTCON accepts exact geometry operations, not a DOS flag. | S3 operation authorization and real NTVDM client geometry fixture. Original VGA execution/resume algorithm stays in its owner. |
| Snapshots/publication | Same console_channel callbacks and io_lock for all endpoints. Snapshot lock permits only bounded read operations; activation/input waits are excluded. Standalone and batch share publish_video; prepare before coherent frame/grid/font commit. | S4 actual allocation/revision/projection/geometry failure fixture terminates zero; staged EOF abort and committed state coherence retained. |
| Rendering/display/capture | present_loop renders by CONSOLE_VIDEO_TEXT_FRAME or DIB. window_controller chooses display policy by frame graphics flag and user display selection, never worker kind. Historical dos_text_frame/frontend_window_dos_frame names describe a common format provider, not DOS-only routing. | Source inspected, both text formats use frontend_text_frame_prepare. Capture library safety predicates precede worker delivery and remain unchanged. |
| Real shared clients | NTVDM console_client and NTVWM presentation both call common init, decode_input, activate, video and publish_title. common owns framing, sequence/generation checks, bounded copies and sticky transport failure; worker-base owns broker connection/shutdown mechanisms. | Actual call-site search plus source read; S4 link-ownership verification passes, including deliberate leakage rejection. |
| Session/lifecycle boundary | session_service requests generic frontend channels from NTSRV; one list and channel provider. Console anchors/join snapshots describe frontend identity, not worker kind or task observation. RetireWorkerlessFrontend is checked before channel work. | Source read. Retained broker-ordered worker/frontend loss and independent session gates are S3 r004 evidence; no new lifecycle policy is introduced. |

## Required independent worker behavior

NTVDM receives the common validated mouse sample, then adapts it to its original
relative ingress at win32/console_client.c. Guest coordinates, IRQs, INT33,
VGA selection and software mouse drawing remain device-owned. NTVWM interprets
that same sample against its hidden Console viewport and emits real Windows
mouse records and a display-copy cursor. These are not duplicated frontend
conversion policies; moving either device algorithm to NTCON would violate the
approved boundary. Native target execution and original DOS/WOW execution,
blocking/resume/completion likewise remain their respective worker semantics.

## Fresh observations

Command, with absolute paths and unswitched private desktop:

```powershell
tests/observation/verify-worker-neutral-input.ps1 `
  -BuildRoot O:/repos.hobby/ntvdm64/build/M0-T424/S2/r001 `
  -Observer O:/repos.hobby/ntvdm64/build/M0-T424/S2/r001/observer.exe `
  -LogRoot O:/repos.hobby/ntvdm64/build/M0-T425/S4/r001/focused
```

focused-r001.txt terminates zero: mouse, keyboard, controller, codec,
native-mouse, presentation and input-return all pass. Reports identify actual
scope: native text receiver 133/0, presentation 426/0 and real pipe input-return
688/0; presentation fixtures explicitly do not claim production activation.
Guest/runtime production closure instead comes from the exact S3 r004 matrices
and retained lifecycle/version/WOW evidence, whose artifact/input identity must
remain verified before S4 conclusion.

The current console-frame-failure-test.exe --private-desktop-full invocation
uses frame-failure-report.txt under the S4 run root. frame-failure-r001.txt
records child exit0. The report exercises actual frontend/channel code,
including paired and styled cells, precommit allocator/revision failures,
postcommit projection/geometry failure, real pipe terminal EOF, snapshot
contention, staging abort and multi-pending handoff. Only broker attachment is
substituted; this cannot replace RPC or real guest proof. Stable final handle
count439 has no permitted leak allowance.

tests/component-integration/verify-frontend-link-ownership.ps1 with the current
absolute BuildRoot passes; link-ownership-r001.txt preserves the result and
negative leakage controls. A fresh hash check confirms all 265 frozen S3
source/generated inputs and all eight published product hashes still match.
No production source has changed during this audit.

## Complete owner and operation inventory

All project-owned NTCON C/header families were inspected: main/session_service
(authenticated root/service and channel lifetime); native_console_frontend
(one logical_surface, input, owner/pending, locks, projection, title and
cleanup); console_channel (one authenticated endpoint and callback set);
console_frontend (operation validation/dispatch); console_video (staging and
commit); window_controller/input_queue/keyboard/mouse (display/input policy);
window_frame/text_frame (format-to-library rendering). Imported lib files are
unchanged dependencies, not rewritten frontend providers.

The dispatcher operation inventory has no worker discriminator:

| Operation family | Common rule / owner |
| --- | --- |
| ACTIVATE, PREPARE_TEXT_REGION, BARRIER | Channel ownership, explicit dimensions and ordered acknowledgement. Original VGA selection stays NTVDM-local. |
| SNAPSHOT_BEGIN/END, READ_TEXT_CONFIGURATION | One locked copied grid/configuration; blocked activation/input excluded during snapshot. |
| PUBLICATION_BEGIN/END/ABORT, VIDEO_BEGIN/DATA/TEXT | Private staging, complete validation and common atomic commit; serial high-water retained after abort; external postcommit failure is terminal. |
| WRITE, SCREEN_INFO, CURSOR_POSITION/INFO/GET_CURSOR_INFO, FILL_CHARACTER/ATTRIBUTE, SCROLL, ATTRIBUTE, BUFFER_SIZE, WINDOW_RECT, READ/WRITE_CELLS_A/W | Mutate/read frontend logical grid under common ownership/lock. Explicit worker geometry metadata does not resize the terminal viewport. |
| READ/PEEK_INPUT, PREPEND_KEYS, GET/SET_MODE, CODE_PAGE, KEYBOARD_LAYOUT | One copied input queue and current Console API binding, bounded validated records; pending cannot consume old-owner input. |
| GET/SET/PUBLISH_TITLE_A, WINDOW_QUERY, GET/SET_DISPLAY_MODE, CURRENT_FONT, FONT_SIZE, GET/SET_POINTER, GET/SET_POINTER_CLIP | Operation-specific frontend/host APIs; Window clip ownership prevents a worker operation releasing frontend capture, regardless of worker type. |

Project additions at the unchanged MVDM ingress were not ignored: the original
nt_event mouse path remains connected through the existing NTVDM project
mouse bridge. Its VirtualX/VirtualY scaling, ICA lock, IRQ queue and original
guest cursor consumption are worker device semantics, not a second host input
normalizer. T425 changes no original mirror file or that device implementation.
There is no need to transplant original execution/scheduling into worker-base.

The replaced implementations actually disappeared: frontend native-only
absolute mouse conversion and keyboard dispatch selector (S1); DOS/native
owner slots, single pending slot and bind wrappers (S2); activation kind/VGA
selection in frontend, native-only snapshot/publication authorization and
duplicate final-chunk import orchestration (S3). One provider now serves both
clients. Remaining format validation/byte-glyph import and prepared Unicode
batch commit are explicit operations, not type branches; the latter avoids
lossy reimport of an already prepared Unicode grid.

## Assertion-to-requirement mapping

| Requirement / exact tracked entrypoint | Assertions inspected / authoritative result |
| --- | --- |
| tests/component-integration/frontend_window_mouse_test.c, dos_contract() | Signed extrema, source/geometry validation, atomic ENTER+MOVE, button/reset/LEAVE, failed sink preserves state; S4 focused mouse observer exit0. Despite historical test name there is no worker argument. |
| tests/component-integration/frontend_window_keyboard_test.c --cooked | returned_dos_tests, delivery_tests, input_reset_tests and native_tests verify character-bearing make/break, cooked ab CRLF, dead keys/layout, stale source and failed sink. S4 keyboard exit0. |
| tests/component-integration/frontend_window_controller_test.c | Real private Window X/CAF/AE, CAM capture release, source retirement, joined callbacks, independent instances, graphics/text display policy and cooked input assertions; S4 controller exit0. This is not physical RDP acceptance. |
| tests/app/console_channel_lifetime_test.c, included by console_frame_failure_test.c --private-desktop-full | test_prepare_operation, test_standalone_text, test_failed_text_commit, test_projection_failure_pipe(FALSE/TRUE), test_logical_publication and pending cases: inactive/stale rejection, reserved kind rejected, old frame/grid/font retained before commit, terminal EOF after commit, snapshot contention, input Q ordered across all four handoffs, two pending requests/dedup/head-tail cancellation and timeout/death isolation. S4 full fixture exit0, handle439 equality. |
| tests/observation/ntvwm_presentation_test.c --input-return and ordinary report | Real copied pipe/key return, reply failures and common decoder negatives; S4 688/0 and presentation426/0. Activation provider is substituted and explicitly not guest proof. |
| tests/component-integration/verify-frontend-link-ownership.ps1 | Graph recursively checks no frontend-private source in launcher/workers, no helper/native backend in frontend, retired archives/control pipes absent; deliberate leakage negatives rejected. S4 PASS. |
| tools/audit/Verify-CommandExitStatus.ps1 -OrdinaryFrontend | 17 routes each Console/Window, COMMAND/MEM/EDIT text and nested recovery, exit results, one root receiver, expected output after each command. Final selected S3 r004 summaries each17/zero mismatch; tests inspect output, not merely exit0. |
| tests/observation/verify-command-native-edit-return.ps1 and verify-frontend-relaunch.ps1 | Actual modern EDIT CtrlQ -> CMD -> DOS MEM; outer native cooked return and relaunch. Selected S3 retained phase PASS. |
| tests/observation/verify-ntvwm-management.ps1 -TwoSessions and verify-broker-retirement.ps1 | Independent session survives selected close; both workers' death and frontend loss return explicit direct failure and broker-ordered retirement. Selected S3 retained phase PASS. |
| verify-s7-rpc-fixtures.ps1, verify-native-gui-routing.ps1, Verify-ProductVersions.mjs | RPC11 and GUI5, explicit wait37/default0, old app/protocol/interface rejection1306. Selected S3 rpc/versions phases terminate0. |
| observe-wow-frontiers.ps1 and exact baseline signature review | Three separate observations retain WINMINE window and original SOL/WRITE error boundaries. No gameplay or SOL/WRITE functionality acceptance is invented. |

Initial attempts to locate mouse/keyboard/controller fixtures under tests/app
failed because their actual owner is tests/component-integration. The correct
tracked sources above were subsequently read; the failed lookup is not a test
failure or a reason to replace the fixtures.

## Exact retained release and disposition

r001/verify-retained-release.ps1 verifies 265 source/generated inputs, all
eight cache/stage/published files, both 17-row matrix reports, five completed
release phases and 47 imported-library source hashes. It also verifies no
T425 mirror/library diff from T424 ff50ff588. retained-release-r001.txt passes
all checks. No production edit or rebuild/republication occurs in S4: the
already verified S3 artifacts remain the exact owner-test set at O:/winnt.

| Product | Published SHA256 |
| --- | --- |
| run16.exe | 9BF75F5B634550503417242AB2A515292E03E60FC5C3AF860F0C755DEDB8647B |
| ntsrv.exe | 8ACA4D82B3D9CAE1E04BE7CF937ADE263DB0B2BF5B293C0D7BA16EE88DC1DB7E |
| ntvdm.exe | F29B70759B94843C78AF83C4F277BAD72FFF13FF77C40BEFFDF1CEC575E8E0B7 |
| ntvwm.exe | 9AD76C4D824B774D8C0F0B026BF00788586CF8FB06123AF4270602A748B646A1 |
| ntcon.exe | 98E10156BF6B1ED60304F24C4FD960D0B8D8008EC2F02D8019A3BCBFEB5EA76B |
| ntmon.exe | 9B861F15171C13785A1E9274C354062B073AA5B24DD7570BCDF20318B1B0E6B5 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.DLL | 19228039788CFD099553A090B30E463B564101468160EFC31948C8A1651C307A |

The implementation audit finds the approved worker-neutral objective satisfied
in production input/ownership/publication/rendering paths. Original/device
differences, font/extent repair, timer-polling cleanup, physical RDP behavior
and host scrollback are not silently converted into passes; they retain their
approved independent/excluded dispositions. No new helper, task scheduler,
observation graph, transport or component is added. S4 is documentation/audit
delivery only; owner T acceptance remains the final gate.

Documentation review initially replaced CURRENT's required Active prefix with
an audit-complete heading; the structural gate rejected that wording. The
required active S4 packet heading is restored, explicitly waiting for owner
verification with no further implementation. Governance and relative-link
checks then pass, as does diff whitespace review. The containing P records
the completed S4 audit; no T closure is authorized by it.
