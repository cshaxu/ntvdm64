# Proposal — host-rendered Win32 applications inside guest Windows

## Status and objective

Owner requested this independent candidate at the end of the T task queue on
2026-11-06. [Queue](../states/QUEUE.md) owns its order. It is not admitted into
the active task and does not allocate a T number.

Investigate running a Win32 application and rendering its content entirely on
the modern host while presenting its interactive interface inside guest
Windows 3.1. This is remote presentation, not guest execution of Win32 APIs.
No Win32s, original guest rebuild, binary patch or runtime hot-patch is allowed.
An independently authored Win16 presentation client and a bounded bridge are
possible mechanisms, subject to source-policy review and feasibility evidence.

## Research scope

- Compare a single guest proxy window showing the complete remote application
  surface against separate guest proxy windows for host top-level/dialog/popup
  windows. Prefer the smallest workable first scope; do not presume a second
  window manager is necessary or that arbitrary applications are supported.
- Identify reliable host capture/rendering mechanisms and their behavior with
  hidden, minimized and occluded windows, including accelerated drawing.
  External-window invisibility is a question to prove, not a promised feature.
- Account for menus, tooltips, owned windows and modal dialogs; capturing only
  a main client rectangle must not be described as complete application support.
- Investigate mouse/keyboard forwarding, coordinate transforms, focus,
  capture/dragging, shortcuts and input methods. Posted messages alone must
  not be assumed equivalent to real input for every application.
- Assess guest geometry, palette/color-depth conversion, dirty updates,
  allocation, bandwidth and latency. Reuse suitable existing frame/I/O
  components without assuming their current contracts fit this new consumer.
- Define close versus disconnect, host-process loss, guest-client/VM loss and
  cleanup. Preserve NTSRV authority where the current lifecycle model applies;
  do not silently turn observed native GUI applications into managed workers.

## Ownership and deliverable

Host Windows and the actual application own program logic, controls and
rendering. The guest client owns its local presentation window and input
collection. Native HWNDs cannot simply be parented into the separate guest
window system. The bridge must not duplicate the application's controls or
business logic in the guest.

Deliver a feasibility report, supported/unsupported application matrix,
architecture alternatives and tradeoffs, minimal implementation proposal and
end-to-end test plan. Distinguish source/API reasoning from measured behavior.
The separate bidirectional-launch-routing candidate selects execution location;
this candidate concerns presentation location and may consume that work without
duplicating it. Implementation requires subsequent admission; queue placement
does not expand the current guest multitask task.
