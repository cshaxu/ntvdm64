# M0 T391 S9: final audit closure

## Scope audited

T391 owns the app-entry disposition and the original COMMAND child lifecycle:
resolved DOS/Win16 images enter the VDM, resolved native images use public
CreateProcess, unresolved text uses COMSPEC, and 54:08 restarts the current
product without classifying its tail.

## Evidence

The committed S7 clean Console matrix passed in the real O:\ntvdm64 container:

- command/c ver
- command.com /c ver
- command.com /c command.com /c ver
- interactive command.com followed by exit

Each row returned zero; both /c controls required the MS-DOS Version 5.00.500
marker.  The independent controls prove unresolved `ver` uses COMSPEC and a
resolved native `cmd.exe /d /s /c exit 37` returns 37 without VDM startup.

T391 S8 then embedded exactly bios1.rom, bios4.rom and v7vga.rom in ntvdm32.
The resource probe proved bios2.rom, profile.spc and cmos.ram absent.  The same
four clean Console cases still passed after all external package ROM files were
removed.  build/output/ntvdm32.exe and O:\ntvdm64\ntvdm32.exe match SHA-256
DDDC8B0C097B3C20E345189DE06AAE4ECD215C283CA872DF751A871C7930A054.

## Closure

All T391 exit criteria are satisfied for the published x86 product.  T391 does
not claim x64 execution; that is the separately admitted T392 recovery task.
