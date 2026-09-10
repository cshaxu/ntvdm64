# M0 T286 S2 — product-shell overlay evidence

## Input and procedure

- Compared pinned Bochs 2.6 `config.h` and `pc_system.cc` with their
  `bochs-core` counterparts.
- Reviewed the prior `adapter-bochs/minimal_product_shell.cc` definitions and
  every remaining `bx_pc_system_c::Reset`, `bx_pc_system_c::exit` and
  `bx_atexit` definition in production code.
- Moved the file with `git mv` to
  `bochs-core-overlay/pc_system_lifecycle.cc`; updated the formal component
  manifest so it compiles only into the `bochs-core` library.
- Regenerated the formal Ninja graph at `build/M0-T286/S2-x64`.

## Findings

`BX-CONFIG-001` and `BX-CONFIG-002` are generated Bochs configuration facts:
CPU5/P-MMX guest execution and host pointer-size configuration. They cannot
be expressed by an external adapter without either selecting an incompatible
CPU profile or lying to Bochs headers, so they remain minimal mirror changes.

The finite `Reset` and `exit` definitions access `bx_pc_system_c` timer state,
`BX_CPU` globals and A20 mechanics. They are material Bochs-method bodies,
not adapter composition. Their former location in `adapter-bochs` violated the
private-overlay placement rule. The new overlay location is the correct
private owner; `pc_system.cc` retains its registered product-shell crop.

No non-core source defines the two `bx_pc_system_c` methods after the move.

## Formal build limitation

Ninja graph generation did not reach compilation. The pre-existing
`t260-s8-component-manifest.json` still names multiple removed pre-rebootstrap
paths. The first stale `session_input.c` entry was corrected in this S; the
next generator failure is the absent
`adapter-mvdm-host-out/win32/facade/opennt_error_dialog_facade.c`. This is a
global manifest reconstruction issue, not a Bochs compilation result. T286 S6
owns rebuilding that manifest from current component inputs; S2 makes no
passing x86/x64 build claim.

## Conclusion

The only safe S2 code move is complete. Configuration remains in the mirror;
private product-shell methods are now private overlay code. Device, CPU and
memory groups remain with T286 S3--S5.
