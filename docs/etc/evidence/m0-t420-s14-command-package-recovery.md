# M0 T420 S14 COMMAND Package Recovery

## Scope and source-first result

S14 closes the selected `src/mvdm/dos/command` package.  Its reached
COMMAND EXEC, native-child, environment, return, error and redirection
interfaces were compared against the pinned OpenNT source.

The recovery removes two x86-only adapter carriers which did not adapt an
external boundary:

| Carrier | Former adapter | Restored original shape | Reason it is removable |
| --- | --- | --- | --- |
| COMMAND redirection record | `mvdm_command_redirection.{c,h}` | `PREDIRCOMPLETE_INFO` is passed as the original fixed DWORD through COMMAND's own process | No process change, asynchronous hand-off, guest address, or lifetime change occurred. |
| VDMREDIR named-pipe handle | `mvdm_redirector_handle.{c,h}` plus shadow `vdmredir.h` | original `HANDLE_FROM_WORDS` macro from `src/mvdm/inc/vdmredir.h` | BP:BX describes a handle in the same worker address space; the wrapper only split and rejoined it and incorrectly rejected zero. |

The first recovery restores direct original carrier expressions in `cmdexec.c`,
`cmdmisc.c` and `cmdredir.c`; the second lets `vrnmpipe.c` consume its original
header again.  The obsolete focused redirector wrapper fixture and generator
were removed.  `redir/include` remains in the build include list because it
also owns the still-required async and guest-copy interfaces.

The selected COMMAND files `cmdkeyb.c` and `cmddisp.c` were additionally
returned to their pinned source spelling/format.  The remaining COMMAND-side
bindings are not empty carriers: guest-state operations require a bounded
guest-memory lease, native-child execution snapshots data for a detached
worker, and original cdecl thread entries require the WINAPI boundary bridge.

## Similar-logic sweep

The audit searched all product adapters, BaseSrv, launcher and selected MVDM
code for pointer-width casts, split-word reassembly and identity wrappers.
Only the two entries above meet every removal condition.  The other classes
are retained because they carry real state:

| Area | Why it is not an empty wrapper |
| --- | --- |
| BaseSrv receipt tables | Cross-process resource IDs have authenticated generation, role and revocation lifecycle. |
| Redirector async and guest-copy | Guest locations are leased across asynchronous I/O and copied before completion. |
| `run16` task cleanup | It passes an original Base API task value within the launching process. |
| Original MVDM split-word arithmetic | These are guest ABI values, selectors, offsets, file positions or descriptors; they are not host-pointer adapters. |

Accordingly, this sweep removes **two production adapter implementations,
three adapter headers/shadow headers, two obsolete test/build helpers, and
the associated build/README registrations**.  The tracked change is 17 added
lines and 210 removed lines, for a net reduction of 193 lines.  No mirror file
was added.

## Verification

- Fresh x86 formal graph: `build/M0-T420/S14/formal-x86-001`.
- `product-programs` rebuilt `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  `dtmgr.exe` and `VDMREDIR.dll`; `Verify-VdmTibStorage.mjs` passed with
  owner `mvdm_vdm_tib.obj`, 4208 bytes and no overlap.
- Focused original COMMAND redirection fixture rebuilt and exited `0`.
- Automated established workloads previously passed from the S14 build
  procedure: direct `MEM.EXE`, `COMMAND.COM /C MEM.EXE`, and EDIT startup.
- Owner acceptance for the full interactive route was recorded on
  2026-09-17 as “都正常”: start COMMAND through `run16`, run nested COMMAND,
  MEM and EDIT, return to the usable parent prompt, then run MEM normally.

The final rebuilt artifacts await replacement into `O:\winnt` until the
owner's currently running validated session exits; no live worker was
terminated merely to replace files.
