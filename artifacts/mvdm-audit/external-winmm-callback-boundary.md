# External WINMM callback layout: first cross-inventory reconciliation

WOW32 audit row `SRC:829a694d21ac1a82871c` is OpenNT-4.5
`nt/private/windows/media/winmm/mmwowcb.h`, SHA-256
`8C083D99F880CDB958C5DDCCDBFA3DB703E9E287CAB6C9C641631C13B2E50714`.
It describes the sixteen-slot `CALLBACK_DATA` queue, producer/consumer counts
and IRQ constants. It is not itself a callback implementation.

The distinct MVDM inventory path `inc/wowmmcb.h` retains the corresponding
layout and declarations. Its SHA-256 is
`792D12C29A5913EE4DD9D46F3173F4E7AC8F1CD88CFA91272268BD8BCAFB8E8C`;
the raw comparison adds the registered DIV-294 comment and a cdecl
`call_ica_hw_interrupt` declaration. No claim of byte identity or permission
to overwrite either file follows. The external path and MVDM path must
remain separately accountable.

Original `wow16/mmsystem/thunks.h`, `init.c` and `stack.asm` show the guest
consumer and queue registration; actual shipped guest bytes were not altered.
T466 S3 (`docs/etc/evidence/m0-t466-s3-async-multimedia-owner-audit.md`)
records that the current x86 native boundary resolves eleven original
private WINMM exports, including `NotifyCallbackData`, and explicitly does
not prove real asynchronous callbacks, ring integrity, task exit or device
completion. A separate T466 synchronous witness exercises `tid32Message`,
not this queue. The modern OS WINMM is the producer owner; the external NT4
header alone does not prove its implementation/layout is identical.

Disposition remains pending: identify the actual guest layout and host
compatibility contract for the target versions, then verify asynchronous
queue delivery, wrap/full behavior, unregister and task-loss teardown with
a real guest and device or an honestly bounded host fixture. Do not import
the NT4 WINMM subsystem, declare this header fully restored, or mark the
whole multimedia capability complete based on static layout or export lookup.
