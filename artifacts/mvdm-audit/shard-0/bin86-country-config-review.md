# Shard 0: COUNTRY.SYS and SYSTEM.INI package identity

At this read-only observation, `bin86/country.sys` matches the frozen
OpenNT-4.5 source identity `BA263D4276024A904ECE68FE193F8CA6DA928DC9B35F8948F69D458DBB6AC3D0`;
the frozen OpenNT edition has no corresponding file. No byte-identical member
was found in `assets/winnt.zip`. `O:/winnt/system32/COUNTRY.SYS` is present
but has SHA-256 `9DDF2B738307B5950699513E6B8221E2044BB686487A8C216AF7C6FD3F853B54`.
The source package, installed version, active DOS configuration and supported
country tables require further tracing; different bytes alone do not prove a
failure or missing locale functionality. This source remains pending.

`bin86/system.ini` matches both original editions at SHA-256
`6F533CCC79227E38F18BFC63BFC961EF4D3EE0E2BF33DD097CCF3548A12B743B`.
No byte-identical archive member was found. The user-owned
`O:/winnt/system.ini` is present with different bytes, while the direct
system32 path is absent. Determine each actual launch profile and consumer;
do not overwrite a user configuration or infer that it is unused. This source
remains pending.

These observations are point-in-time file comparisons, not evidence of the
runtime search path, application behavior or source-to-binary provenance.
No guest, configuration or installed file was modified.
The [observer](bin86-country-config-observer.py) and
[receipt](bin86-country-config-observations.json) retain the comparison.

## Follow-up: selected package paths

`tools/build/Stage-OriginalSoftpcRuntime.mjs` selects the separate
`dos/v86/dev/country/COUNTRY.SYS` for `system32/COUNTRY.SYS`. At this
observation its SHA-256 is `9DDF2B738307B5950699513E6B8221E2044BB686487A8C216AF7C6FD3F853B54`,
matching the installed system32 copy, not the `bin86` file. Original
`dos/command/cmdconf.c` constructs the country entry using the user locale,
OEM code page and selected root `system32`; the BIOS parses that entry.
These edges explain the selected default input but do not validate all locales
or user-supplied configuration. The frozen bin86 file remains pending until
its original build and packaging role is traced.

For `system.ini`, `mvdm_softpc_system_find_file` selects the session MVDM root;
WOW profile access creates a session-owned shadow of that file to avoid host
SYSTEM.INI profile redirection. `app_package_layout` checks the root profile.
This identifies the actual profile source; it does not establish that the
frozen bin86 sample is the selected configuration, or prove arbitrary profile
updates and teardown. Keep it pending pending original packaging-role review.
