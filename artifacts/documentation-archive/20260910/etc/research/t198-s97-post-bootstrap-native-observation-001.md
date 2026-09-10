# T198 S97 post-bootstrap native observation 001

## Question

After S96 composes the contained COMMAND bootstrap pair, what is the first
unresolved boundary in one rebuilt exact-input source-built NTIO/NTDOS/
COMMAND/SHARE fixture?

## Procedure

`tools/probe/Invoke-T198S97PostBootstrapNativeProbe.ps1` reused S94's immutable S93
input preparation and one-run harness, rebuilt the current manifest (including
`bx_ntvdm_cmd_comspec_bootstrap_service.c`) under MSVC x64 `/MT`, and made
exactly one native execution.  The retained root is
`artifacts/build/t198-s97-post-bootstrap-native-r1`.

## Observation

The one run returned process exit code 2 from the bounded fixture.  That is
not a DOS or COMMAND result.  Its final report records:

- `status=4`, `observed-stop=1`, and `next=50:42`;
- first `50:11`, `50:3B`, DTA, hard-error and drive continuations completed;
- the source-derived SpcKbd stream transaction completed;
- `fast-read observed=1` at `0032:6351`, with `EAX=4005h`, `ECX=20h` and
  `EDX=0Ah`;
- no observation of `54:02`, `54:0F`, `50:3C` or `54:11` occurred in this
  single run.

Thus the S96 COMMAND component was rebuilt into the native executable, but
the source-built guest's actual path reaches a DEM `50:42` boundary first.
The existing fast-read observation is insufficient to claim this later
`50:42` invocation has the same operation, lifecycle state or failure
contract.

## Interpretation

The next task is source-only: map the complete original DEM `50:42` family,
its caller register/continuation contract, and the current provider's accepted
state/operation domain.  It must decide whether the observed invocation is a
missing lifecycle condition, an unsupported original operation, or a provider
semantic mismatch before any route or guest rerun is admitted.
