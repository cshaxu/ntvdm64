# M0 T318 S2 P47 — Original configuration-media dependency cohort

## Scope

P46 closes only selection of the original `config.nt` and `autoexec.nt`
inputs.  This audit follows their complete original `cmdconf.c` processing
cohort before another runtime observation.  It prevents a one-file-at-a-time
startup repair loop.

## Selected original dependencies

`cmdconf.c::ExpandConfigFiles(TRUE)` still obtains `achSysRoot` from the host
Windows directory, canonicalizes it with `GetShortPathNameA`, and writes both
the synthesized `country=...%SystemRoot%\\system32\\country.sys` line and the
source-shaped permanent `shell=%SystemRoot%\\system32\\command.com` line.
The exact original selected `config.nt` additionally contains:

```
dos=high, umb
device=%SystemRoot%\system32\himem.sys
files=20
```

The subsequent selected `autoexec.nt` asks for `mscdexnt.exe`, `redir`, and
`dosx` under that same historical system-directory convention.

This is one configuration-media/guest-path cohort, not six unrelated BOPs:

```
GetPIFConfigFiles -> cmdGetConfigSys -> ExpandConfigFiles
  -> short DOS-visible system root
  -> CONFIG.NT country/himem/command inputs
  -> AUTOEXEC.NT optional mscdex/redir/dosx inputs
```

## Current product evidence

The new formal stage contains original `config.nt` and `autoexec.nt`, but not
the complete contents implied by their source-shaped expansion.  Original
`COUNTRY.SYS`, `HIMEM.SYS`, and `COMMAND.COM` exist in the selected guest
mirrors.  Source-built `REDIR.EXE` and `DOSX.EXE` exist in the established
DOS output area.  A selected product `MSCDEXNT.EXE` has not yet been admitted
as a runtime media input.

The remaining problem is not merely copying names.  Replacing
`GetWindowsDirectory` with an arbitrary long executable-relative host path
would break the original `GetShortPathNameA`/DOS-visible pathname assumption.
The next correction must therefore define one app-selected, bounded
DOS-visible system-directory projection and stage its complete selected
original inputs.  It must retain the original PIF override, configuration
transformer, guest device/load ordering, and failure direction.

## Disposition

No additional runtime observation is admitted from the P46 product.  The P46
timeout only proves that the prior absent configuration-file terminal was
removed.  The next source change must close this entire media/path cohort,
then produce a new product EXE and a replacement immutable stage before the
single fixed-container observation.
