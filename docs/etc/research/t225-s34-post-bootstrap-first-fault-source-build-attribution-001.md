# T225 S34 post-bootstrap first-fault source/build attribution

## Fixed facts

The S30 paired runs record the first non-UD fault as real-mode
`#GP(13)` at `0C41:2357`, with a DS word access at offset `FFFF`. The physical
address calculation is `0x0C41 << 4 + 0x2357 = 0xE767`.

S29 establishes only a conditional image correspondence: if the same run's
`50:11` caller loaded NTDOS with `DI=0C41`, `0xE767` is file offset `0x2357`
in the exact 0x6CD2-byte NTDOS image. The source-built NTDOS map covers
DOSCODE/DOSDATA ranges but has no function-symbol-to-file-offset listing;
`msdos.lnk` supplies object order only. The local immutable-byte window was
already disassembled in S29 and does not yield a source function identity.

S31 separately identifies `DEVIOCALL2`'s `MOV AX,[SI+6]` as an exact static
shape candidate for the DS:FFFF word read when `SI=FFF9`, but the first-fault
EIP is not an instruction-boundary witness and the captured CLI report omits
ESI. It remains a candidate, not an attribution.

## Relocation and transcript limits

S33 proves that a retained S19 native run reaches the original `sysinit`,
`DosInit`, and post-`charinit` DPB checkpoint. It is strong evidence that the
current source-built composition can execute those guest stages. It is **not**
the same invocation as S30: S19 used its own 100,000-tick BOP/UD observation;
S30 used a 1,000,000-tick first-fault observer and selected no BOP observer.
No source rule permits merging their dynamic control-flow histories.

Accordingly, none of the following is proven for the S30 fault run:

- the load `DI` value and whether `CS=DS=0C41` describes its loaded image,
  temporary relocation, final DOS code, or unrelated transient segment;
- its completed sequence of original `sysinit`, `DosInit`, `charinit`, and
  `DEVIOCALL2` checkpoints;
- ESI/other registers needed to evaluate the `SI=FFF9` static candidate;
- an exact NTDOS source routine at `0C41:2357`.

The current evidence rejects only false certainty. It cannot justify a
real-mode wrap change, a synthetic device call, a host/BOP provider change, or
any Bochs repair.

## Four-rung ledger and next evidence

| Rung | Disposition |
| --- | --- |
| Original source reuse | Source-built NTDOS image/map and OpenNT sources are the normative address and control-flow baseline, but cannot identify the fault instruction alone. |
| Smallest existing seam | Reuse the already-bound default-off CLI BOP sequence and first-fault observers **together** in one immutable run. Both carry copied fixed-width records and do not alter route, CPU state, or guest bytes. |
| External-code intrusion | Rejected. The required two observers already exist in bx-vdm/bx-mantle; no Bochs hook, decoder, memory, or exception change is needed. |
| Newly authored behavior | Rejected. A new PC trace, guest-memory dump, or synthetic breakpoint would be broader and less faithful than the existing observation combination. |

## S35 admission contract

The next package is one paired, bounded native evidence run in Direct and
Readonly using immutable S10 inputs, the existing startup-source environment,
`--observe-bop-sequence --observe-first-fault`, and the same fixed
one-million-tick budget. It must record full fixed-width BOP sequence,
first-fault and segment-access output, exit/terminal, stdout/stderr hashes,
and any difference between modes. It must not modify code, guest bytes,
profiles, source-built inputs, machine mechanics, or BOP/provider behavior.

The evidence may establish only the sequence reached before the first fault in
that invocation. If it records no BOP sequence before fault, that is a valid
result; it cannot be overwritten with S19's sequence.