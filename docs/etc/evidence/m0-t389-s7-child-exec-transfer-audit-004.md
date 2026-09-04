# M0 T389 S7 — original child EXEC transfer audit

## Question

Does the fixed explicit-`command.com` observation reach the original NTDOS
transition that transfers control to the non-first child `COMMAND.COM`, or is
CPU40 keyboard delivery still the first missing owner?

## Original control flow

The selected NTDOS source
`src/mvdm-guest/dos/v86/doskrnl/dos/msproc.asm::$Exec` reaches
`Xfer_To_User` after it has prepared the child entry and stack.  The source
order at that label is:

```text
SVC SVC_DEMENTRYDOSAPP (50:36)
DOCLI
install child SS:SP
STI
retf to child entry
```

The selected original host table maps `SVC_DEMENTRYDOSAPP` to
`demEntryDosApp` in `src/mvdm-host/dos/dem/demdisp.c`.  Its source body is
debug/VDD notification only; it does not own keyboard, the CPU IF state or
the child instruction stream.

## Fixed-container result

The formal x86 CPU40 product and immutable `O:\ntvdm64` media from the S7 IF
observation reported the following ordered fragment after the normal-child
`54:04` current-directory return:

```text
MVDM-BOP-DISPATCH 54:04
MVDM-BOP-RETURN 54:04 ... if=0
MVDM-BOP-DISPATCH 50:12
MVDM-BOP-DISPATCH 50:42
MVDM-BOP-DISPATCH 50:02
MVDM-BOP-DISPATCH 50:36
```

The `50:36` hit is the source-established `$Exec` handoff boundary.  It
proves the normal child load has progressed beyond the immediately preceding
directory/file work and into the original NTDOS-to-child transfer sequence.

## Disposition

S7 has reached its source-continuation exit: neither a `54:04` return mutation
nor the Console/8042/ICA/CPU40 keyboard cohort is the first missing owner for
the explicit child.  The observation still does not prove that the child has
completed initialization, reached `DoReEnter`/`Do16BitPrompt`, or consumed a
key.  The next bounded owner is therefore the original second-`COMMAND.COM`
`REGCOM`/re-entry sequence, not a keyboard repair.

The child handoff must not be confused with a second boot batch.  Original
`tcode.asm::REGCOM` branches a non-first command directly to `DoReEnter`; its
`SCS_REENTERED=0` state selects `exec_comspec`, and (when `SCS_CMDPROMPT` is
not `Prompt32`) it calls `Do16BitPrompt`.  CONFIG/AUTOEXEC creation and
`SVC_GETAUTOEXECBAT` belong to first-command initialization in `init.asm` and
are not a prerequisite that the child repeats.

No keyboard, BOP, CPU, guest-memory or guest-media semantics were changed by
this audit.
