# M0 T310 S8 P4 — SoftPC video call-contract closure

## Scope

This evidence closes one original SoftPC video/BIOS call-contract cluster in
the S8 whole-tree ABI audit.  The scope is limited to the selected
`CPU_40_STYLE` + `C_VID` configuration of `mvdm-host`; it does not change
video algorithms, guest state, or any adapter boundary.

## Source findings and disposition

- `base/bios/virtual.c` calls the selected SoftPC patch provider
  `ClearInstanceDataMarking` during `VxD_System_Exit`, but the split tree has
  no declaration carrier.  The call now has the provider's exact `VOID(VOID)`
  declaration; its lifecycle order is unchanged.
- `base/video/gfx_updt.c` calls the original `host_stream_io_update` provider
  from `host/src/nt_graph.c` and the original C-video `setMarkPointers`
  selector.  Both now have their exact `void` declarations at the source call
  boundary.
- In the selected C-video profile, the old generic `MEM_HANDLERS` fields do
  not describe the actual EVID glue functions in `cvidc/ev_glue.c`.  The
  selected conditional branch now carries those six exact original callback
  signatures.  The old generic A2/debug `vid_handlers` table is not installed
  under that profile and remains compiled only for its original non-C-video
  selections.
- `base/video/ega_writ.c` publishes its existing `setWritePointers` selector
  contract rather than making an implicit host-int call.

These are declaration/selection repairs only.  Private native video pointers
remain `IHPE` where the original source uses them; no guest address is treated
as a host pointer and no mapping-manager identity is introduced.

## Verification

The source-shaped generated C-video copies were refreshed with Node 22 using
the already registered generation rules, then the formal Ninja graph rebuilt
the complete original BIOS and video archives outside the sandbox:

- `build/M0-T310/S8/p1-machine-source/x64/p4-video-abi.log`
- `build/M0-T310/S8/p1-machine-source/x86/p4-video-abi.log`

For `virtual.c`, `gfx_updt.c`, and `ega_writ.c`, each log contains zero
`C4013`, `C4113`, `C4057`, `C4028`, `C4133`, `C4311`, and `C4312` diagnostics.
Other original warnings remain compiler-visible and are deliberately outside
this call-contract cluster.
