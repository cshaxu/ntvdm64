# M0 T318 S2 P73 — short-root COMMAND attribution boundary

## Purpose

P72 repaired the Base VDM host-current-directory carrier and observed the
original COMMAND bootstrap ingress.  This P tests whether the subsequent
short-root failure belongs to the admitted `--ordinary-child` declaration or
to its eventual `EXEC`/parent-return path.

No product source, guest media byte, formal product EXE, console owner, or
observer implementation changed for this check.  The only container variant
is an equivalent disposable stage rooted at
`build/runtime-t318-cpu40`, so that the original COMMAND 64-byte boot-file
carriers have a representable path.

## Paired observation

The fixed non-debug, console-owning observer ran the same staged x86 CPU40
product twice:

- `-f -o --ordinary-child`; and
- `-f -o` only.

Both runs exited with `0xC0000005` within the fixed eight-second window.  The
no-declaration row still records the same original bootstrap sequence through
DEM `50:11`, `50:3B`, `50:0F`, `50:1B` and COMMAND `54:05`.  It therefore
rules out the app-declared `/C VER` input as the cause of this short-root
failure.  It does not establish whether the selected screen/console capture
contains a later COMMAND service: its trailing bytes are not a stable
service record.

The raw paired records are retained without interpretation:

- `artifacts/research/m0-t318-s2-p73-short-root-command-config-observation.txt*`;
- `artifacts/research/m0-t318-s2-p73-short-root-no-ordinary-child-observation.txt*`.

## Source boundary

The reached `54:05` is original `cmdSetInfo`; it records the guest `SCSINFO`
location before any declared child is consumed.  The original NTDOS bootstrap
later calls `54:0C` to obtain the preprocessed CONFIG file.  Neither service
is `54:01`, `54:08`, `50:36`, `50:3C`, or `54:0B`.

The original top-level exception filter has no captured frame for these rows,
so the access violation remains unattributed.  Assigning it to
`cmdGetConfigSys`, a child `EXEC`, Base VDM command transport, or parent
return would be speculation.  No BOP route, COMMAND workaround, guest loader,
or replacement executor is admitted by this P.

## Disposition

The ordinary-child lifecycle remains unobserved.  The next recovery unit must
be a complete original post-`cmdSetInfo` / early guest-bootstrap owner cohort,
with a source-defined failure or transfer; it must not be a trace-driven
`54:xx` patch.
