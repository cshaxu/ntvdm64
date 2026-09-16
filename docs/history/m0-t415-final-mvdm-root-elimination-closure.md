# M0 T415 final MVDM-root elimination closure

T415 completes the physical cleanup following T414. All 42 OpenNT WinNLS font
blobs are now at `src/opennt-host/base/win32/winnls/fontsup/system`; the two
original `v86/util` inputs are at `src/mvdm/v86/util`; and the non-original
extracted printer carrier is at
`src/mvdm-overlay/v86/monitor/i386/monitor_printer.c`. The retired
`src/mvdm-host`, `src/mvdm-guest`, and `src/mvdm-tools` roots no longer exist.

`Test-T415FinalRootElimination.ps1` proves the 42 fonts and two tool inputs
are exact OpenNT blobs and that the carrier did not change while moving. The
current formal generator selects the overlay carrier. A fresh x86 formal graph
ran all 478 of its exported compile/link commands under the historical MSVC
host-x64/x86 environment; it linked `run16.exe`, `basesrv.exe`, and
`ntvdm.exe`, with the VDM_TIB map gate passing. Documentation governance and
`git diff --check` pass. No runtime package was published to `O:\winnt`:
this is a placement-only task.
