# M0 T280 S20 P1 — persistent guest-location consumer sweep

The five `GetVDMAddr` expressions in `demSetDTALocation` do not all create
the same kind of state. The complete source sweep is the adjacent
[ledger](m0-t280-s20-dem-persistent-guest-location-ledger-001.tsv).

Four native pointers persist after the setter returns: `pulDTALocation`,
`pusCurrentPDB`, `pExtendedError`, and `pSFTHead`. `pDosWowData` is only a
local temporary used to derive the SFT address; it must not be treated as a
fifth persistent descriptor.

The decisive dependency is `pusCurrentPDB`: in addition to three DEM callers,
the selected original SoftPC host-control files `nt_fdisk.c` and `nt_rflop.c`
dereference it. S20 cannot safely change only `demgset.c`/`demsrch.c`, nor can
it publish a raw pointer compatibility API. Its numeric descriptor/read
facade must have one source-compatible form for every reached consumer, with
the SoftPC consumers receiving it through `adapter-softpc`.

`pSFTHead` additionally needs a source-layout/chain-extent audit before an
exact lease size is selected. The current sweep is therefore an implementation
admission prerequisite, not a claim that the five locations are already
recovered.
