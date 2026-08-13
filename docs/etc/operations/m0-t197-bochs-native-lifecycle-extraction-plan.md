# M0 T197 Bochs Native Lifecycle Extraction Plan

## Purpose

T197 makes the architecture's `bx-core` / `bx-mantle` / `bx-vdm` separation
physical and buildable without turning the project into a Bochs rewrite.
Bochs mechanical semantics remain adopted code; the mantle only assembles the
small native lifecycle needed to host them; the VDM layer owns all OpenNT and
BOP meaning.

## Ordered S Packets

| S | Bounded delivery | Gate before the next S |
| --- | --- | --- |
| S1 | Rename `bx-ntvdm-adapter` and its test root to `bx-vdm`; map every current Bochs build-bearing area as pure core, pure mantle, or mixed. | Old live path is absent; map names an exact candidate move set and all mixed edges. |
| S2 | Use `git mv` to place only S1-proven pure core and pure mantle files under their final roots; repair only path references and build metadata. | Source identity is preserved; mixed source remains in place and recorded. |
| S3 | Define the narrow native mantle lifecycle contract: SIM, logging, no-device time, decode initialization, RAM, and default port-space ownership. | Every mantle-to-core symbol is named; product-shell and VDM negative retention is proven. |
| S4 | Extract and compile the admitted mantle assembly through native Bochs code; resolve mixed edges only with an explicit per-edge decision. | No selector, service, DOS, OpenNT, GUI/plugin, BIOS-loader, CMOS, or device assembly is introduced. |
| S5 | Admit and execute one minimal MSVC closure build with fixed configuration and fixture-root evidence. | Object/link map proves only the stated core/mantle closure; no full `bochs.exe` target is used. |
| S6 | Reconcile physical paths, provenance, exception register and focused boundary tests; close T197 or record the first source-backed remaining lifecycle edge. | Architecture, source layout, build contract and actual file placement agree. |

## Placement Rule

A file moves in S2 only when its current implementation and direct native
dependencies establish one owner without interpretation:

- **Pure core:** CPU/decode, exception, RAM/ROM or admitted no-device
  mechanics, with no product composition or VDM meaning.
- **Pure mantle:** native Bochs lifecycle assembly, parameter/logging/time
  setup, and retained mechanical object composition, with no VDM, BOP,
  OpenNT, DOS, GUI/plugin or unadmitted device semantics.
- **Mixed:** reaches both a core capability and historical product-shell,
  firmware/device, plugin/GUI, or VDM-facing behavior. It is not moved by
  mechanical cleanup; the exact edge becomes a later design decision.

## Non-goals

This package does not continue BOP runtime closure, recreate SoftPC, replace
Bochs CPU/memory/device algorithms, import new source, or use a successful
full Bochs build as evidence of the minimal closure.
