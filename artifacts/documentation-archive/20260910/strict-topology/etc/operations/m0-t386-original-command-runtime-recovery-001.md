# M0 T386 — Original COMMAND runtime recovery

## Purpose

Reopen the inadequate T385 closure and restore one immutable original
`COMMAND.COM` to actual command-loop operation on the selected
CPU40/SoftPC/NTIO/NTDOS product path. Reaching `54:01`, a BOP boundary, a
formal link, or a controlled process exit alone is not completion.

The selected `COMMAND.COM` and its guest source/build inputs remain immutable.
Its selected artifact SHA-256 is
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
No T386 work may modify, rebuild, replace, patch, or derive a new COMMAND
binary.

## Ordered subtasks

1. **S1 — Original HMA/arena startup-chain audit.** Audit the complete
   source-owned chain from NTIO's INT 2Fh `4A01/4A02` handler through
   `HMAPtr`, `MoveDOSIntoHMA`, `FTryToMovDOSHi`, A20/HMA access,
   `FreeHMAPtr`, DOS-high resident sizing, MCB construction/allocation, and
   COMMAND's environment/first-command handoff. Classify the actual
   disposition of `MVDM-HOST-DIV-200`; do not write production code.
2. **S2 — Original capacity-contract recovery.** Restore the original
   `cmdGetInitEnvironment` required-paragraph result and establish whether
   the guest's own retry remains safe in the current layout. The corrected
   selected-product observation proves the original `0010 -> 015F` retry
   and its BOP stack return; the earlier `0270` overwrite claim came from a
   stale experimental build graph and is not an admissible result.
3. **S3 — Original DOS arena/layout precondition recovery.** Audit the
   source-owned resident resize, MCB construction, allocation strategy and
   transient relocation contract that must make the original retry safe.
   Recover the original Base-client `BaseCreateVDMEnvironment` projection as
   the earliest proven owner, with only a same-shaped direct-entry lifecycle
   binding. Do not filter, cap, replace or otherwise shorten variables, and
   do not alter guest bytes.
4. **S4 — Formal CPU40/x86 closure.** Link the affected formal product and
   prove the repaired precondition, both valid and failure directions, with
   focused source-shaped checks. Verify no guest source/artifact hash changed
   and no guest object enters the host link graph.
5. **S5 — Immutable COMMAND runtime proof.** In one fixed console-owning,
   non-debug container, prove original COMMAND reaches a real command-loop
   operation: a declared original built-in command is accepted, emits its
   original console result, and follows the original command continuation or
   documented original terminal route. The observation must demonstrate more
   than `54:01`/BOP ingress and must retain product/media/container identity.
6. **S6 — Closure review.** Review source/adapter ladder, actual diff,
   immutable COMMAND identity, formal build, focused tests and S5 runtime
   observation. Update the runtime program only after the full proof passes;
   otherwise keep T386 active with the immediate original owner named.

## Boundaries

T386 owns pure-DOS COMMAND startup and command-loop continuity only. It does
not prove external `.COM`/MZ execution, `cmdExec32`, native-child lifecycle,
WOW/Win16, Redirector, DPMI, cross-process broker semantics, CPU30, Bochs or
x64 guest runtime. It cannot solve failure by changing COMMAND, NTDOS/NTIO
guest binary bytes, or fabricating command input/guest loader behavior.

## Required closure

T386 closes only when all of the following are evidenced:

- selected original COMMAND source and binary identities are unchanged;
- HMA/A20/arena ownership and the final disposition of the prior fallback are
  source-proven;
- the smallest selected recovery is formally built on CPU40/x86;
- the fixed container demonstrates actual original COMMAND built-in execution
  and continuation/terminal behavior; and
- no result is presented as external-program, native-child or Win16 proof.

If an immediate original owner outside this package is proven to prevent S4,
the dependency-first activation rule applies: admit its complete source,
binding, formal-link and same-workload runtime closure before proceeding.
