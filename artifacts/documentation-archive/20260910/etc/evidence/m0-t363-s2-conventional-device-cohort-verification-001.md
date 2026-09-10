# M0 T363 S2 — Original conventional-device cohort verification

## Inputs

- Selected formal graph: `build/M0-T362/S2/x86/build.ninja`.
- Verifier: `tools/audit/Verify-SoftpcConventionalDeviceCohort.mjs`.
- Product profile: Win32/x86, `CPU_40_STYLE`, `NTVDM`, and no `MONITOR`.

## Procedure and result

```text
node tools/audit/Verify-SoftpcConventionalDeviceCohort.mjs .
SoftPC conventional-device cohort source/graph verification passed.
```

The verifier reads only the selected graph and original mirrored source. It
proves the selected graph compiles the original CPU virtual-I/O, reset, IOS,
BIOS-printer, LPT-emulation and host-LPT units; `reset.c` calls
`printer_init` and `printer_post`; `printer_post` initializes the BDA LPT
base port and original `0x14` timeout; `printer_init` registers the original
callbacks and ranges; `ios.c` dispatches through those tables; ordinary BIOS
`printer_io` preserves `AH=0/1/2` and its status/timeout direction; and the
selected non-MONITOR `host_lpt_status` reads original local `host_lpt[]`
state, including `HOST_LPT_BUSY` used by `notbusy_check`.

## Disposition

No new device provider, virtual printer, direct-port layer or guest policy is
needed. The complete immediate conventional-device cohort is original code
already selected by the product graph. S3 can close the formal product link
without changing this behavior.
