# CPU40 package-stage observation closure

## Purpose

Create one reproducible executable-relative runtime package from a fresh
CPU40/x86 formal product and unchanged MVDM media, then make one fixed
non-debug observation through the existing staging/observer contract.  This
is the source-defined successor of T356 S4's app-owned media rejection.

## Scope

- Use `tools/build/Stage-OriginalSoftpcRuntime.mjs`; it must create a new
  output directory and must never overwrite a prior stage.
- Preserve all staged media hashes and place the formal executable beside the
  staged `mvdm` directory, as the app's `GetModuleFileNameA` contract requires.
- Use `tools/observation/ObserveSoftpcStartup.mjs` and one non-debug,
  console-owning fixed observation.
- Classify only the first source-owned post-admission result.

## Exclusions

No guest/media/source mutation, no product code change, BOP leaf repair,
SoftPC/CPU/device change, host child surrogate, alternate root alias,
CPU30, Bochs, x64 runtime, CSRSS/kernel recreation, or observation retry.

## Completion

The staged package's product and immutable media identities are recorded, and
one observation either reaches a source-defined next execution boundary or
records an exact terminal.  It does not itself claim DOS child/parent return.
