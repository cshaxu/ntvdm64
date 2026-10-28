# Shard 2: Win16 display and keyboard guest resources

Four pending entries were compared with both original trees and their historical resource/build relationships. The companion read-only observer and JSON capture the file identities and exact-path release observations.

| Entry | Evidence and bounded recommendation |
| --- | --- |
| `wow16/drivers/display/display.rc` | Matches both originals; it includes `display.rcv` and names `config.bin` as an OEM binary resource. The original makefile compiles the resource and applies it to `display.exe`, subsequently copied to `vga.drv`. Candidate exclusion as immutable guest build input only. |
| `wow16/drivers/display/DISPLAY.RES` | Matches both originals and is named by that driver recipe. Candidate exclusion as a historical guest resource build artifact only; exact generation and deployed-image provenance are not proved. |
| `wow16/drivers/keyboard/keyboard.rcv` | Matches both originals; original `keyboard.rc` includes it and the makefile compiles/applies the keyboard resource. Candidate exclusion as immutable guest build input only. |
| `wow16/drivers/display/config.LST` | Current-only, absent from both original trees and catalogs. Its contents resemble a MASM listing and the historical makefile invokes MASM with listing enabled, but this does not prove this particular file's generation. Keep supplemental disposition pending. |

At inspection the source-tree, ZIP direct `WINNT/SYSTEM32`, and installed direct `O:/winnt/system32` copies match for each of VGA.DRV and KEYBOARD.DRV (hashes recorded in JSON). This establishes individual image identities only, not source-to-binary derivation or loaded module selection. These are the WOW16 guest driver targets and must not be conflated with a standalone Windows 3.1 DOS system's Video7 driver. Display geometry, drawing, keyboard state, layout and callback/lifetime behavior remain independently unverified.

Replay `python artifacts/mvdm-audit/shard-2/win16-driver-resource-observer.py` and compare the companion JSON. No guest or product was executed or rebuilt, and no shared ledger, installed file or build tree was changed.
