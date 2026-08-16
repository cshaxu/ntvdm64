# T221 S3 P37: Virtual handle typed-token routing

## Question

Does a Virtual namespace open result enter private handle mechanics without a
possible Direct Win32-handle fallback?

## Procedure and observations

The private handle backend now accepts either distinct private token kind
(`OVERLAY_FILE` or `VIRTUAL_FILE`) only after typed session validation. The
Direct handle lookup remains unable to accept either kind. The whole-provider
fixture opens a Virtual private file, seeks it, closes it and proves the stale
token is no longer claimed. MSVC x64 `/MT` passed at `build/M0-T221-S3/114`.

## Interpretation and follow-up

This is typed routing and lifecycle evidence, not handle subfamily closure.
The remaining controlled guest-memory write/read/truncate/commit paths must
be covered before the six-service Virtual handle family is called complete.
