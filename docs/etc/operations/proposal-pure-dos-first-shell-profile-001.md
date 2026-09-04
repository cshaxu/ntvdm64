# Pure-DOS first-shell profile

## Purpose

Recover a source-shaped first-shell configuration disposition before proving
the explicit second `COMMAND.COM` interaction profile. The current staged
default `AUTOEXEC.NT` launches `DOSX.EXE`; that reaches the DPMI/WOW bootstrap
and is a distinct predecessor from the pure-DOS `COMMAND.COM`/DOS-CON path.

This package selects a named, immutable pure-DOS configuration pair without
modifying guest `COMMAND.COM`, NTDOS or NTIO and without adding an app-owned
DOS parser or keyboard producer.

## Original carrier and boundary

`softpc.new/host/src/nt_pif.c:GetPIFConfigFiles` is the retained original
configuration carrier:

* without a PIF override it resolves the default `config.nt` or `autoexec.nt`;
* with the original NT PIF extension, `GetPIFData` supplies
  `achConfigFile` and `achAutoexecFile` into `pchConfigFile` and
  `pchAutoexecFile`;
* `dos/command/cmdconf.c:ExpandConfigFiles` consumes that selected pathname,
  retains the source preprocessing/error ordering, and builds the guest-side
  configuration input.

The product may bind an explicitly selected pure-DOS launch profile to this
existing carrier only after S1 proves the exact PIF/session disposition. It
may not replace `ExpandConfigFiles`, patch an active default file in place,
or claim PIF compatibility before the selected original fields and failures
are recovered.

## Work sequence

### S1 — Configuration-carrier and stage audit

1. Trace the original PIF/default selection through `GetPIFData`,
   `GetPIFConfigFiles` and `ExpandConfigFiles`.
2. Identify the smallest original-format pure-DOS `config.nt` and
   `autoexec.nt` inputs which do not launch `DOSX.EXE`, and record their
   origin, content, hashes and required companion media.
3. Define the app-to-original-carrier boundary for only the explicit
   interactive profile; preserve normal default and direct-target behavior
   until a later package proves their own disposition.
4. Record the selected failure behavior for an absent, malformed or too-long
   profile configuration reference.

### S2 — Source-shaped binding and media manifest

Bind the explicit profile to the audited original configuration carrier;
stage named immutable pure-DOS assets under the product root through the
normal package/media manifest. Keep the default pair untouched. Any
non-original binding code belongs in the relevant app or adapter component,
not in guest code.

### S3 — Formal product and focused configuration proof

Build the selected CPU40/x86 formal product, prove that the selected paths
are passed to the original preprocessing body, and prove that `DOSX.EXE` is
not selected by the explicit pure-DOS profile. Exercise original missing-file
failure semantics.

### S4 — Fixed-container runtime predecessor proof

Use one stable non-debug console-owning container and immutable media to show
that the first resident shell reaches the one normal `COMMAND.COM` child
record without entering the DPMI/WOW bootstrap.

### S5 — Second COMMAND console proof

The earlier explicit-interactive packet established the original second-shell
contract but did not prove it at runtime.  Keep this prerequisite package open
through one source-shaped completion step: prove the selected child reaches
the original banner/prompt and DOS `CON` wait in the same fixed container, and
that a normal Console key follows the original Console worker, 8042/ICA,
BIOS and DOS path.  This is not permission for an app line parser, synthetic
BOP, direct guest-buffer write, guest-binary change, or CPU workaround.

## Acceptance and exclusions

Completion requires source/ABI proof, formal-link evidence, fixed-container
predecessor observation, and source-shaped second-shell banner/prompt/DOS-CON
input evidence. It does not claim DOSX/WOW completion or general PIF product
compatibility. Any need for an invented PIF reader, a host prompt loop, a
guest binary change, synthetic BOP/keyboard input, or a CPU workaround is
outside this package.
