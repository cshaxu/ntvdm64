# M0 T310 S8 P1 — `i386` architecture-source validation

## Decision

`i386` is the original **32-bit x86** source-selection macro.  It is neither
a synonym for an x86 host nor a declaration that NT4 V86/MONITOR facilities
exist.  In particular, the x64 candidate must not define it merely to reuse a
historical x86 path: `#ifndef i386` is a real x64 source branch that requires
its own later semantic-owner audit.

## Source-selected build change

`tools/build/New-T310OriginalSoftpcNinja.ps1` now applies `/Di386` only for
`-Architecture x86`.  The x64 graph deliberately has no equivalent define.
It records `i386Define=true|false` in its generated manifest and writes below
`build/M0-T310/S8/p1-machine-source/<architecture>`.

This is build selection only.  It does not assert that every differing branch
is semantically complete; the queued `i386` conditional audit must classify
each behavioral branch and assign its selected x86 and x64 owner.

## Formal result

Outside the sandbox, the following succeeded with MSVC `/MT`:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 `
  -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p1-machine-source/x86 original-softpc-candidate

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 `
  -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p1-machine-source/x64 original-softpc-candidate
```

Both graphs archived the selected original CCPU, BIOS, keymouse, system,
support, video and selected host-root groups.  The x64 command line contains
no `/Di386`; the x86 command line contains it exactly once.  Existing original
source warnings remain warnings only.

## P1 machine-source implication

The original reset path remains the P1 authority:

1. `base/bios/reset.c` initializes ICA, CMOS, PPI, diskette and disk POST,
   then invokes `host_reset()` and `q_event_init()` for `NTVDM`.
2. `base/system/rom.c::rom_init` loads the immutable `bios1.rom` and
   `bios4.rom` (plus selected video ROM) through the original
   `host_read_resource` shape.
3. `base/system/cmosnt.c::cmos_init` binds the CMOS ports and RTC EOI hook;
   `cmos_post` reconciles selected machine configuration.
4. `base/disks/sources` is selected in full with host `nt_fdisk.c` and
   `nt_rflop.c`; `nt_rez.c` preserves the original `host_read_resource` ROM
   call shape. Both formal graphs archive this startup-media group directly.

The full `base/disks/sources` group plus these three host roots now archives
successfully on both architectures. It is still an initialization/source
closure only: its actual sector I/O, media admission and guest-visible
failure-direction verification remain later P1 implementation work.

## Firmware resource binding

Original `softpc.new/host/src/nt_unix.c::host_find_file` is the single source
of truth for name lookup before `nt_rez.c::host_read_resource` opens a ROM or
CMOS resource. Its original `GetSystemDirectory` search is an NT4 product
installation assumption, not the current product's firmware boundary.

`MVDM-HOST-DIV-037` changes only that lookup step: it calls the same-shaped
`mvdm_softpc_firmware_find_file` adapter before retaining the original
`host_error` failure order. The adapter receives a caller-owned ANSI buffer,
reads an immutable root selected by the active `session`, and uses public
`GetFileAttributesA` to accept an existing file. It owns no firmware bytes,
Windows HANDLE, mapping-manager ID or guest state. A mapping manager is not
applicable because no host object crosses an MVDM fixed-width ABI here.

The application will resolve this root relative to its executable directory:
`softpc/` contains firmware assets while `dos/` and `win16/` contain separate
guest images. ROM, BIOS and data assets remain external package inputs; none
is linked into the executable.

Outside the sandbox, this formal test passed on both architectures:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310FirmwareResourceNinja.ps1 `
  -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p1-firmware-resource/x86 verify

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310FirmwareResourceNinja.ps1 `
  -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p1-firmware-resource/x64 verify
```

Each fixture binds a session, resolves the shipped `bios1.rom`, rejects a
missing ROM with an empty output buffer, and rejects a too-small caller
buffer. x86 compiles with `i386`; x64 deliberately does not.

The separate `New-T310PackageLayoutNinja.ps1` fixture also passed on x86 and
x64. It proves that app derives `C:\\installed\\ntvdm64.exe` to
`C:\\installed\\softpc\\roms` before activation and rejects a bare relative
executable name. The public `GetModuleFileNameA` wrapper uses exactly the same
path derivation for the real executable; no firmware path is read from the
current working directory or an NT system directory.

No Bochs device, MONITOR, kernel VDM, `src.old`, raw native identity or
preprocessor-derived capability decision was introduced.
