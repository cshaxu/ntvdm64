# M0 T420 S8 — SoftPC disks package recovery

## Selected original manifest

The original `softpc.new/base/disks/sources` manifest selects ten units:
`floppy`, `floppy_i`, `diskbios`, `fdisk`, `fla`, `gfi`, `gfi_ibm`,
`gfi_mpty`, `gfi_sflp`, and `gfi_test`.

## Source-first disposition

All ten selected files are byte-identical to pinned OpenNT. `floppy.c` had
only a line-ending drift, recovered by the S8 format-normalization P; it has
no code, binding, overlay or adapter difference. There are no reached local
disk-policy replacements to retain or migrate.

## Verification

Fresh x86 graph: `build/M0-T420/S8/formal-x86-001`.

- `original-softpc-disks.lib` and all four x86 product programs link. The
  final VdmTib audit passes with `mvdm_vdm_tib.obj` as the unique 4,208-byte
  owner.
- Deployed `O:\winnt` identities are
  `run16=B5B75B07618B2D556B264B45EB72676B8F27EB10BEB487E0D5E4317FB29CD1E0`,
  `basesrv=9907F0BF4684F919F7FBE7DDE39459BE52DFCF9115CACE5730B4FF08670C076F`,
  `ntvdm=3CEDE3B752271AB143D6E9B45E535CC7CE9489F550B7FA3C6426CE90099D0274`, and
  `dtmgr=D791B42923E5F8C4054E88654FD5ECC29EE0C0B7176D98E8D9A81FDC5B06F51A`.
- Direct MEM, repeated MEM, nested COMMAND→MEM, `COMMAND /C`, and EDIT return
  pass. Console 80x25, 45x34, 60x50, 120x30, resize and mouse integration
  pass; five short-window EDIT→MEM runs retain expected MEM output and
  geometry.

## Next owner

S9 owns `softpc.new/base/comms` and may consume this exact disk contract
without adding a duplicate backing-store policy.
