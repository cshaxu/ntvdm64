# T144 S1 NTDOS `$Exec` guest-side lifecycle audit 001

## Original COM lifecycle

Original `msproc.asm` allocates a DOS block for a COM image, records the block
as `Exec_Load_Block`, and sets `Exec_DMA = Exec_Load_Block + 10h` paragraphs.
It reads the COM file with DS=`Exec_DMA`, DX=0 through `$READ`/`ExecRead`.
`ExecRead` temporarily changes allocation ownership to the system during the
read, then restores ownership to `CurrentPDB`. For a non-overlay COM load,
the source sets `Exec_Init_CS = Exec_DMA` and `Exec_Init_IP = 100h`, creates
the PDB/header, and later supplies those values to `Xfer_To_User`.

## T130 correlation

The observed `$Exec` transfer has `DS:SI=0BC1:0100`. It therefore directly
identifies `Exec_DMA=0BC1`: this is the command application's source-owned
load segment, not an NTDOS segment. Its file image begins at physical
`0x0BC10`; the source-built COMMAND artifact is 50,384 bytes, so its file
content interval would end at `0x180E0` if fully read into that segment.

The late fetch `0x1BC0F` is beyond that file-content interval, though it may
remain inside the larger DOS allocation block. T130 has no allocation-size,
`Exec_Load_Block`, PDB-header, or command bulk-read record. It cannot prove
whether execution entered an uninitialized/tail region, whether a valid
command control path intentionally reached it, or which ordinary instruction
caused the final increment.

## Disposition

This refines the owner classification: the unresolved state belongs to
**original NTDOS COM load/allocation plus guest command execution**, not the
BOP router, adapter file-provider architecture, or Bochs reset/device layer.
The next useful evidence is a static audit of the existing command image and
its source build/listing/map to establish whether any legitimate path reaches
the allocation tail; no runtime diagnostic or BOP patch is admitted.
