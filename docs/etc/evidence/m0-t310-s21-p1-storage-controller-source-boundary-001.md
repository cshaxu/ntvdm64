# M0 T310 S21 P1 — storage-controller source boundary

## Question

Which complete original storage units belong to controller recovery, rather
than the later host storage endpoint package?

## Result

The S21 selected boundary is the original FDC/WD2010/BIOS/LIM controller side.
The companion ledger records all original units and their interface direction.
Six units are byte-exact OpenNT mirrors. `floppy.c` has no semantic source
change, and `emm_mngr.c` retains the registered private-native-backing
correction; neither creates a guest or cross-component host identity.

The audit finds two decisive later-owner families:

- `host_fdisk_*` implements actual fixed-media access and write policy;
- `host_rpc_*`, host floppy configuration/discovery and remote-service forms
  implement host endpoint selection.

They are S22 host-storage work, not reasons to replace the original FDC,
WD2010, BIOS or EMS controller algorithms.  No Bochs, MONITOR, kernel VDM,
synthetic media or `src.old` path is selected.

## Next S21 work

The next pass reads the `disk_func[]` and `activecmd` initializer/declaration/
call contracts together with the original floppy dispatch forms.  It will
separate any real x86 call-ABI repair from visible original range/default-int
diagnostics before changing a mirror line.
