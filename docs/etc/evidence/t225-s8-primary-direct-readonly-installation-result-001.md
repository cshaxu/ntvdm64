# T225 S8 Primary Direct/Readonly Installation Result 001

The four manifest-bearing primary stages (NTIO, NTDOS, COMMAND, SHARE) were
staged by `Stage-T225S8PrimarySourceBuiltInputs.ps1` into
`build/M0-T225-S8/primary-inputs-r2`.  The formal Ninja target
`bin/t225-s6-four-image-profile-fixture.exe` was built under MSVC x64 `/MT`.

With the staged `fixture-config.nt` and `fixture-autoexec.nt` supplied through
the declared environment inputs, the fixture returned zero: Direct accepted,
Readonly accepted, and deferred Overlay plus retired Virtual each rejected at
installation (`stage=-1`).  No guest CPU loop or guest trace was run.

The runtime profile identity intentionally matches the existing v8 fixture
contract; primary provenance remains separately enforced by
`primary-source-built-manifest.json`, whose four inputs name only the new
original-toolchain source-build stages.
