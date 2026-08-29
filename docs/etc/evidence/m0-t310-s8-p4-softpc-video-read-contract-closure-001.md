# M0 T310 S8 P4 — SoftPC C-video read-contract closure

The selected `CPU_40_STYLE` + `C_VID` configuration binds the original EVID
read glue through `READ_POINTERS`. Its historical `ULONG` fields and three
implicit imports only matched the real `IU32` contracts through 32-bit typedef
coincidence. This change retains all original provider bodies and table order,
but selects the exact `ev_glue.c` read signatures for the selected C-video
configuration. Other historical selections retain their original carrier.

Formal Ninja rebuilt `original-softpc-video.lib` for both architectures after
the source-shaped generated inputs were refreshed using the registered rules:

- `build/M0-T310/S8/p1-machine-source/x64/p4-video-read-abi.log`
- `build/M0-T310/S8/p1-machine-source/x86/p4-video-read-abi.log`

Each log reports zero `C4013`, `C4113`, `C4057`, `C4028`, `C4133`, `C4311`,
`C4312`, or `C4431` diagnostics for `ega_read.c`. Unrelated original warnings
remain compiler-visible.
