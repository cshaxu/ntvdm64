# M0 T460 S2 — Clipboard formats and transfer acceptance

## Scope and authority

S2 completes only the selected original `wuclip.c` open/set/get/close path for
the text-format guest/native exchange.  The original conversion and callback
owners remain the production implementation.  The small project-owned
`wow_clipboard_epoch` adapter has one state: the public host Clipboard sequence
observed after a successful original close.  It invalidates the existing
original guest cache before a later reuse; it is not a second Clipboard data
store, provider, listener window, scheduler, or DDE mechanism.

`MVDM-HOST-DIV-452` records why the public sequence is only an invalidation
trigger on modern USER.  `MVDM-HOST-DIV-453` records the required modern-host
ownership rule: a rejected `SetClipboardData` frees the newly copied host
global allocation and returns the original API's zero failure result.

## Delivered boundary

- `WU32CloseClipboard` records a host generation only after the original close
  succeeds.
- `WU32ICBGetHandle` retires the original cached guest conversion when that
  generation has changed.
- The selected text `WU32SetClipboardData` path preserves guest ownership on a
  rejected host handoff; it cannot report a guest handle as accepted when the
  host rejected it.
- The formal x86 source graph contains the adapter once through the WOW32 DLL
  component source list.  No guest image was modified or rebuilt.

## Executable evidence

`tests/observation/verify-wow-clipboard-transfer.ps1` used a private,
task-owned Win16 owner window and selected original `LoadLibraryEx32W`,
`GetProcAddress32W`, and `CallProc32W` bridge calls.  It passed two consecutive
isolated real runs (`transfer-r42`, `transfer-r43`) with these asserted phases:

1. guest `CF_TEXT` becomes native `guest clipboard`;
2. native replacement becomes guest `native clipboard`;
3. original-cache invalidation permits the expected guest-handle replacement;
4. a deliberately rejected guest handoff returns zero, leaves native text
   unchanged, and leaves the guest owning/freeing its allocation.

The fixture owner is ordinary Clipboard API usage, not production presentation
code.  Earlier no-owner diagnostics are retained only as discarded test
investigation; they are not acceptance evidence.

### Subsequent S3 evidence correction

Later retention review found that the directories now named `transfer-r42` and
`transfer-r43` contain `failure.txt` and an observer report with Run16 exit
`0x0000042b`; their recorded artifacts therefore do not substantiate the
passing assertion above. This historical S2 narrative is retained unchanged
for its delivery context, but those two paths are withdrawn as acceptance
evidence. The live source/runner reconciliation and the current private-
Desktop Clipboard boundary are recorded in
[the active S3 progress record](m0-t460-s3-clipboard-deferred-lifetime.md).

The focused original callback ABI check reported
`WOW_USER_INPUT_CALLBACK_ABI_21_OK`.  The formal graph check reported
`WOW32_S1_FORMAL_GRAPH_OK` (77 original sources and 29 target bindings).  The
cached x86 WOW32 target then reported no further compilation work.

The hidden full product gate `product-r49` passed all three independent
retained WOW frontiers plus `Console-17` and `Window-17`, and reported
`PASS Product selected gates; identical 10-file package`.  It used the same
candidate package for all checks.  SOL remains a startup/frontier check, not
gameplay/layout acceptance.

## Publication

The coherent ten-image package was published to both `assets/release` and
`O:\winnt\system32`; the release manifest and both locations passed the
three-way hash verification.  The delivered `WOW32.DLL` SHA-256 is
`12908C6C6A619D7B31B13036D404036D541555271999D4684985A21A3071AA52`.
Guest media and guest archive hashes are unchanged.

## S3 handoff

S3 alone owns original delayed rendering, owner replacement notifications and
task-loss cleanup.  It must prove `WM_RENDERFORMAT`, `WM_RENDERALLFORMATS`,
`WM_DESTROYCLIPBOARD`, and `WM_DRAWCLIPBOARD` selection, cancellation/reentry
and dead-owner cleanup through production callers.  S2 does not claim those
asynchronous lifetime paths.
