# M0 T318 S2 P61 — BaseSrv first-command contract closure

## Scope

The admitted ordinary-child declaration must reach the unchanged guest
`cmdGetNextCmd` through the original Base VDM / `VDMINFO` contract.  This
record corrects two composition-side shapes which had diverged from the
original client/server pair; it does not add a BOP, guest loader or alternate
COMMAND lifecycle.

## Original evidence

- `src/mvdm-host/dos/command/cmdmisc.c::cmdGetNextCmd` creates the first
  request with `ASKING_FOR_FIRST_COMMAND`, obtains the DOS environment from
  guest `CMDINFO`, and then asks Base VDM for the command record with
  `Enviornment == NULL` and `EnviornmentSize == 0`.
- The original BaseSrv body,
  `O:/repos.external/opennt/base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand`,
  requires `CmdLine`, but treats `AppName`, `Env`, `PifFile`,
  `CurDirectory`, `Title`, `Reserved`, and `Desktop` as optional capture
  buffers.  It copies each only when both the source and caller buffer are
  non-null.
- The same original `cmdGetNextCmd` asserts that the supplied `CmdLine` ends
  in CR/LF, and separately receives the image path through `AppName`.

## Recovered composition

- `src/app/launch_declaration.c` now publishes
  `AppName=<session dos root>\\COMMAND.COM` and
  `CmdLine=/C VER\\r\\n\\0`.  It no longer duplicates `AppName` into the
  command-tail field.
- `src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c` now mirrors the
  original BaseSrv mandatory/optional capture-buffer rule.  A first
  `ASKING_FOR_FIRST_COMMAND` request may therefore omit `Env` while still
  receiving the declared command, image and current directory.
- `tools/build/New-T302BaseVdmLocalNinja.ps1` now links the test's actual
  current dependencies: `app/launch_declaration.c` and the original-Cdecl
  thread-start compatibility boundary.

## Verification

- Regenerated T310 formal graphs with the repository Node 22 runtime; both
  selected x86 and x64 `original-softpc-process.exe` targets are current.
- T302 Base VDM local fixture: x86 compile/link/run passes; x64 compile/link
  passes.  The fixture covers both the CR/LF/NUL launch declaration and an
  original first-command request with no host environment capture buffer.
- One permitted changed-product run in the fixed r33 container still exits
  `0xC0000005`.  This is not attributed to the now-corrected BaseSrv shape;
  it neither proves NTIO/NTDOS/EXEC continuity nor authorizes a new observer.

## Transfer

The next source audit remains the original guest `CMDINFO` consumer and its
mapped SCS/CMD buffers on the first COMMAND request.  The Base VDM command
record itself is no longer permitted as an unexamined substitute for the
original mandatory/optional capture semantics.
