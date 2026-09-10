# T182 S3 finite-sequence build and observation disposition 001

Date: 2026-08-12  
Packet: M0 T182 S3  
Disposition: the one permitted build linked; the observation was not started.

## Build result

The exact r2 MSVC x86 command completed once:

```text
nmake /nologo /f ntdos64-t182-finite-sequence.mak ntdos64-t182-finite-sequence.exe
```

It compiled the current closure, including all six replacements and
`cli\\byob_launch_plan_v2.obj`, then linked
`ntdos64-t182-finite-sequence.exe`. Its log is retained at
`artifacts/build/current/t182-s2-finite-sequence-r2/t182-s3-build.log`.
No Bochs object or archive was built. The inherited link emitted only the
pre-existing duplicate-`main.o` and CRT-model warnings; the process exit was
zero.

An earlier outer-shell quoting error did not enter `cmd.exe`, create a build
log, or invoke a compiler/linker. It is not a build attempt. The command above
is the sole actual T182 build.

## Observation disposition

The current v5 runner requires `ntdos64-bochs-engine` to receive BYOB
profile/root/plan metadata and construct the native Bochs command with its
fixed bundle. The T181 MinGW build contains `ntdos64-run.exe` but not this
shim; no retained shim/bundle executable was found in the artifact tree.

Passing BYOB arguments directly to the newly linked native Bochs image would
cross the CLI/Bochs boundary and cannot be called a v5 CLI observation.
Building a new shim after the one permitted engine build was not admitted by
this packet. No guest process, BOP, `54:01`, `QUIT.COM`, controlled stop,
normal return, or DOS-result observation is claimed.

## Transfer

The next candidate is a bounded CLI shim/bundle composition package: build a
current `ntdos64-bochs-engine` bundle bound to the T182 engine, fixed ROM and
configuration inputs, and the current v5 runner. Only that packet may admit a
finite-sequence observation.
