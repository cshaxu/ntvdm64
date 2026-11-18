# T483 S9 sequential audit

Baseline fcaae36df. Follow order.csv. Initial C/header footprint is 2,298 physical lines. Documentation is measured separately.

## 01 console_grid.c

90 lines, 4,279 bytes at entry and exit. Keep both operations. The NTVDM client uses the adapter on its native-handle path. NTCON uses it for frontend creation, mode changes and geometry requests. OpenNT windows/core/ntcon/server/output.c:3225 owns ResizeScreenBuffer over private row structures. The T412 S11 recorded control demonstrates native reflow when the window itself changes. Dropping either snapshot path would lose the cell-grid guarantee. The before and after buffers describe different geometries; the transfer helper checks clipping on both reads and writes. Cleanup preserves the error across buffer release. No demonstrated smaller equivalent implementation. Source and historical runtime evidence reviewed; no new runtime run or product change. Completed 1/40, 39 remaining; cumulative reduction 0 lines.

[Original comparison and runtime controls](../../docs/etc/evidence/m0-t412-s11-console-handoff.md).
