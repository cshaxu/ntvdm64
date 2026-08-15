# T204 S4 — contained configuration-provider core

## Result

S4 adds one `bx-vdm` session-owned, source-derived transform/image core:

- `bx_ntvdm_startup_configuration_input_v1` is a fixed-width copied record of
  the T204 S2 policy, copied OEM CONFIG/AUTOEXEC bytes and copied root/country/
  code-page facts.
- `bx_ntvdm_startup_configuration_provider_v1` owns generated CONFIG and
  AUTOEXEC images plus the double-NUL AUTOEXEC merge tuples.
- The sole build result is `READY`, `SOURCE_INVALID`, `BOUNDS`, or
  `CONSOLE_DEFERRED`.  Each non-ready result has no generated data.
- Clear zeroes the complete provider state.

The input and result carry no host path, host handle, guest address, BOP
selector, Bochs object or callback.  This is deliberately a same-island
transform provider, not an imported OpenNT build object.

## Source-derived behavior

The core implements the S3 `cmdconf.c` contract selected for this profile:

- CONFIG removes case-insensitive `country=` and `shell=` directives after
  leading whitespace, expands case-insensitive `%SystemRoot%`, normalizes
  retained nonblank lines to CRLF, then appends controlled country and shell
  lines.
- A stock COMMAND shell preserves a `/e:` option; a third-party shell becomes
  the explicit `CONSOLE_DEFERRED` result instead of silently initiating a
  console capability.
- AUTOEXEC retains its transformed lines while independently recording
  PROMPT, PATH and well-formed SET name/value data as a double-NUL merge
  sequence.
- Every source provenance uses the single mode copied from T201/T204 S2.
  Direct does not permit mutation of a source file; generated images remain
  session-owned in all four modes.

The provider does not yet publish an OEM guest path, use checked guest RAM,
replace the old boot namespace, apply merge tuples to COMMAND context, run a
trace, or handle `5E` UMB/DASD actions.  Those are later owner-package
integration work.

## Verification

The following was run from the repository root on 2026-08-15 and exited zero:

```powershell
cmd.exe /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I src\bx-vdm /Fe:artifacts\build\t204-s4-contained-configuration-provider-r1\bx_ntvdm_startup_configuration_provider_v1_test.exe src\bx-vdm\bx_ntvdm_mutation_profile_v1.c src\bx-vdm\bx_ntvdm_startup_configuration_policy_v1.c src\bx-vdm\bx_ntvdm_startup_configuration_provider_v1.c tests\bx-vdm\bx_ntvdm_startup_configuration_provider_v1_test.c && artifacts\build\t204-s4-contained-configuration-provider-r1\bx_ntvdm_startup_configuration_provider_v1_test.exe'
```

The focused regression checks exact generated CONFIG/AUTOEXEC bytes and merge
tuple layout; readonly, overlay and virtual policy copies; third-party shell
deferral; invalid input; output growth beyond the fixed image bound; and
zeroed teardown.  No host I/O or mutation occurs in the fixture.

## Next boundary

The next S must integrate this provider as one paired startup-configuration
package: contain and publish its generated image/path state, bind the paired
COMMAND `54:0C/0D` and existing context consumer only after the image is
ready, and remove the default dependency on fixed synthetic boot bytes.  It
must use the current checked guest-RAM mechanics and package regression; no
native trace is admitted until that complete integration passes.
