# T180 S1 SHARE direct observation prelaunch result 001

T180 created a fresh evidence root with the exact T177-recovered SHARE.EXE,
immutable v4 profile, retained boot inputs, ROMs and configuration. All copied
identities were recorded in preflight.json. The sole direct-controller
invocation stopped before ProcessStartInfo construction: PowerShell rejected
the controller assignment to $pid because $PID is its read-only automatic
variable.

No Bochs process, bochs.log or observation.json was created. Therefore this is
a controller prelaunch failure, not a SHARE load, DOS termination, BOP, 54:11
or continuation result. T180 did not retry. T181 is limited to renaming that
controller-local field and adding a static guard against automatic-variable
collisions before any future runtime admission.

