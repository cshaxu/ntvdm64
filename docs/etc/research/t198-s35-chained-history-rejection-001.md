# T198 S35 chained-history rejection 001

## Scope

S35 tested the smallest default-off, selector-blind core-to-mantle history
candidate permitted by `BX-CORE-070`: record fixed CPU id, CS, RIP and an
execution sequence at the existing CPU-loop `BX_INSTR_BEFORE_EXECUTION` site.
The mantle owned a four-entry copied ring.  This was an observation experiment,
not a new runtime feature.

## Exact inputs and procedure

The experiment used the existing CPU5/Pentium-MMX finite fixture and its
MSVC x64 `/MT` closure:

```text
cmd /c artifacts\build\t198-s35-instruction-history-r1\compile.cmd
cmd /c artifacts\build\t198-s25-native-command-r1\link.cmd
artifacts\build\t198-s23-native-ntio-run-r1\command-r1.exe
```

The temporary build compiled only the changed `bx-core/cpu/cpu.cc`, the new
mantle ring object and the exact fixture, with
`BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1`.  The fixture first verified
the default disabled state, rejected capacity 17, then selected capacity four.

## Observed result

The compilation, link and existing composed startup transaction succeeded.
The terminal line included:

```text
observed-first-generic-ud=1 fault=0000:0a84 ... observed-stop=1 ...
history=4:0070:01ba/0070:01d6/0070:01c7/0000:0ac1
```

The same generated CPU5 configuration defines
`BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS 1`.  In the retained original CPU loop,
the selected before-execution hook runs once before
`BX_CPU_CALL_METHOD(i->execute, (i))`; the adjacent original comment states
that this one call executes an entire trace when chaining is enabled.

## Interpretation

`0000:0AC1` is a chained-trace entry, not a demonstrated immediate predecessor
of the generic `#UD` at `0000:0A84`.  The experiment therefore cannot meet
S35's required exact-predecessor evidence.  The failure is about granularity,
not the x64 toolchain, the ring layout, or the existing BOP transaction.

The temporary core inclusion, mantle files, fixture changes and derivative
recipe were physically removed after the run.  `BX-CORE-070` remains as a
rejected exception record rather than an active core modification.

## Boundary and successor

No instruction object, guest bytes, BOP selector/service, OpenNT/DOS term,
CPU mutation, product instrumentation, device, firmware or adapter dependency
entered the candidate.  Direct instruction-level tracing under the retained
chained CPU closure would need a newly admitted, source-proven native seam;
disabling chaining or enabling debugger/instrumentation is not an acceptable
substitution.  Per `rules/EXECUTION.md`, the failed stop condition pauses this
S and requires re-admission before a different boundary is attempted.
