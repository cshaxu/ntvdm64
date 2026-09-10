# T217 S4 — DEM Normal-File Package Integration Result

## Question

Does the installed whole DEM normal-file/handle provider accept the second
SYSINIT `SVC_DEMOPEN`, and if not, which owner package—not which leaf BOP—must
be recovered next?

## Procedure

A default-off fixture observer was enabled only for the source-built full
composition. It copies the second already accepted `C4 C4 50 12` event and its
typed outcome after the existing composition returns. It does not read guest
memory, retain a guest pointer, expose a pathname, select a provider, alter a
result, or alter Bochs/mantle behavior.

The fresh MSVC x64 `/MT` run is retained under
`artifacts/build/t217-s4-dem-open-r1`. The full composition rebuilt, linked,
and ran to the pre-existing controlled terminal.

## Observation

```text
t217-s4 dem-open seen=3 captured=1 state=0034:00006bdf
  ds-si=00a7:03d2 eax=00000000 ebx=00000000
  accepted=1 disposition=1 resume=6be3
  gpr16-mask=00000001 ax=0002 flags-mask=00000001 flags-value=00000001
```

`disposition=1` is typed resume. The only written general register is AX, with
`AX=2`; the written carry flag is set. OpenNT DOS therefore receives its normal
DOS `file not found` result and branches from `sysinit1.asm:1581-1584` to
`comerr`, then `SVC_DEMEXITVDM`. This matches the observed direct terminal and
does not implicate EXEC, CPU execution, memory allocation, or a missing Bochs
device.

## Interpretation

The existing provider is selected and returns a well-formed failure; the
failure is not a dispatcher decline. Its namespace path code returns a
successful immutable declared image only when the resolved current
drive/canonical path matches the installed declared-image namespace. Since an
immutable COMMAND image would have returned a token and carry clear, this run
proves that the actual SYSINIT Open identity did not match that declaration at
the provider boundary.

The observer intentionally does not expose the pathname bytes. It therefore
does not distinguish a default-drive/CWD mismatch, a canonicalization mismatch,
or a declaration attachment mismatch. Those are one owner package: DEM
declared-image namespace identity, including path resolution, drive/CWD,
declaration installation, immutable-open result, and normal handle lifecycle.

## Recovery-Ladder Disposition

| Rung | Result |
| --- | --- |
| Original source | `file.asm`/`handle.asm` and `demfile.c` remain the normative DOS/DEM contract; their CCPU/SAS host composition cannot directly link into the x64 CLI process. |
| Smallest seam | Admit a whole DEM declared-image namespace identity audit and repair, preserving normal Open/seek/read/close failure behavior. It must use the existing source-derived provider rather than add a new BOP handler. |
| External intrusion | Rejected: typed provider failure proves Bochs mechanics are not the boundary. |
| New behavior | Rejected: the original owner and current source-derived whole provider exist. |

## Next Work

The next subtask first maps the actual profile declaration, session CWD/default
drive, path canonicalization and namespace installation into one source/ABI/
failure table. It then repairs the complete declared-image identity seam and
regresses both focused declared-image operations and the native SYSINIT
Open→seek/read/close sequence. It must not add an EXEC implementation, a
special `50:12` handler, or a synthetic terminal success.
