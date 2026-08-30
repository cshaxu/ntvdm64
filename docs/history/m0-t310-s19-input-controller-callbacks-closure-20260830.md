# M0 T310 S19 closure — input controller callbacks

S19 closes the original SoftPC input-controller callback boundary for the
selected Win32/x86 CCPU40 profile. Keyboard initialization retains its
original callback assignments and EOI hook; INT 15h retains IVT validation
before its original BIOS table call; the mouse dispatcher retains its complete
range-checked 53-entry table and original unavailable branch.

The only retained mirror corrections are the paired scalar-pointer contract
and private SoftPC RAM-backing correction. Neither introduces an adapter,
mapping-manager identity, BOP route or host-input policy. Public host-console
input is deliberately S20 work and display/fullscreen behavior remains S27.
Integrated keyboard/mouse delivery is mandatory S49 validation, not claimed
by this source-boundary closure.

- [boundary audit](../etc/evidence/m0-t310-s19-p1-input-controller-contract-boundary-001.md)
- [x86 assembly](../etc/evidence/m0-t310-s19-p2-input-controller-x86-assembly-001.md)
- [contract disposition](../etc/evidence/m0-t310-s19-p3-input-controller-contract-disposition-001.tsv)
