# T217 S13 — DOS Loader/EXEC Owner-Package Map 001

## Question

After the S12 COMMAND-shell capability seam removed the former SYSINIT
`50:3D` failure, what complete original owner package contains the newly
reached `50:36`, and what is the next source-first implementation unit?

This is a package map, not an authorization to implement the reached
selector.

## Inputs

- `src/opennt/base/mvdm/inc/dossvc.h:92-161` and
  `src/opennt/base/mvdm/dos/dem/demdisp.c:98-168`.
- Guest DOS sources: `dos/v86/doskrnl/bios/sysinit1.asm`,
  `dos/v86/doskrnl/dos/file.asm`, `handle.asm`, `msproc.asm`, and
  `msctrlc.asm`.
- Original DEM sources: `dos/dem/demfile.c`, `demhndl.c`, and `demmisc.c`.
- The current whole-DEM ledger:
  [T199 S37](t199-s37-dem-working-service-ledger-001.md), and the prior
  package maps [T217 S3](t217-s3-sysinit-dos-exec-whole-package-map-001.md),
  [T143 S1](t143-s1-original-dem-command-image-lifecycle-audit-001.md), and
  [T178 S1](t178-s1-normal-dos-return-predecessor-map-001.md).
- Current S12 native evidence:
  [command-shell capability result](t217-s12-command-shell-capability-result-001.md).

## Original Package Boundaries

```text
SYSINIT
  -> DOS $Open / SVC 50:12
  -> handle seek/read/close: 50:00, 50:16, 50:02
  -> NTDOS $Exec allocation, PSP/load image and register construction
  -> 50:36 SVC_DEMENTRYDOSAPP (notification only)
  -> guest far return to DS:SI application entry
  -> target INT 21h/4Ch and DOS teardown: 50:3C
  -> restored COMMAND: 54:11
```

`50:36` is therefore neither a loader nor a process launcher.  It occurs only
after guest `$Exec` has formed the target entry/stack/PDB state, and before the
guest far-returns into that state.  `demEntryDosApp` reads the PDB from `DX`,
optionally calls `VDDCreateUserHook` after the first call, and otherwise
returns `VOID`; it has no success/failure result, guest-memory write, CPU
emulation, or file operation.

## Selector, Owner, ABI, Failure, And Current Disposition

| Members | Original owner | Contract and failure rule | Current profile disposition |
| --- | --- | --- | --- |
| `50:12` | DOS `$Open` → `demOpen` (`file.asm`, `demfile.c`) | Guest `DS:SI` path and open mode; returns DOS-compatible handle or carry/error. | Existing whole DEM namespace provider; declared images are immutable, host namespace follows selected mutation capability. |
| `50:00`, `50:16`, `50:02` | DOS handle layer → `demChgFilePtr`/`demRead`/`demClose` (`handle.asm`, `demhndl.c`) | Split host-handle/offset/buffer protocol; seek/read/close errors remain DOS carry/error results. | Existing shared handle route and checked read transaction; no new leaf provider is admitted. |
| `50:42` | Original DEM table marks it `demNotYetImplemented` | Original slot is not `demRead`; a modern compatibility path needs independent source/ABI justification. | Existing separately admitted FastRead compatibility; package regression must prove it cannot bypass normal handle ownership. |
| guest `$Exec` | `msproc.asm` | Allocates environment/PSP/load block, reads COM/EXE image, checks short reads, constructs `DS:SI`, `AX:DI`, `DX`, `BX`; each allocation/read/format failure returns through original DOS error path. | Guest-owned source-built NTDOS. Adapter and Bochs must not recreate it. Its environment scan is limited to 32 KiB. |
| `50:34`, `50:35`, `50:36-3C` | DEM misc/lifecycle (`demmisc.c`, `demdisp.c`) | Debug symbols, entry notification, DOS dispatch notifications, diagnostics, PDB teardown. `50:36` is a `VOID` no-VDD return; `50:3C` is teardown notification. | Current no-debug/no-VDD lifecycle dispositions from the DEM package ledger. No selector may be treated as a process-launch substitute. |
| `54:11` | COMMAND `cmdReturnExitCode` | Reached only after target termination, DOS teardown and restored COMMAND context; original host implementation mixes directory/redirection/event-thread and next-command machinery. | Not reached or implemented by this package; belongs to the complete COMMAND lifecycle package, not CLI exit-code synthesis. |

## Recovery-Ladder Audit

| Rung | Result |
| --- | --- |
| 1. Direct original source | Retained as the normative guest/DEM owner. Guest DOS executes as source-built artifact; historical DEM/COMMAND C translation units cannot directly compose with the modern x64 process because they require CCPU/SAS, VDD and the NT4 product shell. |
| 2. Smallest seam | Retain the existing package-level DEM namespace/handle provider and its typed checked-memory transactions. Any gap must be repaired at that shared provider boundary, preserving the listed service contracts. |
| 3. External intrusion | Rejected. This path supplies no evidence of missing Bochs CPU, RAM, firmware, PIC or device mechanics. |
| 4. New behavior | Rejected. In particular, no new `50:36` handler, loader, EXEC implementation, DOS filesystem, VDD subsystem, or CLI exit result is justified. |

## Current Evidence And Limits

S12's one native integration run reaches `50:36`, establishing that the
previous SYSINIT command-interpreter failure no longer determines control
flow.  It does not identify a target, prove image-byte placement, prove a
normal target return, or prove COMMAND `54:11`.

Prior source maps establish that `50:36` is pre-target transfer, and that the
first real loading concern is the whole `50:12/00/16/02[/42]` handle/image
lifecycle, not its final notification.  The existing DEM ledger already gives
every member a disposition.  The outstanding proof is whether the *composed*
normal image-loading route maintains one token/handle, offset, byte-count and
checked guest-write contract across that family under the current profile.

## Selected Next Subtask

**T217 S14 — composed DOS image-loader handle-family regression.**

It will not add a BOP handler or run a native trace.  It will source-map the
existing DEM namespace, handle-route and FastRead composition seams to the
original `demfile.c`/`demhndl.c` ABI, then exercise the complete open → seek →
read/fast-read → close family with success and source-shaped failure cases.
The fixture must prove a single provider-owned token/lifetime and checked
guest write, while negative cases prove no host mutation, no bypass via
`50:42`, and no invented EXEC/CLI result.  Only after that family closure may
a single native trace be used for integration ordering.

## Confidence

High for selector ownership, `50:36` notification semantics and the package
division; medium for current native image-load coverage because S12 observes
only the arrival at `50:36`, not every preceding ABI field.
