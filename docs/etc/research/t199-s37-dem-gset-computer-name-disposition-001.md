# T199 S37 DEM GSET Computer Name Disposition 001

## Question

How should `50:41 demGetComputerName` behave in a non-invasive CLI profile
that has no explicitly configured guest machine identity?

## Source

OpenNT `src/opennt/base/mvdm/dos/dem/demgset.c` calls
`GetComputerNameOem`. On success it writes a padded 15-character OEM name plus
a NUL and sets `CX=01ff`. On failure it writes one NUL at `DS:DX` and clears
only `CH`; it does not set CF in that branch.

## Disposition

The active CLI admission grammar carries no stable machine-name field. Reading
the live host machine name at BOP time would create ambient-host policy and
make the guest result non-reproducible. The provider therefore implements the
source's `GetComputerNameOem` failure branch as the declared no-identity
capability:

- checked one-byte zero write at `DS:DX`;
- `CH=0`, while `CL` and flags remain unchanged;
- four-byte BOP continuation;
- no host API call, path, handle, or host name retained by bx-vdm.

`artifacts/build/t199-s37-dem-package-r42/` contains the source-built MSVC
x64 `/MT` family regression. Its focused assertion starts with `CX=a5b7`,
proves `CX=00b7`, verifies the zero byte at the translated guest address and
confirms no flags delta.

## Follow-up

If a future CLI contract admits a configured stable guest identity, it needs a
separate pre-bind immutable snapshot and a success-path regression. It must
not change this profile into an ambient host-name projection.
