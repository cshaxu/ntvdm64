# M0 T358 S10 — original COMMAND continuation observer

## Delivered boundary

The existing original `cmddisp.c::CmdDispatch` table boundary now emits a
separate continuation record only for the already-decoded
`SVC_CMDGETNEXTCMD` (`54:01`) entry.  It emits one record immediately before
and one immediately after the unchanged original table call.

Each record contains only copied scalar values:

- stage (`0` entry or `1` return);
- `CS:IP`, `AX`, `BX`, and carry;
- original COMMAND `IsFirstCall` and `IsRepeatCall`; and
- a locked, read-only snapshot of the bound Base VDM DOS-record state.

The new `base_vdm_local_observe_dos_record_state` accessor returns a scalar
or `UINT32_MAX` when no valid bound record exists.  It neither dispatches,
wakes, clears, copies, publishes, nor retains a Base VDM record.

## Default-off contract

`mvdm_softpc_record_command_continuation` has its own
`MVDM_COMMAND_CONTINUATION_REPORT_PATH` channel.  When absent, it does no
I/O at all.  It does not reuse the older generic COMMAND recorder's stderr
channel, so disabled output is byte-for-byte unchanged.  When enabled, its
fixed record is:

```
MVDM-CMD-CONT svc=01 stage=N cs=XXXX ip=XXXX ax=XXXX bx=XXXX cf=N first=N repeat=N dos-state=XXXXXXXX
```

No command text, guest pointer, native pointer, Base VDM record pointer,
host handle, BOP result, command record, guest byte, CPU state, session state
or worker behavior is changed.

## Verification

- `tests/adapter-mvdm-host-out/softpc/t358_s10_command_continuation_observation_fixture.c`
  proves both default-off no-file behavior and the enabled fixed scalar
  format.
- The focused x86 fixture was compiled with the current adapter source and
  exited successfully.
- The formal selected CPU40/x86 target
  `build/M0-T359/S2/formal-x86:original-softpc-process.exe` rebuilt and
  linked successfully after the source change.

## Disposition

S10 does not run a guest workload.  S11 alone may run the frozen valid-root
container with this report channel and determine whether the second `54:01`
is source-shaped parent reacquisition after `/C EXIT`, or whether an earlier
owner boundary remains.
