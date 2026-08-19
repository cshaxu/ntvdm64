# T229 S2 P17 — DEM GSET clock-mutation Direct/Readonly conformance

OpenNT `demgset.c:demSetDate` and `demSetTime` read the current local time,
overwrite only the DOS-specified fields, call `SetLocalTime`, and change AL
only: `AL=00` on success or `ERROR_PRIVILEGE_NOT_HELD`; otherwise `AL=ff`.
Carry is untouched.

The GSET clock provider now consumes the shared DEM `HOST_GLOBAL` policy:

- **Direct** invokes `SetLocalTime` and retains the original AL result rule;
- **Readonly** returns `AL=ff` before any Win32 clock mutation;
- Overlay and Virtual obtain no clock-mutation behavior.

The drive-view provider owns this policy selection. The selector-blind GSET
plane continues to handle only read-only `50:14` and `50:15` clock queries.

Formal Ninja regression used invalid date/time inputs (`CX=DX=0`) so it cannot
change the host clock. In Direct, Windows may reach the original
privilege-not-held fallback (`AL=00`) before rejecting the invalid input;
`AL=ff` is also accepted. Readonly is required to return `AL=ff`.

```text
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b bin/t225-s19-dem-package-lifecycle-fixture.exe
...fixture.exe direct
...fixture.exe readonly
...fixture.exe direct pdb-lifecycle
...fixture.exe readonly pdb-lifecycle
# all exit 0
```

This is code and local-profile closure for `50:19` and `50:1C`; it does not
claim a user-authorized successful host-clock change was performed in test.