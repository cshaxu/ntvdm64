# T95 S6 r26 BIOS `15h` trace

## Result

The immutable observation
`artifacts/analysis/t95-s6-r26-bios15-020-20260811-001` proves that the
adapter consumed exactly the reached `C4 C4 15` / `AH=88h` operation.  It is
not a general INT 15 or BIOS implementation.

The linked binary is
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r26-bios15/ntdos64-native-startup-transaction.exe`,
SHA-256 `49875EBEDE3CEF75A3745C34C1A7DBC87F1644D0A589520E2E29662B0A2B3ABC`.

## Evidence sequence

| Order | Observed boundary | Classification |
| ---: | --- | --- |
| 1--7 | Prior `50:11`, `12`, `50:3b`, `50:0f`, `50:1b`, `50:32`, `54:05`, `50:46` prefix | Previously admitted, source-derived transitions. |
| 8 | `15` at `8DC8:0655` | `BIOS_CASSETTE_IO`; the new AH=`88h` profile result resumes at `0658`. No unmatched-UD line follows this record. |
| 9 | `50:0d` at `8DC8:0667` | `SVC_DEMGETBOOTDRIVE`; listener-only and followed by the original unmatched `#UD`. This is the next candidate. |
| 10 | `54:04` at `9346:6774` | `SVC_CMDGETCURDIR`; observed but still listener-only. It is downstream of the unimplemented boot-drive request. |

The observer preserves the exact source-built NTIO, NTDOS and COMMAND
identities, source-built NTDOS RAM transaction, `megs: 4` profile and passive
catalogue listener.  It records one generic write commit, eleven BOP
observations and no broad interceptor.  Its 30-second watchdog termination is
retained as downstream fail-closed evidence, not treated as a successful boot.

## Source interpretation

`doskrnl/bios/sysinit1.asm:1005-1020` loads AH=`88h`, performs `BOP 15h`, and
stores AX as extended-memory KiB.  `softpc.new/base/inc/bios.h:43` and
`softpc.new/base/bios/bios.c:162` identify selector `15h` as the historical
`cassette_io` owner.  The accepted `megs: 4` profile makes the returned
post-1MiB capacity `0C00h` KiB.  The adapter therefore performs only a typed
AX/RIP result; it neither reads CMOS nor imports the historical SoftPC BIOS
implementation.

`dossvc.h:105` defines the next selector/service pair as
`SVC_DEMGETBOOTDRIVE` (`50:0d`).  Its original dispatcher and NTDOS call site
must be audited before implementation.  In particular, it must be decided
from source whether the adapter's fixed startup profile owns an explicit boot
drive value or whether this requires a host/CLI capability; it must not be
guessed from the later current-directory service.
