# M0 T276 S1 file identity and package taxonomy

## Inputs

- `m0-t274-s1-non-guest-mvdm-union-ledger.tsv` (canonical selected source);
- `mvdm-file-recovery-ledger.tsv` (one row per selected path);
- `mvdm-package-dependency-ledger.tsv` (declared package counts); and
- local `src/opennt-mvdm-host` mirror plus selected external source paths.

## Procedure

For every selected host path, hash the canonical selected source and local
mirror, compare both with the canonical union hash, then rebuild the file
ledger in deterministic target-path order. The procedure does not compile,
link, enable or alter a source body.

## Result

- Verified paths: 1689
- Verified package roots: 23
- Mirror identity: all rows are `byte-exact-verified`.
- Composition and build-profile fields remain `unreviewed`.

### File kinds

- `build-control`: 2
- `declaration`: 857
- `resource-or-other`: 218
- `source`: 612

### Package path counts

- `bde`: 13
- `dbg`: 3
- `dirs`: 1
- `dos`: 40
- `dpmi`: 59
- `dpmi32`: 25
- `fax`: 21
- `ieuvddex`: 23
- `inc`: 96
- `makefil0`: 1
- `oemuni`: 6
- `sim16`: 8
- `sim32`: 5
- `softpc.new`: 998
- `suballoc`: 8
- `v86`: 35
- `vdd`: 47
- `vdmdbg`: 6
- `vdmexts`: 25
- `vdmredir`: 25
- `vdmutils`: 28
- `wow32`: 201
- `xms.486`: 15

## Limitation

This is source identity and taxonomy evidence only. It makes no claim that
any file is directly composable, adapter-bound, linked, or runnable.

