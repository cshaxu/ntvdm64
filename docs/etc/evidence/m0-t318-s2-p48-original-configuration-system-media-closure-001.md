# M0 T318 S2 P48 — Original configuration system-media closure

## Source-shaped owner repair

P47 identified one complete original configuration cohort.  The repair keeps
its original owners and boundaries:

```
nt_pif.c::GetPIFConfigFiles
  -> cmdconf.c::{cmdGetConfigSys, ExpandConfigFiles}
  -> original COMMAND configuration guest flow
```

`cmdconf.c::ExpandConfigFiles` now obtains only its initial `%SystemRoot%`
value from the existing session DOS-media root binding
(`mvdm_softpc_dos_copy_root`).  It still performs the original
`GetShortPathNameA` normalization, source parsing, generated COUNTRY and
SHELL line order, temporary-file handling and fatal direction.  This is
registered as `MVDM-HOST-DIV-158`.  No BOP, guest loader, file-system
provider, command parser, or alternate DOS path resolver was introduced.

## Immutable package contents

The formal stage now places the selected source-shaped system-directory
payload beneath `dos/system32`:

- byte-identical original `COMMAND.COM`, `COUNTRY.SYS`, and `HIMEM.SYS`;
- already established source-built original `REDIR.EXE` and `DOSX.EXE`;
- byte-identical original `config.nt` and `autoexec.nt` at the DOS root.

`MSCDEXNT.EXE` is absent from every approved OpenNT baseline as a selected
runtime product.  Its AUTOEXEC line remains an explicitly unclosed guest
artifact disposition, rather than receiving a fabricated replacement.  It is
not an excuse to alter the current original configuration/COMMAND code.

## Verification

The formal one-time-MSVC Ninja graphs rebuilt the affected original
`cmdconf.c` and `nt_pif.c` units, their original archives, and
`original-softpc-process.exe` in both x86 and x64.  Existing historical
warnings remain visible; this change neither suppresses nor reclassifies them.

The one permitted x86 observation used newly linked code and the new immutable
`build/M0-T318/S2/runtime-r33-config-system-media` stage.  Its manifest
SHA-256 is `e40dd218968ddfa009a9751cd534b3e1a5a243b20c61b5d43fe2eeaa94bcd11b`.
The non-debug, console-owning launcher recorded the expected bounded
eight-second `timeout` (`0x53504354`) with no stdout or stderr.  The prior
immediate configuration terminal did not recur.

This stage is now the fixed runtime container.  Any next observation must
retain its media, firmware, arguments, console ownership and timeout, and may
replace only a newly linked product EXE.

## Boundary

The timeout is liveness evidence only.  It does not prove guest NTIO
reachability, NTDOS configuration completion, AUTOEXEC completion, COMMAND
startup, or ordinary child execution.  `MSCDEXNT.EXE` remains a named guest
media owner transfer; all other next work follows the layered source-first
continuity procedure.
