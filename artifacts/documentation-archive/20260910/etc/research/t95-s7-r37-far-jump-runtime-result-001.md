# T95 S7 r37 Far-Jump Runtime Result

## Question

What target does original OpenNT pseudo-ROM `int10h_vector` obtain from its
saved `host_int10` pointer immediately before the r36 TF loop?

## Inputs and Procedure

Used the one admitted r37 build root and one 15-second immutable observation:
`artifacts/analysis/t95-s7-runtime-trace-030-20260811-001`.  The binary
SHA-256 is `A18BD47CBEBAE2AB72267E99B85F4DC0C45B1D9928AABDFE5A64285931F53A16`.
`BX-TRACE-051` logs only original decoded real-mode immediate far jumps.

## Observation

The final relevant row is:

```text
old-cs=073b old-ip=0939 target-cs=0000 target-ip=0000
```

The immediately preceding r36/r37 source-correlated path is original guest
INT29 `outchr` → INT10 pseudo-ROM vector.  `spckbd.asm` shows that the vector
uses an `EA` far jump through `host_int10`; `InstSpcKbd` copies that field from
the pre-existing IVT INT10 entry before replacing the IVT entry with its own
pseudo-ROM handler.  Thus the saved pre-existing INT10 vector is zero.

The run otherwise retains 39 BOP observations and 10 committed transactions.

## Interpretation

High confidence: the post-close TF loop is downstream of a missing initial
machine BIOS INT10 vector.  OpenNT guest code faithfully saves and invokes
`0000:0000`; Bochs faithfully performs the far jump and later interrupt/IRET
mechanics.  This is neither a BOP/DEM/filesystem gap nor an adapter result
error, and clearing TF or repairing IRET would hide the actual missing
startup-machine precondition.

The next design decision must classify how the selected native Bochs container
can establish an original usable BIOS IVT before the NTIO guest initialization
starts.  It must not synthesize an OpenNT vector in the adapter or alter guest
code.  Candidate work is limited to Bochs-owned native firmware/reset
lifecycle evidence.
