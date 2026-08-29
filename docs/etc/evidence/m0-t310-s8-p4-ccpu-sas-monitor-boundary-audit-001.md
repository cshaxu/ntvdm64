# M0 T310 S8 P4 — CCPU/SAS monitor-vector boundary audit

## Cluster

The unresolved `c_effective_addr`, `c_sas_touch`, and
`c_VirtualiseInstruction` forms are one `SPC-CCPU-EXECUTOR-DISPATCH`
owner-contract cluster, not three independent linker fixes.

`ccpu386/ccpusas4.c` installs `ccpu386/sascdef.c`'s `cSasPtrs` vector.  That
selected original vector contains the `c_sas_touch` and
`c_VirtualiseInstruction` slots, while `cpu4gen.h` maps CCPU's
`effective_addr` spelling to `c_effective_addr`.

## Original boundary

The available OpenNT monitor implementation of `effective_addr` is in
`mvdm/v86/monitor/i386/sas.c`.  It delegates to `Sim32GetVDMPointer` and
returns the resulting process alias as an `ULONG`.  This is a historical
kernel/monitor product boundary: it cannot be linked as a modern user-mode
SoftPC implementation, and the alias must not be recreated as a raw pointer
on either host width.

The NTVDMx64 `fmstubs.c` source contains debug-break placeholders for the two
remaining SAS-vector slots.  They are evidence of unrecovered monitor work,
not valid normal-runtime implementations.

## Rejected false recovery

`softpc.new/base/cvidc/sascdef.c` is an original file but it is **not** a
drop-in replacement for `ccpu386/sascdef.c`.  Its older `SasVector` initializer
omits the `c_sas_touch` and `c_VirtualiseInstruction` tail slots and has a
different CPU-private include family.  Selecting it would make the DLL link
look cleaner by changing the CCPU vector ABI; it would not recover the
selected CCPU machine.

## Required recovery shape

The subsequent implementation must retain the three original CCPU spellings
and fixed-width numeric inputs, but replace the monitor's process-pointer
assumption with the selected SoftPC machine's own numeric SAS/physical-memory
state.  Any source address crossing must use the session mapping-manager
rules; protected-mode selector resolution needs a source-shaped SoftPC
descriptor/state owner, not the excluded kernel VDM monitor and not a Bochs
fallback.  The three slots therefore stay unresolved until that complete
machine-facing contract is implemented and tested together on x86 and x64.

## Consequence

This audit removes an invalid shortcut.  It neither admits a monitor import
nor marks the cluster closed; the normal forced-link log remains the evidence
of the remaining three unresolved forms.
