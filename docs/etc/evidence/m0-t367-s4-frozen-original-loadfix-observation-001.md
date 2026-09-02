# M0 T367 S4 — Frozen original `LOADFIX.COM` observation

## Immutable workload

The fixed stage was regenerated at `O:\ntvdm64` from the current formal
product and immutable source media.  The prior stage was preserved recoverably
as `O:\ntvdm64.pre-t367-s4` before the new stage was created.

`LOADFIX.COM` was copied unchanged from
`src/mvdm-guest/dos/v86/cmd/loadfix/LOADFIX.COM`.  Its staged and source
SHA-256 values are both:

```text
8e62654a49bd88c784c53f25e9fba13c641624d6a02214385115f66ccf1ecfdb
```

It is also byte-identical to `build/output/dos/LOADFIX.COM`; no synthetic test
program or guest-media edit was used.

## One fixed invocation

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T356/S4/observation/console_startup_observer.exe \
  --product build/M0-T365/S2/x86/original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report O:\ntvdm64\observation-t367-s4.txt \
  --product-command "LOADFIX.COM" \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t367-s4.txt.bop-return.txt
```

The console-owning non-debug observer ended with its prescribed eight-second
controlled timeout (`0x53504354`), exit code zero, and no product exception.
The fixed media manifest hash was
`1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.

## Observation result

The recorded source-shaped trace includes original COMMAND initialization,
the declared first `54:01` delivery and return, ordinary `50:12`/`50:42` file
activity, and then a later `54:01` call whose continuation has no recorded
return before the controlled timeout.

The trace does **not** record an attributable `$Exec` entry, normal `50:16`
completion, `LOADFIX.COM` child entry, `$EXIT`/parent restoration, or `54:0B`
result.  Repeated `50:42` observations alone are not evidence that the
declared child was loaded; they may belong to other original COMMAND/bootstrap
file reads.

Therefore the earliest current terminal is the post-record original COMMAND
`54:01` continuation/wait.  This is an owner transfer to the COMMAND
post-record execution lifecycle, not evidence of a DEM read failure and not
authorization for a selector-level repair or a second trace.

Raw retained output is at `O:\ntvdm64\observation-t367-s4.txt`, its JSON
sidecar, and its BOP-return sidecar.
