# T222 S2 Drive-View Selected-Drive Regression

## Question

Does `50:0D demGetBootDrive` use the provider's admitted selected-drive state
through normal Direct/Readonly package-session dispatch?

## Observation

The normal session now sets its existing drive-view boot state to admitted C:
and invokes `50:0D` under both Direct and Readonly.  Each result resumes at
the bounded post-BOP RIP and writes `AL=3`, the original one-based C: drive
value.  MSVC x64 `/MT` build/link/run from
`build/M0-T222-S2/025-drive-view-boot-drive` exited zero.

## Interpretation

The result uses shared selected-drive state, not the historical registry or a
fixture-only fallback.  It remains only one identity in T222's full family
regression.
