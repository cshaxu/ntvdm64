# T228 S1 P12 - large reverse-copy and far-pointer mechanics admission

## Purpose

The original COMMAND lifecycle uses a reverse `STD; REP MOVSB` move over its
complete transient image, then relies on copied far-pointer handoff data. P11
and the HeadCall source map make those generic machine mechanics a necessary
prerequisite to classify before any COMMAND/NTDOS repair. This package proves
the mechanics without reproducing a COMMAND layout or treating a BOP as the
failure owner.

## Fixture contract

The test owns opaque bytes only. It writes a 50,640-byte pattern at one
real-mode source segment, places an opaque four-byte far pointer at a fixed
offset within the pattern, and executes a reverse `STD; REP MOVSB; CLD` into a
separate real-mode segment. It then executes an ordinary indirect far call
through the copied pointer; a preexisting test-owned `RETF` byte returns to a
fixture-only `UD2` controlled stop. A terminal snapshot checks surrounding
sentinels and the copied four-byte value.

The fixture does not use a COMMAND image, OpenNT source data, BOP selector,
service, host path, guest filesystem, adapter, or production CLI interface.
The numeric ranges are isolated fixture memory and have no relation to a live
guest address.

## Source/ABI/failure ledger

| Rung | Disposition |
| --- | --- |
| Original source fact | COMMAND `rdata.asm` uses reverse `REP MOVSB` for the transient image; `tcode.asm` uses an indirect far call via a copied handoff field. |
| Existing seam | `bx_ntvdm_finite_run` already owns opaque test entry bytes, checked ordinary RAM, real-mode setup, controlled `UD2` stop, and terminal snapshots. |
| External intrusion | None. |
| New behavior | One fixture and manifest target only; no production source/ABI behavior. |

## Failure interpretation

A failing copy, pointer, call, return, or sentinel proves a selector-blind
bx-core/bx-mantle mechanical prerequisite is incomplete. A pass does not prove
COMMAND startup correct; it eliminates this complete generic shape and leaves
the original COMMAND/NTDOS lifecycle owner domain for the next package.