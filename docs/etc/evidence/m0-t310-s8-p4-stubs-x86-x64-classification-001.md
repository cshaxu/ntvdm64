# M0 T310 S8 P4: stubs.c x86/x64 Classification

## Question

Do the complete original softpc.new/host/src/stubs.c diagnostic cluster contain host x86/x64 ABI defects that require source repair?

## Inputs and procedure

- Rebuilt the formal stubs.obj target under both x64 and x86.
- Read every diagnosed definition, its selected declarations, and reached callers.
- Compared the value classes with the S8 rule: only native pointer, handle, function-pointer/table, or fixed-width guest-identity crossings are x86/x64 repair candidates.

## Observations

- log1p, sas_manage_xms, sas_loadw, and host_speed have typed scalar or source-shaped native-pointer contracts. Existing DIV-095 keeps the only private native pointer out of guest carriers.
- Default-int warnings are old diagnostic stubs: mouse, historical locking, A2CPU diagnostics, floppy, and configuration fallback. Their incomplete product semantics are not a width conversion and occur identically on x86 and x64.
- Neither formal build reports a pointer truncation, incompatible function pointer table, or calling-convention difference for this unit.

## Result

All tracked stubs.c records are classified in the P4 worklist. The visible non-x86/x64 warnings remain unsuppressed and the original source is unchanged. Their functional owner-package work remains separate from this ABI audit.
