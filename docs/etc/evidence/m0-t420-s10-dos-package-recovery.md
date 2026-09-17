# M0 T420 S10 — SoftPC DOS package recovery

## Scope and mirror audit

The complete selected `softpc.new/base/dos/sources` manifest contains one
translation unit: `emm_mngr.c`. Its pinned counterpart is
`OpenNT/base/mvdm/softpc.new/base/dos/emm_mngr.c`.

The S10 bytewise and normalized comparison leaves exactly two retained,
previously registered source-shaped differences:

- `MVDM-HOST-DIV-061` adds the standard allocator declaration. The original
  `host_malloc` call is reachable in the selected non-MONITOR build whereas
  the historical declaration was selected only by `MONITOR`.
- `MVDM-HOST-DIV-148` selects the original LIM byte/word/string writeback
  bodies for the CCPU40 `SAS_MM_LIM` profile. It changes only the outer
  preprocessor condition; the original bodies and their ordering are intact.

There is no new DOS policy, adapter implementation, overlay, or autonomous
logic. This S restores every line ending in the selected source to the pinned
OpenNT CRLF form: 2,133 CRLF lines, no bare LF lines, and a final newline.
Thus the net functional footprint is zero lines; the tracked mirror diff is
now only the two documented source-shaped divergences rather than a
format-only whole-file diff.

## Build and verification

Formal root: `build/M0-T420/S10/formal-x86-003`.

The local Ninja runner stopped after successfully producing child objects but
before reaping them. This was independently reproduced with an unchanged
single-object target and is a runner/process-wait defect, not a source error.
The selected S10 source was therefore compiled directly with the generated
Win32/x86 CCPU40 flags into `obj/dos/emm_mngr.obj` (12,787 bytes), then
`original-softpc-dos.lib` and all four product executables were freshly
linked. The other 435 objects and 38 libraries were reused from S9 only after
the source review established that their inputs were unchanged; S10's sole
selected source was not reused. The final worker link ran the independent
storage gate:

```
PASS: VdmTib owner=mvdm_vdm_tib.obj bytes=4208 address=0x6dd370; no overlapping symbol
```

The deployed `O:\winnt` SHA-256 values are:

- `run16.exe`: `32BB09C35159FD6DC41917EC152FC86F4E5DD5E404CCD739CFDADA039596510E`
- `basesrv.exe`: `C00D70CF493F8D22892E88AA6459CF46A27D4C9628DDC5AB00AB2279490F26E1`
- `ntvdm.exe`: `701FB2575CB01990F15823D2A050BF14DAC2E9224733A4F168135F10940ADB36`
- `dtmgr.exe`: `7431F438AEA43735C7ED0223DEC4B6C21487919DAB7FCC697631ADC96EB788FA`

Deployed checks passed: direct MEM (exit 0), repeated MEM (exit 1), nested
COMMAND→MEM (exit 1), `COMMAND /C VER` (exit 0), and EDIT return (exit 1).
The C-VID integration fixture also passed 80x25, 45x34, 60x50, 120x30,
resize, mouse, and all five short-window EDIT→MEM geometry observations.

## Follow-up

S11 owns the complete `softpc.new/base/debug` package. It must retain the S10
DOS source and its two registered CCPU40/allocator compatibility conditions;
it may not add a second DOS/BOP policy.
