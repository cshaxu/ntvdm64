# Existing WOW window drawing residue

## Status and owner request

Unadmitted candidate recorded on 2026-10-12. The owner subsequently prioritizes
basic functionality, basic display, comprehensive interaction, comprehensive
display and comprehensive functionality. This belongs to comprehensive display,
before the everyday usability checkpoint and residual hard-error/service work.
Queue owns its current position. No numeric T is allocated and
the active window-style experiment is not displaced.
Owner request: "算了，你把这个问题作为队列末尾T任务做调查和修复吧".
This supersedes immediate diagnostic execution. No diagnostic build, runtime
experiment, source repair or deployment was performed in this side conversation.

## Observed problem and scope

The owner reports that existing NTVDM WOW Win16 applications, including
WINMINE, show residual pixels at the top of their content below the menu:
shapes resembling the bottom of a left system-menu button, two right caption
buttons and a caption's bottom edge. The screenshot shows a modern host frame
around Minesweeper and predominantly black/white application content.
The reference attachment is
`codex-clipboard-fd3630d2-47c9-491d-8749-5294fd4d3dcb.png` in this conversation;
its temporary clipboard location is not a durable acceptance dependency.
Capture a new retained reproducer at admission if that attachment is unavailable.

This concerns the existing WOW route, not Windows 3.1 running inside DOS VGA
and not the independent win16-style demo. The visual resemblance to the
demo's native-caption leakage is a hypothesis, not proof of shared cause.
Do not assume the reported shapes were drawn by a Win3.1 caption implementation.

### Minimum-diff admission gate

This candidate is **Yellow** under the shared [WOW32 minimum-diff admission
governance](../etc/operations/wow32-queue-minimal-diff-admission-governance.md),
which links its work to the external OpenNT-only seam ledger. Before production
implementation, record the original owner, build condition, ABI and
normal/failure/retirement ownership for every changed edge. trace the actual painter and client/non-client boundary before any visual repair; masking is not a fix. Static
ledger closure, a symbol match or a successful link does not prove this
candidate's product/runtime behavior.

## Read-only findings and limitations

The side investigation searched the existing message/DC bindings, read the
following functions and consulted retained WINMINE evidence. It did not collect
a new runtime paint trace or reproduce the defect independently.

- [USER16](../../src/mvdm/wow16/user/user.asm) routes DefWindowProc through
  its message-bit filter and WOW thunk. [WU32DefWindowProc](../../src/mvdm/wow32/wumsg.c)
  converts arguments and calls native DefWindowProc. This is not evidence of
  a second independent Win3.1 default-caption painter.
- The project [default-message table](../../src/wow32-dll/source/wow_private_user_compat.c)
  includes WM_NCCALCSIZE, WM_NCPAINT and WM_NCACTIVATE. Their mere inclusion
  does not prove correct runtime handling.
- [Host-to-guest conversion](../../src/mvdm/wow32/wmdisp32.c) and
  [guest-to-host conversion](../../src/mvdm/wow32/wmsg16.c) round-trip
  non-client geometry. WM_NCPAINT preserves the special region value 1;
  the HDC-named conversion uses the shared GDI alias domain and is not by
  itself proof of a region/DC type error.
- [GetWindowDC](../../src/mvdm/wow32/wuwind.c) and
  [BeginPaint/GetDC](../../src/mvdm/wow32/wuser.c) retain separate native
  API calls. Their runtime origin, clipping, state and resource lifetime
  remain unverified for the reported residual pixels.
- [Window-view projection](../../src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c)
  currently takes screen-relative window/client rectangles from GetWindowInfo.
  [Earlier evidence E54](../etc/evidence/m0-t422-s2-user-client-lifecycle-ledger.md)
  records a repaired zero-origin projection defect; that history is a lead,
  not proof that the same defect remains or caused this screenshot.
- Existing window/callback witnesses record message entry/results, not the
  exact pixel writer, DC origin and clip needed for causal attribution.
  Prior visible-window/lifecycle passes do not certify this visual boundary.

Root cause remains unknown. The screenshot's monochrome content is a separate
known investigation and must not be silently declared either the same cause
or unrelated without evidence.

## Relationship to queued work

Reuse results from [USER objects/shared view](proposal-wow32-user-objects-shared-view-001.md),
[GDI/DC/drawing](proposal-wow32-gdi-dc-drawing-001.md) and
[resource conversion](proposal-wow32-menus-resource-conversion-001.md).
Those packages retain their capability ownership, including WINMINE's
monochrome/DC investigation. This candidate owns reproduction and closure of
the specific residual-pixel report, not a second implementation of their work.
At admission, recheck whether preceding repairs already resolve it; if so,
retain a focused regression and causal evidence rather than manufacture a diff.

## Proposed stages

| Stage | Work | Required result |
| --- | --- | --- |
| S1 | Reproduce and attribute the painter | Pin product/guest hashes, host build, DPI/theme and actual window/client geometry. Correlate pixel changes with client painting and non-client activation/painting, including native/guest arguments, DC origin, mapping and clip. Identify the responsible boundary; screenshots alone do not establish it. |
| S2 | Minimal source-shaped repair | Repair only the demonstrated cause at its existing owner. Compare with original OpenNT, classify project versus mirror defect, and register any necessary minimal mirror diff. Preserve normal native frame ownership. |
| S3 | Regression and delivery | Prove the original negative control fails and the corrected package passes pixel/content and lifecycle checks; perform applicable product gates, coherent publication, commit/push and owner visual acceptance. |

## Investigation and acceptance contract

Use a bounded owned probe and background build/test processes with artifacts
under build/. Do not disturb unrelated sessions or depend on computer use.
Observe before/after client paint, non-client paint/activation, hover, menu
open/close, expose, minimize/restore and applicable move/resize transitions.
Record actual client-boundary coordinates, not a guessed caption height.
Include an independent expected-content oracle: equality between two already
contaminated captures is not a pass. Preserve screenshots of the full window
and the affected strip, with the message/drawing evidence that attributes it.

The repair must preserve the minefield, digits, face, input and window close,
not simply erase a strip of legitimate application content. Check another
reachable WOW window/dialog and retain independent SOL/WRITE frontiers without
calling their known failure states functional passes. Distinguish real guest,
native fixture, physical observation and untested host/DPI/theme boundaries.

No caption-height padding, overpainting, unconditional suppression of native
default painting, global theme changes, guest-binary changes, additional
runtime component or application-name workaround may substitute for root-cause
repair. The standalone style experiment's fix is not automatically applicable.
If runtime instrumentation or additional original-source recovery is needed,
admit its bounded files and source/provenance contract before implementation.
