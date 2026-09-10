# M0 T327 S3 — COMMAND configuration package and CPU40 owner transfer

## Scope

S3 selects one declared guest command, `COMMAND.COM /C EXIT`, solely because
the original COMMAND source defines its normal controlled-exit route as
`SVC_CMDEXITVDM (54:00) -> cmdExitVDM -> TerminateVDM`. This selection does
not claim that the guest command was delivered or executed. No BOP provider,
guest image, device result, CPU behavior, or command dispatcher was changed.

## Original configuration package

- Original `dos/command/cmddisp.c` maps `54:0C` to `cmdGetConfigSys` in
  `cmdconf.c`.
- `cmdGetConfigSys` calls original `ExpandConfigFiles(TRUE)`. Its original
  host provider is `softpc.new/host/src/nt_pif.c::GetPIFConfigFiles`; the
  selected no-PIF branch obtains the immutable staged `config.nt` through the
  existing app-selected DOS-media binding. It retains the original caller
  buffer, failure ordering, transformed temporary-file path, and fatal
  outcome.
- `cmdconf.c` deliberately keeps the historical `CHAR achSysRoot[64]` and
  treats a `GetShortPathNameA` result of 64 bytes or more as the existing
  `ED_BADSYSFILE -> TerminateVDM` failure direction. The first S3 stage had an
  81-byte DOS root, so it could not satisfy that source-defined prerequisite.
  The replacement fixed stage has an otherwise byte-identical 37-byte DOS
  root. This is a container/media-location correction, not a source relaxation
  or a synthetic configuration result.

## Fixed-container result

The fixed console-owning, non-debug x86 observer used the same selected media
manifest (`fixedMediaManifestSha256`
`b46ad6c47e692ba3b1213dd11b9c1a9b3c686410028c3d70e172086b26b62ce2`) at the
short root. Its durable BOP report is:

```
50:11 -> 50:3B -> 50:0F -> 50:1B -> 50:32 -> 54:05 -> 50:46 ->
50:0D -> 50:21 -> 54:0C -> 50:12 -> 50:00 -> 50:21 -> 50:42 ->
50:02 -> 50:21
```

The post-`54:0C` entries are the original DEM file operations used by the
configuration preprocessing path. This proves the source-defined
configuration package is no longer the immediate block. The report contains
neither `54:01` nor `54:00`; it therefore makes no guest-command-consumption,
`EXIT`, `EXEC`, PSP, or parent-return claim.

The process then exits with `0xc0000005` at address zero. Its recorded host
return offset is again `0x4fba8` from the product image base, the same original
CPU40/FDC `wait_int` continuation already classified by S2. The exact
successor is consequently the original SoftPC FDC/INT15/CPU40 wait chain, not
COMMAND configuration, BaseVDM command delivery, or a selected DEM leaf.

Raw result: `artifacts/research/m0-t327-s3-command-exit-short-root.txt`, its
`.bop.txt`, `.console.txt`, `.exception.txt`, and `.json` sidecars.

## Disposition

T327 exits by its second permitted criterion: one narrow, source-defined
owner transfer. The next package starts with the complete original CPU40
FDC/INT15 wait-continuation contract. It must not repair an individual BOP
observed in this run or turn the temporary observation-only BOP report into a
production route.
