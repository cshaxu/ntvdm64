# M0 T387 — Positional COMMAND CLI recovery

## Purpose

Make the ordinary single-process product surface accept an unadorned DOS
command line:

```text
ntvdm.exe "echo hello"
ntvdm.exe dir
ntvdm.exe program.com arg1
```

The app must convert that declared input to the same original Base VDM
`VDMINFO.CmdLine` shape already used for `--command`; it must not make
`COMMAND.COM` parse host argv or bypass the original COMMAND/BaseClient path.

## Ordered subtasks

1. **S1 — Positional declaration and runtime proof.** Define one unambiguous
   app-owned positional-command rule; preserve explicit `--command <text>` as
   a diagnostic-compatible spelling; keep recognized original SoftPC switches
   in the argv passed to the original entry; translate the positional command
   only at the existing app declaration boundary; run the fixed product with
   positional `echo` redirection and publish a candidate only if it improves
   the prior output.
2. **S2 — Installed-root publication and no-argument disposition.** Publish
   the formally linked executable both as a versioned review/archive copy in
   `build/output` and as the selected `ntvdm.exe` plus versioned copy in the
   existing runnable `O:\\ntvdm64` root, where its immutable sibling media
   already lives.  Classify no-argument launch as an interactive session, an
   explicit user-visible unsupported mode, or another source-owned terminal.
   Do not silently exit.
3. **S3 — Closure review.** Review positional/explicit equivalence, argument
   quoting, original SoftPC switch preservation, guest identity, package
   contents and actual fixed-container result.

## Boundaries

This task owns only app CLI declaration and runnable-package admission. It
does not implement external `.COM`/MZ execution, `cmdExec32`, interactive
keyboard input, BaseSrv cross-process transport, WOW/Win16, guest changes or
SoftPC parser changes.
