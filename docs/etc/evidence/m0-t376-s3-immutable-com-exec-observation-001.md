# M0 T376 S3 — Immutable original `.COM` execution observation

## Fixed observation

One console-owning, non-debug observation was run with the existing fixed
`O:\\ntvdm64` stage and current formal product:

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T375/S1/observer/console-startup-observer.exe \
  --product build/M0-T371/S2/formal-x86/original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report O:\ntvdm64\observation-t376-s3-loadfix.txt \
  --product-command LOADFIX.COM
```

The observer exited `0x00000000` within the fixed 8,000 ms limit.  Product
SHA-256 was `1b1fc86d15524bf269ff3a770e0f2ec85c85e33cf42ba21389dc33b105262c2f`;
the immutable stage media hash was
`1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.

## Source-owned result

- The report's original `54:01` `cmdGetNextCmd` continuation delivered the
  declared command (`first=1`, then normal copied DOS-record state).
- The default-off source observer in unchanged `demOpen` records a successful
  original open of exactly `O:\\NTVDM64\\LOADFIX.COM` (`phase=1`, `CF=0`).
- The same BOP stream then contains `50:42` (the original kernel-fast-read
  contract) and `50:02` (original close).  The selected user-mode profile
  retains the original `CF=1` fast-read fallback when it cannot use that
  contract; this observation's file lifecycle completed without adding a
  provider.
- The later original `54:01` has `first=0`, which is the COMMAND continuation
  request after the consumed one-shot command.  The app's explicitly declared
  session policy ends only at that next BaseVDM request, so the observed zero
  exit is not a host executable result or synthetic DOS return.

Together these markers prove one immutable original external `.COM` was
opened through the original DOS/DEM path and returned to original COMMAND
continuation.  They do not prove MZ execution, a general interactive command
producer, or a native-child route.

## S4 handoff

S4 uses the prevalidated original `FASTOPEN.EXE` MZ input in a newly created,
short executable-relative stage.  It must make one frozen observation and
require the original MZ file/seek/parent return path; no second `.COM` run is
permitted.
