# Proposal — Native indexed-16 text capability and palette fidelity

## Status and objective

Owner-requested queue-head candidate dated 2026-10-10. This proposal allocates
no numeric T and admits no active packet. Restore native CUI indexed-16 colour
fidelity in both NTCON presentation routes and make the supported capability
discoverable without claiming end-to-end RGB VT support.

The contract is character cells with 16 foreground/background colour indices
and the actual mutable 16-entry RGB palette. RGB values in that palette are
supported; independent arbitrary RGB values per cell are not. Native x86 and
x64 CUI targets share this contract. Indexed colour does not imply a fixed
DOS palette, nor permission to change application client behaviour.

## Checked baseline and limits

The read-only source inspection found that
[NTVWM capture](../../src/ntvwm-exe/console_state.c) reads `CHAR_INFO` and
`CONSOLE_SCREEN_BUFFER_INFOEX`, and its
[text packer](../../src/ntvwm-exe/text_frame.c) transmits colour indices and
the actual palette. [Window decoding](../../src/ntcon-exe/window_frame.c)
retains the palette. In contrast,
[Console projection](../../src/ntcon-exe/frontend_session.c) copies cell
attributes without applying the received palette to the visible Console.
These are static findings, not a new end-to-end runtime pass.

The [historical native Edit colour audit](../etc/evidence/m0-t423-s22-native-edit-colour-audit.md)
also found that an ordinary Console did not answer OSC colour queries; terminal
environment variables alone did not make Edit select colour output. Do not
promise that a new environment variable makes arbitrary applications adapt.
At admission, recheck the current source and selected application version.

## Ownership and implementation constraints

- NTCON owns application of committed presentation colours. Keep it neutral
  between DOS and native workers; use the frame palette rather than a native
  worker special case. Audit Console and Window transitions, owner handoff,
  palette-only changes and borrowed-Console restoration together.
- NTVWM owns its ordinary Console capture; run16/NTVWM own construction of the
  target's effective launch capability environment. Preserve redirected streams
  and the host user's environment outside that target launch scope.
- No ConPTY replacement, generic VT emulator, guest modification, new polling
  process, lifecycle redesign or true-colour cell protocol is admitted here.
- Existing host Console VT parsing remains responsible for conversion of RGB
  and 256-colour output into API-observable indexed cells. Do not add a second
  quantizer or assume an undocumented nearest-colour algorithm.
- A palette change must not leave cells and colours from different committed
  frames visible. Define failure/rollback and restoration of the caller's
  original palette, including abnormal frontend teardown where the borrowed
  Console remains alive. Reuse the existing commit/ownership boundary.

## Proposed sequence

| S | Deliverable and verification |
| --- | --- |
| S1 | Repair Console palette projection: apply the committed frame's actual 16-entry palette to the visible Console; audit logical storage and handoff consumers so stale palettes are not inherited. Verify non-default colours, palette-only updates, frame failure/rollback, Console/Window switching, DOS/native handoff and restoration of the borrowed caller palette. |
| S2 | Establish the discoverable indexed-16 capability contract for native CUI launches. Inventory actual target capability detection, inherited environment and existing Console query behaviour before choosing the minimal truthful mechanism. Verify x86/x64 direct and nested launches and a capability-aware target that detects the contract and selects indexed output. Remove or scope misleading RGB/256-colour claims only where the product owns that launch contract. Record which ordinary applications can consume the mechanism; a private marker alone is not proof of automatic third-party adaptation. |
| S3 | Verify host VT degradation and complete end-to-end fidelity: exercise direct RGB/256-colour senders as well as standard cell writers, compare NTVWM capture and both NTCON routes to the host Console's observed indexed result, run affected builds and serial product regressions, and publish only verified artifacts. |

If S2 proves a selected application's colour mode requires unanswered standard
terminal queries, identify the exact query and consumer. Stop for owner review
before adding a bounded query-response implementation or broadening the
ordinary-Console architecture. Never forge RGB/256-colour support merely to
enable that application's theme. An incapable or ignoring application may
retain its native fallback; report that result explicitly.

## Verification and acceptance

Use build-owned x86/x64 native CUI fixtures for standard 16-colour cell output,
palette mutation, VT standard/256/RGB foreground and background, SGR reset,
fragmented escape sequences and VT mode on/off. Read host cells and palette as
the conversion oracle; compare captured indices/palette and final Console
state, and check Window-rendered colours against the same palette. Colour
fidelity does not claim unrestricted Unicode/glyph fidelity from the existing
byte-glyph protocol.

For file/pipe redirection, verify raw bytes remain unchanged and no presentation
conversion occurs. A program that ignores capability negotiation must not cause
lost text, a broken session or a false claim that its original RGB survived.
Use a real capability-aware native application as an additional witness where
available; retain unsupported negotiation as a limitation, not a pass.

Acceptance requires identical supported indexed colours in Console and Window,
correct custom-palette restoration, truthful target-visible capability evidence,
and explicit measured RGB degradation. Retain normal DOS palettes, input,
cursor, owner handoff and completion semantics. Compile only with explicit
object/output paths under `build/`; no root-directory build products. Complete
documentation governance, relative-link validation and diff review before
delivery. Queue admission and numeric task allocation remain CURRENT-owned.
