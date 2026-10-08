# Proposal: installed Windows 3.1 portable launch repair

## Objective

Make an already installed, otherwise clean Windows 3.1 tree portable for this
project without requiring the original Setup workflow to run again. The work
is deliberately split into two tools:

- `win31-launch` owns only the installation-local compatibility `PATCH`,
  accepted add-ons/recoverable images, and explicit `WINSTD`/`WIN386` profiles.
- `win31-path` owns discovery and repair of relocatable Win3.1 system-path
  references.

## Product boundary

This is an installed-tree repair, not an installer and not a Windows 3.1
compatibility claim. It must never mutate supplied retail media. It must retain
the original `KRNL386.EXE` and `WIN386.EXE` before installing the two existing,
identity-bound approved candidates. The tool uses released `MOUSE31.DRV` only.

The normal original Setup post-copy protected-mode transition remains a
separate queue-tail package. A successful portable launch cannot be cited as a
Setup pass; a Setup repair cannot be silently folded into this tool.

## Intended behavior

1. Both tools accept an explicit installed-root path and discover the existing
   Win3.1 system layout from required files, not fixed drive letters.
2. Both reject absent, malformed, linked, or identity-incompatible
   installations without changing them.
3. `win31-launch` creates/updates an owned `PATCH` directory with shared
   `CONFIG.NT` and `AUTOEXEC.NT`, and explicit `WINSTD.CMD/.PIF` plus
   `WIN386.CMD/.PIF`. `APPLY.CMD` preserves each original beside the replacement
   with a `.BAK` suffix and installs the released mouse driver at
   `SYSTEM\MOUSE.DRV`; `PATCH386.EXE` performs only the two checked
   binary transformations.  Generic `PIF.EXE`, `HASH.EXE`, and `MOUSE31.DRV`
   are consumed from `assets/release`, never duplicated in the tool directory.
4. `win31-path` repairs only configuration and Program Manager references that demonstrably
   name an object in the selected tree.  It parses supported `PMCC` group-item
   executable-path fields rather than doing raw binary replacement, preserves
   unrelated user configuration, and records every changed file/line.
5. `win31-launch` ensures generated launch files use `run16` from `PATH`, never a fixed
   `O:` location, a package path, repository path, default NT profile, or a
   drive substitution.

## Verification

Use fresh build-owned copied installation fixtures. Assert launch PIF/profile
integrity, recovery-copy and candidate hashes separately from path discovery,
rewrite precision, idempotence and negative no-write cases. When a valid
runtime fixture is available, establish that standard and 386-mode profiles
invoke the expected `run16` target and retain their actual result. Record no
enhanced-mode success claim.
