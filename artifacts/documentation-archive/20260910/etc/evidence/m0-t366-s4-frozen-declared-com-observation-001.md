# M0 T366 S4 — Frozen declared `.COM` observation

## Fixed invocation

One console-owning, non-debug CPU40/x86 observation used the existing fixed
stage and its existing immutable `COMMAND.COM` child exactly once:

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T366/S4/observation/console-startup-observer.exe \
  --product build/M0-T365/S2/x86/original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report O:\ntvdm64\observation-t366-s4.txt \
  --product-command "COMMAND.COM /C EXIT" \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t366-s4.txt.bop-return.txt
```

The observer returned its controlled eight-second timeout (`0x53504354`) with
no product exception. The fixed-stage manifest hash was
`d58a396332401eedc0e482331f5bb1225290d7e398ba5dcf1370ad84b14abdaa`, fixed
media hash was
`43f09b928f459fbca4c26a6cd6a24f08faf23e74c5ea3a7c291c955828de1cfb`, and
the staged product hash was
`eb7214703f7ec9562a84dcba37d9ed5b17c9b194cc9eeac3be7098cb34a5d43d`.

## Observation

The recovered record reached original `54:01` with `first=1`, then continued
through original COMMAND initialization (`54:02`, `54:0D`, `54:0F`) and the
original DEM open boundary. The retained `dem-open` sidecar includes successful
phase-1 opens of the immutable staged path:

```text
path=O:\NTVDM64\SYSTEM32\COMMAND.COM
```

The same sidecar also records normal configuration/host files and later DOSX/
REDIR discovery. It does not show a synthetic path or a new guest artifact.

Before the controlled timeout, the BOP sidecar reaches another original
`54:01` with the local DOS record no longer available. There is no DEMREAD
sidecar, `$Exec` completion marker, child exit or parent-return report.

## Interpretation

T366 proves the formerly omitted app record discriminant unblocks the intended
original COMMAND → DEM file-open direction for an existing `.COM` image. It
does not prove that the child executed or returned. The next owner is the
already queued DOS `.COM`/MZ execution and parent-return package, beginning
with the original `$Exec`/JFN/SFT/PSP transition—not a BOP leaf repair.

No second S4 observation is admitted.
