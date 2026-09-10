# M0 T310 S16 P5 — startup system-file disposition

## Scope

This evidence closes the selected product binding for original
`softpc.new/host/src/nt_msscs.c:AddSystemFiles`.  It does not claim a booted
DOS guest, a completed reset, or any controller behavior.

## Original contract

On the first `GetNextVDMCommand(NULL)` result, `scs_init` calls the original
void `AddSystemFiles` helper.  Its complete original body probes the real
`C:\\` root, then creates zero-length hidden/system/read-only `MSDOS.SYS` and
`IO.SYS` placeholders if their IBM/MS counterparts are absent.  The source
comment identifies this solely as a workaround for a historical Brief 3.1
installer probe.

## Selected product disposition

The selected CCPU40 product preserves the original `scs_init →
AddSystemFiles` order and void completion form.  It defines
`MVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION`, so the mirror calls
`mvdm_softpc_prepare_system_file_compatibility` instead.  That named
`adapter-mvdm-host-out/softpc` operation intentionally performs no host write.

This is deliberately narrow:

- admitted ordinary host-drive projection is not changed;
- no virtual drive or substitute boot volume is created;
- no `IO.SYS`, `MSDOS.SYS`, `IBMBIO.SYS`, or `IBMDOS.SYS` is copied or created
  merely to satisfy the installer probe;
- the complete original creation body remains in the mirror's unselected
  branch as source evidence.

The product has no observable result to return from this helper, so this
binding does not fabricate a guest success result.  It removes only the
installation-style side effect forbidden by the selected app/session policy.

## Source and divergence records

- `MVDM-HOST-DIV-134` in `src/mvdm-host/README.md` records the mirror edit.
- `ADAPTER-SOFTPC-026` in
  `src/adapter-mvdm-host-out/softpc/README.md` records the independent
  adapter disposition.
- `src/mvdm-host/softpc.new/host/src/nt_msscs.c` retains the original helper
  and its `CreateFile` calls unchanged below the selected branch.

## Formal verification

The formal selected Win32/x86 CCPU40 graph was regenerated at
`build/M0-T310/S16/original-softpc-ccpu40-s16p5` with the selected macro and
completed:

```text
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p5 original-softpc-candidate
# 369/369 successful
```

The build compiled both the changed original `nt_msscs.c` and the named
adapter implementation into the same formal source archive.  Existing
original diagnostics remain visible; none is suppressed by this binding.

## Deferred integration

`scs_init` subsequently calls original COMMAND, DEM, XMS and DBG startup
providers, while `InitialiseDosEmulation` proceeds into reset and firmware
controller paths.  Their live machine behavior remains owned by later S17–S49
packets.  This P5 record proves only the selected source binding and its
absence of the forbidden boot-marker host mutation.
