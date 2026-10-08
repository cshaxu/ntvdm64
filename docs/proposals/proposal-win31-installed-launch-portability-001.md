# Proposal: installed Windows 3.1 portable launch repair

## Objective

Make an already installed, otherwise clean Windows 3.1 tree portable for this
project without requiring the original Setup workflow to run again. The tool
creates only the installation's `PATCH` directory, validates the installation
topology, installs the accepted add-ons and recoverable images, repairs known
relocatable references, and writes explicit standard/real-mode launch profiles.

## Product boundary

This is an installed-tree repair, not an installer and not a Windows 3.1
compatibility claim. It must never mutate supplied retail media. It must retain
the original `KRNL386.EXE` and `WIN386.EXE` before installing the two existing,
identity-bound approved candidates. The tool uses released `MOUSE31.DRV` only.

The normal original Setup post-copy protected-mode transition remains a
separate queue-tail package. A successful portable launch cannot be cited as a
Setup pass; a Setup repair cannot be silently folded into this tool.

## Intended behavior

1. Accept an explicit installed-root path and discover the existing Win3.1
   system layout from required files, not fixed drive letters.
2. Reject absent, malformed, linked, or identity-incompatible installations
   without changing them.
3. Create/update an owned `PATCH` directory with `MOUSE31.DRV`, checked
   recovery originals, approved candidate images, shared `CONFIG.NT` and
   `AUTOEXEC.NT`, and explicit `WINSTD.CMD/.PIF` plus `WINREAL.CMD/.PIF`.
4. Repair only configuration and Program Manager references that demonstrably
   name the old installation root; preserve unrelated user configuration and
   record every changed file/line.
5. Ensure generated launch files use `run16` from `PATH`, never a fixed
   `O:` location, a package path, repository path, default NT profile, or a
   drive substitution.

## Verification

Use a fresh build-owned copied installation fixture. Assert topology
discovery, path rewrite precision, PIF/profile integrity, recovery-copy and
candidate hashes, idempotence, and negative no-write cases. When a valid
runtime fixture is available, manually/automatically establish that standard
and real-mode profiles invoke the expected `run16` target and retain their
actual result. Record no enhanced-mode success claim.
