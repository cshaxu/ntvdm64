# M0 T354 S4 — original COMMAND package-path admission enforcement

Date: 2026-09-01

## Source contract

Original `mvdm-host/dos/command/cmdconf.c::ExpandConfigFiles` emits the
`shell=` value using:

```c
"%s%s /p %s\\system32"
```

Original NTDOS `mvdm-guest/dos/v86/doskrnl/bios/sysconf.asm::trys` copies that
value directly into `sysinit2.asm::commnd`, whose storage is 64 bytes. The
guest performs no environment-variable expansion in this path. The permitted
value is therefore 63 visible bytes plus its NUL terminator.

The current 40-character package root produces a 114-character value. It is
an invalid input to the retained original guest contract, rather than a DEM
or BOP failure.

## Selected disposition

The owner rejected temporary drive letters, DOS-device aliases, `subst`,
junctions, virtual boot volumes and guest changes. App now measures the exact
unchanged `cmdconf.c` value before original host startup. If the public Windows
short spelling cannot satisfy the 63-character limit, it shows:

```text
NTVDM64 cannot start from this package location.
The original NTDOS COMMAND startup buffer accepts at most 63 characters for
its generated shell path. Install or move the package so its mvdm directory
has a shorter Windows path, then start NTVDM64 again.
```

It returns `APP_STARTUP_MEDIA_REJECTED`. No guest byte, media file, DOS name,
DEM result, BOP route, host namespace or mapping manager changes.

## Verification

The existing isolated Win32/x86 package-layout graph was rebuilt and run:

```text
cmd.exe /d /s /c "cd /d O:\repos.hobby\ntvdm64\build\M0-T310\S8\p1-package-layout\x86 && ninja.exe -v verify"
```

It compiled `package_layout.c`, linked `package-layout-fixture.exe`, and the
fixture exited successfully. The formal CPU40/x86 product was then rebuilt
incrementally:

```text
cmd.exe /d /s /c "cd /d O:\repos.hobby\ntvdm64\build\M0-T352\S2\return-observation-x86 && run-ninja-parallel.cmd original-softpc-process.exe"
```

It rebuilt `app/package_layout.c` and `app/entry.c`, rebuilt
`app-machine-shell.lib`, and linked `original-softpc-process.exe` successfully.
No runtime launch is appropriate from the known-invalid long package root:
successful admission would contradict this S's result.

## Limitation and follow-up

T354 does not claim permanent `COMMAND.COM` execution. A user may install the
unchanged package beneath a path whose Windows short spelling satisfies the
original bound; only then may a separately admitted fixed-container run test
the already source-owned bootstrap continuation.
