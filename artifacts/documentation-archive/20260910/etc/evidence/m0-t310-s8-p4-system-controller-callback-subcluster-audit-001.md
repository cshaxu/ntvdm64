# M0 T310 S8 P4 — System-controller callback subcluster audit

## Scope

This source-first review covers the CMOS/RTC, PIC and quick-event members of
the `SPC-SYSTEM-CONTROLLER-CALLBACKS` ABI cluster:

- `softpc.new/base/system/cmos.c`
- `softpc.new/base/system/cmosnt.c`
- `softpc.new/base/system/ica.c`
- `softpc.new/base/system/qevnt.c`
- `softpc.new/base/system/quick_ev.c`

It is a subcluster finding, not closure for the 98-candidate parent cluster.

## Findings and disposition

- The selected original machine graph compiles `cmosnt.c`, `ica.c` and
  `qevnt.c`; it does not compile the alternative `cmos.c` or `quick_ev.c`
  sources.
- `cmosnt.c` declares all four CMOS conversion callbacks with their actual
  parameter forms: `bin2bcd(int)`, `_24to12(half_word)`, `bcd2bin(int)` and
  `_12to24(int)`.  Their assignment sites bind the matching local conversion
  routines, and each observed indirect call supplies the corresponding scalar
  value.  No pointer-width or mapping boundary occurs.
- `ica.c` declares its port, hardware-interrupt and clear-interrupt callbacks
  with the same `IPT` forms exported by `ica.h`; initialization assigns the
  matching `SWPIC_*` routines.  Its callback parameter is an `IU32` controller
  value, not a native pointer.
- `qevnt.c` imports the original `Q_CALLBACK_FN` shape from `quick_ev.h` as
  `void (*)(long)`, stores it unchanged in the original event record, and
  invokes it only with the original `long Param` field.  This is a native
  host-private event callback; no guest address, handle or mapping identity is
  transported.
- `cmos.c` and `quick_ev.c` remain byte-identical unselected original owner
  variants.  Their older unprototyped forms are recorded as static audit
  candidates but do not enter either x86 or x64 selected product build and do
  not authorize a mirror edit.

## Verification

The selected original-SoftPC candidate completed on both architectures after
the corresponding P4 profile correction:

- `build/M0-T310/S8/p1-machine-source/x64/config-i386-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/config-i386-x86-build.log`

No system-controller callback warning appeared for selected `cmosnt.c`,
`ica.c` or `qevnt.c`.  This proves only the reviewed selected callback shapes;
the remaining parent-cluster records retain their existing audit disposition.
