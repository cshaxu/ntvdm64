# Adapter Session Resource ABI v1

## Role

The existing CLI BYOB profile validates exact identities for `NTIO.SYS`,
`NTDOS.SYS`, and `COMMAND.COM`. Session resource ABI v1 is the pointer-free
adapter-side identity record for that already-admitted set.

It contains three fixed resource descriptors in canonical order, each with a
kind, byte count and SHA-256 bytes. It deliberately carries no path, HANDLE,
file cursor, payload buffer or host allocation.

## Boundary

The runner remains responsible for parsing the UTF-16 CLI arguments, opening
the caller-selected root, validating the manifest and reading an exact image.
The adapter may receive only the resulting identity and same-island staged
payload through a later explicit handoff. It must not enumerate a root, parse
guest paths, or reopen a file by name.

## Validation

The fixed resource set requires, in order, non-empty NTIO, NTDOS and COMMAND
identity records with zero flags. This prevents a first-profile session from
silently omitting an expected guest component or exchanging the roles.

The UCRT C11 fixture checks the 160-byte layout, absent-resource rejection,
valid canonical set, swapped role rejection, flagged-resource rejection and
null rejection.
