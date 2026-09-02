# M0 T380 S2 — COMMAND native-child lifecycle confirmation

## Source review

S1's selected cohort is already represented by original COMMAND bodies and
four narrow registered bindings. Re-reading current `cmdexec.c` confirms that
the worker still owns original command conversion, `cmdXformEnvironment`,
`CreateProcess`, suspend/resume, wait, exit-code, re-entry decrement and
`ExitThread` ordering. No app child launcher, second dispatcher or synthetic
guest command is present.

The retained adapter work is necessary at its boundary and cannot be replaced
by another original MVDM body:

- asynchronous guest addresses must be captured as bounded session snapshots;
- a historical cdecl `VOID` worker must enter the modern Win32 thread ABI;
- guest stream identities must resolve to child-only `STARTUPINFOA` endpoints,
  not mutate this process's standard handles; and
- the one-session Base VDM record must wait for the original re-entry interval.

These remain `MVDM-HOST-DIV-196`, `-109`, `-149`, and `-197` respectively.
No new or removable production diff was found in this cohort.

## Focused x86 proof

The existing focused graph was regenerated at
`build/M0-T380/S2/command-native-child-x86` using
`tools/build/New-T370CommandNativeChildNinja.mjs`. In a fresh MSVC x86
environment, Ninja compiled the nine selected translation units and linked
`command_native_child_fixture.exe`; the fixture exited zero.

The proof covers the whole selected contract: checked guest command,
environment and standard-handle snapshots; original CR-to-NUL mutation;
active re-entry wait and decrement completion; no fabricated next command;
worker cancellation/cleanup; and release of session-owned state. It is local
source-owner proof only, not evidence that guest COMMAND reached `54:08`.

## Result

S2 closes with no production code change. The original COMMAND lifecycle and
its minimum same-shaped boundaries are current-source and focused-x86 closed.
S3 may now formally link the current CPU40 product for the selected workload.
