# T204 S5 P3 — CLI paired configuration-source admission

`ntdos64-run` now accepts `--config-source <path>` and
`--autoexec-source <path>` only as a pair for a BYOB DOS engine invocation.
It rejects either option for PE execution and rejects an unpaired DOS request.
For the child engine it replaces only these two values in the newly constructed
child environment:

- `NTDOS64_STARTUP_CONFIG_SOURCE`
- `NTDOS64_STARTUP_AUTOEXEC_SOURCE`

The runner does not open either file or retain its bytes.  Existing profile,
root, launch-plan and drive-policy propagation is unchanged.

The CLI source closure was rebuilt on 2026-08-15 with MSVC x64 `/MT`, `/W4`
and `/WX`; it includes the existing target-selection source plus `shell32.lib`
and `bcrypt.lib`.  The resulting executable is
`artifacts/build/t204-s5-cli-admission-r1/ntdos64-run.exe`.

The next P is adapter installation-time admission: validate the pair, read
bounded source data with normal user-mode APIs, convert/copy it into T204 S4
input, bind the provider, then remove the synthetic production default.
