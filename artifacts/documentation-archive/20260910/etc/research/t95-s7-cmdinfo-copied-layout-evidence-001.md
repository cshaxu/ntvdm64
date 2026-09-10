# T95 S7 Copied CMDINFO Layout Evidence

## Source Inputs

* `src/opennt/base/mvdm/inc/cmdsvc.h`: `#pragma pack(2)` declares the
  36-byte `CMDINFO` record.
* `src/opennt/base/mvdm/inc/CMDSVC.INC`: confirms its 16-bit field sequence
  and `COM_EXTENTION = 8`.
* `src/opennt/base/mvdm/dos/v86/cmd/command/tcode.asm`: passes DS:DX to
  `SVC_CMDGETNEXTCMD`, declares `COMBUFLEN = 128` and `EXECPATHLEN = 273`.
* `src/opennt/base/mvdm/dos/command/cmdmisc.c`: tests environment capacity
  before command delivery; its carry return has AX equal to the required
  environment byte count. It then writes the command line, executable result
  fields, drive/code-page values and optional environment result.

## Copied ABI

`bx_ntvdm_cmdinfo_v1` reads only an already copied exact 36-byte little-endian
wire record. It preserves all 17 fields, including the historical redirector
DWORD and all segment/offset pairs, but does not calculate an address, turn a
pair into a host pointer, or retain any guest-memory reference.

`bx_ntvdm_cmdinfo_response_plan_v1` has four outcomes:

1. deliver when every supplied requirement fits;
2. environment retry when the required environment exceeds `EnvSize`;
3. command rejection when the command requirement exceeds `CmdLineSize`;
4. executable rejection when the pathname requirement exceeds `ExecPathSize`.

The last two are closed modern safety outcomes: the historical initial caller
supplies 128 and 273 respectively, so a smaller value is not a valid first
profile request. Requirements above those source maxima are rejected before a
plan is emitted. This codec itself does not choose a DOS-visible error, write
RAM, change flags, or dispatch a BOP.

## Verification

The fresh admitted root
`artifacts/build/t95-s7-cmdinfo-layout-c11-001` used CMake 4.1.0 and
`C:\\msys64\\ucrt64\\bin\\gcc.exe` (GCC 16.1.0) to build only
`bx-ntvdm-cmdinfo-v1-test`.

The first process exited `1` because the fixture inverted its assertion for
the intentionally rejected over-maximum requirement. The codec had correctly
returned zero. After correcting the fixture-only condition, the same target
printed:

```
bx-ntvdm-cmdinfo-v1-test: copied CMDINFO layout and response capacities verified
```

## Boundary Consequence

The next admission, if runtime evidence supports it, may compose a generic
checked 36-byte gather from DS:DX, invoke this codec, and produce a generic
bounded result. It must not add an adapter-specific memory backdoor, send a
guest pointer into CLI/Bochs, or claim that the temporary contained
`COMMAND.COM` envelope represents the eventual CLI target mapping.
