# M0 T180 post-target COMMAND lifecycle closure

Date: 2026-08-12  
Status: closed by source/trace decision; no runtime behavior changed.

## Closed scope

T180 established that `54:01`, rather than `54:11`, is the next observed
normal-return edge after the one declared target.  It mapped the retained
OpenNT COMMAND caller and dispatcher, reconciled the exact T179 trace, and
classified the historical no-next protocol as an NT-host broker that is not
composable in the CLI profile.

The final decision is recorded in
`docs/etc/research/t180-s3-finite-command-lifecycle-decision-001.md`:
reject a partial `cmdGetNextCmd` rehost and reject a fabricated successful
empty reply.  The selected successor is an immutable finite sequence of
profile-declared guest targets, terminated by the existing source-built
`QUIT.COM` controlled-stop contract.

## Retained facts

- The first `54:01` delivered the current declared target and resumed.
- The target's normal return produced a second `54:01` at the same COMMAND
  site; the current one-target provider declined it.
- The later vector-06 path is native machine ownership, not a COMMAND result.
- `54:11` remains unobserved.  No DOS exit status reaches the CLI.

## Transfer and exclusions

M0 T181 owns only design/admission of the finite declared-target-sequence
provider.  It must keep `54:01` as the original guest ABI and preserve the
existing ownership split: Bochs owns machine execution, DOS/WOW16 owns guest
semantics, and the adapter host-service plane owns bounded copied ABI routing.

The following remain excluded: `GetNextVDMCommand`, a host command queue,
host process execution, ambient files/directories, `54:11` result transport,
generic machine-BOP behavior, and Bochs device or CPU changes.
