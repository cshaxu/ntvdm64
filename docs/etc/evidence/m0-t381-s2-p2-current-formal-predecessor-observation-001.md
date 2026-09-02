# M0 T381 S2 P2 — Current-formal `cmdExec32` predecessor observation

## Question

Does the current formal CPU40/x86 product reach original COMMAND `54:08` for
the selected `C:\\Windows\\System32\\cmd.exe /c exit 37` request, so that the
active `cmdExec32` worker cohort can be recovered?

## Inputs

- Formal product:
  `build/M0-T371/S2/formal-x86/original-softpc-process.exe`, SHA-256
  `6df5e9767fa7e383275bf63eab3f8398634ff94a344cb19e7a80e12140722271`.
- Fixed immutable package root: `O:\\ntvdm64`.
- Fixed non-debug, console-owning observer and eight-second timeout.
- Declared child request: `C:\\Windows\\System32\\cmd.exe /c exit 37`.
- The default-off P1 payload observer, selected by
  `MVDM_NATIVE_CHILD_REPORT_PATH`.

## Procedure

The current formal product was rebuilt, copied as the only mutable executable
into the identity-verified stage, and observed once.  No guest media, command,
container, timeout, CPU profile or product source was changed for the run.

## Observation

The observer reported the expected controlled timeout `0x53504354`.  The BOP
return trace reaches both original `54:0F cmdGetInitEnvironment` calls, then
contains no `54:01`, `54:07`, `54:08` or payload report.

The stopped CPU thread supplies an exact original-machine predecessor:

- Stop EIP is `0x001A2D7D`.  Rebasing the formal map from `0x00400000` to the
  observed `0x00120000` image base maps it to original `ios.c:inb`.
- The immediate return address is `0x00360314`.  It maps inside original
  `base/comms/printer_.c:printer_io` (`0x00360210..0x00360450` in this image).
- The first stack value is `0x03BD`.  Original `ios.h` defines LPT1 as
  `0x3BC..0x3BF`; its status port is therefore `0x3BD`.
- Original `printer_io` reads `printer_io_address + 1` through `inb`; original
  `printer.c` owns the IOS LPT adapter and `host/src/nt_lpt.c` owns the NT4
  host-printer status/backend state.

The old T363 observation used SHA-256
`e2a8d705159738b5900ea197b8d7aed64bbac178102e98d2954eabdcc01730f2`.
It cannot establish behavior of this current formal product, even though it
reported progress beyond the same historical owner family.

## Interpretation

The selected workload has not reached the `cmdExec32` interval.  This is not
evidence of lost child arguments, failed `cmdCreateProcess`, or a Base VDM
re-entry failure.  The current predecessor is the complete original CPU40
SoftPC LPT/IOS/host-status cohort.  No port-specific return value, virtual
printer, guest mutation, BOP leaf patch or COMMAND workaround is authorized
by this observation.

## Transfer

Dependency-first recovery transfers runtime work to the current-formal
SoftPC LPT conventional-device package.  That package must reconcile the
current original `printer_.c -> ios.c -> printer.c -> nt_lpt.c` initialization,
status and timeout path before a newly admitted COMMAND observation can test
`54:08` again.
