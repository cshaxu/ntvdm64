# M0 T310 S32 P1 — other-machine source boundary

## Reclassification result

The inherited `SPC-OTHER-MACHINE-SOURCE` static ABI bucket has 110 records in
20 original files.  It is not a remaining machine implementation package.  It
contains historical callback declarations or non-selected declaration/tool
carriers that were not assigned to their real original source owner during the
earlier all-tree x64-oriented warning census.  The selected Win32/x86 CCPU40
profile requires no new wrapper or generic adapter for this bucket.

## Complete source disposition

- `bios.h`, `cntlbop.h`: BIOS control-table declaration carriers; S16 firmware
  and S31 host-BOP bindings retain the original tables and call order.
- `decode.h`: CCPU decoder read callback declarations; S8–S11 own the selected
  CCPU executor/access/SAS contract.
- `egacpu.h`, `egamode.h`, `gfx_upd.h`, `gmi.h`, `gore.h`, `video.h`: original
  video/graphics callback vectors; S25–S27 own the generated C-video,
  controller and host-video contracts.  S32 does not duplicate them.
- `error.h`, `host_sig.h`: source-shaped host error/signal declaration forms;
  S28 owns their platform error/diagnostic binding.
- `ica.h`, `timer.h`, `tmstrobe.h`: PIC/PIT/strobe declarations; S17 owns the
  original controller bodies and host timer coupling.
- `unix_asy.h`: historical asynchronous communication declaration carrier; its
  selected host communications owner is S24.  No Unix provider is selected.
- `harness.h`: historical test-harness declaration only; not a selected
  product input.
- `lm_attr.h`, `lm_clien.h`: bundled FLEXlm declaration metadata; no license
  manager body or callback is selected in this product profile.
- `loader.h`: J-code dynamic-loader declaration carrier; no selected original
  CCPU40 source calls it.
- `dat2obj/dat2obj.c`: source-generation tool with `_X86_` conditional, not a
  runtime compilation input.

## x86 CCPU40 conclusion

The old-style function-pointer declarations remain exact original mirror
content.  In this x86-only recovery phase they do not prove an ABI mismatch,
and rewriting them would create a project-owned compatibility layer with no
selected caller.  Any eventual x64 ABI work must revisit them under the owner
listed above, not reopen S32.

The formal 381-edge CCPU40 graph already compiles all selected original owner
bodies.  No S32-specific code or fixture is warranted; integrated behavior is
validated by S49 through the owning controller/package paths.
