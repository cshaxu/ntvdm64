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
