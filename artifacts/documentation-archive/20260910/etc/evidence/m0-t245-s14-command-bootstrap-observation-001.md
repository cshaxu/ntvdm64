# M0 T245 S14 — guest `commnd` bootstrap observation

## Question

Does the source-built `SHELL=` value emitted by the imported COMMAND provider
actually reach the original guest `commnd` buffer before the existing
`50:3D` terminal?

## Source map and boundary

The exact source-built `NTIO.SYS` map at
`build/M0-T225-S8/ntio-primary-source-build-r8/base/mvdm/dos/v86/doskrnl/bios/ntio.map`
maps `commnd` to `009d:3466` and `stall` to `009d:0984`.
`sysconf.asm:1779..1804` writes the executable token into `commnd`; the
source-built native run reaches the accepted `5E` boundary at loaded
`8e08:08af`, after that configuration phase.

The default-off observer is consequently adapter-owned and fixed-width:

- it recognizes only accepted real-mode `C4 C4 5E` at `EIP=08af`;
- it uses the existing selector-blind checked physical-RAM read to copy exactly
  64 bytes from `CS:3466`;
- it changes neither guest RAM nor CPU state and introduces no Bochs source or
  semantic change.

## Verification

Formal MSVC x64 `/MT` Ninja graph:

```text
build/M0-T245-S14/formal-r1
```

The focused `t245-s14-command-bootstrap-observation-fixture.exe` passed.  It
proves the accepted `5E` predicate, translated physical address, fixed copy
and negative-offset rejection.  The same graph linked
`ntdos64-native.exe`.

One Direct source-built native run used `share.exe`, the current DOS/WOW16
bundles and a 1,000,000 instruction budget.  At the accepted boundary it
reported:

```text
command-bootstrap observed=1 read-failed=0 cs=8e08 eip=000008af
physical=000914e6
bytes=4255494c445c4f55545055545c444f535c434f4d4d414e442e434f4d00...
```

The byte prefix decodes as `BUILD\\OUTPUT\\DOS\\COMMAND.COM`.  This is the
guest-owned `commnd` token and directly proves that the S13 original
`cmdconf.c → SHELL=` route reached guest NTDOS.  The run still ends at
`50:3D`, loaded `8e08:0984`.

## Conclusion and remaining boundary

`8e08:0984` maps to original `sysinit1.asm:stall`, immediately following the
`comerr` error display path.  Thus the terminal is guest-owned NTDOS bootstrap
failure, not a missing COMMAND BOP, a failed source-path publication, or a
Bochs dispatch problem.

This observation deliberately does **not** distinguish the two original
incoming `comerr` branches: the pre-EXEC command-file `INT 21h/AH=3Dh` open
failure and the later `INT 21h/AH=4Bh` EXEC failure.  That distinction requires
a separately bounded, source-mapped DOS interrupt/error observation; it must
not be guessed from the `50:3D` terminal or repaired by a virtual drive.
