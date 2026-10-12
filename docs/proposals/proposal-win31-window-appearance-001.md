# Proposal — Windows 3.1 window appearance

## Status and objective

This proposal has been admitted for ownership/API audit and design through
[Current Work](../states/CURRENT.md). It is no longer queued. Current Work
alone allocates the active packet; the sequence below remains its scope
reference, not permission to skip review or change live window behavior.

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

## Opt-in and delivery boundary

Owner-approved entrypoints are `run16 --win16-window-style` (Win16 windows
only) and `run16 --win16-window-style-all` (Win16/Win32/Win64 windows).
Both propagate along controlled launch chains, including through a native
intermediate that does not itself render the Win16-only style. Unrelated tasks
in a shared WOW worker must remain unaffected. Default non-inherited launches
retain native appearance.

Policy/propagation is delivered and verified first through a visible
`(Win16 Style) ` title prefix on enabled windows. This is an enablement witness,
not proof of retro rendering. Actual classic rendering follows only after
that boundary is proven. The admitted package's multi-S sequence and acceptance
matrix live in the [working plan](../etc/operations/t445-win31-window-style-plan.md);
[Current Work](../states/CURRENT.md) alone admits the next stage.

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
