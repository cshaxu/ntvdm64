# M0 T360 S1 — original SystemRoot root-consumer map

## Source-defined consumers

The reached consumers divide into two independent roots:

- `src/mvdm-host/dos/command/cmdconf.c::ExpandConfigFiles` receives the
  session system root, normalizes it with `GetShortPathNameA`, and emits the
  unchanged `shell=<root>\\system32\\command.com /p <root>\\system32` line.
  Its `achSysRoot[64]` and guest `commnd` contract retain a 63-visible-byte
  value bound.
- `src/mvdm-host/softpc.new/host/src/nt_pif.c::GetPIFConfigFiles` uses the
  same system-root binding for default `config.nt` and `autoexec.nt`.
- `src/mvdm-host/dos/dem/dem.c` establishes the default DOS directory from
  that same root; `demmisc.c::demLoadDos` derives `ntdos.sys` from it.
  The app already validates root `ntio.sys` for the matching start path.
- `src/mvdm-guest/dos/v86/doskrnl/bios/sysinit2.asm` retains the guest
  fallback `\\COMMAND.COM`; therefore the unchanged command image must also
  be staged at the root in addition to the host configuration path
  `system32\\COMMAND.COM`.
- `src/mvdm-host/softpc.new/host/src/nt_unix.c::host_find_file` is the
  original firmware lookup shape.  Its adapter takes the distinct session
  firmware root.  `rom.c` requests `bios1.rom`, CPU40 `bios4.rom`, and
  `v7vga.rom`; `nt_rez.c` requests `profile.spc` and `cmos.ram`.

## Current mismatch and selected layout

Current app `package_layout.c` and `Stage-OriginalSoftpcRuntime.mjs` both
invent `mvdm` below the executable directory.  No reached original caller
adds that path component.  It lengthens every SystemRoot use and makes the
unchanged `cmdconf.c` line invalid even at `O:\\ntvdm64`.

S2 is therefore limited to this source-shaped package topology:

```text
<package-root>\\original-softpc-process.exe
<package-root>\\NTIO.SYS
<package-root>\\NTDOS.SYS
<package-root>\\COMMAND.COM
<package-root>\\config.nt
<package-root>\\autoexec.nt
<package-root>\\system32\\COMMAND.COM
<package-root>\\system32\\COUNTRY.SYS
<package-root>\\system32\\HIMEM.SYS
<package-root>\\system32\\REDIR.EXE
<package-root>\\system32\\DOSX.EXE
<package-root>\\softpc\\{bios1.rom,bios2.rom,bios4.rom,v7vga.rom,profile.spc,cmos.ram}
```

All entries are existing immutable bytes; only their stage destination and
app-selected roots change.  This retains original source behavior and avoids
a drive alias, a host C copy, a guest rewrite and an additional media mapper.
