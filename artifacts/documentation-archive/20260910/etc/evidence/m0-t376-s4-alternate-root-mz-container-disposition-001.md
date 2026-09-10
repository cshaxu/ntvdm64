# M0 T376 S4 — Alternate-root MZ container disposition

## Result

S4 is closed as a **rejected observation container**, not as an MZ loader,
relocation, DEM, or parent-return result.

The only S4 run used the newly created `O:\ntvdm64-t376` package, current
formal product `original-softpc-process.exe`, and immutable original
`FASTOPEN.EXE`.  The console-owning observer ended after its fixed 8000 ms
budget with `exit=0x53504354`.  It retained no BOP continuation report, no
DEM-open sidecar, no DEM-seek sidecar and no terminal report.  Therefore no
source-owned MZ edge was reached that could distinguish media loading from
container startup.

## Exact invocation

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T375/S1/observer/console-startup-observer.exe \
  --product build/M0-T371/S2/formal-x86/original-softpc-process.exe \
  --stage O:\ntvdm64-t376 \
  --report O:\ntvdm64-t376\observation-t376-s4-fastopen.txt \
  --product-command FASTOPEN.EXE \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64-t376\observation-t376-s4-fastopen.txt.bop-return.txt
```

The alternate stage manifest hash was
`7bfc49d5e790f17dbac85b284a21078a9d5856204c3ec872b0402ac700dfaed6`.
This is evidence that the new root was not a valid substitute for the
user-selected fixed `O:\ntvdm64` runtime root; it does not prove why.

## Disposition

- No retry occurs in S4.
- No guest byte, loader, BOP, DEM provider, CPU, or device behavior changes.
- S5 owns a source-controlled extension of the established root.  It must
  prove every retained staged byte before adding the missing byte-identical
  `FASTOPEN.EXE` asset and must reject any mismatch.
- Only S6 may make the new, single, correctly rooted MZ observation.
