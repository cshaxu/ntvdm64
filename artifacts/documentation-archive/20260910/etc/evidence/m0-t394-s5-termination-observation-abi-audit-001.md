# M0 T394 S5 — termination, observation and original-host ABI audit

## Result

`mvdm_softpc_termination.{h,c}` is a mixed default-off observation carrier,
not a relocatable component. No production source or call site changes in S5.

| Symbol/state cohort | Direct path family | Final owner / follow-up |
| --- | --- | --- |
| `terminate_current_session`, TLS origin | termination calls from host/DEM/COMMAND paths | `session`, S6 |
| continuation/stream report-path capture; main return/unhandled exception | `app/entry.c`, product entry/exception boundary | `app`, S6 (app capture) and S8 (container probes) |
| CPU interrupt/return, keyboard, ICA/PIC, SAS store/direct-RAM observations | CCPU, keymouse, system/ICA and SAS paths | named `softpc` machine-observation interface, S7 |
| COMMAND call/continuation/result/environment/stub observations | COMMAND BOP/service paths | `basesrv` service-local observation, S8 |
| DEM open/create/chmod/read/write/seek/config observations | DEM provider paths | DEM service owner, S8 |

All observation paths remain default-off; this map does not authorize an
execution, interrupt, input, timing, memory or report-semantic change.

## `error_abi.h`

The current declaration bridge documents the split exactly: host
`softpc.new/host/inc/error.h` declares table use but lacks `ERROR_STRUCT` and
the `EH`/`EV` enums; compatible spelling/layout definitions are in original
`softpc.new/base/inc/error.h`. The header is therefore an original-host ABI
carrier, with `opennt-abi` the correct final owner in S8. It is not a Win32
platform feature. Its force-include/formal graph behavior must remain
unchanged when moved.

## Rejections and disposition

`monitor` remains restricted to historical VDM monitor/TIB/DPMI control and
cannot own generic reporting. `win32` is likewise rejected as a blanket ABI or
diagnostics destination. S6--S8 may extract only the listed symbol groups,
with one definition/state owner and no generic logging family.
