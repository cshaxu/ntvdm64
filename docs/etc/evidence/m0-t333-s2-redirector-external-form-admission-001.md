# M0 T333 S2 — Redirector external-form admission

## Question

Which immediate external interfaces used by the original Redirector can enter
the current non-invasive CPU40 composition, and which must retain an exact
unavailable result or move to a named owner package?

## Inputs and procedure

Read every external include/call family reached from the original provider
sources (`vrinit`, `vrmisc`, `vrnmpipe`, `vrmslot`, `vrnetapi`, `vrremote`,
`vrnetb`, `vrdlc5c`, and `vrdlcpst`), then compared those forms with the
current Redir, SoftPC, Win32 and session bindings. The assessment preserves
the source-first hierarchy: public APIs can be a binding behind an original
body, never a substitute provider.

## Result

- Public named-pipe and mailslot calls are directly composable *behind* their
  original bodies. Their guest buffers and handles still use the existing
  bounded span and opaque-identity bindings.
- Source-local Sim32/ICA mechanics remain CPU40/SoftPC forms. S3 may only
  admit paths that do not require an asynchronous hardware completion;
  interrupt-completion ownership remains S4.
- Public local NetAPI calls and the selected original `ntstatus.c` helper are
  candidates for source-body recovery, but their full result layouts and error
  mapping are an S5 task. This is not a claim that remote/RAP behavior works.
- `RxpTransactSmb`, `Xs*` worker/RAP marshalling, NetBIOS completion, VDD
  window hooks and DLCAPI/NtAcsLan are distinct historical product/driver
  boundaries. They cannot be inferred from a similar modern API. The exact
  provider policy remains in the original mirror; future packages either bind
  a finite same-shaped form or retain the original failure/unavailable path.

## Conclusion

S2 is closed. S3 has a bounded first cohort: original lifecycle plus
synchronous named-pipe/local broker code through only public Win32 and the
already-existing mapping/identity/thread forms. It excludes async completion,
RAP, NetBIOS, DLC and VDD. No source body, dispatch row, adapter behavior or
formal product selection changed in S2.
