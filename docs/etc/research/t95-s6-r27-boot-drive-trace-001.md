# T95 S6 r27 `SVC_DEMGETBOOTDRIVE` trace

## Result

The r27 observation proves that the adapter consumed the exact reached
`C4 C4 50 0D` boundary as the original OpenNT noninvasive fallback: it
preserved AH, supplied AL=`03h` (`C:`), and resumed at `RIP+4`.

The linked binary is
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r27-boot-drive/ntdos64-native-startup-transaction.exe`,
SHA-256 `A57580CA0FA09B9105C2B53C0403C57FBB8E96C42958C2E4C9D5759D91178A8B`.
The immutable observation is
`artifacts/analysis/t95-s6-r27-boot-drive-021-20260811-001`.

## Evidence sequence

The retained log contains the established source-built transaction and BOP
prefix through BIOS `15h`, then:

| Boundary | Evidence | Classification |
| --- | --- | --- |
| `50:0d` at `8DC8:0667` | No unmatched-UD follows. | `demGetBootDrive` consumed as its source-defined registry-unavailable/non-fixed fallback (`C:`). |
| `54:04` at `9346:6774` | Followed by the original unmatched-UD. | `SVC_CMDGETCURDIR`; next real capability boundary. |
| `5a` at `9346:350b` and later records | Downstream of the unimplemented current-directory path. | Listener evidence only; not a new implementation order. |

The observer recorded 14 BOP identities, exactly one source-built NTDOS RAM
transaction, no broad interceptor, and a 30-second watchdog terminal outcome.
It does not prove a DOS boot or current-directory behavior.

## Why `54:04` must not receive a one-off result

`cmdmisc.c:517-568` defines `cmdGetCurrentDir`: it reads the requested drive
from AL, validates it using physical/host drive type, obtains or creates the
per-drive `=?X:` environment value, writes its directory string to guest
`DS:SI`, and defines two error results.  NTDOS `macro.asm:394-421` calls it to
synchronize its CDS/NetCDS state and relies on CF.

The current r27 profile contains an all-zero static drive inventory and has no
installed CLI-derived host namespace.  Returning `C:\\` would falsely claim a
DOS-visible host root; returning a fabricated error would bypass the approved
host-capability route without establishing that the guest can make progress.
Neither is admitted.  The required next unit is the already-designed cohesive
current-directory capability: consume only an adapter-session frozen,
CLI-filtered namespace root; use a bounded guest output transaction; and
retain the source-defined success/error/CF rules.  It must not expose a host
path or turn a guest request into a rescan.
