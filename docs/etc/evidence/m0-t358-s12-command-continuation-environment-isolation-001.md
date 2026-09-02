# M0 T358 S12 — COMMAND continuation observer environment isolation

## Scope

The optional continuation observer is diagnostic-only.  Its report-path
configuration must not alter the environment imported by original
`cmdenv.c::cmdGetInitEnvironment`.

## Binding

`app/entry.c` calls `mvdm_softpc_capture_command_continuation_report_path()`
before entering original MVDM startup.  The helper captures the bounded
`MVDM_COMMAND_CONTINUATION_REPORT_PATH` value and immediately removes only
that variable from the process environment.  The later observer uses its
private copy; it does not enumerate environment variables.

This preserves the original COMMAND environment when observation is disabled
and when it is enabled.  It changes neither guest bytes, COMMAND records,
BOP disposition, media, nor ordinary report variables.

## Verification

The focused continuation fixture proves all three required cases:

1. no configured path produces no record;
2. a configured path is absent from the inherited environment after capture;
3. the private captured path produces the fixed-width continuation record.

The formal CPU40/x86 Ninja target links with the declaration visible at the
application call site.  The next frozen product observation is separately
admitted as S13; S12 itself runs no product workload.
