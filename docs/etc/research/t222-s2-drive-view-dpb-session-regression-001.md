# T222 S2 Drive-View DPB Session Regression

`50:25 demGetDPB` and `50:46 demGetDPBList` now have one Direct/Readonly
normal-session regression over the same admitted C: volume snapshot.  The
full-DPB case verifies the source-assigned fields, preserves the deliberately
unassigned DPB bytes, and returns the source-shaped `AX=15, CF=1` terminal for
an excluded drive without writing the caller buffer.

The list case verifies its two source-written fields (`DriveNum`/`Unit` and
`Next`), the terminating `FFFF:FFFF` link, `BP` advancement by the packed
35-byte `A_DPB`, and no flags mutation.  A test offset was corrected during
this work: `A_DPB.Unit` is byte 10, not byte 9.  No runtime provider behavior
changed.

The focused MSVC x64 `/MT` fixture compiled and linked from
`build/M0-T222-S2/030-drive-view-dpb`; its executable exited zero.  This is a
single grouped family entry.  Computer identity and the final all-identity
matrix remain open.
