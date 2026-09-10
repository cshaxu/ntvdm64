# M0 T301 S1 P14 — Rebased First-Degree Global Original-Source Map

## Index

P14 creates a reusable, local Ctags definition index over both approved source
trees: OpenNT and OpenNT-4.5.  The cache is an audit acceleration artifact, not
a product source, build input or runtime dependency.  It contains 64,059 tag
records and is regenerated only from the approved trees.

## Rebased frontier result

Every one of the 12,487 physical P13 first-degree calls is mapped to the
packages containing same-spelling original function-definition candidates:

- 6,066 calls have a CRT-source candidate;
- 1,475 have a host-boundary candidate (`base/win32/client`, CSR or kernel
  VDM families);
- 1,557 have a non-MVDM OpenNT-source candidate;
- 3,055 have no function-definition candidate in either approved source tree;
- 334 retain a MVDM variant/parser-gap candidate.

The map preserves physical caller identity and records candidate packages, not
an unproven chosen implementation.  For example, `strcpy` exposes several
historical definitions; P14 correctly classifies it only as a CRT-source
candidate until header/ABI selection proves the public leaf binding.

## Next work

P14 partitions the remaining T301 work into source-backed package resolution,
public/CRT leaf confirmation, private hard-boundary confirmation, MVDM-variant
correction and source-absent interface classification.  It does not select a
provider or inspect any second-degree body.
