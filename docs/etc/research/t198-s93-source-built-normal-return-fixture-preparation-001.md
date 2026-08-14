# T198 S93 source-built normal-return fixture preparation 001

## Question

Can the native composition fixture replace its synthetic COMMAND/target bytes
with the declared source-built normal-return inputs without executing a guest
or widening any runtime boundary?

## Procedure

`New-T198S93SourceBuiltNormalReturnFixtureInputs.ps1` reads only the locked
COMMAND.COM and TARGET.EXE/SHARE inputs from the retained v6 root, verifies
their byte counts and SHA-256 values, and emits compile-only C++ arrays. The
preparation script also writes a fresh v7 manifest in its build directory and
validates it against the unchanged v6 artifact root. It then compiles:

1. the profile-selection witness with the existing CLI sources and `/MT`;
2. the native fixture with `T198_S93_SOURCE_BUILT_NORMAL_RETURN`; and
3. the two generated array translation units.

It deliberately performs no full link and no guest execution.

## Result

`artifacts/build/t198-s93-source-built-normal-return-prepare-r3` records:

| Input | Identity | Generated symbol |
| --- | --- | --- |
| COMMAND.COM | 50,384 bytes, `908a77ac…c732c43` | `t198_s93_command_bytes` |
| TARGET.EXE / SHARE.EXE | 882 bytes, `69dabbdb…3996fc` | `t198_s93_share_bytes` |

The exact v7 profile pair accepted against the existing root with
`TARGET.EXE` and `guest_display_state: stream-io-v1`. This is not a disguised
v6 default: the parser supplies the value only for the explicit v7 declaration.
The compile log proves the fixture's source-built conditional binds both new
arrays and `\\TARGET.EXE`; the synthetic `{90,C3}` / `{F4}` arrays remain only
in the default historical startup-fixture branch.

## Boundary and exclusions

No OpenNT or Bochs source changed. No BOP provider, guest-memory operation,
machine feature, link, CPU loop, guest run, terminal status, or host
filesystem capability entered the work. The new profile manifest is generated
evidence in the build directory and validates the unchanged source-built root;
it is not a guest artifact or a default runtime input.

## Follow-up

S94 may separately admit one full current-object relink and bounded execution
of this exact source-built-input fixture. Its observation contract must record
the ordered markers `50:3B`, `50:0F`, `54:0C`, `54:01`, `50:3C`, and `54:11`,
must stop on any unimplemented service, and must not add a provider in response
to a marker.
