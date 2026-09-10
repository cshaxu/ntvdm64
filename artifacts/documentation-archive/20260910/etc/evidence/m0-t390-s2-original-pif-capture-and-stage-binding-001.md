# M0 T390 S2 — original PIF capture and pure-DOS stage binding

## Result

The explicit `ntvdm.exe command.com` profile now uses the existing original
PIF configuration carrier end-to-end.  It no longer reaches the current
default `AUTOEXEC.NT -> REDIR.EXE -> DOSX.EXE` path during the bounded
observation.

## Recovered record contract

The first S2 implementation copied `PifFile` only as far as the local BaseVDM
record.  The fixed-container observer then reported `pif-bytes=0`; audit
identified the missing middle copy in `base_vdm_broker` / its fixed-width
`broker_base_vdm_record`.

The final S2 path is now:

```text
app explicit COMMAND declaration
  -> base_vdm_command.pif / pif_bytes
  -> broker_base_vdm_record.pif / pif_bytes
  -> base_vdm_local.pif / pif_bytes
  -> original VDMINFO.PifFile / PifLen (ASKING_FOR_PIF)
  -> original GetPIFData / GetPIFConfigFiles / ExpandConfigFiles
```

Every hop carries a bounded NUL-terminated ANSI host path.  It is neither a
guest value, mapping-manager identity, native handle nor raw pointer.  The
only new behavior is an adapter's copied original BaseSrv capture; PIF parsing
and configuration preprocessing remain in original MVDM code.

## External immutable profile

The user-authorized product stage contains:

```text
O:\ntvdm64\profiles\pure-dos\pure-dos.pif
O:\ntvdm64\profiles\pure-dos\config.nt
O:\ntvdm64\profiles\pure-dos\autoexec.nt
```

`pure-dos.pif` is the selected original OpenNT 4.5 `_default.pif` binary
shape with only its two NT PIF configuration pathname fields changed to the
two named profile inputs.  The original template SHA-256 is
`75caf5aca5b155afbd4ce7effe825fe1bf4767a0afee64a8d726c5e9694fa33a`.
The external profile manifest records the resultant hashes.  The copied
`config.nt` retains the original-style HIMEM setup.  The profile `autoexec.nt`
contains only original DOS batch syntax and deliberately omits optional
`MSCDExNT`, `REDIR` and `DOSX` startup programs.  The default root pair was
not overwritten.

## Formal and fixed-container evidence

The formal CPU40/x86 Ninja target linked successfully after the complete
record fix:

```text
ninja -C build\M0-T389\S6\formal-x86-r2 -j 8 original-softpc-process.exe
```

The resulting staged product SHA-256 is
`79855efa812d50d5f9cd70ad40b3009fb43c4123bfbdedd8e5f634a3e05db2b8` and
was also published as `build/output/ntvdm64-0243.exe`.

The fixed non-debug console-owning command used the exact declared product
target `command.com`, a 15-second bound and a default-off original DEM-open
observer.  Its BaseVDM report contains:

```text
state=0104 ... pif-bytes=42 status=00000000
state=0000 ... pif-bytes=42 status=00000000
```

The original DEM-open trace reached `HIMEM.SYS`, `COUNTRY.SYS` and
`COMMAND.COM`; it contains no `REDIR.EXE` or `DOSX.EXE` open.  The container
still timed out, so the evidence proves only first-shell configuration
selection and predecessor removal, not a banner, prompt, DOS `CON` wait or
interactive completion.

## S2 exit assessment

S2 is complete.  The remaining work is to prove the selected PIF's original
failure/fallback behavior and then use this stable pure-DOS path for the
second-COMMAND runtime predecessor observation.  No guest modification,
synthetic command, keyboard event, BOP behavior or CPU change has been made.
