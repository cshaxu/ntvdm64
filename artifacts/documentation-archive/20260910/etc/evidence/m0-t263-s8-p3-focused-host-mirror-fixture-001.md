# M0 T263 S8 P3 — focused OpenNT host-mirror fixture closure

Date: 2026-08-24

## Scope

This is a focused closure check for the four S8 host mirror files. It does
not claim that every VDMREDIR product feature, including mailslots, remote
redirector requests or VDD-owned asynchronous completion, is recovered.

## Formal build record

- Fresh disposable formal graph: `build/M0-T263-S8/r001`.
- Generator: `tools/build/New-T260S8FullNinjaGraph.ps1 -Refresh`.
- Generated input SHA-256:
  `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`.
- Focused target:
  `bin/t263-s8-opennt-host-mirror-fixture.exe`.
- Formal Ninja compiled the four mirrored translation units, archived
  `lib/opennt-host.lib`, and linked the focused fixture.

## Fixture result

`tests/opennt-host/t263_s8_host_mirror_fixture.c` exited `0` and printed:

```text
T263 S8 OpenNT host mirror: utility, lifecycle and pipe paths pass
```

The fixture directly verifies:

1. `VrpMapDosError`, `VrpMapLastError`, and
   `VrpTranslateDosNetPath` from the restored `vrputil.c` body.
2. The retained static lifecycle gate in `vrinit.c`.
3. `VrIsNamedPipeName`, `VrAddOpenNamedPipeInfo`,
   `VrIsNamedPipeHandle`, `VrWriteNamedPipe`, `VrReadNamedPipe`, and
   `VrRemoveOpenNamedPipeInfo` against an ephemeral local public-Win32 named
   pipe.

The same fresh graph also rebuilt and ran
`bin/t237-s5-bop-nosupport-fixture.exe` with exit `0`. That fixture crosses
the BOP 59 route into the restored `nt_error.c` direct-access provider, so the
fourth S8 mirror is verified through its actual composed caller rather than
only as a translation unit.

The pre-existing broader `t251-s3-redir-ingress-fixture` reached these host
pipe operations before later exiting `8` in its unrelated mailslot regression.
That failure belongs to `opennt-bop/vdmredir/vrmslot.c`; it is neither hidden
nor treated as a pass for S8.

## Conclusion

The currently admitted source blocks in `nt_error.c`, `vrinit.c`,
`vrnmpipe.c`, and `vrputil.c` now have focused source-build/runtime evidence.
The deferred whole blocks remain owned by future Redirector/VDD admission and
must be restored through their original owner interfaces rather than added to
these synchronous host mirrors.
