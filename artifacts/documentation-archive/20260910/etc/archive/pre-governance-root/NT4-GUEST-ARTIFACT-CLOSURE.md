# NT4 Guest Artifact Closure

## Scope

This record defines the NT4 DOS guest artifact scope used by the historical
toolchain work.  It contains the files installed into, or directly loaded by,
the DOS guest environment.  It does not include the Win32 NTVDM host, CCPU,
DEM, SoftPC providers, or the broader WOW16 Win16 runtime DLL families.

The five WOW16 device-driver artifacts are listed separately because they are
guest-adjacent drivers, not DOS `.SYS`/`.COM`/`.EXE` install payloads.

## Direct DOS Payload: 33 Files

### Source-built: 22

| Artifact | Historical stage |
| --- | --- |
| `NTIO.SYS` | `ntio-tools16-opennt-v1` |
| `NTDOS.SYS` | `ntdos-tools16-historical-v1` |
| `DOSX.EXE` | `dosx-tools16-historical-v6` |
| `COMMAND.COM` | `command-tools16-opennt-v1` |
| `HIMEM.SYS` | `himem-tools16-opennt-v1` |
| `ANSI.SYS` | `ansi-tools16-opennt-v1` |
| `KEYBOARD.SYS` | `keyboard-tools16-opennt-v1` |
| `REDIR.EXE` | `redir-tools16-opennt-v1` |
| `MEM.EXE` | `mem-tools16-opennt-v1` |
| `SETVER.EXE` | `setver-tools16-opennt-v1` |
| `APPEND.EXE` | `append-tools16-opennt-v1` |
| `DEBUG.EXE` | `debug-tools16-opennt-v1` |
| `EDLIN.EXE` | `edlin-tools16-opennt-v1` |
| `EXE2BIN.EXE` | `exe2bin-tools16-opennt-v1` |
| `FASTOPEN.EXE` | `fastopen-tools16-opennt-v1` |
| `SHARE.EXE` | `share-tools16-opennt-v1` |
| `GRAPHICS.COM` | `graphics-tools16-opennt-v1` |
| `KB16.COM` | `kb16-tools16-opennt-v1` |
| `LOADFIX.COM` | `loadfix-tools16-opennt-v1` |
| `NLSFUNC.EXE` | `nlsfunc-tools16-opennt-v1` |
| `COUNTRY.SYS` | `country-tools16-opennt-v1` |
| `QUIT.COM` | `quit-tools16-opennt-v1` |

All assembly/link/resource work uses the fixed OpenNT 4.5 tools16 inputs.  The
only narrow exception is the original MS-DOS 4.0 `CL.EXE` plus matching C
runtime closure used to compile the C portions of `MEM.EXE` and `SETVER.EXE`.
Their assembly, library, and final link steps remain OpenNT tools16.

### Static companion/data: 7

`GRAPHICS.PRO`, `QBASIC.HLP`, `EDIT.HLP`, `GORILLA.BAS`, `MONEY.BAS`,
`NIBBLES.BAS`, and `REMLINE.BAS` are data/help inputs, not compiler products.

### Transparent prebuilt inputs: 4

`BACKUP.EXE`, `QBASIC.EXE`, `MSHERC.COM`, and `EDIT.COM` are only `binplace`
outputs in the fixed OpenNT tree.  Their stage,
`nt4-prebuilt-guest-inputs-v1`, records matching source/staged hashes and
`SourceBuilt: false`; they are never represented as rebuilt source outputs.

## WOW16 Device Drivers: 5

| Artifact | Historical stage |
| --- | --- |
| `MOUSE.DRV` | `wow16-mouse-tools16-opennt-v1` |
| `VGA.DRV` | `wow16-display-tools16-opennt-v1` |
| `KEYBOARD.DRV` | `wow16-keyboard-tools16-opennt-v1` |
| `COMM.DRV` | `wow16-comm-tools16-opennt-v1` |
| `SOUND.DRV` | `wow16-sound-tools16-opennt-v1` |

Each driver is rebuilt through the original MASM/LINK16/RC16 path and matches
the corresponding OpenNT reference bytes.  `VGA.DRV` also uses the already
reconstructed historical `EXE2BIN.EXE` to generate its embedded `CONFIG.BIN`.

## Current Result

The controlled historical closure is 38 artifacts: 27 source-built, seven
static companion/data files, and four transparently staged prebuilt inputs.
This satisfies the direct DOS payload scope and all five declared WOW16 device
driver families.  It is not a claim to have rebuilt the host-side WOW16 runtime
or the rest of the MVDM host build graph.
