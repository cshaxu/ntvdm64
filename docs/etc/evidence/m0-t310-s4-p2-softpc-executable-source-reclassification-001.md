# M0 T310 S4 P2: executable SoftPC source reclassification

## Question

Which of the previously firmware-rooted `softpc.new` paths are executable
original SoftPC packages required by the selected SoftPC backend, rather than
immutable firmware media?

## Inputs

- `mvdm-host/softpc.new/host/src/nt_msscs.c`:
  `InitialiseDosEmulation()` calls `reset()` before it initializes the MVDM
  package and loads NTIO.
- Original package manifests
  `softpc.new/base/bios/sources` and `softpc.new/base/keymouse/sources`.
- `base/bios/reset.c`, `bios.c` and `mem_size.c`; `base/keymouse` controller
  bodies; and the exact selected OpenNT hashes already carried in the mirror.
- The S4 nine-family workset and T310 source policy.

## Observations

- `base/bios/sources` selects fourteen C translation units, including
  `reset.c`, `bios.c`, `mem_size.c`, CMOS/RTC, BOP tables and virtual-device
  control. `reset.c` defines `reset()`; it initializes CMOS and invokes
  `AT_kbd_init()`. `bios.c` installs both `reset` and `memory_size` in the
  original BIOS BOP table.
- `base/keymouse/sources` selects five executable keyboard/mouse/controller
  translation units. These are not ROM, CMOS data, or a load-only guest
  artifact.
- The previous placement of both packages beneath
  `mvdm-softpc-firmware` would make the original SoftPC initialization path a
  forbidden firmware-library edge. It contradicted the selected SoftPC
  backend's source-first composition requirement.
- The two packages contain 47 already tracked files. The 24 source, makefile
  and manifest files have exact selected-OpenNT counterparts and SHA-256
  verification. The other 23 are historical `obj/` products with no selected
  upstream counterpart at this source root; Git moved them byte-identically
  from the prior project mirror and they remain evidence-only products. No
  file content or path below `softpc.new/base` was changed by this move.

## Decision

`softpc.new/base/bios` and `softpc.new/base/keymouse` are executable original
SoftPC host-runtime packages. Their unique component owner is `mvdm-host`.
`mvdm-softpc-firmware` retains only the immutable input roots
`softpc.new/bios`, `softpc.new/roms` and `softpc.new/data`.

This is a source-root correction, not a runtime-enable claim: neither package
is newly linked or allowed to fall back to Bochs. Their existing bodies remain
the source authority. Any modern Win32, mapping-manager, or source-shaped
adapter binding is still subject to the S4 family workset.

## Follow-up

P3 selects the first bounded original compilation group in initialization
order: `base/bios` reset/ROM/CMOS plus CCPU/SAS memory and the exact immutable
input manifest. It must classify every resulting external form before it may
call the SoftPC backend runnable.
