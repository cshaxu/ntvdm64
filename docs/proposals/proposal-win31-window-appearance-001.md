# Proposal — Windows 3.1 window appearance

## Status and objective

This is an unadmitted queue candidate.  It does not allocate a numeric task,
alter the active packet, or change current window behavior.

Provide a coherent Windows 3.1-style visible window appearance for supported
Win16, Win32, and Win64 **window-program** presentations.  The result is a
user-interface policy, not a program classifier: DOS/text Console projection,
native Console ownership, worker lifecycle, program execution, and the guest
window's client pixels remain outside this candidate. This candidate concerns
application HWNDs only: it does not involve NTCON, Console surfaces, text
handoff, or the outer presentation of a complete Windows desktop running as
a DOS guest.

## Owner clarification and checked source paths — 2026-10-09

Win16, Win32 and Win64 window applications converge on host USER HWNDs.
Their appearance must therefore use one shared HWND non-client implementation,
not three family-specific renderers. The difference is how that implementation
is installed into the owning process.

The read-only source check establishes the following static paths; it is not
runtime appearance acceptance:

- WOW's [window creation binding](../../src/wow32-dll/source/wow_window_creation_binding.c)
  calls host `CreateWindowExA/W`. Its scoped `WH_CBT` hook binds WOW metadata;
  it is not an appearance hook.
- WOW's [dialog creation binding](../../src/wow32-dll/source/wow_dialog_creation_binding.c)
  calls host `CreateDialogIndirectParamA/W`. Dialog HWNDs must also reach the
  shared appearance installation path.
- [Native execution](../../src/ntvwm-exe/execution.c) installs nthook before
  resuming the native target. The [installer](../../src/nthook32-dll/installer.cpp)
  selects `nthook32.dll` or `nthook64.dll` from the target's actual machine
  type. [Child creation interception](../../src/nthook32-dll/create_process.cpp)
  also installs hooks in native GUI children, without requiring text frontend
  capabilities.
- Current `nthook_attach` intercepts only `CreateProcessA/W`; it does not yet
  install window subclasses or implement non-client appearance. The checked
  NTVDM launch path does not install nthook into the WOW host.

The proposed design is **one appearance implementation, two installation
entries**. Build the shared project-owned implementation for x86 and x64.
The WOW host links and invokes it directly at its project-owned window/dialog
boundary; native target processes use the same implementation through the
matching nthook DLL. Do not inject nthook into the WOW host merely to share
code. Exact specialist source placement and installation timing remain subject
to admission review; this proposal does not authorize a new generic helper root.

The common implementation owns caption/border/button drawing, palette, metrics,
DPI adaptation, non-client geometry and hit testing. Installation must preserve
the existing HWND and application client rendering. In particular, retain
[WOW's native window procedure](../../src/wow32-dll/source/wow_window_words_binding.c),
its guest callbacks, metadata and message conversions. Messages outside the
proved appearance contract continue through the original window procedure;
system commands retain their native behavior. Review subclass ordering,
application procedure replacement, creation failure and `WM_NCDESTROY` cleanup
before selecting the exact mechanism.

Appearance state belongs to the target process/window and must remain usable
after run16 returns. It must not attach GUI windows to the text execution
handoff chain. Begin with standard captions and borders, then standard menus;
custom-drawn captions, menus and controls require an explicit support boundary,
not an assumption that every application can be restyled.

## Boundary to establish at admission

The three program families do not necessarily have the same window owner:

| Family | Candidate question |
| --- | --- |
| Win16 | Prove the WOW host HWND and dialog creation paths, existing procedure/metadata ownership, and the direct shared appearance installation boundary. |
| Win32 / Win64 window programs | Prove same-process nthook coverage of target and child HWND creation, including dialogs and later GUI threads, and the shared appearance installation boundary. |

Admission begins by tracing those ownership paths and the current title,
activation, sizing, menu, system-command, DPI and destruction flow.  It must
not assume that a process-creation hook already covers window creation.

## Required appearance contract

- The visible non-client presentation uses the agreed Windows 3.1 visual
  language: classic caption, border, system menu/minimize/maximize controls,
  menu treatment, active/inactive colors, metrics and resize feedback.
- Preserve the target's client area, input semantics, accelerators, focus,
  modality, sizing constraints, title updates, system commands and process
  completion.  A visual layer must not intercept or reinterpret arbitrary
  application messages beyond the proven presentation boundary.
- Apply the same user-visible policy to all supported Win16/Win32/Win64
  window-program paths.  Family-specific adapters are allowed only where the
  traced ownership boundary differs; the policy and acceptance criteria stay
  common.
- Respect current host DPI/scaling and accessibility/high-contrast constraints
  where the operating system exposes them.  Do not hard-code a pixel scale or
  make a Windows 3.1 look depend on a specific monitor or RDP session.
- Keep appearance in the application HWND's owning process. A hook is finite
  and same-process; it does not become a window registry, task
  observer, process-tree monitor, or lifecycle controller.
- Unmanaged/unsupported HWNDs retain native appearance rather than receiving
  a filename-, class-, or process-name-based special case.

## Proposed sequence

| S | Deliverable and stop condition |
| --- | --- |
| S1 | Ownership and API audit for one representative Win16, Win32 and Win64 window program.  Record the exact visible-frame owner and all title/activation/resize/destruction edges.  Stop for design review if one family has no authenticated project presentation boundary. |
| S2 | Define the shared Windows 3.1 metrics, palette, non-client state and DPI/accessibility policy at the proven owner boundary.  Add focused rendering/state tests before changing live target behavior. |
| S3 | Connect the same appearance implementation through direct WOW-host installation and matching native nthook installation, preserving client input, title propagation, system commands, native-window fallback and existing lifecycle. |
| S4 | Run focused Win16/Win32/Win64 appearance and interaction tests, then serial Console/Window product regressions.  Compare direct-native behavior separately so this package does not claim to restyle applications outside `run16` ownership. |

## Acceptance

- Representative Win16, Win32 and Win64 window applications launched through
  the supported product path present the same Windows 3.1-style non-client
  appearance while retaining their own client rendering and normal input.
- Caption/title changes, activation, focus, move/resize, minimize/maximize,
  system menu, keyboard accelerators, modal dialogs and close/exit remain
  correct for each family.
- Text Console applications, DOS/Win16 text paths, direct non-product native
  launches and unsupported HWNDs are unchanged.
- No NTCON or Console integration is introduced; no new persistent process,
  polling loop, observed-task store, process enumeration or guest/media
  modification is introduced.
- Affected width artifacts build and focused tests pass before the relevant
  serial product matrix and publication are attempted.
