# T425 S7 Idle Cursor And Window-only Startup

## Question and baseline

Owner requests natural idle native Console cursor blinking and local skipping
of character-I/O startup for window-only NTVDM/NTVWM work. Reference is S6
66841b8b7 and its coherent eight-file publication. T425 stays open.

## Source audit and implementation

| Location/source | Finding | Disposition |
| --- | --- | --- |
| NTVWM presentation.c, project adaptation | Thirty-millisecond native sampling suppressed equal cell tiles, but still published a full frame on every capture. | Keep sampling; compare the acknowledged Unicode grid, viewport, cursor position/shape/visibility, attributes and composed frame/font/palette/mouse state under the instance lock. Unchanged capture sends no publication. |
| NTCON native Console projection, project adaptation | Full frame projection writes the visible grid; cursor setters already avoid identical updates. | No frontend worker-kind branch or renderer change. Removing repeated idle frames prevents repeated host repaint, rather than adding blink timers. |
| NTVDM standalone bootstrap, project adaptation | Attempted character startup before original WOW initialization and accepted NOT_SUPPORTED. | Recognize the mandatory original -w switch locally and skip the character callback. Original classifier and VDMForWOW initialization remain in place. |
| worker-base/connection.h, existing worker-only boundary | Both workers need the same finite startup gate, not a shared classifier. | Inline gate invokes a required text callback, propagates its result, and skips it for window-only work. No scheduler/resources or new component. |
| NTVWM execution.c, project adaptation | GUI capability absence already distinguishes its admitted route. | Use the shared gate; native process creation, completion and resource ownership stay local. |
| NTSRV frontend_registry.c, project service | Normal WOW startup relied on an unsupported-operation result. | Remove that convention. Erroneous authenticated WOW character acquisition is denied; normal WOW does not request it. |

Original BaseGetVdmConfigInfo in opennt-host/base/win32/client/vdm.c supplies
the mandatory -w switch for WOW. Original mvdm/softpc.new/host/src/nt_reset.c
recognizes - or / and case-insensitive w. No original mirror, guest or imported
library is changed; no helper or wire revision is introduced (RPC37/I/O25).
The publication cache owns its copied cells/frame, updates only after ACK,
is replaced/freed locally, and starts empty for a new endpoint.

## Focused verification

Validated incremental cache: build/M0-T424/S2/r001, MSVC 14.43.34808,
SDK 22621, Win32/x86 /MT CCPU40. Run records: build/M0-T425/S7/r001.

- ntvwm-presentation-test: before repair 444 checks, three failing idle
  assertions; after repair 444/0. Extended actual Console cursor-only and
  changed-cell cases pass 459/0, including unchanged repetitions sending no
  RPC/publication. Fonts, frame content and cursor assertions remain.
- ntvwm-execution-lifetime-test: 1077/0, remaining handles zero. Shared gate
  tests cover GUI skipping, missing text callback and actual callback error.
  Two older text fixtures lacked their I/O initializer; they now bind the
  existing initializer. Actual exit code, target survival, failure and cleanup
  assertions are retained, not weakened. Initial 1067/8 failure is retained.
- basesrv-service-reservation-test --wow-start-late-query: PASS original
  Check/Update/Get/ExitVDM and startup completion/reuse.
- basesrv-service-reservation-test --io-authority: PASS no eviction,
  both-end close acknowledgement and explicit resumed-parent grant.
- verify-frontend-link-ownership.ps1: PASS positive and leak negatives.

The initial external shared-gate definition exposed a fixture link-boundary
issue; the finite gate is now inline without pulling broker connection state
into execution-only fixtures. Wrong-case DLL build-target invocations failed
before building; the exact VDMREDIR.dll and separate wow32.dll targets passed.

## Package verification and limits

Candidate build/M0-T425/S7/r002/runtime overlays all eight current build
outputs on the retained S6 guest/configuration package. Its manifest checks
all eight hashes. A first driver invocation under Windows PowerShell 5 failed
because ProcessStartInfo.ArgumentList is unavailable, before any guest test;
the same hash-verified stage is tested under PowerShell 7 instead.

Private, unswitched desktop observations retain the separate WINMINE visible
game-window, SOL memory-dialog and WRITE memory-dialog frontiers against S6
and historical retained references. Bounded observation timeout is not a
claim of gameplay or repair of the retained SOL/WRITE limitations.

Console17 and Window17 pass. RPC11, GUI5, five real version-mismatch negatives
and actual modern EDIT return (Ctrl+Q, CMD echo, DOS MEM and launcher completion)
pass. Retained lifecycle gates also pass, with the failed combined-driver
handoff separately attributed below. The coherent eight-file candidate is now
published at O:/winnt; postpublication smoke passes and the containing reviewed
P delivers S7. T425 remains open for owner acceptance.
The inherited 284-input source seal is supplemented
by s7-changed-inputs.json for all five changed production and three changed
fixture files, including the standalone bootstrap absent from the old seal.

No actual desktop/RDP blink phase is observed. The tested causal contract is
absence of unchanged native publication/host repaint, leaving blink to the
host. Physical interaction remains owner-waived and awaits side testing.

## Failed combined-driver handoff and source attribution

The first r002 handoff-final nested-window run times out after actual CAF
success and CMD's run16 command input. The timeout stack resolves against the
candidate map to original illegal_op_int -> host_error -> ErrorDialogBox in
the DOS CPU thread, not a proven pipe wait. Native operation 39 / error 1168
is the optional absent text-configuration read; it also appears in the
successful S6 control and is explicitly handled, so it is not the root cause.
One fresh candidate handoff-repeat and the immediate S6 handoff-s6-control
both pass actual DOS/MEM/parent-return and direct exit 23. Those successes do
not erase the earlier failure. Publication was stopped for attribution.

RPC/service16, independent Console isolation, four worker/frontend-loss cases,
cooked outer CMD return and repeated DIR pass. A fresh strict-dir-recorded
run retains machine-readable stdout as well as raw/cell evidence because the
already-loaded first driver did not capture its successful DIR stdout.

Two ordinary S6 controls and two ordinary S7 controls pass the same strict
nested Window assertions. Repeating with the seven extra variables left by
the combined version/DIR driver reproduces the failure on the unchanged S6
package too. This is not declared a harness-only bug or fixed by a retry.
The existing observer was rebuilt unchanged under r003 for a timeout-only,
read-only guest snapshot, using exact candidate-map Start_of_M_area RVA
2d86c4. No product/mirror/guest write or debugger hook was added.

verify-guest-environment.ps1 records: PSP 03F4, environment 049F, MCB owner
03F4, 0167 paragraphs and retained COMMAND allocation 00A2 paragraphs.
Discarded INIT EnvSiz at linear 05F7C lies within valid environment range
[049F0,06060), but contains 5500 instead of 0167. COMMAND.COM hash is
908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43.
These match the original stale-INIT-variable defect and immutable guest proved
in [S12 r125-r126](m0-t423-s12-ntw32-backend.md#startup-failure-attribution-r125-r126)
and [S35](m0-t420-s35-xms-capability-progress.md#direct-environment-copy-witness).
The source-policy standing disposition for proven original-guest limitations
applies. Failed enlarged-environment runs remain failures; no guest patch,
environment truncation, /E: injection or product workaround is introduced.
Ordinary acceptance uses separate invocations with the unchanged inherited
host environment, not accumulated test-only version/DIR variables.

## Publication

publish.ps1 verifies all retained gate reports, both strict ordinary candidate
handoffs, source/fixture seals and eight current product hashes. It preserves
the exact S6 published set in r002/accepted-s6-recovery, then replaces all eight
O:/winnt files and verifies every hash. Guest/configuration is untouched.

| Published file | SHA-256 |
| --- | --- |
| run16.exe | 9C19B7C6C5CD82748B66D9F450E6D88341F5E35572093E0DD8BCBA0E51864E6A |
| ntsrv.exe | 8BE09E891D0C326F4A58234AD0426550D516826366AD984D6DD7216ADC4465C1 |
| ntvdm.exe | E89C31A8A42E8199E2E61A74A48A26816BD1558BDBEEBFB6B400E62F685F0AE0 |
| ntvwm.exe | 349C5B169C83896E5DA01ED54D955DA0498CA315E60FDB7FCD799F52C0BC7A57 |
| ntcon.exe | 0E28F68A0024EB0CD49CC668CE788D94B8B598311E4AA76A707002CA9BC5E88C |
| ntmon.exe | 9FDC95C114DBE2473D46415043AD5193CCB66DFA4B97D56A09B6351D49757EF2 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.DLL | 110D00408CCEC6EE3F3F4B0FC7E8D58534DD3744BBC31FC27A248C7739A93E67 |

Publication retains APP0.0.425, RPC37/I/O25, x86 /MT CCPU40. Detailed recovery,
source and product manifests remain below build/M0-T425/S7/r002. Other-session
Queue/proposal changes remain outside this delivery.

postpublication-smoke.ps1 passes 12 rapid native/DOS pairs, 12 interactive
native CMD relaunches, cooked outer-CMD return/exit 19 and GUI startup/wait
0/37. Final eight published hashes remain identical. Source/fixture seals,
mirror/library no-diff review, governance, relative links and actual diff
review pass. Changed-state publication, failure/cancellation and zero-handle
lifecycle assertions remain intact. Physical blink observation is owner-waived,
not a visual pass; no polling-removal or original guest repair is claimed.
No next S or owner T acceptance is implied.
