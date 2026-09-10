# T225 S22 P1 Native Startup Continuity Observation

## Question

Does the current source-built primary guest input cross the original NTIO
`50:11` NTDOS load boundary and continue into original NTDOS/COMMAND startup
in both supported mutation modes? If it stops, what is the first observable
owner boundary without turning an observed BOP into a new implementation task?

## Inputs and Procedure

- fresh disposable Ninja root:
  `build/M0-T225-S22/startup-continuity-r1`;
- formal manifest: `tools/build/t225-s7-full-module-manifest.json`, MSVC x64
  `/MT`, CPU5/P-MMX config projection
  `DDB0D13E35C2FD9E7EC29896E6296F83ECE7F6BF5D2AD474E8937D389F97872F`;
- immutable source-built primary input root:
  `build/M0-T225-S10/primary-bootstrap-inputs-r2`;
- fresh native executable SHA-256:
  `3C112DF8A5C91303F9DA58289E14B6B62EF31B615918FFD206FAA74292B16AB9`.

The native CLI was run once in Direct and once in Readonly with the same
profile/root/target, paired startup configuration environment variables, both
existing BOP and generic-UD observers, and a one-million-instruction budget.
No guest byte, BOP provider, Bochs feature, or adapter route changed.

## Observation

Both modes exit with the normal internal budget terminal (`exit=4`,
`terminal=4`, `detail=0`, `lifecycle=5`, `presentation=5`). Their stdout is
byte-identical:
`6F5B36636075ED584D26F111B4F58AE863E593E47F78DC942CE422E8F588D5EB`.

The common prefix proves the original startup sequence passed these package
boundaries in order:

1. NTIO at `0070:0475` issued `50:11`; the existing source-derived
   `demLoadDos` seam copied the identity-checked NTDOS image at the
   guest-selected address and resumed.
2. NTDOS initialization issued its established DEM/COMMAND setup calls,
   including `50:0F`, `50:1B`, `50:32`, `54:05`, `50:46`, and later normal
   namespace/file calls.
3. The sequence reached `50:36`, which is original NTDOS `$Exec`'s
   entry-to-DOS-app notification.
4. Original COMMAND then reached `54:02`, `54:0D`, and two calls to `54:0F`.
   The two environment calls match the original `cmdGetInitEnvironment`
   paragraph-capacity/retry contract: it first reports the required paragraph
   count and then supplies the COMSPEC plus double-NUL initial environment.

After those calls, the observer records a source-defined `C4 C4 06` at
`074A:0740`, repeatedly until its fixed record capacity overflows. The Bochs
log then reports a real-mode stack-prefetch fault and third unresolved
exception. The observer's `selector=06`, `has-service=0`, `disposition=0`
means only that this is not a mapped host-service route. S22 P3 identifies it
as the original SoftPC INT 06 `illegal_op_int` machine path, not an adapter
service or debugger selector.

## Original-Source Comparison

OpenNT `bios/msinit.asm` loads NTDOS by placing `dos_load_seg` in DI, calling
`SVC_DEMLOADDOS`, and then continuing directly to `sysinit`. NTDOS
`inc/mshead.asm` begins at `$STARTCODE` and jumps to `DOSINIT`; its
`dos/msinit.asm` initialization registers the DTA/hard-error/CMD locations
and enters its normal boot progression. This observed path is therefore the
expected ownership route, not an adapter-created NTDOS entry.

OpenNT `command/cmdenv.c:cmdGetInitEnvironment` uses ES:0 plus BX paragraphs.
It first reports required paragraphs when insufficient, then writes COMSPEC
and the retained double-NUL environment and returns BX paragraphs. The current
COMMAND provider follows this same copied-transaction order. This observation
alone does not prove the post-return memory/register state is correct.

## Interpretation and Follow-up

S22 has crossed the S21 prerequisite: NTDOS and the early original COMMAND
startup are demonstrably native-reachable with identical Direct/Readonly
behavior. The next investigation is one startup/COMMAND ABI-state comparison
at the return from `54:0F`: guest ES/BX/stack/return continuation and the
exact initial-environment bytes must be compared against source before any
repair is admitted.

No BOP implementation, guest EXEC implementation, host child process, or
Bochs device follows from this observation. In particular, the repeated
selector `06h` is a source-defined machine boundary, not a leaf-task queue.