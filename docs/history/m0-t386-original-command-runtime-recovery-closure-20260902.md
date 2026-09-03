# M0 T386 — Original COMMAND runtime recovery closure

## Result

Closed. The selected immutable OpenNT `COMMAND.COM` now executes a real DOS
built-in command in the CPU40/x86 SoftPC product and produces an observable
original DOS redirection result. No guest image was modified.

## Proven path

- The selected `COMMAND.COM` remains SHA-256
  `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
- The original Base-client `BaseCreateVDMEnvironment` projection supplies the
  prelaunch environment while preserving the source-owned environment retry
  and DOS arena lifecycle.
- The fixed console-owning, non-debug container ran the unchanged guest
  command `echo M0T386 > M0387.TXT` from `O:\\ntvdm64` and exited zero.
- It created `O:\\ntvdm64\\M0387.TXT` with exact bytes
  `M0T386 <space>\\r\\n`.
- The BOP trace reached original `50:03` create, then `50:43`; the latter
  declined the unavailable NT4 kernel fast-write interception with carry, so
  unchanged NTDOS took original `50:1E demWrite`, then `50:02` close.
- The formal CPU40/x86 Ninja target linked after the change. The resulting,
  actually observed candidate is published as `build/output/ntvdm64-0237.exe`
  (SHA-256 `1CC9DF57CFEF1296FB3512F9C391945582DEDBAA5577E4106A6AE2E21406A95D`).

## Retained implementation boundary

`MVDM-HOST-DIV-204` is the smallest source-shaped modern disposition of
`SVC_DEMFASTWRITE`: it returns carry rather than faking a fast write, allowing
the guest's original normal-write fallback. `MVDM-HOST-DIV-205` is default-off
bounded diagnostic observation used to verify the create/write contract; it
retains neither a guest pointer nor payload and changes no provider result.

## Scope boundary

This closure proves pure-DOS COMMAND built-in execution and redirection only.
It does not prove external `.COM`/MZ execution, `cmdExec32` native-child
lifecycle, Redirector, DPMI, WOW/Win16, cross-process broker behavior, or x64
runtime. The next candidate is the ordered original COMMAND `cmdExec32`
stage-to-return recovery package in `docs/QUEUE.md`.

## Evidence

- [T386 plan](../etc/operations/m0-t386-original-command-runtime-recovery-001.md)
- [Base VDM environment recovery](../etc/evidence/m0-t386-s3-original-base-vdm-environment-recovery-001.md)
- [COMMAND runtime observation](../etc/evidence/m0-t386-s3-original-command-continuation-disposition-001.md)
