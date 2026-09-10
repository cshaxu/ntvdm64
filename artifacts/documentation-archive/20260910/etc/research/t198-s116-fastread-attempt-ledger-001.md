# M0 T198 S116: Bounded DEMFASTREAD Attempt Ledger

S116 adds a fixture-only ledger with capacity eight. It records only canonical
`C4 C4 50 42` attempts at the composition return boundary: copied CS:EIP and
register fields, accepted/declined bit, and accepted typed outcome metadata.
It excludes every other selector, retains no pointers or guest bytes, and
does not affect outcome routing.

The standalone C11 test passes in
`artifacts/build/t198-s116-ledger-test-r1`; it proves selector exclusion,
accepted/declined ordering, copied result fields and capacity clamp. The
CPU5/x64 bridge and ledger objects compile together in
`artifacts/build/t198-s116-bridge-compile-r1`.

`Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1` now declares and links
the ledger object for a later, separately admitted one-run observation. No
guest execution occurred in S116.
