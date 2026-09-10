# M0 T359 S6 — verified-root app-layout boundary

## Fixed inputs

The user-designated format-3 container `O:\\ntvdm64` was used directly. Its
sixteen fixed media entries matched `runtime-manifest.json`; only the explicitly
mutable formal product was replaced. The resulting product SHA-256 was:

```text
B8B852035E5ADC6585C0DEE05DACCE3130350582F2512DA116C193491D9D9B27
```

The selected app SystemRoot was `O:\\ntvdm64\\mvdm` (15 visible bytes).

## Source correction and result

The original `cmdconf.c` format is not only a command-image pathname. It
generates `SHELL=<SystemRoot>\\system32\\COMMAND.COM /p
<SystemRoot>\\system32`. With the selected root this is 65 visible bytes,
which exceeds the original 63-visible-byte `commnd` input contract.

The sole console-owned observation timed out with no BOP, SAS-store or
exception report. Its primary frame `0x00D41017`, after subtracting runtime
base `0x00D40000`, maps to app `app_report_media_root_rejected` at RVA
`0x00401000`; `0x00D41290` maps inside app `main`. This is app layout rejection
before SoftPC, COMMAND or the transient lifecycle. Absence of their reports
has no semantic interpretation.

## Disposition

S6 is closed as a precise app-owned predecessor. M0 T360 owns restoration of
the original SystemRoot package shape. T359 remains paused; no CPU, COMMAND,
DEM, BOP, guest-media or firmware conclusion is made here.
