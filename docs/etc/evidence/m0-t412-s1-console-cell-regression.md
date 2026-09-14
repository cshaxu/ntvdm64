# M0 T412 S1 — EDIT text-cell regression diagnosis

## Result and provenance

The owner requested continued investigation of the rebuilt EDIT regression.
The old publication is the T410 S1 product recorded in
[T410 S1 evidence](m0-t410-s1-native-console-window-retirement.md): SHA-256
`C0771B414F93B1F1644765447F89439860E23E6F9FB67A1319222955B267652F`.
It is not a build of the present source. Its earlier build directory has
since been reused; the saved EXE hash, not that directory's current contents,
identifies the comparison artifact.

Commit `64acfb123` changed `RegisterConsoleVDM` in
`src/adapter-mvdm-host-out/win32/source/console_compat.c` from the selected
non-MONITOR four-byte allocation to unconditional two-byte cells. The stated
reason confused native x86 architecture with the retired MONITOR profile.

The selected original `nt_cga.c:73-81` defines `TEXT_INCVAL` as four without
MONITOR. Its `nt_text` writer uses that value for destination offsets, copy
lengths and row strides (lines 321-360). This file has no text diff from
`O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/nt_cga.c`, pinned
OpenNT revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`. The formal x86
graph selects CCPU40/C_VID without MONITOR. Original comments explain the
four-byte layout as VGA interleaving; it is not a native pointer width.

The adapter allocates `columns * rows * cell_bytes`, then its presentation
reader advances by that same cell size. Thus the original writer emits
four-byte cells while the adapter interprets two-byte cells. Reserved bytes
become apparent characters and row offsets disagree. The smaller allocation
also fails the full registered 80-by-50 writer-capacity contract; this run
does not prove a particular out-of-bounds write occurred.

## Single-variable real-program comparison

An ignored diagnostic script at
`build/M0-T412/S1/event-profile/probe-text-cell.mjs` generates a copy of the
current adapter translation unit with exactly one executable token changed:
the allocation argument `2u` becomes `4u`. Production source stays unchanged.
It derives `cell4.ninja` from the formal graph, gives the changed object,
library and executable distinct names, and links all other current objects
unchanged, including the repaired HALT consumer and current C-VID calibration.
Build returned zero; candidate SHA-256 is
`26DF40A86A533C1B47FB6738D3FB7A70F05AAA8698BD02CE0DE92F93DEF0C829`.

Both products were copied under distinct temporary names beside the unchanged
publication and run through the existing Console observer as `<exe> EDIT.COM`.
Same working/media directory, no scripted keyboard/mouse input, ten-second
observation. Reports are `O:/ntvdm64/logs/t412-s1-cell2-edit.txt` and
`t412-s1-cell4-edit.txt`; snapshots append `.console.txt`.

| Product | Snapshot bytes / NUL count | Snapshot SHA-256 |
| --- | --- | --- |
| Current two-byte adapter | 4096 / 729 | A54C555EFEC9F961716B733896F598C3205D1DBFAA04E618E358CADD477D8643 |
| Single-change four-byte adapter | 4096 / 0 | BC26704CAA93719DB9C34797D1C990C66BAD591E20822DD75071BFA398AD0E35 |
| Old published product, prior control | 4096 / 0 | BC26704CAA93719DB9C34797D1C990C66BAD591E20822DD75071BFA398AD0E35 |

This proves the diagnosed display corruption is removed by restoring the
adapter's selected-producer cell contract. It does not require changing the
CPU, original painter, or quick-event provider. Both interactive observations
end at the watchdog, not normal guest exit; byte-identical screen snapshots
are rendering evidence, not full EDIT keyboard/mouse or WRITE acceptance.

## Repair disposition

The minimum product repair is the adapter allocation argument and its false
explanatory comment. Keep original `nt_cga.c` unchanged, keep MONITOR retired,
and add a regression that checks the actual producer stride against adapter
allocation/presentation, including full registered capacity. Re-run formal
x86 build, MEM/COMMAND and EDIT before publishing. S1 remains open.
This diagnostic turn does not change production source or replace either
published `ntvdm32.exe`; both retain C0771B...652F. Diagnostic EXEs and graph
remain recoverable under build, and observation logs stay under the runtime
logs directory.

## Approved repair and verification

The subsequent owner request, "请你连续执行", admits the minimum repair and
verification described above. Production change is one allocation argument
plus correction of its comment; original painter, CPU and overlay are unchanged.
Recovery ladder: original `nt_text` remains directly composed; private Console
Server registration cannot be imported because its section/controller state is
inside CSRSS. The existing ADAPTER-WIN32-032/048 public-Console binding is the
smallest seam and now preserves the selected producer's four-byte contract.
No imported-source intrusion, new ABI or new rendering algorithm is needed.

The existing Console fixture had duplicated the wrong two-byte assumption.
It now checks allocation before writing, poisoned reserved bytes, characters
and attributes, full 80-by-50 capacity, final-row/final-column presentation,
short-snapshot rejection, out-of-range rectangle rejection and unregister /
same-size re-register pointer retention. Its original queue/input tests remain.
It explicitly owns a fresh Console rather than consuming an inherited user's
input. The initial inherited-Console run rejected resize with error 87;
detaching that fixture and allocating its own Console makes the full test pass.
No assertion is skipped on an unavailable Console.

Build root: `build/M0-T412/S1/text-cell-repair`. A new formal product graph
compiled all 441 steps, with no reused previous-run product objects. Toolchain
is MSVC Win32/x86 `/MT`; generator uses Node 22.22.1. Existing observer binary
is reused unchanged from event-profile; it supplies no product/link input.

Verification:

- `Verify-ConsoleTextCellContract.mjs <formal-build.ninja>`: pass; selected
  original painter profile, four-byte allocation and fixture agree, no MONITOR.
- Actual linked adapter/session fixture: pass, exit 0. The same new fixture
  linked to adapter source at `bcb623db5` fails at the pre-write capacity
  assertion, exit 24. Inputs and results are preserved in the build root's
  `test-fixture.mjs`, `fixture/before.ninja` and `fixture/results.json`.
- Fresh formal `ccpu-halt-reset-test.exe`: exit 0, `AX=beef producer=0`.
- Published formal product, positional `MEM.EXE`: exit 0. Its pre-existing
  implausible largest-program-size output remains; this is launch/return
  smoke, not proof that MEM's memory accounting is correct.
- `COMMAND.COM /c ver`: exit 0, `MS-DOS Version 5.00.500`.
- Two `EDIT.COM` observations: both snapshots equal the old clean reference
  `BC26704C...E35`, 4096 bytes and zero NULs. Both end at the bounded interactive
  watchdog; this proves stable initial rendering, not full input/exit acceptance.

Real reports: `O:/ntvdm64/logs/t412-s1-cell-repaired-{mem,command,edit,edit-repeat}.txt`
with their `.console.txt` snapshots. Build scripts/results stay under build.
Only the formal `ntvdm32.exe` was staged this turn, using StageProductExecutable.
Both publication paths contain 3,228,672 bytes, SHA-256
`33C551379435F054EDBEA1FE253B2A0A82E7A712D9D641A4D820B7C7E7AE43AA`.
The old publication remains recoverable at
`build/M0-T412/S1/event-profile/ntvdm32-before.exe`. No media changed.

This closes the diagnosed text-layout prerequisite, not T412 S1 or the broker
design. The existing MEM output limitation and full EDIT/WOW interaction remain
explicit; no additional rendering or CPU implementation is admitted by this fix.
