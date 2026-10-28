# Directory and interrupt declarations checkpoint

The [directory review](shard-2/curdir-abi-review.md) identifies the original
`curdir.h` as a dependency of selected DEM and WOW sources. The [interrupt
review](shard-2/dpmi-interrupt-interface-review.md) identifies `precomp.h`
and `intapi.h` as dependencies of the selected DPMI interrupt object. Their
three declaration roles are partial; the originals and current headers match, while
runtime ABI, state synchronization, nesting and lifecycle remain unproved.

The [bus mouse and PPI review](shard-2/dos-mouse-ppi-device-review.md)
also identifies two selected original source roles. Both are partial: mapped
port-registration functions do not prove the reset branch or actual I/O.

The historical `CURDIR.INC` and `VINT.INC` have additional guest or original
assembly consumers; neither is classified from these limited observations.
The [merge verifier](directory-interrupt-headers-role-merge.py) records the
source identities, exact dispositions and frozen-ledger consistency. It does
not execute guest functionality or measure behavioral recovery.
