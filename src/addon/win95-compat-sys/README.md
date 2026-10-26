# Win95 compatibility guest driver

`compat.asm` is the independently authored real-mode DOS character driver
used by this project's Win95 launch profile. It is an add-on, not an OpenNT
mirror or host link input. It retains the installed driver formerly built
under the separate Win95 research directory. There is no dependency on that
directory to compile it.

The owner requested this location and the optional boot-logo feature on
2026-10-25. This is separate from the active WOW32 packet. Do not modify the
ten host components, original NTIO/NTDOS or Win95 binaries to build/use it.
The already adapted installation's VMM32 consumer bindings remain necessary;
this driver is not a complete DOS 7 implementation.

## Build

Requires NASM with 386 real-mode instruction support (verified producer:
NASM 3.01). No MASM, Microsoft DDK, original kernel build or external source
generator is needed. From the repository root:

```powershell
& src/addon/win95-compat-sys/build.ps1 -OutputRoot "$PWD/build/t460-s3/win95comp-logo-owner/example"
```

Launch builds/tests in an owned background process under the repository's
execution rules. The script requires a fresh output below `build/`, writes
`compat95.sys`, its listing and `build.json` with binary/source identities.
The equivalent assembler command, from this source directory, is:

```text
nasm -f bin -I ./ compat.asm -o <build-output>/compat95.sys -l <build-output>/compat95.lst
```

`-CompatibilityOnly` defines `COMPAT95_NO_LOGO=1`. It reproduces the previously
installed 2435-byte binary exactly: SHA-256
`EDA2454FD4A356D42FA2D6ABB432E0E3F4B944B436E4B823CC045C981E7C1156`.
This provides an independent byte-level control for the source migration.

## Compatibility services

The retained providers implement the installation's system-registry pathname,
BIOS/A20 callback, actual loaded-character-driver RMD description, keyboard
return bridge, driver-owned disk-vector exchange, file-only physical-disk
refusal and bounded TSR state. They query original NTDOS data rather than
patching its instructions or reserved data. Existing private diagnostic
queries E795/E797/E798/E799/E79A/E79B are preserved. Hardware disk boot,
arbitrary DOS 7 callers and full reboot/TSR history are not supplied.

Provenance is recorded in [provenance.json](provenance.json).  It identifies
the owner-supplied independent source snapshot and its exact source hashes;
it is not an external runtime, build or acceptance dependency.
The original COM-provider composition has been flattened into this single
portable NASM source. Its private ABI comments and implementation remain;
the source includes no original Microsoft machine code.

## Optional startup artwork

The base configuration remains:

```text
device=O:\WINDOWS\PATCH\compat95.sys
```

The opt-in configuration is:

```text
device=O:\WINDOWS\PATCH\compat95.sys /LOGO=O:\WINDOWS\PATCH\LOGO.SYS
```

Use one unquoted DOS path without spaces, at most 127 bytes. No `win.cmd`,
PIF program, autoexec helper or production delay is added. The driver handles
WIN.COM's existing INT 2F AX=160E/BL=0 query. It validates a complete
uncompressed 320x400, 8-bit BMP and V7 capability before setting mode 66h,
then displays the original pixels at the original 640x400 physical aspect
(each source pixel spans two horizontal pixels). V7 packed scanout uses the
already present video BIOS/memory implementation. No alternate renderer or
host presentation path is introduced.

The V7 extension register gate is enabled before bank writes. Original BIOS
text bookkeeping is saved/restored around the hardware image, so DOS and
WIN.COM retain the original startup mode. This is distinct from restoring
the hardware mode: the picture remains until Windows takes the display.
New code/data is appended after the original resident prefix. The adapted
IOS consumer's existing data offsets must not shift; the prefix regression
permits only relocation of the branch to discardable INIT.

DX=0 deliberately declines WIN.COM's native planar-checksum takeover; no
planar checksum or successful native-logo ownership is fabricated. The next
normal BIOS mode change retires this driver's active-logo state and hands
display ownership to Windows. BL=4/5 maintain the queried flag; other native
subfunctions retain the old provider chain. The supplied original artwork's
bottom strip animates by rotating DAC entries 236..255 left by one entry on
each even BIOS tick, matching the supplied IO.SYS. Pixels and the other 236
palette entries stay unchanged. NTIO's existing INT 1C callback drives it;
there is no PIT/PIC reprogramming, new IRQ handler or startup delay. The INT 2F
receiver permits interrupts while loading/showing the bitmap, as the original
animation does, and preserves its interrupt-return flags.

Video/tick hooks are installed only after image validation and mode/palette
setup. Before the next normal mode change, DAC writes stop and both vectors
are restored if still directly owned. A later provider's vector is never
overwritten; a retained chained callback is passive (diagnostic tick state 2,
compared with 0 for an unlinked callback and 1 while animating). The observed
Windows timer provider chains the callback before changing display mode;
stopping at the earlier AX=1605 broadcast would prevent any visible animation.
Missing/invalid/disabled
art installs neither hook. A read/mode error restores the previous mode.

Animation provenance is the owner's already extracted, read-only IO.SYS
identified below: file offsets `0xDD22..0xDD3C` read `biClrImportant` as the
palette boundary; `0x08E3..0x0978` select even BIOS ticks and rotate each next
DAC entry into its predecessor, wrapping the first to the last. For this
bitmap, the header's value is 236 and the ring contains 20 entries. The
project receiver uses that contract without embedding original machine code
or copying DOS boot/retrace-wait/self-modifying code. Other BMP animation
metadata variants are not claimed; they display statically.

The existing system must support the V7 640x400 packed mode. Supplying the
option is not permission to change the generic host or DOS core. E79C/95AB
returns ES:DI to six diagnostic words: query count, attempt count, completed
draw count, later BIOS-mode-change count, last error, record version 1.
Errors: 1 capability, 2 file/header, 3 pixel-read/mode failure.

## Obtain the image locally

After the four-case runtime gate passes, publish with
`tools/build/Stage-Compat95Release.ps1 -BuildRoot <producer-output>
-VerificationRoot <case-parent> -Acceptance verified`. The shared add-on
manifest retains both mouse drivers and utilities; the ten-host-image
manifest is separate. Both assembly inputs are hash-checked before staging.

Install into an existing prepared Win95 tree with
`tools/win95-launch/Apply-Compat95.ps1 -WindowsRoot O:\WINDOWS
-BackupRoot <fresh-backup-outside-PATCH> -Logo <local-LOGO.SYS>`.
It consumes the released `compat95.sys`, preserves the previous files and
configuration, updates the existing CONFIG.NT entry, and removes the obsolete
W95COMP.SYS/COMPAT.SYS filenames only after the new installation is verified.
`-NoLogo` installs the same driver without the artwork option. Existing
WIN.COM/PIF/autoexec and mouse-driver output names are retained.

`LOGO.SYS` contains Microsoft artwork and is deliberately absent from this
repository and independent add-on release. Use the owner's existing extracted
IO.SYS copy, never mount or write the original disk image. The extractor is
identity-bound to the supplied localized retail IO.SYS SHA-256
`325695A4B4022A6FB373263D41E51F8CA0947F449DBA61741EAFAABC8AD34673`:

```text
python extract-logo.py --io-sys <existing-read-only-copy>/IO.SYS --output <fresh-local-output>/LOGO.SYS
```

It reads the DS-compressed bitmap, checks every block and exact BMP geometry,
and refuses existing outputs or unrecognized inputs. Expected output is
129078 bytes, SHA-256
`D955E60ED9BFF8056DC3829832DE7AB35CCD33752DB4481287B4B1CA9524C62F`.
The offline DS decoder's MIT attribution/license is in the Python header and
[DS-LICENSE.txt](DS-LICENSE.txt); it never enters the runtime binary.

## Verification

[verify-win95-bootlogo.ps1](../../../tests/observation/verify-win95-bootlogo.ps1)
uses the existing physical-package-stage/private-desktop observer ownership
helpers. Supply the runtime package, built add-on, local BMP, existing native
observer, guest desktop observer, Python, exact worker MAP and native-layout
JSON, plus a fresh build-owned output. It keeps the normal WIN.COM PIF target,
changes only the copied profile and temporarily installs a guest observation
autorun, restoring installed hives/INIs/logs afterward.

The gate requires an actual host DIB exactly equal to the extracted image
after horizontal expansion, then a 640x480 populated desktop and guest
Shell_TrayWnd/Progman plus OBSERVE_COMPLETE. Guest GDI success alone is not
splash acceptance. Animation additionally requires at least two actual DAC
ring phases while the image pixels and non-ring palette remain unchanged,
followed by Windows-owned vectors, a disabled/unlinked callback, and a frozen
animation counter across at least
three desktop observations. Negative cases must not install either hook.
This uses no Computer Use or input injection. Passing
stages are removed through the existing cleanup owner; failing stages are
retained with their ownership marker for diagnosis. Physical input and
unbounded desktop lifetime require separate owner testing.

Interface references: [RBIL AX=160E](https://fd.lod.bz/rbil/interrup/dos_kernel/2f160e.html),
the supplied unchanged WIN.COM's actual 160E call sites, and the selected
original V7 BIOS mode table and `set_v7_bank_for_seq_chain4` contract in
`src/mvdm/softpc.new/base/video/video.c` and `base/inc/egaports.h`.

The initial static 2026-10-25 release passes all four LogoCase values (Enabled, Missing,
Invalid, Disabled). Enabled captures the exact original bitmap and then the
Explorer desktop; the other cases reach the desktop without a logo. Installed
static `compat95.sys` is 4554 bytes, SHA-256
`F918A0A068BF083049B84AEA5414454A741BDF943E9B083C375CEDEAA5B5359C`.
Retained results are in `O:/WINNT/logs/win95-compat-logo`; the previous installed
profile is in `O:/WINNT/builds/pre-compat95-logo-20261025`.

The animated 2026-10-25 release passes all four cases with the stronger gate:
three actual ring phases (11/12/14), unchanged logo pixels/non-ring colours,
Explorer desktop, and 51 post-handoff samples with the frame counter frozen.
Windows owns the vectors; its chained tick callback is passive (state 2).
The new `compat95.sys` is 4856 bytes, SHA-256
`BFD8E8D3AF62D4398BE414087B36E30EFF9798308387B5454C4B109ED4490D17`.
Results are in `O:/WINNT/logs/win95-compat-animation`; the replaced static
driver/profile is in `O:/WINNT/builds/pre-compat95-animation-20261025`.
