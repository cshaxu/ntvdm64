# M0 T386 S3 — Original Base VDM environment recovery

## Question

Can immutable `COMMAND.COM` receive the historically projected environment by
directly using the original OpenNT `BaseCreateVDMEnvironment` body, rather
than by a project-authored environment limit or filter?

## Inputs

- Canonical original source: `base/win32/client/vdm.c`, functions
  `BaseCreateVDMEnvironment`, `BaseDestroyVDMEnvironment`, and
  `BaseGetEnvNameType_U`.
- Selected immutable `COMMAND.COM`, SHA-256
  `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
- CPU40/x86 formal product and fixed `O:\ntvdm64` media container.

## Procedure

1. Imported the three original Base-client functions as the selected true
   subset in `src/opennt-host/base/win32/client/vdm.c`.
2. Kept the original full-environment copy and its path-only projection:
   drive current-directory variables, `PATH`, `WINDIR`, and `SYSTEMROOT` use
   the original `GetShortPathNameW` logic; all other variables pass through.
3. Added only the current composition binding required because the CLI enters
   SoftPC directly instead of creating a separate `ntvdm.exe` child: install
   the original projected block before original MVDM startup and restore the
   app block after it returns.
4. Generated and built the formal CPU40/x86 Ninja target, then observed the
   immutable `ver` workload in the fixed console-owning non-debug container.

## Observation

The formal link completed. The fixed run exited `0x00000000` and recorded:

```text
MVDM-CMD-ENV svc=0F stage=0 ... bx=0010
MVDM-CMD-ENV svc=0F stage=1 ... bx=0148
MVDM-CMD-ENVSIZ ... value=0148
MVDM-CMD-CONT svc=01 stage=0 ... first=1
MVDM-CMD-CONT svc=01 stage=1 ... first=0
```

The same current environment without this Base prelaunch path required
`015F` paragraphs and overwrote the released `DS:203C` `EnvSiz` scalar during
the immutable retry. The original Base projection reduced that source-owned
requirement to `0148`, preserved the second-call scalar, and reached the
original `54:01` continuation. No guest byte, provider capacity result, MCB
layout, loader, or environment variable was filtered or capped.

## Interpretation

The selected recovery is the original OpenNT environment algorithm, not an
invented workaround. The only divergence is the bounded current-process
install/restore lifecycle: historical Base gives the result to a new child;
the CLI has no child boundary. This observation proves recovery through the
first original command continuation, but not yet visible execution of the
declared `ver` built-in; T386 remains active for that runtime proof.
