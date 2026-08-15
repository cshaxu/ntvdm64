# T202 S6 COMMAND host processor context

## Question

How can `54:0A` obtain the historical host COMSPEC input without recapturing
ambient process state from a BOP handler or silently replacing it with the
guest `COMMAND.COM` image?

## Implementation

The copied COMMAND host context now has an optional bounded OEM processor
path.  CLI composition admission captures the normal user-mode `ComSpec`
environment value once, converts it to OEM, and copies it into this context.
The general COMMAND environment snapshot continues to omit COMSPEC, so a BOP
cannot recover it from ambient process environment data.  If no valid host
processor exists, the context remains valid but has no processor capability.

This is an explicit direct-host capability input, not a guest path, raw
pointer, handle, child process or installation change.  It gives the future
typed `54:0A` lifecycle a source-equivalent host command processor input
without changing its currently deferred outcome.

## Verification

The COMMAND fixture sets a known processor path, proves its bounded copy, and
passes under MSVC x64 `/MT`:

`artifacts/build/t202-s6-command-host-context-processor-r4/`

Its manifest reports `linkExitCode: 0`, `runExitCode: 0`, and `passed: true`.
The runtime composition translation unit was additionally compiled with:

```text
cl.exe /nologo /std:c11 /W4 /WX /MT /I src /I src\bx-mantle /I src\bx-vdm /I src\cli /c src\bx-vdm\bx_ntvdm_composition_runtime_v1.c
```

The object is retained at
`artifacts/build/t202-s6-command-host-context-processor-r3/`.

## Interpretation and follow-up

No `CreateProcessW` call is made by this change.  The remaining work is one
typed child state machine which receives only copied command/environment data,
validated opaque stream tokens, and this copied processor capability.  It
must preserve source-shaped terminal-versus-reentry result states before
direct-profile child execution can be admitted.
