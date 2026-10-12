# T445 Windows 3.1 window-style working plan

## Authority and stage boundary

S1 closed through its [installation design](../evidence/m0-t445-s1-installation-design.md).
The owner inserts accepted uncommitted palette repair delivery as S2.
The previously admitted standalone experiment moves to S3; integration moves
to S4--S7. Historical evidence and build directories retain their original
numbers. No integration starts before owner-confirmed experiment stability.

| Stage | Bounded result |
| --- | --- |
| S1 | HWND ownership, original Win3.1 references, task/launch policy owners and installation design; preserve explicit remaining implementation proofs. |
| S2 | Audit, commit/push and close the owner-accepted NTVWM palette geometry repair, tests and published assets. |
| S3 | Implement experimental src/win16-style and independently launch an authored Win32 GUI test program linked directly to it. Prove basic frame visuals/interaction without any other product component; obtain owner stability acceptance. |
| S4  | Both CLI options, explicit/inherited policy resolution, end-to-end propagation, per-Win16-task isolation and real-window title marker; build, verify, publish, commit/push for owner acceptance. No automatic activation of experimental retro painting in production. |
| S5  | Reuse, integrate and harden the already-tested common frame, font/palette/DPI and geometry through WOW/Hook paths; do not build a second renderer. |
| S6  | Standard menus, MDI and remaining supported interaction/state coverage; document custom-frame and host-shell limitations rather than inventing special cases. |
| S7  | Integrated visual/interaction and serial product regression, coherent release/publication and final owner acceptance. |

## S3 standalone experiment contract

- Library source remains src/win16-style. The authored Win32 API demo and tests
  live under tests/app; build/test orchestration under tests/observation. Produce
  executable, objects, logs and captured images only under build/M0-T445/S3.
- Launch the demo directly, not with run16 and not by injecting into an unrelated
  application. Direct linking is deliberate: it isolates the UI from transport,
  guest behavior and Hook installation. No other product library/runtime is a
  dependency. Build x86/x64 variants; this is one fixture source, not new workers.
- Show ordinary min/max, minimize-only, and dialog-style windows, plus a native
  comparison. Exercise activation, title changes, palette changes, pressed
  buttons, move/size, restore, close and repeated create/destroy. Preserve the
  actual client area. Explicitly label any incomplete menu/MDI behavior.
- Capture only the owned fixture's rendered output with test instrumentation;
  no computer-use automation. Automated message/layout tests and render captures
  do not establish physical pointer or RDP acceptance; retain those distinctions.
- Keep library API small and explicit. Reuse the experiment's actual layout,
  painter and window state later; do not make a disposable mock UI or a general
  skin framework. Select/verify original-looking metrics and fonts here, with
  later integrated refinement. Do not redistribute unreviewed historical assets.
- Deliver the experiment as a build-owned test executable for visual review,
  not a replacement in assets/release or O:/winnt/system32. No deployed ten-image
  behavior changes, no RPC/Hook/WOW integration, no global host theme changes.
- Stability means focused repeatable tests plus owner acceptance of the visual
  trial, not merely a successful compile. S4 waits at that gate.

## Policy contract

Use one policy with three values: off, Win16-only, all-window-programs.
CLI explicitness is separate from its effective value: omitted options inherit
the caller's controlled launch context; without one, default is off. An explicit
option overrides inherited policy; when both options occur, all wins. Do not
introduce an unrequested disable option or change existing run16 target parsing.

| Effective policy | Win16 window marker | Native x86/x64 window marker | Child propagation |
| --- | --- | --- | --- |
| Off | Absent | Absent | Off |
| Win16-only | Present | Absent | Preserve Win16-only, including through native intermediates |
| All | Present | Present | Preserve All |

The scope is the controlled launch chain, not every program on the machine.
Text/DOS intermediates preserve policy where they launch a supported descendant,
but neither their Console title nor NTCON's outer window becomes a style target.
Launching an entire Windows desktop as a DOS guest does not restyle its pixels.

Carry policy through existing launch/request/Hook context boundaries, with
appropriate version validation if records change. NTSRV retains task policy,
not a window registry. WOW retains policy per owning Win16 task, never as one
worker-global toggle or an unscoped current-task variable. A window uses its
actual owning task's policy even when shared-worker tasks interleave. Native
Hook contexts carry policy across both bitnesses and run16 redirection. Do not
alter global user environment, add IPC, polling, helper processes or filename
exceptions. Any unreached guest child-creation boundary must be audited before
claiming propagation; a direct-launch-only implementation cannot close S4.

## S4 visible marker

Reuse the owner-approved `src/win16-style` experimental library in S4: x86
for WOW32/nthook32, x64 for nthook64. Each consumer supplies its effective
window enable decision; the shared marker and later renderer do not determine
program type or own propagation. No temporary parallel marker implementation.

An enabled supported application window displays exactly one decoration:

`(Win16 Style) <application title>`

The same literal prefix is used for either option. Disabled windows remain
unchanged. Cover window/dialog initial titles, later application title changes,
empty titles, A/W paths and later GUI threads. Repeated notifications must not
accumulate prefixes. Keep the application's undecorated title separate from the
presentation prefix; do not infer policy by searching title text or remove an
application-authored matching string. The supported title-read semantics and
installation/detach ordering must be specified and tested before implementation
is accepted, avoiding reentrant WM_SETTEXT loops. The S1 decision uses a real
caption decoration: title-read APIs and exact-title lookup observe the changed
caption. Do not virtualize those APIs or claim unmarked-title equality; test
and report title-dependent application compatibility explicitly.

The marker uses the same per-window enable decision and installation boundary
that later rendering will consume, not a temporary process-wide caption scanner.
No production retro colours, font, borders, menus or layout are required in S4. A visible
marker proves enablement only, not delivery of the Win3.1 appearance.

## S4 acceptance matrix

- Default launches: existing Win16 and x86/x64 native windows remain unmarked.
- Each option: exact expected marker on real eligible windows, absent on excluded
  families; both options together; explicit override and omitted inheritance.
- Shared WOW worker: marked and unmarked tasks coexist in either startup order;
  create/update/destroy windows without leaking policy into another task or a
  subsequent launch. Worker reuse is part of the test, not bypassed by isolation.
- Launch chains: Win16 to Win16, Win16 to native to Win16, native to Win16,
  x86 to x64 and reverse, with both policy values. Verify the retained native
  intermediate policy even when its own window is unmarked. Include existing
  DOS/shell/run16 relay boundaries, returns and failed child launch.
- Real windows: normal windows and modal/modeless dialogs, secondary GUI threads,
  changing/empty/non-ASCII titles, application-authored prefix, destroy/recreate,
  and no doubled decoration or stale per-task/window state.
- Evidence combines actual HWND titles, owning task/process identities and
  child-launch provenance; a screenshot or a stored flag alone is insufficient.
  Preserve documented title-API behavior with the explicitly decorated caption
  and existing run16 CLI behavior, not identical unmarked title strings.
- External activation brokers, pre-existing target processes and unsupported
  custom captions are explicit boundaries, not silent passes. Do not broaden
  the existing controlled Hook propagation guarantee without owner review.
- Follow normal production delivery: background-owned builds/tests, focused
  positive/negative/lifecycle checks, serial required product regressions,
  coherent ten-image publication to assets/release and configured system32,
  hash verification, commit/push and owner acceptance before style implementation.

## Planning verification

This is a documentation-only replanning delivery. Governance, relative-link
and diff checks are retained under build/M0-T445/S1/r003. The subsequent
source-owner planning update admits the specialist directory without creating
it; checks for that update are under build/M0-T445/S1/r004. Neither planning
delivery changes a protocol, product binary, guest or installed configuration.
The new standalone-stage admission first hit the Current Work size limit in
build/M0-T445/S2/r001. Its closed S1 packet was retained in indexed evidence;
the corrected admission is checked under build/M0-T445/S2/r002. This is planning
only, not a built or launched UI experiment.
