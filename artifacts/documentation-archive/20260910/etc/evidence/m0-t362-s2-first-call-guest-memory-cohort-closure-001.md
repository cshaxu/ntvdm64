# M0 T362 S2 — First-call guest-memory cohort closure

## Question

Can the complete original permanent-COMMAND first-call guest-memory cohort
cross the modern session boundary without retaining the NT4 low-address
`GetVDMAddr` alias?

## Inputs

- Selected original `mvdm-host/dos/command/{cmdmisc.c,cmdenv.c}`.
- Selected guest `mvdm-guest/dos/v86/cmd/command/{init.asm,spc.asm,comequ.asm}`.
- Existing `adapter-mvdm-host-out/softpc/mvdm_guest_location` and the
  session-owned guest-memory mapping manager.
- Formal CPU40/x86 original-command Ninja graph.

## Recovered cohort

1. `cmdComSpec` retains its original first-call gate, `COMSPEC=` prefix,
   `cbComSpec` calculation and `AL` disposition.  It copies the bounded
   `DS:DX` NUL string under one read lease before the original host buffer
   consumes it.
2. `cmdGetInitEnvironment` retains environment construction, paragraph
   sizing and the original insufficient-buffer `BX` outcome.  Only after that
   gate does it take one `ES:0` write lease and make the original COMSPEC then
   environment copies in their original order.
3. `cmdGetNextCmd` snapshots the fixed packed `CMDINFO` under one read lease,
   calls the already selected Base VDM local client with caller-owned
   command/application buffers, writes the command-tail and any transformed
   environment through independent short write leases, then commits the
   original updated `CMDINFO` as one fixed record.  No lease crosses the Base
   VDM call and no guest pointer is retained in `cmdVDMEnvBlk`.

Unavailable/malformed source or destination spans return the explicit
existing COMMAND boundary failure `CF=1`, `AX=ERROR_INVALID_ADDRESS`; no
guest write is committed for a rejected span.  This is registered as
`MVDM-HOST-DIV-192` through `-194` in the component README.

## Verification

- `ninja -C build/M0-T282/S12/x86 test` passed the existing focused mapped
  guest-location fixture: positive read/write/far/word/string paths and
  negative unterminated/out-of-range paths.
- `ninja -C build/M0-T362/S2/x86 -j 8 original-mvdm-command.lib` rebuilt
  the selected original COMMAND package successfully with the MSVC x86
  CPU40 graph.  Historical warnings remain visible; no warning was disabled.
- Static sweep of `cmdGetNextCmd` found no remaining `GetVDMAddr` use in its
  function body.  `cmdComSpec` and `cmdGetInitEnvironment` likewise route
  their S2 first-call span through `mvdm_guest_location`.

## Result and next step

The complete S2 source cohort is code/build closed.  This does not prove the
previous timeout was caused by the aliases and makes no runtime-continuity
claim.  T362 S3 alone may now perform the one frozen console-owning `/C EXIT`
observation described by the T362 plan.
