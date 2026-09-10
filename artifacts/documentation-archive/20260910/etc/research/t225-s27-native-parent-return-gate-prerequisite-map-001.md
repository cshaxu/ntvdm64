# T225 S27 P4 Native Parent-Return Gate Prerequisite Map

## Question

Can the remaining S27 native gate be satisfied by the existing native CLI
without manufacturing a `54:0B` request, and what exact evidence must precede
that observation?

## Inputs

- S27 P1 source/ABI/failure map and P3 bound-composition family result.
- Original OpenNT COMMAND source: `base/mvdm/dos/command/cmddisp.c`,
  `cmdexec.c:cmdReturnExitCode`, and
  `base/mvdm/dos/v86/cmd/command/tcode.asm`.
- Original guest EXEC/PSP source map: T225 S21 P1 and T225 S4 P1.
- Current engine and composition runtime:
  `src/bx-mantle/bx_ntvdm_engine_run_v1.c` and
  `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c`.
- Existing primary source-built observation: T225 S10 result.
- Existing engine test:
  `tests/bx-mantle/t212_s5_engine_cancellation_test.c`.

## Procedure

The review followed the terminal fact from the bound `54:0B` provider to the
engine, then compared its required source-owned caller path with the only
available source-built native observation. No guest bytes, BOP request,
provider, machine configuration, or native trace were changed or executed.

## Observations

1. Original COMMAND maps service 11 to `cmdReturnExitCode` in `cmddisp.c`.
   Its guest callers in `tcode.asm` invoke `SVC_RETURNEXITCODE` only after a
   guest child has returned to COMMAND. Carry selects declared-command reentry;
   clear carry selects the ordinary parent result.
2. The current engine resets the machine, queries only the opaque runtime
   ordinary-terminal fact, then resets the runtime. A controlled stop maps to
   `ORDINARY_GUEST_COMPLETION` only when that fact is present. It does not
   decode a selector or inspect guest state.
3. `t212_s5_engine_cancellation_test.c` proves that ordering using a fake
   runtime return value. It is a mechanical unit test, not proof that the real
   composition acquired the fact from original guest execution.
4. S27 P3 proves the real bound composition records the exhausted-plan fact
   for an accepted `54:0B`, distinguishes `54:00`, and preserves the branch in
   Direct and Readonly. Its event is still a fixture-supplied fixed-width BOP
   event, not an NTDOS/COMMAND execution.
5. The current immutable primary source-built native observation reaches the
   established early startup sequence and terminates at its instruction budget;
   it does not establish continuous NTDOS initialization, COMMAND handoff,
   guest `$Exec`, child termination, or a reached `54:0B`.

## Decision

A native S27 proof must be a bounded run in which the real installed runtime
receives `54:0B` from the original guest COMMAND parent-return path after a
source-built guest child exits. It must observe a controlled stop and the CLI
ordinary-completion terminal, while retaining the `54:00` distinction already
proved by P3.

Injecting `C4 C4 54 0B` into guest RAM, directly calling the composition from a
native executable, or treating the fake-runtime engine test as that proof is
rejected. Each would bypass the guest-owned NTDOS EXEC/PSP/termination path
and would falsely convert a component fixture into an end-to-end claim.

## Required Predecessor

The gate first requires a separately admitted, source-built continuous guest
execution closure:

1. original NTIO reaches original NTDOS;
2. original NTDOS reaches original COMMAND;
3. original COMMAND invokes NTDOS `$Exec` for a declared source-built COM or
   EXE child through the admitted DEM file boundary;
4. the child performs original DOS termination and returns through the original
   PSP/parent path; and
5. only then may its reached COMMAND `54:0B` serve as the S27 native
   observation.

This predecessor belongs to the guest/machine continuity boundary identified
by S21 and S4. It is not authorization for an adapter DOS loader, host child,
new BOP leaf implementation, or extra Bochs device. Its packet must identify
any newly reached machine prerequisite by its native bx-core/bx-mantle owner.

## Interpretation And Follow-up

S27 P1--P3 close the source, fixed-width seam, and full composition-family
parts of the parent-return package. The remaining native criterion is real but
not currently executable from the proven startup state. S27 remains active;
no terminal claim is made. The next admissible action is to plan and prove the
source-built NTIO-to-NTDOS-to-COMMAND continuity predecessor, after which this
same P4 gate can run once in Direct and once in Readonly.
