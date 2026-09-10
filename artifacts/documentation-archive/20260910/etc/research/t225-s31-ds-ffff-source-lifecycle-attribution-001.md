# T225 S31 DS:FFFF source/lifecycle attribution

## Fixed runtime facts

S30's immutable Direct and Readonly runs are identical: the original
`read_virtual_word_32` direct-limit branch reads two bytes through segment
index `3`, selector `0C41`, at offset/limit `FFFF`.  Adopted
`src/bx-core/cpu/cpu.h:29-34` defines index `3` as `DS`.  This is a DS word
read at the end of a real-mode segment, not an instruction-fetch or CS-limit
event.

## Exact original source/build candidate

The original device header structure in
`src/opennt/base/mvdm/dos/v86/inc/devsym.inc:12-18` defines:

```text
SDEVNEXT = +0 (far pointer)
SDEVATT  = +4 (word)
SDEVSTRAT = +6 (word)
SDEVINT  = +8 (word)
```

`src/opennt/base/mvdm/dos/v86/doskrnl/dos/dev.asm:518-542` states that
`DEVIOCALL2` enters with `DS:SI` at a device header and immediately performs
`MOV AX,[SI.SDEVSTRAT]`.  The source-built address record establishes:

```text
DEVIOCALL2 map address = 0000:4D47
final NTDOS.SYS file offset = 0x4D47 - 0x2250 = 0x2AF7
immutable bytes             = 8B 44 06  (mov ax,[si+6])
```

Consequently `SI=FFF9` would form exactly the observed DS word access at
`FFFF`.  This is the sole identified original-source instruction whose field
layout and source-built bytes directly match the observed width, DS segment,
and effective end offset.

## Lifecycle comparison

This is a candidate, not a dynamic identity proof.  The first original NTIO
resident device header is `CON`; `msbio1.asm:76-84` defines it with `strategy`
and `con_entry`, and the retained source-built NTIO record maps those pointers
to `0070:01EC` and `0070:01F7`.  Normal original
`charinit -> DEVIOCALL2` requires that `DS:SI` still identify that source-built
resident header.  Its own source contract is recorded at `dev.asm:520-540`.

The native tuple instead has `DS=0C41`.  It therefore cannot by itself prove
the normal first-CON header path, and it rules out treating the fault as a
generic host mutation or an arbitrary CPU wrap issue.  The first-fault EIP
`0C41:2357` is not an instruction-boundary witness for `0C41:2AF7`; S30
deliberately did not copy instruction bytes or guest memory.  No claim is made
that DEVIOCALL2 has dynamically executed.

## Source-first disposition

| Candidate owner | Disposition |
| --- | --- |
| BOP/provider/profile mutation | Rejected; Direct and Readonly are identical and no BOP fact is present. |
| Bochs real-mode wrap/segment repair | Rejected; original direct-limit behavior is observed, but source evidence has not shown a correct caller requiring wrap. |
| Original NTDOS DEVIOCALL2 producer | Strong static candidate only; exact source/build shape matches, dynamic entry remains unproven. |
| Original NTDOS/NTIO bootstrap device-header lifecycle | Next owner package; it must establish the original `sysinit`/`DosInit` relocation and `DS:SI` device-header provenance without prewriting a header or pointer. |

## Consequence

No code change is admitted.  The successor must audit the complete original
guest bootstrap/device-header lifecycle package—NTDOS load location, sysinit
temporary relocation, DosInit DOSDATA state, resident NTIO device chain and
the input to `DEVIOCALL2`.  It must use source evidence and a package map
before any live observation or machine repair; it may not apply a segment-wrap
change, preseed `DS:SI`, `CALLDEVAD`, request status, or a fake CON device.
