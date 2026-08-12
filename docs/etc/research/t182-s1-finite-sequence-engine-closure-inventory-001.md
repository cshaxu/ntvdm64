# T182 S1 finite-sequence engine closure inventory 001

Date: 2026-08-12  
Packet: M0 T182 S1  
Disposition: complete read-only inventory; a narrow S2 recipe is justified.

## Retained machine derivative

The candidate retained engine is
`artifacts/build/current/t177-s4-native-post-machine-composition-r1/ntdos64-t98-current-adapter.exe`,
SHA-256 `8758F4335CB32B4FB97688ED3860E8B0C9E86D8155E6AA316E54F346177E8BFA`.
Its manifest records the already-built native POST and machine-composition
form: `main.o` and `cpu\\exception.o` are its two historical Bochs object
replacements; the remaining Bochs archives are retained inputs.  T182 does not
change, rebuild, or make those objects or archives prerequisites.

The exact retained archive identities are:

| Retained input | SHA-256 |
| --- | --- |
| `cpu\\libcpu.a` | `E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB` |
| `iodev\\libiodev.a` | `2850F6C041B2401B3627EF98D11E19CC6F13F9BD59BD6AA075E9FEC2BE195A1C` |
| `memory\\libmemory.a` | `1FD886D3C692913BBABABB6B275A85C7BC7B0273339DB69A6B9CB7F7984C4FD0` |
| `gui\\libgui.a` | `A2FE28E22E47549612D7F2E6FB673FEF5506F6B6B3C79F1BE9935F8FFC0C30C2` |

The retained makefile links the two already-present Bochs objects, the three
already-present machine-composition objects, and 67 adapter plus four CLI
objects.  It never makes a Bochs archive a prerequisite.  This is the
appropriate fixed machine base for the T181 sequence; it is not evidence that
the new sequence has run.

## Current-source delta

The retained manifest was compared directly with the current T181 sources.
Six listed compilation units changed and one additional CLI compilation unit
is now mandatory:

| Owner | Current compilation unit | Reason for replacement/addition |
| --- | --- | --- |
| adapter | `bx_ntvdm_readonly_namespace.c` | Five-file immutable namespace. |
| adapter | `bx_ntvdm_adapter_runtime.c` | Identity-load and bind the v2 plan. |
| adapter | `bx_ntvdm_boot_namespace_provider_v1.c` | Materialize the fixed terminal image. |
| adapter | `bx_ntvdm_profile_search_snapshot_v1.c` | Preserve the fifth immutable entry. |
| adapter | `bx_ntvdm_cmd_get_next_service.c` | Commit-driven two-slot `54:01` delivery. |
| CLI | `byob_profile.c` | Exact v5 profile acceptance and terminal identity. |
| CLI | `byob_launch_plan_v2.c` | New mandatory v2 plan parser used by adapter runtime and COMMAND service. |

The last unit is absent from `tools/New-T98S1CurrentAdapterEngineDerivative.ps1`.
Its current list is therefore 67 adapter plus four CLI units, rather than the
required 67 adapter plus five CLI units.  The generator is a useful retained
link-template reference, but it is not a valid T182 recipe unchanged.

`ntdos64_run.c` is deliberately outside the engine link: it is the separate
CLI launcher that creates the v2 environment handoff.  T181's local MinGW
runner build is the source/test evidence for that launcher.  Any later T182
observation must use that current launcher together with the T182 engine, and
must record both identities.

## Boundary and S2 decision

No unresolved machine or provider dependency was found.  The only missing
link closure is the explicitly named `byob_launch_plan_v2.obj`; all other T181
changes replace objects already present in the retained object set.  S2 may
therefore define one fresh derivative recipe that:

1. copies the retained T177 machine derivative and verifies its fixed hashes;
2. copies current adapter/CLI headers and the 67 existing adapter plus five
   CLI source units;
3. compiles only those 72 source units, including the seven listed current
   delta units; and
4. final-links against the retained Bochs/machine inputs without any archive,
   device, firmware, or Bochs-object rule.

S2 must reject any recipe that inherits `main.o` or `cpu\\exception.o` as a
build prerequisite, rebuilds an archive, enables a device, or changes a Bochs
macro.  This inventory makes no compile, link, guest, BOP, or normal-return
claim.
