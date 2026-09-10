# M0 T396 S13 — controlled original error-panel observation 001

## Admission and boundary

S11 reaches original `illegal_op_int -> host_error`; S12 establishes that the
selected NTVDM `host_error` body owns its visible `ERRORPANEL` and returns
`ERR_CONT` rather than implementing generic `ERR_RESET` or `ERR_DEF` behavior.
S13 admits no product repair. It adds only a test-owned action that may select
the same original Continue/Ignore control a user can select, so the next
source-owned boundary can be observed.

## Source-shaped selector

The observer enumerates only top-level child-process windows and requires all
of these predicates before posting one `WM_COMMAND`:

1. child PID equals the launched product PID;
2. the visible window class is the standard dialog class `#32770`;
3. original `ERRORPANEL` control ID 102 exists and is visible; and
4. that control's text is exactly `&Ignore`.

The driver records the selected `HWND` before incrementing its count. A
subsequent 100-ms observation of that same HWND is ignored; after the window
is destroyed that value is cleared, so each live original panel receives at
most one test action. It does not inspect guest RAM, alter the product, or
interact with unmatched, private-WOW, or guest USER dialogs.

## Recovery ladder

| Rung | Disposition |
| --- | --- |
| Original source reuse | `ERRORPANEL` and its control ID 102 remain the product's original reachable UI. |
| Smallest retaining seam | Observer-only Win32 `EnumWindows`/`PostMessage` invokes the same control command after exact source-derived predicates. |
| External intrusion | Not used. |
| New product behavior | Not used; the observer is not linked into the product. |

## Runtime provenance

`O:\\t396` previously contained the S11 archive executable while its manifest
still named the S10 archive. Before any S13 result, the project's
`Stage-OriginalSoftpcRuntime.mjs --update --replace-product` operation rewrote
the manifest with the S11 archive hash and retained the declared original
WOW32 companion and media inputs. This fixes provenance only; it is not a
runtime repair.

## Required result record

The retained run record must state the selected product hash/manifest, modal
selection count, process outcome, loaded-module/WOW32 state, BOP/session
reports when present, and whether the source panel was actually reached. A
zero selection or an earlier terminal is an observed boundary, never proof
that the panel contract changed.

## Result

The x86 observer rebuilt with MSVC `/W4 /WX /MT`. The S13 run used the staged
product hash
`0f1777c169cd6fe4598a500608d9160bb55396a2917932feb766ea57be53a41e` and
the rewritten manifest hash
`4547a87671414072b276017ddee0aa3e6b2854da66dfe6d0069bef7b07a0cc6e`.
It exited after 6500 ms with `0xc0000409`, `loaded-module-count=15`,
`loaded-wow32=no`, and `observer-modal-continue-selections=0`.

The matching archived S7 observer control also exits at the same early state
(6485 ms, `0xc0000409`, 15 modules, no WOW32). Therefore the result does not
attribute the terminal to the new S13 selector. The retained BOP tail reaches
original `50:36`/DEM transfer and then returns from `FE2E:340A` to zero before
the prior S11 `ARPL` observation. No original `ERRORPANEL` was reached, so S13
does not assert a selected continuation or a dialog repair. S14 owns the
reproducible earlier DEM/native-fast-fail provenance audit.
