# M0 T271 S3 P6 — COMMAND VDM API facade recovery

## Source contract

OpenNT `public/internal/base/inc/vdmapi.h` defines `VDMINFO`, the
`INCREMENT_REENTER_COUNT` / `DECREMENT_REENTER_COUNT` transitions, and the
same-named `GetNextVDMCommand` and `SetVDMCurrentDirectories` declarations.
`base/win32/client/vdm.c` implements them through the historical BaseSrv/CSR
product broker. Reached COMMAND code calls these names while it creates and
returns from a host child.

## Recovery

`adapter-win32/facade/opennt_vdm_api_facade.{h,c}` owns the reached
same-shaped public facade. The header retains the original names, flags,
calling convention, field order and result type. `opennt-bop` no longer owns
the API definitions.

The standalone single-session provider does not recreate BaseSrv/CSR. Instead
it binds neutral copied `session_input` state only for a historical caller.
The source-shaped COMMAND worker binds that input before it calls imported
`cmdCreateProcess` and unbinds it after the body returns. The original
worker's increment/decrement calls consequently retain their order and
observable success/failure without carrying an active BOP call, guest pointer,
raw host handle, selector or service into the worker.

This is `WIN32-DIV-009` in `src/adapter-win32/README.md`. The historical
multi-VDM/BaseSrv broker remains deferred and is not claimed as recovered.

## Verification

On 2026-08-25, formal Ninja root `build/M0-T271-S3/r001` rebuilt affected
libraries. These executables exited zero outside the sandbox:

- `t231-s7-command-get-next-direct-fixture.exe`;
- `t234-s2-command-dynamic-environment-fixture.exe`;
- `t231-s10-command-native-session-fixture.exe`; and
- `t236-s1-command-local-child-fixture.exe`.

The last fixture proves the original child worker observes a balanced
re-entry count and a peak of one. Its previous failure exposed the
thread-local binding gap; this worker-scoped bind/unbind closes it without
changing the source dispatcher/provider contract.
