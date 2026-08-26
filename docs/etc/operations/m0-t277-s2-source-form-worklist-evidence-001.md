# M0 T277 S2 — source-form worklist evidence

## Result

The T277 S2 generator read all 3,697 `call-declaration-correlation` rows from
the S1 interface ledger and joined every caller to its final T276 file
disposition. It created a 325-row source-form review worklist only for callers
that are final `adapter-backed`, `overlay-required` or `binding-only`.

The worklist distribution is:

- `softpc.new`: 197;
- `dos`: 73;
- `dpmi32`: 28;
- `wow32`: 12;
- `xms.486`: 9;
- `vdmredir`: 5; and
- `suballoc`: 1.

Of the 325 rows, 286 originate in adapter-backed files, 38 in
overlay-required files and one in a binding-only file. The source correlation
states remain 275 `source-name-correlation-not-abi-proven` and 50
`source-external-boundary-candidate`.

## Guard result

The verifier proves no final `not-host-runtime`, `guest-only`, `tool-only` or
`firmware-only` caller entered the worklist, every work-item identity is
unique, all 3,697 original correlations remain accounted for, and no row claims
that a source form, ABI owner or failure contract has been completed.

## Next boundary

The next S reviews these 325 entries in package order against their original
caller and provider forms. It may assign a same-shaped adapter owner only with
source/ABI/failure evidence; it may not choose an owner from a symbol spelling
or an include edge.
