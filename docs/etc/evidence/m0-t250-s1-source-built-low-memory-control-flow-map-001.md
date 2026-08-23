# M0 T250 S1 — source-built low-memory control-flow map

## Result

The bytes executed at `0000:0A87` are **not** an original NTDOS far-call
entry and do not publish a legitimate far target.  They are byte `0x0F` of
the original NTDOS `DOSDATA:OPENBUF` uninitialised work buffer after
`DosInit` has relocated `DOSDATA` to low memory.  The observed far call and
its `5859:5F04` zero-RAM target are consequently downstream effects of an
earlier guest control transfer into non-executable DOS data.

This closes the proposed target-publication premise.  It does not identify
the earlier erroneous return/transfer, nor does it demonstrate the original
`$Exit/$Abort/reset_environment` parent-return sequence.

## Source and image calculation

The primary original-toolchain `NTDOS.MAP` gives:

| Logical range | Class | Fact |
| --- | --- | --- |
| `00000..07938` | `DOSCODE` | Relocatable NTDOS code. |
| `07940..08F21` | `DOSDATA` | `0x15E2` bytes copied by `DosInit`. |

`origin.asm` sets `PARASTART`; `stripz.c` removes the first `0x2250` bytes
of `NTDOS.BIN` to form `NTDOS.SYS`.  Therefore logical NTDOS address `L`
maps to `NTDOS.SYS` file offset `L - 0x2250` after the normal initial load
and `sysinit1.asm` temporary relocation.

The T249 writer record was:

```text
physical-write address=a87 bytes=1 captured=1 sequence=37093
cs=9386 base=93860 eip=7424 ss=8e08 sp=05fa data=ff
```

`0x7424` disassembles in the source-built image as the `rep movsb` in
`dos/v86/doskrnl/dos/msinit.asm:DosInit`:

```asm
mov si, ax                       ; rounded offset memstrt = 7940h
mov ds, ax                       ; original temporary DOSCODE segment
mov es, cs:[InitBioDataSeg]
mov es, es:[DosDataSg]           ; final low-memory DOSDATA segment
xor di, di
mov cx, offset MSDAT001e         ; 15e2h
rep movsb
```

The source-built `NTDOS.SYS` contains the observed `FF 9F 26 03` sequence
exactly once, at file offset `0x5707`.  The strip relation maps that to
logical `0x7957`, i.e. `DOSDATA + 0x17`.  The recorded post-EXEC stack
segment is `00A7`, which corroborates the original destination calculation:
`00A7:0017` is physical `0x0A87`.

`dos/v86/inc/ms_data.asm` defines the first `DOSDATA` objects in order:

```asm
TIMEBUF  6 bytes                 ; DOSDATA + 0
DEVIOBUF 2 bytes                 ; DOSDATA + 6
OPENBUF  128 bytes               ; DOSDATA + 8
```

So `DOSDATA + 0x17` is `OPENBUF + 0x0F`.  The same file explicitly describes
this region as “Uninitialized data overlayed by initialization code.”  Its
initial source-image bytes must not be interpreted as a callable instruction
stream.  In particular, the accidental decode `call far [bx+0x326]` does not
give `BX+0x326` a source-defined callback or far-pointer meaning.

## Ownership and disposition

| Question | Answer |
| --- | --- |
| Who changed the staged NTIO byte? | Original NTDOS `DosInit` `rep movsb`, exactly as T249 established. |
| Was the overwrite itself valid? | Yes.  The source intentionally relocates `DOSDATA` into the BIOS-selected low-memory segment, superseding the no-longer-needed NTIO staging aperture. |
| Is `0000:0A87` a valid NTDOS code entry? | No.  It is `DOSDATA:OPENBUF+0x0F`, uninitialised guest storage. |
| Does `5859:5F04` identify a missing BOP, Bochs CPU feature, device, or firmware datum? | No.  It is the operand result of accidental data execution; no such repair is selected. |
| What remains? | Determine the prior guest transfer/return that entered `OPENBUF`, within the existing guest EXEC/parent-return continuity owner. |

## Observation decision

No new bx-core/bx-mantle observer is admissible or necessary for the
far-target question.  Existing T248 position history and T249's mechanical
write record, combined with the source/image map above, answer it without
decoding a BOP or modifying guest state.  If a later guest EXEC continuity
package needs to identify the *predecessor* transfer into `OPENBUF`, it must
admit a separate selector-blind observation with that bounded question.

## Limits

This evidence does not claim the guest can return to its parent normally. It
does not alter `OPENBUF`, synthesize a target, fabricate a PSP/PDB return, or
choose a BOP/provider repair.  The unresolved owner remains the original
guest EXEC/parent-return control-flow chain described by
`BOP-DEPENDENCY-115`.
