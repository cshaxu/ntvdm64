# M0 T375 S1 — Pure DOS boot/COMMAND topology and fixed-container admission

## Question

Does the current CPU40/SoftPC x86 product reach original COMMAND, and what is
the first source-owned reason it cannot complete the declared one-shot command?

## Inputs

- Formal product: `build/M0-T371/S2/formal-x86/original-softpc-process.exe`,
  SHA-256 `7b6011aaa477eff1e00a541e08fc6e0a6f294fef1cbdf67e91cdf8fdeff5d457`.
- Rebuilt observer: `build/M0-T375/S1/observer/console-startup-observer.exe`,
  SHA-256 `350cb5919d8bea4d32e5c3a7d87c9b7a53c1efd0a50ad2dabfe66adafae9ba01`.
- Immutable stage manifest: `O:\ntvdm64/runtime-manifest.json`, SHA-256
  `5af5a7c9d9eecaeceadc325da899fd2d276fbea98557c20f5df193df23bf1077`.
- Original sources: `mvdm-host/dos/command/cmdmisc.c`, current Base VDM
  adapter/broker sources, and OpenNT
  `base/win32/{client/vdm.c,server/srvvdm.c}`.

## Procedure

1. Rebuilt the observer in `build/M0-T375/S1/observer` with the admitted
   MSVC x86 Ninja graph.
2. Made exactly one non-debug, console-owning invocation with the fixed stage:

   ```text
   node tools/observation/ObserveSoftpcStartup.mjs
     --launcher build/M0-T375/S1/observer/console-startup-observer.exe
     --product build/M0-T371/S2/formal-x86/original-softpc-process.exe
     --stage O:\ntvdm64
     --report O:\ntvdm64\observation-t375-s1.txt
     --product-command "COMMAND.COM /C EXIT"
     --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t375-s1.txt.bop-return.txt
   ```
3. Compared the reached `54:01` path with original `cmdGetNextCmd`, client
   `GetNextVDMCommand`, and server `BaseSrvGetNextVDMCommand` state ordering.

## Observations

- The stage retained the fixed root `O:\ntvdm64`; the staged product hash was
  the formal product hash above. The observer returned its controlled
  eight-second timeout, not an exception.
- The report reached original boot, NTIO/NTDOS and COMMAND initialization, then
  original `54:01`, `54:0E`, `54:04`, normal DEM paths and a returned first
  `54:01`. Its first record changed from `dos-state=00000001` to
  `dos-state=00000002` (available to busy) in the existing local binding.
- A later original `54:01` reached `cmdGetNextCmd` with `first=0`, `repeat=0`,
  `dos-state=00000002`, then did not return before timeout. No guest loader,
  `.COM` child, MZ child or guest-media mutation was introduced.
- `app_launch_declaration_publish` publishes only the initial DOS record.
  `base_vdm_local_wait_for_command` then waits indefinitely on its local
  events when no later record exists.
- Original `srvvdm.c` instead owns a DOS-record/console state machine: it
  selects `VDM_TO_TAKE_A_COMMAND`, marks delivery `VDM_BUSY`, creates or
  resets paired wait objects for an interactive no-command request, returns
  immediately for `RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME`, and has a
  separate `BaseSrvExitVDM` worker. Original `vdm.c` owns the client retry
  that adds `ASKING_FOR_SECOND_TIME` after its returned wait object signals.

## Interpretation

The first blocker is not SoftPC boot, NTIO, NTDOS, guest media, a DOS BOP
leaf, or a missing guest loader. It is the incomplete host-side Base VDM
DOS-record lifecycle. The current adapter correctly models initial copied
record delivery but not the complete original no-command, wake, terminal and
exit disposition. Simply completing the session after the first record, or
injecting a second command, would violate the original state contract.

## Follow-up

T375 S2 owns the finite reached BaseClient/BaseSrv DOS-record lifecycle.
It must first retain/import the useful original client/server state structure,
then bind its private CSR/console operations through the smallest one-session
same-shaped adapter. Only after local lifecycle proof may S3 repeat a fixed
`/C EXIT` observation.
