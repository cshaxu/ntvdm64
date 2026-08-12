# T95 S7 BOP 5E Config-Completion Notification Audit 001

r19 reaches `C4 C4 5E` with `AL=0` immediately after the guest `ConfigDone`
site in `dos/v86/doskrnl/bios/sysinit1.asm:1427-1431`. The source defines this
as CONFIG.SYS-complete notification (`bop.h:37`). Historical `MS_bop_E` in
`softpc.new/host/src/nt_bop.c:710-724` calls `UMBNotify(0)` and `demDasdInit()`.
`nt_umb.c:584-589` shows `UMBNotify` is itself an empty return. `demdasd.c:48-54`
only initializes host floppy/fixed-DASD tables for raw-disk services.

The contained CLI profile supplies neither raw physical disks nor INT25/26
capability. It must not invent DAD tables, host-drive probing, UMB mappings or
host notifications. Therefore the exact real-mode `C4 C4 5E` with AL=0 may
resume at RIP+3 with empty CPU delta. Other AL values and all malformed/mode
mismatches remain listener pass-through. The guest then retains ownership of
its following `AllocUMB` attempt and its no-UMB outcome.
