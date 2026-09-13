# M0 T407 S9 — Fast Kernel-Carrier Retirement

## Decision

The standalone product excludes every NT-kernel-dependent FastWOW, FastBOP
and FastStack mechanism. It retains the original non-fast WOW callback path
(`host_simulate`) and the original failed `GetFastBopAddress` service.

## Removed selected code

| Surface | Removed | Reason |
| --- | ---: | --- |
| `dpmi32/modesw.c` | 109 lines | Synthetic PM/V86 dual TSS, private GDT slots, I/O bitmap and TR installation emulated an NT kernel task carrier. |
| `dpmi32/dpmi32.c` | 19 lines | The 4 KiB guest FastWOW TEB/TD projection had no selected reader. |
| `dpmi32/{data.c,dpmi32p.h}` | 9 lines | Sole state declarations for the removed carriers. |

Net selected product diff: **137 lines deleted**, with no new behavior.

## Retained original non-fast behavior

- Selected `dpmimscr.c:27` sets `BX`, `DX` and `ES` to zero for BOP `53:03`
  (`GetFastBopAddress`), the original non-fast failure contract.
- The selected provider uses `FASTBOPPING=0`; original `wcall16.c` calls
  `host_simulate`, not `FastWOWCallbackCall`.
- Original mirror source and guest media remain unmodified.

## Verification

| Check | Result |
| --- | --- |
| Static symbol sweep | No remaining FastWOW projection, synthetic task-state or private TSS-install symbol in selected source. |
| Fresh formal x86 build | `build/M0-T407/S9/r001-fast-carrier-retirement`, generated with Node `v22.22.1`. |
| Link and fixture | `original-softpc-process.exe` linked; `cpu40-descriptor-domain-fixture.exe` exited `0`. |
| Staged executable | `O:\ntvdm64\ntvdm32.exe`, 3,233,792 bytes, SHA-256 `da086dff5cd6c8845517d8a4970ffa141e864e29f5f28b5bd15b4c6e127d2324`. |
| Runtime | Isolated `MEM.EXE` and `COMMAND.COM /c exit` both exited `0`; raw stdout/stderr are under `O:\ntvdm64\logs\m0-t407-s9-*`. |

The direct-console invocation did not return an observable exit status and is
not counted. The accepted runtime rows use `Start-Process -Wait -PassThru`
with a 30-second bound.

## Limitation

This does not accept `WRITE.EXE`, DOSX protected-mode entry or IDT lifecycle.
Those remain T407 work and must not reintroduce Fast carriers as a shortcut.
