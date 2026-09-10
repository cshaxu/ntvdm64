# T229 S2 P18 — DEM GSET/drive/CWD/volume whole-group conformance

The complete owner group is the 15 `demdisp.c` slots:
`0D,0E,0F,10,13,14,15,18,19,1A,1B,1C,25,41,46`.

`tests/bx-vdm/bx_ntvdm_dem_drive_view_session_v1_test.c` now registers the
same DEM `SESSION_CONTEXT` and `HOST_GLOBAL` classes used by the package
provider. It runs Direct then Readonly through a single installed session,
using a captured admitted C: volume and restoring the host current directory
after the Direct leg.

The test covers drive count and boot drive; free-space, VOLINFO, DPB and DPB
list layouts; clock query registers; profile-gated invalid Date/Time setters;
computer name; checked CWD/default-drive operations; and DTA registration.
For setters it supplies invalid date/time fields, so the test cannot adjust
the host clock. Direct accepts OpenNT's AL=00 privilege fallback or AL=FF
validation failure; Readonly requires AL=FF before Win32.

```text
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b bin/t226-s2-dem-drive-state-conformance-fixture.exe
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t226-s2-dem-drive-state-conformance-fixture.exe
# exit 0
```

This closes the group’s local code/conformance gate, while the ledger retains
unexercised host/error variants for the later all-73 convergence matrix.