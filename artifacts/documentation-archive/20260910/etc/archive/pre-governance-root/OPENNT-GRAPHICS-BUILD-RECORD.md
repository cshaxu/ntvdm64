# OpenNT GRAPHICS Build Record

## Scope

This record covers the OpenNT source tree at revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`, specifically
`base/mvdm/dos/v86/cmd/graphics`. It is a compatibility-build experiment only.
It does not certify retail identity, a runnable DOS payload, or a working
printer/display path.

## Source Closure

The historical makefile declares two payloads:

* `GRAPHICS.COM`, linked from thirteen assembler objects in `graphics.lnk`.
* `GRAPHICS.PRO`, the printer/profile data file placed alongside the command.

The staged build copied the `graphics`, `dos/v86/inc`, and
`dos/v86/messages` source directories into
`toolchain-probe/graphics-stage-v1`. The EN-US message source is
`messages/usa-ms.msg` with its matching `usa-ms.idx`.

`BUILDMSG` has the historical argument contract `country skeleton`, so the
successful generated-message invocation from the graphics working directory
was:

```text
buildmsg-host.exe ..\..\messages\usa-ms graphics.skl
```

It generated `GRAPHICS.CLA`, `GRAPHICS.CLB`, `GRAPHICS.CLC`, `GRAPHICS.CL1`,
and `GRAPHICS.CL2`. All thirteen original assembler sources were then assembled
with the OpenNT-source-built MASM compatibility probe, with no warning or
severe assembler errors:

```text
graphics grint2fh grpattrn grctrl grcpsd grcolprt grbwprt
grinst grparse grparms grload grload2 grload3
```

The attempted link order exactly follows `graphics.lnk`.

## Stage-Local Linker Adjustment

`graphics.asm` is the primary module and declares `END START`. `grcpsd.asm`
also declares `END COPY_SHARED_DATA`. Historical `LINK16` accepts that object
layout, while the tested OpenWatcom WLink rejects it with "multiple starting
addresses found." In the disposable stage only, the latter directive was
changed to bare `END`, then `grcpsd.obj` was reassembled. No instruction or
data line was changed; the source checkout remains untouched.

## Result and Limitation

WLink produced `graphics.exc`, and the local `opennt-reloc` utility converted
it to a candidate `GRAPHICS.COM` at load segment zero. WLink also reported
seven `W1162` diagnostics for `grpattrn.obj`: "relocations on iterated data not
supported." This is a material linker-compatibility limitation, not a benign
warning. The candidate must not be considered runnable or equivalent to the
reference file until a linker that preserves those relocations is used and
runtime behavior is independently checked.

| File | Source / result | Bytes | SHA-256 | Status |
| --- | --- | ---: | --- | --- |
| `GRAPHICS.PRO` | staged source copy | 21,232 | `B734F750A473F83D33AA773FE3D6C7BCDD209D97339F37B03B8C5F3F8EED890B` | byte-identical to the OpenNT-tree companion data file |
| `GRAPHICS.COM` | staged compatibility candidate | 19,950 | `B1138F73E251EF9F0AF745F3FEE2EC352978FAC89ECEB6AC365819CF6EC590A6` | source closure and conversion exercised; blocked from runtime use by WLink relocation loss |
| `GRAPHICS.COM` | OpenNT-tree reference | 19,694 | `989B38806EC1E1BDD30DA3DC6B41CC85BDC81DDA74F870FCC7DF399876EB2756` | comparison reference only; not a generated result |

The OpenNT-tree `GRAPHICS.EXC` is 20,462 bytes, whereas the staged WLink
`graphics.exc` is 19,982 bytes. Those differing image sizes, together with the
iterated-data relocation diagnostics, are sufficient to prohibit a byte or
runtime equivalence claim.

## Next Requirement

Before promoting this command beyond a source-closure candidate, reproduce the
link with a linker that supports OMF iterated-data relocation records, then
verify the COM load image and exercise installation, display interaction, and
printer interrupt behavior under the non-invasive runtime experiment.
