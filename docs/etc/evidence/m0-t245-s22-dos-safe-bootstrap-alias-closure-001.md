# M0 T245 S22 — DOS-safe bootstrap alias closure

## Question

Can the original source-built `COMMAND.COM` start through the guest-owned
`cmdconf.c → SHELL= → sysconf.asm/sysinit2.asm:commnd` route when the real
host bundle path contains a component that the guest's 8.3 canonicalizer
cannot represent?

## Inputs

- S21 native observation: the guest changed `O:\REPOS.HOBBY\...` into the
  nonexistent `O:\REPOS.HOB\...` before `demfile.c:demOpen` called the host.
- Original OpenNT/guest route: `cmdconf.c`, `sysconf.asm`, `sysinit2.asm` and
  `sysinit1.asm`; `commnd` has fixed 64-byte storage.
- Public Win32 `GetShortPathNameW`, `CreateSymbolicLinkW`, `FSCTL_SET_REPARSE_POINT`
  and `RemoveDirectoryW`.

## Recovery

`src/cli/ntdos64_dos_safe_alias_v1.*` admits the existing DOS bundle root in
this strict order:

1. retain a host-provided short spelling if it is component-wise DOS 8.3 and
   fits the original `SHELL=` payload budget;
2. otherwise create a session-owned directory symbolic link under the public
   temporary directory;
3. when the public symbolic-link API is unavailable to the caller
   (`ERROR_PRIVILEGE_NOT_HELD` on this host), create an equivalent local
   mount-point junction using the public file-system control API;
4. remove only that link at CLI-session teardown.

The junction points at the existing root. It does not copy `NTIO.SYS`,
`NTDOS.SYS`, `COMMAND.COM`, `CONFIG.NT`, `AUTOEXEC.NT`, or any other guest
input; it does not create a virtual drive/boot volume. The OpenNT DEM and
COMMAND source bodies are unchanged. This is a named CLI admission seam
because NT4's installed DOS directory supplied an already DOS-consumable
spelling, whereas a modern unpack-and-run directory need not have one.

## Verification

Fresh formal Ninja graph: `build/M0-T245-S22/formal-r1` (MSVC x64 `/MT`).

```text
T245 S22 DOS-safe root admission verified (session-link=1)
```

The fixture uses the actual `O:\repos.hobby\ntdos64\build\output\dos`
bundle root. It opens `COMMAND.COM` via the created alias, verifies the real
target remains present after teardown, and verifies the session link itself no
longer exists. The initial symbolic-link attempt was denied with error 1314;
the junction fallback was then exercised successfully.

The bounded source-built Direct run now records six accepted `50:12 demOpen`
calls and a final `CreateFileOem` with `first-error=0 retry-error=0`; the old
permanent-command `AX=3` path is gone. Its original guest control flow reaches
COMMAND `54:02`, `54:0D`, `54:0F`, then a distinct machine `BOP 06` edge. The
sequence record overflows only because that unimplemented independent selector
repeats; it is not evidence against the completed path admission.

## Conclusion and transfer

S22 closes the CLI/root-admission defect and makes original permanent
`COMMAND.COM` open succeed without changing Bochs, BOP routing, DEM, or the
guest. The next observed edge is an already separately owned machine/BIOS
`BOP 06`/illegal-operation route. It is recorded as `BOP-DEPENDENCY-114` for
the selector-blind machine package; this closure does not turn it into a
trace-led COMMAND or DEM patch.
