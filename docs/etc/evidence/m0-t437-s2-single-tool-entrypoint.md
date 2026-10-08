# M0 T437 S2 — single interactive tool entrypoint

## Scope

The owner found a duplicate Win1.01 user entrypoint: `APPLY.CMD` and the
legacy `apply-setup.cmd` wrapper. S2 removes the wrapper. It does not alter
the underlying `apply-setup.ps1` implementation or any patch payload.

The adjacent tools were swept at the same time. `win31-launch` and
`win31-path` already expose one `APPLY.CMD` each. `win31-setup` is a
build-only package producer and intentionally has no interactive `.cmd`
entrypoint; its generated guest `PATCH\\SETUP.CMD` is original-Setup payload,
not a repository tool entrypoint.

## Verification

- Enumeration of `*.cmd` under all four tool roots finds exactly
  `win101-setup\\APPLY.CMD`, `win31-launch\\APPLY.CMD`, and
  `win31-path\\APPLY.CMD` — no second user wrapper and no repository-side
  `win31-setup` launcher.
- Piping empty input to the retained Win1.01 `APPLY.CMD` displayed the
  one-prompt validation and returned `64`; it did not invoke the patch
  implementation or write guest media.
- Search confirms no active tool or test invocation requires the removed
  `apply-setup.cmd`. Historical evidence retains its prior spelling only as
  historical fact.

## Result

Every interactive add-on tool now has one consistently named human entrypoint:
`APPLY.CMD`. Programmatic/package automation continues to call the underlying
`.ps1` implementation directly where needed.
