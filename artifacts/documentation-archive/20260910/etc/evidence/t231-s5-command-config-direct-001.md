# T231 S5 OpenNT `cmdconf.c` Direct witness

## Question

Can the original OpenNT COMMAND configuration owner (`54:0C` and `54:0D`)
run as its own translation unit in the CLI composition, including generated
temporary files, OEM guest output and its historical terminal failure rule?

## Inputs and source-first closure

`src/bx-vdm/bop/opennt/command/cmdconf.c` is the direct mirror of OpenNT
`base/mvdm/dos/command/cmdconf.c`.  Its filtering, line ordering, country and
shell generation, AUTOEXEC environment extraction, temporary-file lifetime and
error call sites remain in that file.

The historical dependencies unavailable as a modern standalone closure are
concentrated in `bop/shim/command_config_shim.c`:

- `GetPIFConfigFiles` retains the original `nt_pif.c` source order: a selected
  PIF/session path first, otherwise the public system-directory `config.nt` or
  `autoexec.nt` default;
- the four RTL ANSI/Unicode/OEM routines retain the original conversion
  contract using public Win32 conversion APIs; and
- the pre-existing typed COMMAND boundary supplies the original 64-byte
  DS:DX result span and represents non-returning `TerminateVDM` as a controlled
  stop, rather than resuming after the original error site.

The only edits in `cmdconf.c` are inline-commented x64 build corrections:
`INVALID_HANDLE_VALUE`, explicit original-DWORD bounds at `strlen` and pointer
differences, and definite initializers required because modern analysis cannot
prove historical `TerminateVDM` is non-returning.  No CONFIG or AUTOEXEC
behavior was moved into the shim.

## Procedure and observations

The focused fixture compiled all admitted COMMAND sources and shims under MSVC
x64 `/W4 /WX /MT`, then ran in
`build/M0-T231-S5/command-config-r4/` with disposable host input files.

- a CONFIG input with an old `country=` and a `shell=` `/e:512` was processed
  by the original owner: old country setting was filtered, ordinary `device=`
  survived, and generated shell plus `/e:512` survived;
- AUTOEXEC input preserved its file lines and populated the original
  `lpszzcmdEnv16` multi-string extraction for `PROMPT`, `PATH` and `SET`;
- each generated temporary pathname was returned in the checked DS:DX 64-byte
  output span, and `DeleteConfigFiles` performed the original cleanup;
- a nonexistent selected config path reached `RcErrorDialogBox` then the
  original `TerminateVDM` call site; the boundary yielded typed controlled
  stop, never a false resume.

The fixture printed:

```text
T231 S5 direct OpenNT cmdconf config, autoexec, and terminal failure paths verified
```

## Scope

This proves local Direct source parity for `54:0C` and `54:0D`.  It does not
claim PIF parsing/selection lifecycle, `54:0F` environment initialization,
COMMAND launch, selector integration or native guest completion.
