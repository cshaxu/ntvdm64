# T95 S7 r45 Keyboard-Layout Runtime Result

## Question

Did the single admitted fixed `BOP_CMD:0E` response resume real COMMAND, and
what is the next source-owned BOP reached?

## Procedure and Observation

Fresh root `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r45`
was generated from r4 with only the keyboard-layout adapter source added, then
its declared MSVC/x86 nmake target was run once. The resulting executable is
2,560,000 bytes, SHA-256
`BF3CAFA99AD1C250BB6A83CB4346A29D7AF6F0A4CD6095E43503C78ED759CD9A`.

One 30-second deferred-startup trace was retained in
`artifacts/analysis/t95-s7-native-post-handoff-038-20260811-001`. Its record
is watchdog termination/exit 1, 602518 BOP observations and 14 commits.
It observes `54:01` at `95AB:03C1`, then `54:0E` at `95AB:05D9`, then
`54:04` at `0032:67A1`. The adjacent raw log proves `54:04` is already served:
one four-byte multi-write commits and the CPU resumes at `67A5`. The next
unserved selector is `FE` at `0D00:0100`; its later selector-06 fall-through
loop at `073B:0740` is not evidence that `54:0E` or `54:04` failed.

`src/opennt/base/mvdm/inc/cmdsvc.h` names `54:04` as `SVC_CMDGETCURDIR`, and
`cmddisp.c` assigns it to `cmdGetCurrentDir`; its implementation is in
`dos/command/cmdmisc.c:517-...`. The already-admitted adapter implementation
matches its contained root result (`C:\\0`) and success CF state.

## Follow-up

Identify selector `FE` from the OpenNT BOP inventory and audit its reached
caller before deciding any response. Do not add a second trace or any machine
feature first.
