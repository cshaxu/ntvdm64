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

The production-runner handoff was then rebuilt and exercised from current
source with MSVC x64 `/MT /W4 /WX`.  A v6 owned BYOB profile, four verified
three-byte test artifacts, explicit CONFIG and AUTOEXEC files, and a
test-only engine probe were used.  The parent process first removed both
startup-source variables.  The child probe received the paired nonempty
values together with the existing profile/root, normalized `C,D,E` include,
`E` exclusion, and encoded `2,1,c,082f6320736d6f6b65` launch plan, then
returned its deliberate success code `47`.  Thus the sources originate in
the CLI options rather than inherited ambient state.  The runner still does
not open either source file.

Adapter installation-time admission, bounded copying, provider bind, and
synthetic-byte removal are recorded in the successor S5 P4/P5 evidence; this
CLI proof does not claim a native startup run.
