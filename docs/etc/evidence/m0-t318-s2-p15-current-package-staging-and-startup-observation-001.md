# M0 T318 S2 P15 — Current package staging and startup observation

## Scope

This record separates the current formal CPU40 product image from the older
runtime observation image.  It does not claim NTDOS load, guest `EXEC`, or
parent return.

## Package contract

`app/package_layout.c` selects immutable media relative to the product
executable:

- `dos/NTIO.SYS`, `dos/NTDOS.SYS`, and `dos/COMMAND.COM`;
- `softpc/roms/{bios1,bios2,bios4,v7vga}.rom`;
- `softpc/roms/{profile.spc,cmos.ram}`.

The original `host_find_file` and `host_read_resource` retain their call
shapes.  `host_find_file` asks the session-selected firmware binding for a
caller-owned path; it does not fall back to an NT4 system directory.

`tools/build/Stage-OriginalSoftpcRuntime.mjs` stages exactly that immutable
set and writes a SHA-256 manifest.  It refuses to overwrite an output
directory.  The tool owns no product loading behavior.

## Reproduction

```text
node tools/build/Stage-OriginalSoftpcRuntime.mjs \
  --executable build/M0-T318/S2/cpu40-v7vga-r4/original-softpc-process.exe \
  --output build/M0-T318/S2/runtime-r27-r4
```

The resulting x86 package's executable SHA-256 was:

```text
97d2d4f7e1d56a0c84c698febcfb7e9f1e76fec1760bee68e9759feb89208ee5
```

## Observation

With all required media adjacent to the current r4 executable, a five-second,
debugger-neutral console-owning observation (`-f --ordinary-child`) remained
alive and used 140 ms CPU.  It did not locate the observer's NTIO prefix or
NTDOS offset anchor.  Those scans are negative observations only: they do not
prove that the original loader was not reached.

The prior `runtime-r11-v7vga` executable is a distinct PE image, even though
its staged DOS media have the same identities.  Its CPU-time observation must
not be used as runtime proof for current r4 composition.

The primary process thread's detached stack snapshot contains the original
`CpuEnvInit`/`AddToCpuEnvList` region and
`VdmUnhandledExceptionFilter`; frame-pointer/SEH layout prevents a reliable
statement of the immediate wait owner.  The observed separate worker starts
through the registered timer thunk.  No timer, Base VDM, DEM, COMMAND, BOP, or
guest source behavior was changed based on this observation.

## Disposition

The missing-sidecar-resource error is closed as a build/package staging issue.
The remaining startup wait is an unclassified original-host execution issue.
It stays inside T318 S2 only for source-level owner attribution; no
trace-selected service or replacement loader is admitted.
