# T225 S25 P3 — Startup-Machine Family Regression and Native Validation

## Scope

S25 validates the whole admitted startup-machine package: `02`, `06`, `12`,
`15:AH=88`, `17`, `5F`, `66`, and `C8`. `5E` remains the separately-owned
OpenNT top-level identity. This is a package validation result, not a claim
that every historical BIOS service or guest program is complete.

## Formal regression

A fresh MSVC x64 `/MT` Ninja root was generated at
`build/M0-T225-S25/startup-machine-r2`. The formal graph built the package
fixture and the fixture exited zero.

The one bound boot-namespace composition surface proves positive routes for
all eight members:

- `02` and `06` run the shared source-derived `unexpected_int` PIC/BDA
  suffix while retaining their distinct direct/frame preludes;
- `12` and `15:AH=88` return only their declared profile memory values;
- `17` remains the no-printer continuation;
- `5F` performs the bounded source-derived stream-state publication;
- `66` returns original no-EMS `BX=0`; and
- `C8` copies the declared four-byte INT 33 vector to the IVT.

The same fixture rejects a non-88h `15`, protected-mode `02`, and a
one-megabyte-aperture-invalid `C8`. It therefore does not promote the package
into a general BIOS dispatcher.

## Native validation

The same fresh formal root linked `bin/ntdos64-native.exe`. One Direct and one
Readonly run used the identical immutable
`build/M0-T225-S10/primary-bootstrap-inputs-r2` inputs, staged paired startup
sources, both existing BOP/#UD observers, and internal
`--instruction-tick-budget 100000`.

| Mode | Exit | BOP / #UD records | Terminal |
| --- | ---: | ---: | --- |
| Direct | 4 | 33 / 33, no overflow | internal budget |
| Readonly | 4 | 33 / 33, no overflow | identical internal budget |

The two stdout logs are byte-identical, SHA-256
`4512F3D7C428B3C919CA37D8A83998FE49796511D1AEC81FC7BE72FFD44D68D0`.
Both report `terminal=4`, `detail=0`, `lifecycle=5`, `presentation=5`, and
`cancellation=0`. Their last BOP is the existing `50:36`; no native
observation is admitted as a new implementation work item.

## Boundary result

The live Direct/Readonly outcome preserves profile-neutral machine behavior.
No new Bochs device, firmware table, host keyboard/mouse/printer/EMS service,
callback/TLS route, or host mutation capability was enabled.