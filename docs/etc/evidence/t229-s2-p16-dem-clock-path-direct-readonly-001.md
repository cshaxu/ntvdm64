# T229 S2 P16 — DEM clock/path Direct/Readonly conformance

OpenNT `demfcb.c:demDate16` always succeeds and returns packed DOS local date
in AX and packed local time in DX.  The existing FCB provider retains that
same register placement for both admitted modes; it has no mutation profile
branch.

OpenNT `demfile.c:demCheckPath` always clears DX, accepts `\\DEV\\` without
opening a host file, and otherwise tests the selected drive directory by
opening `NUL`.  The non-intrusive CLI provider retains the `\\DEV\\` success
and its `DX=0` ABI; other paths are resolved within the admitted namespace and
use the contained failure terminal rather than probing an arbitrary host path.

Formal Ninja lifecycle regression:

```text
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct pdb-lifecycle
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly pdb-lifecycle
# each exit 0
```

The fixture verifies AX/DX write placement and no CF write for `50:30`, then
verifies `50:44` `\\DEV\\` success (`DX=0`, CF clear) and contained failure
(`AX=5`, `DX=0`, CF set) in each Direct/Readonly configuration. It does not
claim host ordinary-path, device-name, or clock-boundary matrices.