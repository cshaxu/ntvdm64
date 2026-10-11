# M0 T443 S12 — COMMAND service → Run16 routing evidence

## Scope

`DOSONLY=0` only.  This record covers the project-owned BOP 54:08 adapter
between original COMMAND and its native child process.  It does not change
guest `COMMAND.COM`, any `COMSPEC` environment value, or ordinary native
application shell behavior.

## Source result

- Original `cmdExec` still creates its original `COMSPEC /c <tail>` text.
  Before that text replaces the captured guest command, the minimal registered
  mirror hook records that this was COMMAND's execute-service branch.
- Original `cmdExecComspec32` records its no-argument interactive-service
  branch.
- The project-owned adapter reads that branch marker and uses
  `common_product_path_a(L"system32\\run16.exe", ...)`, which derives the
  product root from the loaded NTVDM module.  It launches either
  `run16.exe <original-tail>` or `run16.exe command`.  AH=1 replaces only the
  COMSPEC executable; it does not add another `COMMAND /c` wrapper.
- No code writes `COMSPEC`; no component path uses `O:\\winnt` or
  `C:\\Windows` as a product-root literal.

The new state is bounded to the already session-owned native-child snapshot.
It is cleared by the existing finish/abort path.

## Build and runtime witnesses

- Corrected full x86 graph `build/M0-T443/S12/r005/x86` completed all 528 Ninja edges,
  including `ntvdm.exe` and `nthook32.dll`.
- A headless launch is not a valid VDM witness: both pre-change and candidate
  packages return `87` without a Console.  The temporary deep staging root
  also returned `1067` even after restoring its two changed binaries; it was
  rejected rather than treated as a product regression.
- At the actual product root in a real minimized Console, candidate
  `run16 command/c ver` returned `0` and printed `MS-DOS Version 5.00.500`.
- The nested candidate `run16 command/c command/c ver` likewise returned `0`
  and printed that DOS version.
- During a retained interactive `run16 command`, process identity was:
  `test cmd → O:\\winnt\\system32\\run16.exe → ntsrv.exe → ntcon.exe + ntvdm.exe`.
  No additional host `cmd.exe` existed below Run16/NTVDM.  After closing the
  owned Run16, its frontend and worker retired, then NTSRV completed its
  existing ten-second empty retirement.

## Publication

The full ten-image manifest was synchronized from the tested candidate to
`assets/release/` and `O:\\winnt\\system32`.  Hash equality was verified for
all ten files.  The changed image identities are:

| Image | SHA-256 |
| --- | --- |
| `ntvdm.exe` | `9FE49D8775F5B9F02D8D96012C845E4A8022AE67B06503CC7538848CE2378355` |
| `nthook32.dll` | `5A3AD7B0F5CBDB0804BB7F269949F56B59B469241E2647EDFC6BF04CE1084FEA` |

The other eight images are unchanged, but were republished in the same
manifested package as required.

## Remaining acceptance boundary

The prior r003 interactive observation and its "no extra host CMD" conclusion
applied to the erroneous `command /c` wrapper and is superseded by r005.
The r005 source and full x86 build prove the wrapper is absent; an attempted
headless observer launch did not retain a Console report and is not counted as
an execution witness. Owner manual acceptance remains
the full interactive Windows Terminal matrix: triple nesting, ordered exits,
input/output ownership, quoting, pipe/redirection, explicit native CMD,
batch, and failure paths.
