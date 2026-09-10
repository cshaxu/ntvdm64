# OpenNT NLSFUNC Source-Closure Record

## Scope

This record covers `base/mvdm/dos/v86/cmd/nlsfunc`, whose original makefile
declares `nlsfunc.exe`. It records source and EN-US message closure plus a
toolchain blocker; no `NLSFUNC.EXE` candidate was produced.

## Closed Inputs

The disposable stage `toolchain-probe/nlsfunc-stage-v1` copies the tracked
command directory, `v86/inc`, and `v86/messages` trees without modifying the
OpenNT checkout. The source-built local `buildmsg-host.exe` processed the
tracked `messages/usa-ms.msg` and `nlsfunc.skl`, creating:

```text
NLSFUNC.CL1
NLSFUNC.CL2
NLSFUNC.CLA
```

These are the three generated files consumed by the `msg_services` invocation
in `nlsfunc.asm`. The makefile's `nlsfunc.ctl` dependency is a historical
generation marker; it is not itself included by either assembly module.

## Assembly Results

The original link shape is `nlsfunc.obj + nlsparm.obj`. Neither object can yet
be generated with the available compatibility assemblers:

| Tool | `nlsparm.asm` | `nlsfunc.asm` |
| --- | --- | --- |
| JWasm 2.21 | rejects `HEADER` at line 103 | rejects `$` structured macros in `struc.inc` |
| UASM 2.53 | rejects `HEADER` at line 103 | rejects the same `struc.inc` macro control flow |
| Microsoft MASM 14.43, `/omf /Zm` | rejects `HEADER`; later reaches an operand-size incompatibility in `parse.asm` | rejects the same `struc.inc` macro control flow |

The failure is therefore not a missing source file, missing EN-US resource, or
an accidental JWasm-only behavior. `HEADER` and the `struc.inc` `$Push`,
`$TopTest`, and related macro language must be handled by a compatible
historical assembler or a separately verified source transformation before a
link attempt is meaningful.

## Historical Preprocessor Result

The source itself identifies the missing tool more precisely than the
assembler diagnostics do. `nlsparm.asm` states that it must be processed by
the ASMUT preprocessor as:

```text
SALUT NLSPARM,NUL;
```

The NT4.5 and XP V86 make rules independently preserve the same
`salut $*.sal,nul,$*.asm` preprocessing step. No `salut`/ASMUT executable
or source was found in the available fixed inputs or local NT4.5/NT5/XP
reference trees.

Consequently, `struc.inc` is not a candidate for a small MASM syntax patch:
its dollar-prefixed structured-flow macros depend on the historical
preprocessor's expansion and label-management model. Hand-translating those
macros would create a new DOS build frontend, rather than reproduce the
recorded source build. The only admissible unblock is an owner-supplied
historical SALUT/ASMUT input, or a separately proven source-to-source
transformation with instruction/layout equivalence evidence.

## Qualification

No macro replacement, stub, or alternate parser provider is introduced in this
stage. A future build must record an exact historical assembler provenance or
demonstrate an instruction-level-equivalent transformation, then separately
validate NLSFUNC's `INT 2Fh` multiplex behavior, COUNTRY.SYS loading, and error
paths through a fixed research profile.
