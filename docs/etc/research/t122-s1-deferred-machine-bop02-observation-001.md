# T122 S1 deferred machine-BOP observation 001

The single frozen T121 derivative run used its deferred option-ROM profile,
one-million benchmark and 15-second watchdog. It records 31 BOP identities,
four commits, no selector `02h`, and no aperture panic.

The final observed DOS service is `50:36`, which resumes normally at RIP+4.
The following native `cpu loop quit, shutting down simulator` is the existing
FE/UNSIMULATE controlled stop, not an adapter/BOP failure. Thus this profile
has no current machine-BOP requirement for selector `02h`.

The direct observer still reports exit code 1 because it bypasses the CLI and
therefore does not provide the CLI-owned EOF stdin policy. This does not prove
real CLI-to-Bochs launch: Bochs rejects the runner's current non-Bochs argv.
