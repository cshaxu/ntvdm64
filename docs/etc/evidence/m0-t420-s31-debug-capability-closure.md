# M0 T420 S31 — SoftPC debug capability closure

## Decision

The selected product is the original OpenNT `/DPROD` profile. It does not offer
an interactive Yoda shell, instruction trace, backtrace, EGA dump, profiling,
or PIG telemetry. This is a complete source-level profile exclusion, not a
partial debugger implementation and not a missing worker provider.

All eight sources named by the original `softpc.new/base/debug/sources`
manifest are present in the formal x86 archive `original-softpc-base-trace.lib`
and are byte-identical to the pinned OpenNT copies:

| Source | SHA-256 | `/DPROD` disposition |
| --- | --- | --- |
| `debuggng.c` | `C483E5EF755F537C7197FC7CE929ED3837B1227F57F6B874B5C8AB46B15045AD` | `!PROD`/`EGG`; no selected body. |
| `ega_dump.c` | `18C7205169A62E40E80122DEBA5645F1CD7CE0D760EA71D95E9688A5A1DA98AA` | `EGG && !PROD`; no selected body. |
| `btrace.c` | `5A19A8CF73BD5C06C47BF3292189C8989BC970933BB4968336580F4BE0028E3F` | All functional bodies are `!PROD`. |
| `trace.c` | `B776BEC19F7C7C615684711B915C7C7D1C56A124D37E14FC30A575681C83B2E3` | `trace()` is the original no-op; `trace_init()` does not bind `trace_file`. |
| `profile.c` | `93A1891B0A27B336CCECD99A4B198E1D26F769FE31ED83023DF52178E3364A51` | Functional body requires unselected `PROFILE`; original fallback is inert. |
| `pigyoda.c` | `EDE413A201FC0D4115B5BB0CD50AAD3DD34D93A61DC734DF49BFD464132EE7F2` | Requires unselected `PIG`. |
| `yoda.c` | `444F1350D7C50B270652D7B062A375536E327952EBB5BA83937FBA090DB3C817` | Original product `force_yoda()` stub; interactive body requires `YODA` and non-product selection. |
| `dasm.c` | `6F90249D51EEE69AF65F59DE138F915DC05AF3A67B7BB529EA33596C04888B9B` | All disassembler code is `!PROD`. |

`base/support/main.c` calls `trace_init()` only under the same original
`!PROD || HUNTER` gate. The formal `ntvdm.exe.map` resolves `_force_yoda` to
the original `ccpu386/stubs.obj`; `_trace_file` is only a zero-initialized
common symbol. No selected call reaches an interactive debugger transport.

## One release-profile repair

`c_xcptn.c` retained an original, externally controlled
`CCPU_SHOW_EXCEPTIONS` macro. In a product build that macro could set
`show_exceptions` despite the original `trace_init()` intentionally leaving
`trace_file` unbound, and then reach `fprintf(trace_file, ...)`. This is a
latent original release-configuration fault, not a reason to initialize or
recreate the non-product debugger.

`MVDM-HOST-DIV-271` makes only this environment switch inert under `PROD`.
The full original macro remains shaped in non-product builds; there is no new
trace body, output channel, overlay, or adapter. The source delta is eight
added lines in `c_xcptn.c`.

## Verification

- `BuildCcpuExceptionProfile.mjs` verifies the pinned NTVDMx64 provenance hash
  and the exact five retained DIV-268 corrections. Its source-shaped fixture
  at `build/M0-T420/S31/exception-profile-r6` passed all 27 original exception
  cases; its original-spelling control failed 101 assertions as required.
- The same fixture compiles the actual current unit with `PROD`, supplies an
  apparent `CCPU_SHOW_EXCEPTIONS=1`, deliberately leaves `trace_file=NULL`,
  and emits `PASS PROD CCPU_SHOW_EXCEPTIONS is inert without trace_file`.
- `formal-x86-r1` rebuilt the x86 five-artifact product graph plus
  `ccpu-thread-lifecycle-test.exe`; the lifecycle witness emitted
  `CCPU thread lifecycle normal-and-abnormal OK`. Its `c_xcptn.obj` contains
  no `CCPU_SHOW_EXCEPTIONS` text, while the map retains the original product
  `force_yoda` stub described above.
- The matching five artifacts were deployed to `O:\winnt` with hashes in
  `O:\winnt\logs\m0-t420-s31-formal-artifacts-r1.json`. With the actual
  environment variable set, the existing guest `FAULT.EXE` passed in
  `m0-t420-s31-prod-exception-r4-FAULT.txt`.
- The existing text-gated COMMAND/MEM/EDIT matrix passed all 17 cases in
  `m0-t420-s31-command-r1-summary.json`; later r3/r4 split summaries repeated
  direct/nested MEM, `COMMAND /c`, guest exit, and EDIT routes. Their observers
  reject absent guest text and unexpected command-resolution errors.

## Result

S31 is closed: all eight manifest units are retained as original mirror
sources, their release profile is explicit, and the only necessary repair is
the bounded eight-line release gate outside that mirror package. No debug
capability is silently claimed and no non-product service was introduced.
