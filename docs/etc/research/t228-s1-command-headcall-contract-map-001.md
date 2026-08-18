# T228 S1 - COMMAND HeadCall transient-contract map

## Question

What original COMMAND behavior owns the observed scalar transition from
`0C41:25A9` to `2073:7365`, and is that target a valid transient entry?

## Inputs

- P11 scalar CS-transition observation, recorded in
  `t228-s1-cs-transition-ring-result-001.md`.
- Original OpenNT COMMAND sources under
  `src/opennt/base/mvdm/dos/v86/cmd/command/`.
- Source-built COMMAND.COM and command.map from
  `build/M0-T225-S8/command-primary-source-build-r6/base/mvdm/dos/v86/cmd/command/`.

## Source and map facts

`command.map` places `TCOMMAND` at `0249:0104`, `HeadCall` at `0249:95F0`,
and the transient group at `02490h..0C5CFh`.  The observed static location
`0C41:25A9` is fifteen bytes after the source-built TCOMMAND entry.  Its bytes
are `2E FF 1E F0 95`, i.e. `CALL far [CS:95F0]`.

The original `tcode.asm` calls `[HEADCALL]` immediately after transient entry,
with the comment “Make sure header fixed”.  `tspc.asm` defines `HEADCALL` as
the first dword of the transient-side `TranVars` destination block.
`command2.asm` `HavCom` copies the resident `TranVars` block to
`ES:HeadCall` after it has selected `ES=TrnSeg`.  The resident source block in
`rdata.asm` starts with the far address of `HeadFix_Trap`; that stub saves the
transient DS and enters the resident HeadFix path.  Thus the historical
contract is not a call to an arbitrary transient routine: `HeadCall` must hold
the resident HeadFix trap address after the `HavCom` transfer.

The dynamic destination offset `7365h` maps, under the same source-built
TRANGROUP layout, to COMMAND.COM file offset `96F5h`.  That file window begins
ASCII help text (`"ve:][path][filename]..."`), not an executable HeadFix trap
or transient routine entry.  The source-built public-symbol list ends
TRANCODE at `0249:61E1`; `0249:7365` is not a code entry.

## Interpretation

P11's `0C41:25A9 -> 2073:7365` target cannot satisfy the original HeadCall
contract.  This narrows the live fault from the broad dynamic-transient domain
to the complete original COMMAND resident-to-transient transfer lifecycle:

1. allocation and selection of `TrnSeg`;
2. transient initial move and later relocation where applicable;
3. `HavCom` transfer of `TranVars`, especially `HeadCall`;
4. the resident stub/HeadFix return contract; and
5. the guest NTDOS allocation and ordinary-RAM behavior on which those steps
   depend.

It does not establish which producer first wrote the invalid far pointer, and
it does not authorize changing `HeadCall`, copying a source image directly,
or adding a selector-specific Bochs/adapter workaround.

## Follow-up boundary

Any next action must be admitted as one complete COMMAND/NTDOS transient
lifecycle package.  It should establish, using original source layout and a
bounded invariant fixture where possible, whether the TrnSeg allocation/move
and the later HavCom `TranVars` transfer preserve the HeadCall far pointer.
No BOP leaf task, host capability change, guest-image mutation, or Bochs
semantic patch follows from this map alone.