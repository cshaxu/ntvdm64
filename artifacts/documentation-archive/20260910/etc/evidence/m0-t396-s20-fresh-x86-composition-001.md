# M0 T396 S20 — fresh x86 composition 001

## Scope and method

S19 found that the historical v4 test image mixed a `session.obj` that wrote
`0x1270` bytes with an `entry.obj` that reserved the older `0x126c` caller
region.  S20 rebuilt the complete generated x86 graph from the current source
snapshot in a new `build/M0-T396/S20/formal-x86` root.

The interactive host suspends Ninja after completed child waves.  The S20
disposable runner therefore compiled bounded waves and required every requested
COFF output's size to remain unchanged for 1.5 seconds before ending its own
job.  The first runner that merely checked file presence was rejected after
`lib.exe` reported `LNK1136` on `unexp_nt.obj`; its entire root was removed.
The retained root contains 438/438 stable objects.  All 36 generated archive
commands then succeeded, the generated ROM resource compiled, and the exact
generated non-`/FORCE` process link produced:

```text
build/M0-T396/S20/formal-x86/original-softpc-process.exe
SHA-256 EF60D6A993041DF34635C7C867E53D560F624190DE2136CC857AA94399DECF44
```

## ABI result

The fresh map places `main` at `004013e0` and `session_initialize` at
`0040e8f0`.  Its disassembly supplies `0x1268` to `memset`, writes ABI version
6 and records `struct_bytes=0x1268`; this agrees with the rebuilt current
caller layout.  The historical four-byte v4 object mismatch is absent.

## Product observation

The fresh product was staged through the existing `O:\t396` manifest with the
selected WOW32 companion, pure-DOS profile, derived Kernel31 contract and
`system32\WOWEXEC.EXE`.  The ordinary Console-owning observation was:

```text
result=exited
exit=0xc0000409
elapsed-ms=7187
loaded-module-count=15
loaded-wow32=no
```

Thus fresh composition removes the identified stale ABI mix but does **not**
by itself advance the actual WOWEXEC route.  It does not establish a WOW32
load or a guest failure.  A debugger-only contrast changes the terminal to
`0x78`; it is diagnostic-only and cannot replace the normal product result.

## Disposition

S20 closes the formal-composition repair boundary.  The next audit must start
from this fresh product's ordinary `0xc0000409` terminal, distinguish it from
the rejected v4 cookie attribution, and identify one current source/map owner
before any behavior change.
