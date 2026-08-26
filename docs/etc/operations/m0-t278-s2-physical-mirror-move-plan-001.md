# M0 T278 S2 — physical mirror move plan

S2 applies the closed S1 manifest strictly with `git mv`:

- `dirs`, `makefil0`, `inc`, `oemuni` and `suballoc` move to
  `opennt-mvdm-support`;
- `vdmutils` moves to `opennt-mvdm-tools`;
- the 58 paths under the approved SoftPC BIOS, ROM and data subtrees move to
  `opennt-mvdm-firmware`.

The operation is a path relocation only. Files retain their package-relative
path and frozen source SHA-256. No source, README, build manifest or include
input is adjusted in this packet; a later S audits mirror roots and build
isolation after the exact moves are proven.
